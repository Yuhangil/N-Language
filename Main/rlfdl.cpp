#include <iostream>
#include <cstdio>

extern "C" {
    double sibal(double, double);
    double 한길(double, double);
}

int main() {
    double c = 한길(3.0, 4.0);
    printf("%lf", c);

    return 4;
}