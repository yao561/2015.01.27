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
#define interation 1000

using namespace std;


double x[Np][D];
double v[Np][D];
double u[Np][D];

int cur_interation;

double fun1(double * Vect)
{
	double dat = 0.0;
	for(int i = 0; i < D; i++)
		dat += Vect[i] * Vect[i];
	return dat;
}


void init()
{
    cur_interation = 0;
    for(int i = 0; i < Np; i++)
        for(int j = 0; j < D; j++)
            x[i][j] = xmin + (xmax - xmin) * 1.0 * rand() / RAND_MAX;
    double best = 1e20;
    for(int i = 0; i < Np; i++)
    {
        if(fun1(x[i]) < best)
            best = fun1(x[i]);
    }
    printf("%d\t%g\n", ++cur_interation, best);

}

void main_loop()
{
    for(int i = 0; i < Np; i++)
    {
        int r1, r2, r3;
        do
        {
        r1 = rand() * Np / RAND_MAX;
        } while(r1==i);
        do
        {
        r2 = rand() * Np / RAND_MAX;
        } while( r2==i || r2==r1);
        do
        {
        r3 = rand() * Np / RAND_MAX;
        } while( r3==i || r3==r1 || r3==r2 );

        int J = rand() * D / RAND_MAX;
        for(int j = 0; j < D; j++)
        {
            if( 1.0*rand()/RAND_MAX<=CR || j==J )
            {
                u[i][j] = x[r1][j] + F * (x[r2][j] - x[r3][j]);
                if(u[i][j] > xmax)
                    u[i][j] = xmax;
                else if(u[i][j] < xmin)
                    u[i][j] = xmin;
            }
            else
                u[i][j] = x[i][j];
            J = (J + 1) % D;
        }



        double score = fun1(u[i]);
        if(score <= fun1(x[i]))
            for(int j = 0; j <D; j++)
                x[i][j] = u[i][j];
    }
    double best = 10e20;
    for(int i = 0; i < Np; i++)
    {
        if(fun1(x[i]) < best)
            best = fun1(x[i]);
    }
    printf("%d\t%g\n", ++cur_interation, best);
}

int main()
{
    srand(time(NULL));
    init();
    for(int i = 1; i < interation; i++)
    {
        main_loop();
    }
    return 0;
}
