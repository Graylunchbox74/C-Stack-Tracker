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
sed -i '' '1i\
#include "StackWatcher.hpp"\
' $NEWFILENAME;

echo "Adding function structures..."

STACKVAR="x"
# modify all functions to add to stack
for current_function in $FUNCTIONS; do
    echo "Current Function Name: $current_function"
    FUNCTIONLINE="$( cat $USERFILE | egrep -n ".*\s${current_function}.*{.*"| awk -F: '{print $1}' )"
    echo -e "\t Function found at line: $FUNCTIONLINE"
    # find the parameter datatypes from FUNCTIONLINE
    DATATYPES=$(cat $NEWFILENAME | egrep -n ".*\s${current_function}.*{.*"  | awk -F '\\(' '{print $2}' | awk -F '\\)' '{print $1}' | 
        awk -F, '{for(i=1; i<=NF; i++) {print $i}}' | sed "s/^[ \t]*//" | awk '{print $1,","}')
    DT="$( echo "${DATATYPES::-1}" | tr '\r\n' ' ' )"
    echo -e "\t Datatypes sent to function: $DT"

    sed -i '' ''"$((FUNCTIONLINE + 1))"' i\
    StackWatcher<'"$DT"'> '"$STACKVAR"';\
    ' $NEWFILENAME;

    #find the variable names so we can add them to the stack
    VARNAMES=$(cat $NEWFILENAME | egrep -n ".*\s${current_function}.*{.*"  | awk -F '\\(' '{print $2}' | awk -F '\\)' '{print $1}' | 
        awk -F, '{for(i=1; i<=NF; i++) {print $i}}' | sed "s/^[ \t]*//" | awk '{print $2,","}')
    VN="$( echo "${VARNAMES::-1}" | tr '\r\n' ' ' )"
    echo -e "\t Variable Names of function: $VN"


    sed -i '' ''"$(($FUNCTIONLINE+3))"' i\
    '"$STACKVAR"'.AddStack('"$VN"');\
    ' $NEWFILENAME;

    LINESINFILE="$(  wc -l $USERFILE | awk '{print $1}'  )"
    # echo $(( $LINESINFILE - $FUNCTIONLINE + 2))
    ENDOFFUNCTIONLINES="$( tail -n $(( $LINESINFILE - $FUNCTIONLINE + 1)) $USERFILE | egrep -n ".*[a-zA-Z]+\s[a-zA-Z]+.*{.*" | tr -d '[:space:]' | awk -F: '{print $1}' )"
    ENDOFFUNCTIONLINE="$(echo -e "${ENDOFFUNCTIONLINES}" | tr -d '[:space:]')"
    FUNCTIONLINE="$(echo -e "${FUNCTIONLINE}" | tr -d '[:space:]')"
    ENDOFFUNCTIONLINE=$(( $ENDOFFUNCTIONLINE + $FUNCTIONLINE - 1 ))

    HASFOUND=0
    CURRENTLINE=$(( $ENDOFFUNCTIONLINE ))
    while [ "$HASFOUND" -eq 0 ]; do
        CURRENTLINE=$(( $CURRENTLINE - 1 ))
        TMP="$(sed "$CURRENTLINE q;d" $USERFILE | grep '}')"
        if [ ! -z "$TMP" ]; then 
            HASFOUND=1
        fi
    done
    CURRENTLINE=$(( $CURRENTLINE + 3 ))
    while [ "$CURRENTLINE" -gt "$FUNCTIONLINE" ]; do
        CURRENTLINE=$(( $CURRENTLINE - 1 ))
        TMP="$(sed "$CURRENTLINE q;d" $NEWFILENAME | grep 'return' )"
        if [ -n "$TMP" ]; then 
                sed -i '' ''"$CURRENTLINE"' i\
                '"$STACKVAR"'.RemoveFromStack();\
                ' $NEWFILENAME;
        fi
    done
done

g++ $NEWFILENAME -lncurses -std=c++11 -o stack_tracker;
rm $NEWFILENAME