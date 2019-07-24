#include <vector>
#include <iostream>
#include <string>

int factorial(int n, char c, int l, std::string z){  
    if(n == 1){
        return 1;
    }

    int p = n * factorial(n-1,c+1,2,z);
    return p;
}

int main() {
    factorial(10,'a',2, "hello!!!!!!!!");
}
