#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <set>

#define particle_num 20
#define dim 30
#define xmax 100
#define xmin -100
#define vmax 20
#define vmin -20
#define iteration 1000
#define Phi 0.3

using namespace std;

double x[particle_num][dim];
double v[particle_num][dim];
double x_value[particle_num];
double x_mean[dim];
int paired[particle_num];//�������������ÿ������Ϊ��һ��
double fitness[iteration];
int count1 = 0;

double fun1(double * Vect)
{
	double dat = 0.0;
	for(int i = 0; i < dim; i++)
		dat += Vect[i] * Vect[i];
	return dat;
}

void calculate()//��������´λ��õ�������
{
    double best = INFINITY;
    for(int i = 0; i < particle_num; i++)
    {
        x_value[i] = fun1(x[i]);
        if(x_value[i] < best)
            best = x_value[i];
    }
    fitness[count1++] = best;
    for(int j = 0; j < dim; j++)
    {
        x_mean[j] = 0;
        for(int i = 0; i < particle_num; i++)
        {
            x_mean[j] += 1.0 * x[i][j] / particle_num;
        }
    }
}

void init()
{
    for(int i = 0; i < particle_num; i++)
    {
        for(int j = 0; j < dim; j++)
        {
            x[i][j] = xmin + (xmax - xmin) * 1.0 * rand() / RAND_MAX;
            v[i][j] = vmin + (vmax - vmin) * 1.0 * rand() / RAND_MAX;
        }
    }
    calculate();
}

void pairpair()//���������������Ȼ��ŵ�paired[]��������
{
    set<int> myset;
    set<int>::iterator it;
    for(int i = 0; i < particle_num; i++)
        myset.insert(i);
    for(int i = 0; i < particle_num; i++)
    {
        it = myset.begin();
        int temp = rand() % myset.size();
        for(int k = 0; k < temp; k++)
            ++it;
        paired[i] = *it;
        myset.erase(it);
    }
}

void update()
{
    for(int i = 0; i < particle_num-1; i += 2)
    {
        int winer, loser;
        if(x_value[paired[i]] < x_value[paired[i+1]])
        {
            winer = paired[i];//��winer����
            loser = paired[i+1];//��loser����
        }
        else
        {
            winer = paired[i+1];//��winer����
            loser = paired[i];//��loser����
        }
        double r1 = 1.0 * rand() / RAND_MAX;
        double r2 = 1.0 * rand() / RAND_MAX;
        double r3 = 1.0 * rand() / RAND_MAX;
        for(int j = 0 ; j < dim; j++)
        {
            v[loser][j] = r1 * v[loser][j]
                        + r2 * (x[winer][j] - x[loser][j])
                        + Phi * r3 * (x_mean[j] - x[loser][j]);
            v[loser][j] = v[loser][j] > vmax ? vmax : v[loser][j];
            v[loser][j] = v[loser][j] < vmin ? vmin : v[loser][j];
            x[loser][j] += v[loser][j];
            x[loser][j] = x[loser][j] > xmax ? xmax : x[loser][j];
            x[loser][j] = x[loser][j] < xmin ? xmin : x[loser][j];
        }
    }
    calculate();
}

void output()
{
    for(int i = 0; i < iteration; i++)
    {
        cout << i << ' ' << fitness[i] << endl;
    }
    /*for(int i = particle_num-3; i < particle_num; i++)
    {
        cout << endl;
        for(int j = 0; j < dim; j++)
            cout << ' ' << x[i][j];
    }*/
}

int main()
{
    srand(time(NULL));
    init();
    for(int i = 1; i < iteration; i++)
    {
        pairpair();
        update();
    }
    output();
    return 0;
}
