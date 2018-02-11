#include <iostream>
#include <cstdio>

extern "C" {
    double 한길(double, double);
    double 호출(double, double);
}

int main() {
    double c = 호출(3.0, 4.0);
    printf("%lf", c);

    return 4;
}