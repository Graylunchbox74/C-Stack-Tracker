#include <iostream>
#include <vector>
#include <stack>
#include <ncurses.h>
#include <string>
#include <tuple>
#include <stack>
#include <queue>
#include <sstream>
#include <deque>

//g++ StackWatcher.cpp -lncurses

using namespace std;

//global variables
queue<int> inputQueue;
int currentX, currentY;


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
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
}

void menuSetup(){
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
}


template<class... Args>
class StackWatcher{
    public:
    deque<tuple<Args...>> functionStack;
    int colorCounter;

    StackWatcher(){
        setup();
        menuSetup();
    }

    ~StackWatcher(){
           endwin();
    }

    void AddStack(Args... input){
        functionStack.push_back(createTupleObject(input...));
        printMenu();
    }

    void RemoveFromStack(){
        if(!functionStack.empty())
            functionStack.pop_back();
        printMenu();
    }

    void printMenu(){
        colorCounter = 0;
        stringstream ss;
        auto functionCopy = functionStack;
        auto old_buf = cout.rdbuf(ss.rdbuf());
        int maxX, maxY;

        getmaxyx(stdscr,maxY,maxX);
        for(int i = 3; i < maxY; i++){
            for(int x = maxX/2 + 1; x < maxX; x++){
                mvprintw(i,x," ");
            }
        }

        currentX = maxX / 2;
        currentY = maxY - (maxY/10);

        while(functionCopy.size() > 10){
            colorCounter = (colorCounter + 1) % 7 + 1;
            functionCopy.pop_front();
        }

        while(!functionCopy.empty()){
            colorCounter = (colorCounter + 1) % 7 + 1;
            attron(COLOR_PAIR(colorCounter));
            for(int i = maxX/2 + 1; i < maxX; i++){
                mvprintw(currentY,i,"-");
            }
            print(functionCopy.front());


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

            functionCopy.pop_front();
            currentY = currentY - (maxY/10);
            attroff(COLOR_PAIR(colorCounter));

        }

        cout.rdbuf(old_buf);

        while(inputQueue.empty()){ input(); }
        inputQueue.pop();
        return;    
    }
};

template<class... Args>
StackWatcher<Args...> AddToStack(Args... t) 
{
    StackWatcher<Args...> currentStack;
    currentStack.AddStack(t...);
    return currentStack;
}

// global var: bool first;
// if(first)
//     auto x = AddToStack();
// else
//     x.AddStack();



int main(int argc, char ** argv){
    auto x = AddToStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 1);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 2);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 3);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 4);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 5);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 6);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 7);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 8);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 9);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 10);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 11);
    x.AddStack(1,2,3,"hello world", 4.5f, true, "asdf", 90, 11);
    
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    x.RemoveFromStack();
    return 0;
}