#!/bin/bash

#check to make sure there are enough arguments
if [ $# -lt 2 ]; then
    echo "[ERROR] - not enough arguments given"
    exit
fi
#set variables
USERFILE=$1;
echo "File to use:"
echo -e "\t $USERFILE"
# skip past the first argument and set the remaining arguments to the functions variable
shift;
FUNCTIONS=$@
echo "Functions to Track:"
echo -e "\t ${FUNCTIONS[@]}"
#create new cpp file to make changes to
NEWFILENAME="$( echo "$USERFILE" | awk -F. '{print $1}' )StackWatcher.cpp"
echo $NEWFILENAME
cp $USERFILE $NEWFILENAME
# add to the top of the cpp file the header to include our cpp lib
sed -i '1 i #include "StackWatcher.hpp"\' $NEWFILENAME;

echo "Adding function structures..."

STACKVAR="x"
# modify all functions to add to stack
for current_function in $FUNCTIONS; do
    echo "Current Function Name: $current_function"
    FUNCTIONLINE="$( cat $USERFILE | egrep -n ".*\s${current_function}.*{.*"| awk -F: '{print $1}' )"


    # Extract function start and finish lines
    lines_in_file=$(cat $USERFILE | wc -l)
    func_header_line=$(cat $USERFILE | egrep -n "$current_function.*{" | awk -F':' '{print $1}')
    func_rest_of_file="$(tail -n $(( $lines_in_file - $func_header_line )) $USERFILE )"
    file_lines=()
    while IFS= read -r line || [ "$line" ]; do
      file_lines+=("$line")
    done < "$USERFILE" 
    curr_linenum=1
    nested_open_braces=1
    for line in $(seq 0 $((${#file_lines} - 1))); do
            curr_line="${file_lines[$line]}"
            if [ $curr_linenum -gt $func_header_line ] && [ $nested_open_braces -gt 0 ]; then
                    for character in $(seq 0 $((${#curr_line}-1))); do
                            if [ "${curr_line:$character:1}" == "{" ]; then
                                    nested_open_braces=$(( $nested_open_braces + 1 ))
                            elif [ "${curr_line:$character:1}" == "}" ]; then
                                    nested_open_braces=$(( $nested_open_braces - 1 ))
                            fi
                    done <<< "$curr_line"
            elif [ $nested_open_braces -eq 0 ]; then
                    ENDOFFUNCTIONLINE=$(( $curr_linenum - 1 ))
                    FUNCTIONLINE=$func_header_line
                    LINES_IN_FILE=$lines_in_file
                    break;
            fi
            curr_linenum=$(( $curr_linenum + 1 ))
    done





    echo -e "\t Function found at line: $FUNCTIONLINE"
    echo -e "\t Function ends at line: $ENDOFFUNCTIONLINE"
    # find the parameter datatypes from FUNCTIONLINE
    DATATYPES=$(cat $NEWFILENAME | egrep -n ".*\s${current_function}.*{.*"  | awk -F '\\(' '{print $2}' | awk -F '\\)' '{print $1}' | 
        awk -F, '{for(i=1; i<=NF; i++) {print $i}}' | sed "s/^[ \t]*//" | awk '{print $1,","}')
    DT="$( echo "${DATATYPES::-1}" | tr '\r\n' ' ' )"
    echo -e "\t Datatypes sent to function: $DT"

    sed -i "$((FUNCTIONLINE))"' i StackWatcher<'"$DT"'> '"$STACKVAR"';' $NEWFILENAME;

    #find the variable names so we can add them to the stack
    VARNAMES=$(cat $NEWFILENAME | egrep -n ".*\s${current_function}.*{.*"  | awk -F '\\(' '{print $2}' | awk -F '\\)' '{print $1}' | 
        awk -F, '{for(i=1; i<=NF; i++) {print $i}}' | sed "s/^[ \t]*//" | awk '{print $2,","}')
    VN="$( echo "${VARNAMES::-1}" | tr '\r\n' ' ' )"
    echo -e "\t Variable Names of function: $VN"

    sed -i "$(($FUNCTIONLINE+3))"' i'" $STACKVAR"'.AddStack\('"$VN"'\);' $NEWFILENAME;
    
    HASFOUND=0
    CURRENTLINE=$(( $ENDOFFUNCTIONLINE ))
    while [ "$HASFOUND" -eq 0 ]; do
        CURRENTLINE=$(( $CURRENTLINE - 1 ))
        TMP="$(sed "$CURRENTLINE q;d" $USERFILE | grep '}')"
        if [ ! -z "$TMP" ]; then 
            HASFOUND=1
        fi
    done
    CURRENTLINE=$(( $ENDOFFUNCTIONLINE + 3 ))
    while [ "$CURRENTLINE" -gt "$FUNCTIONLINE" ]; do
    	echo "Line $CURRENTLINE"
        CURRENTLINE=$(( $CURRENTLINE - 1 ))
        TMP="$(sed "$CURRENTLINE q;d" $NEWFILENAME | grep 'return' )"
        if [ -n "$TMP" ]; then 
                sed -i "$CURRENTLINE"' i'"$STACKVAR"'.RemoveFromStack();' $NEWFILENAME;
        fi
    done
done
echo ""
echo "Compile Errors: "
g++ $NEWFILENAME -lncurses -std=c++11 -o stack_tracker;
rm $NEWFILENAME
