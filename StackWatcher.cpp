#include <iostream>
#include <vector>
#include <stack>
#include <ncurses.h>
#include <string>
#include <tuple>
#include <stack>
#include <queue>

using namespace std;

//global variables - yes, I do need them to be global
queue<int> inputQueue;




// //print tuple
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
    cout << ")\n";
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
    auto x = tuple_size<decltype(tmp)>::value;
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

int main(int argc, char ** argv){
    int x = 0;
    int y = 2;
    string z = "hello world";
    auto p = createStack(x,y,z);
    print(p.top());
    return 0;
}