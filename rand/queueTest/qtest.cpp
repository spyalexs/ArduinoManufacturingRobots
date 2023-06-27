#include <queue>
#include <iostream>

int main(){
    std::cout << "hello" << std::endl;

    double current = 0;

    std::cout << "Current: " << current << std::endl;

    std::queue<double> test;
    test.push(1.0);
    test.push(2.5);
    test.push(4.0);

    current = test.front();

    std::cout << "Current: " << current << std::endl;

    test.pop();

    std::cout << "Current: " << current << std::endl;

    return 0;
}