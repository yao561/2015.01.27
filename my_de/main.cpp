#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdio>


#define Np 15
#define D 9
#define xmax 100
#define xmin -100
#define CR 0.3
#define F 0.5

using namespace std;


double x[Np][D];
double v[Np][D];
double u[Np][D];

int count1 = 0;

double fun1(double * Vect)
{
	double dat = 0.0;
	for(int i = 0; i < D; i++)
		dat += Vect[i] * Vect[i];
	return dat;
}


void init()
{
    for(int i = 0; i < Np; i++)
        for(int j = 0; j < D; j++)
            x[i][j] = xmin + (xmax - xmin) * 1.0 * rand() / RAND_MAX;
    double best = 10e20;
    for(int i = 0; i < Np; i++)
    {
        if(fun1(x[i]) < best)
            best = fun1(x[i]);
    }
    printf("%d\t%g\n", ++count1, best);

}

void process()
{
    for(int i = 0; i < Np; i++)
    {
        int r1, r2, r3;
        do
        {
        r1 = rand() % Np;
        } while(r1==i);
        do
        {
        r2 = rand() % Np;
        } while( r2==i || r2==r1);
        do
        {
        r3 = rand() % Np;
        } while( r3==i || r3==r1 || r3==r2 );

        for(int j = 0; j < D; j++)
        {
            v[i][j] = x[r1][j] + F * (x[r2][j] - x[r3][j]);
            if(v[i][j] > xmax)
                v[i][j] = 2 * xmax - v[i][j];
            if(v[i][j] < xmin)
                v[i][j] = 2 * xmin - v[i][j];

            if(1.0*rand()/RAND_MAX<=CR || j == rand()%(D+1))
                u[i][j] = v[i][j];
            else
                u[i][j] = x[i][j];
        }
    }
}

void choice()
{
    for(int i = 0; i < Np; i++)
    {
        if(fun1(u[i]) < fun1(x[i]))
            for(int j = 0; j < D; j++)
            {
                x[i][j] = u[i][j];
            }
    }
    double best = 10e20;
    for(int i = 0; i < Np; i++)
    {
        if(fun1(x[i]) < best)
            best = fun1(x[i]);
    }
    printf("%d\t%g\n", ++count1, best);
}




int main()
{
    srand(time(NULL));
    init();
    for(int i = 1; i < 100; i++)
    {
        process();
        choice();
    }
    return 0;
}
