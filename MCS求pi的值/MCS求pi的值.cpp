#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>

using namespace std;

int main()
{
    srand(time(NULL));
    double x, y;
    int tot = 1e4;
    int count0 = 0;
    for(int i = 0; i < tot; i++)
    {
        x =1.0 * rand()/RAND_MAX;
        y =1.0 * rand()/RAND_MAX;
        if(x*x+y*y<=1)
            count0++;
    }
    printf("%f\n",(double)4*count0/tot);
    return 0;
}
