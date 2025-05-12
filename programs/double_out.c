#include <stdio.h>

int out(double num)
{
    int ret = printf("Number output: %lf\n", num);
    fflush(stdout);
    return ret;
}
