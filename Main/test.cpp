#include <iostream>

extern "C" {
    double 한길(double, double);
    char* 호출(double, double);
}

int main() {
    char* c = 호출(3.0, 4.0);
    std::cout << c;
}