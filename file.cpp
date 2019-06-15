#include <vector>
#include <iostream>

int factorial(int n, char c){  
    if(n == 1){
        return 1;
    }

    int p = n * factorial(n-1,c+1);
    return p;
}

int main() {
    factorial(10,'a');
}