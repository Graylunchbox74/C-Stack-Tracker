#include <iostream>
#include <vector>
#include <stack>
#include <ncurses.h>
#include <string>
#include <tuple>
#include <stack>
#include <queue>
#include <sstream>

using namespace std;

//global variables
queue<int> inputQueue;
int currentX, currentY;
string currentStack;


template<class Tuple, size_t N>
struct TuplePrinter {
    static void print(const Tuple& t) 
    {
        TuplePrinter<Tuple, N-1>::print(t);
        cout << ", " << get<N-1>(t);
    }
};
 
template<class Tuple>
struct TuplePrinter<Tuple, 1>{
    static void print(const Tuple& t) 
    {
        cout << get<0>(t);
    }
};
 
template<class... Args>
void print(const tuple<Args...>& t) 
{
    cout << "(";
    TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
    cout << ")";
}


void createTupleObject(){}

template <typename Tfirst>
tuple<Tfirst> createTupleObject(Tfirst first){
    return make_tuple(first);
}

template <typename Tfirst, typename... Trest>
tuple<Tfirst, Trest...> createTupleObject(Tfirst first, Trest... rest){
    return tuple_cat(make_tuple(first), createTupleObject(rest...));
}

template <typename... Tlist>
stack<tuple<Tlist...>> createStack(Tlist... list){
    auto tmp = createTupleObject(list...);
//    auto x = tuple_size<decltype(tmp)>::value;

    stack<decltype(tmp)> stackOfTuples;
    stackOfTuples.push(tmp);
    return stackOfTuples;
}


void input(){
    int x;
    timeout(2);
    x = getch();
    if(x != -1){
        inputQueue.push(x);
    }
}

int setup(){
    initscr();
    raw();
    curs_set(FALSE);
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); //Border
    init_pair(2, COLOR_WHITE, COLOR_BLACK); //normal color
    init_pair(3, COLOR_RED, COLOR_BLACK); //list
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); //label
    init_pair(5, COLOR_CYAN, COLOR_BLACK); // instructions
    init_pair(6, COLOR_BLACK, COLOR_YELLOW); //normal color
    init_pair(7, COLOR_BLACK, COLOR_CYAN);
}

template<class... Args>
class StackWatcher{
    public:
    stack<tuple<Args...>> functionStack;

    StackWatcher(Args... t){
        functionStack = createStack(t...);
        printMenu();
    }

    void AddStack(Args... input){
        functionStack = createStack(input...);
        functionStack.push(createTupleObject(input...));
        printMenu();
    }

    void printMenu(){
        stringstream ss;
        auto old_buf = cout.rdbuf(ss.rdbuf());
        setup();
        int maxX, maxY;

        getmaxyx(stdscr,maxY,maxX);

        attron(COLOR_PAIR(4));
            mvprintw(0,0,"Program Output:");
            mvprintw(0,maxX/2+2,"StackTracker:");
        attroff(COLOR_PAIR(4));

        attron(COLOR_PAIR(1));
            for(int i = 0; i < maxX; i++){
                mvprintw(2,i,"-");
            }

            for(int i = 0; i < maxY; i++){
                mvprintw(i,maxX/2,"|");
            }

            mvprintw(2,maxX/2,"+");
        attroff(COLOR_PAIR(1));

        currentX = maxX / 2;
        currentY = maxY - (maxY/10);

        while(!functionStack.empty()){
            for(int i = maxX/2 + 1; i < maxX; i++){
                mvprintw(currentY,i,"-");
            }
            print(functionStack.top());


            queue<string> eachLine;
            int nextStart = 0;
            while((ss.str().size() - nextStart) > maxX/2 - 1){
                eachLine.push(ss.str().substr(0,maxX/2 - 1));
                nextStart += maxX/2;
            }
            eachLine.push(ss.str().substr(nextStart,ss.str().size()));
    
            for(int i = 1; !eachLine.empty() && i < (maxY/10); i++) {
                mvprintw(currentY+i,maxX/2+1,"%s",eachLine.front().c_str());
                eachLine.pop();
            }
    
            ss.str("");

            functionStack.pop();
            currentY = currentY - (maxY/10);
        }

        cout.rdbuf(old_buf);

        while(inputQueue.empty()){ input(); }

        return;    
    }
};

template<class... Args>
void AddToStack(Args... t) 
{
    StackWatcher<Args...> currentStack(t...);
   // currentStack.AddStack(t...);
//    StackWatcher<int,int,int>::AddStack(t...);
}

int main(int argc, char ** argv){
    AddToStack(1,2,3,"hello world", 4.5f, true);
    // int x = 0;
    // int y = 2;
    // string z = "hello world";
    // auto p = createStack(x,y,z);
    // p.push(createTupleObject(x,y,z));
    // while(!p.empty()){
    //     print(p.top());
    //     p.pop();
    // }
   endwin();
    return 0;
}