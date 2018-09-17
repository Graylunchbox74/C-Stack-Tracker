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
    if(x != -1 && x != KEY_RESIZE){
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

    init_pair(8, COLOR_BLACK, COLOR_BLACK);
    init_pair(9, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(10, COLOR_BLACK, COLOR_RED);
    init_pair(11, COLOR_BLACK, COLOR_GREEN);
    init_pair(12, COLOR_BLACK, COLOR_CYAN);
    init_pair(13, COLOR_BLACK, COLOR_WHITE);
    init_pair(14, COLOR_BLACK, COLOR_BLUE);
    init_pair(15, COLOR_BLACK, COLOR_YELLOW);

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
    int functionCalls;

    StackWatcher(){
        setup();
        menuSetup();
        functionCalls = 0;
    }

    ~StackWatcher(){
           endwin();
    }

    void AddStack(Args... input){
        functionCalls++;
        functionStack.push_back(createTupleObject(input...));
        printMenu();
    }

    void RemoveFromStack(){
        if(!functionStack.empty())
            functionStack.pop_back();
        printMenu();
    }

    void printMenu(){
        menuSetup();
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

        mvprintw(0,maxX/2 + 2 + 14, "function calls: %d", functionCalls);

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

StackWatcher<int> x;  
int factorial(int n){  
    x.AddStack(n);

    if(n == 1){
        x.RemoveFromStack();
        return 1;
    }

    int p = n * factorial(n-1);
    x.RemoveFromStack();
    return p;
}

int main(int argc, char ** argv){
    factorial(20);
    return 0;
}











// void colorBomb(){
//     setup();
//     int maxX, maxY;
//     getmaxyx(stdscr,maxY,maxX);
//     int c = 0;

//     while(inputQueue.empty()){
//         input();
//         c = (c + 1) % 8 + 8;
//         attron(COLOR_PAIR(c));
//         for(int i = 0; i < maxY; i++){
//             for(int x = 0; x < maxX; x++){
//                 mvprintw(i,x," ");
//             }
//         }
//         attroff(COLOR_PAIR(c));
//     }
// }


// void staticColor(){
//     setup();
//     int maxX, maxY;
//     getmaxyx(stdscr,maxY,maxX);
//     int c = 0;
//     int thisY, thisX;
//     while(inputQueue.empty()){
//         input();
//         c = (c + 1) % 8 + 8;
//         attron(COLOR_PAIR(c));
//         for(int x = 0; x < rand() % 100; x++){
//             thisY = rand() % maxY;
//             thisX = rand() % maxX;
//             mvprintw(thisY, thisX, " ");
//         }
//         attroff(COLOR_PAIR(c));
//     }
// }

// void textColorBomb(){
//     setup();
//     int maxX, maxY;
//     getmaxyx(stdscr,maxY,maxX);
//     int c = 0;
//     int randomNum;
//     int currentY = 0, currentX = 0;

//     while(inputQueue.empty()){
//         input();
//         c = (c + 1) % 8;
        
//         if(c == 0)
//             c++;
        
//         attron(COLOR_PAIR(c));
        
//         if(currentX == maxX){
//             currentX = 0;
//             currentY++;
//         }
        
//         if(currentY == maxY){
//             currentY = 0;
//         }

//         randomNum = rand() % 10;
//         mvprintw(currentY, currentX, "%d", randomNum);
//         attroff(COLOR_PAIR(c));

//         currentX++;
//     }
// }
