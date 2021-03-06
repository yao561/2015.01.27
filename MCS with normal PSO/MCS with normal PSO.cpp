//MCS-PSO
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>

//下面是蒙特卡洛相关设置
#define point_num 6 //  节点的数目
#define Lambda 10
#define M 1e6
#define Rc 0.975 //  想要达到的整体结构的稳定性
#define rc 0.8  //  每条边的稳定性的下界

//下面是PSO相关设置
#define particle_num 15 //  粒子的数目
#define dim 9   //  粒子的维度
#define Vmax 1
#define Vmin -1
#define Pmax 1
#define Pmin rc
#define c1 0.8
#define c2 0.8
#define w 0.7
#define iteration 100

using namespace std;

const double Alpha[] = {120, 120, 90, 100, 65, 100, 90, 160, 160};
const double Beta[] = {14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12};

double p[particle_num][dim];
double p_value[particle_num];
double v[particle_num][dim];
double pbest[particle_num][dim];
double pbest_value[particle_num];
double gbest[dim];
double gbest_value;

double fitness[iteration];  //  用来记录每一轮的最佳适应值

double r[dim];
int r2[point_num][point_num];    //  r2[i][j]表示的是i与j两点之间的距离
bool visited[point_num];
int count1 = 0;

void DFS(int a)
{
    visited[a] = true;
    if(a == point_num-1)
        return;
    for(int i = 0; i < point_num; i++)
    {
        if(visited[i] == false && r2[a][i])
            DFS(i);
    }
}

bool is_connected()
{
    memset(visited, false, sizeof(visited));
    DFS(0);
    if(visited[point_num-1])
        return true;
    return false;
}

double MCS(const double *r, int simulation_replication) //  输入每条边的稳定性和蒙特卡洛的迭代次数，输出整个结构的稳定性
{
    int SUCCESS = 0;
    for(int i = 0; i < simulation_replication; i++)
    {
        for(int j = 0; j < point_num; j++)
            for(int k = 0; k < point_num; k++)
                r2[j][k] = 0;
        if(1.0*rand()/RAND_MAX < r[0])
            r2[0][1] = 1;
        if(1.0*rand()/RAND_MAX < r[1])
            r2[0][3] = 1;
        if(1.0*rand()/RAND_MAX < r[2])
            r2[1][3] = 1;
        if(1.0*rand()/RAND_MAX < r[2])
            r2[3][1] = 1;
        if(1.0*rand()/RAND_MAX < r[3])
            r2[1][2] = 1;
        if(1.0*rand()/RAND_MAX < r[3])
            r2[2][1] = 1;
        if(1.0*rand()/RAND_MAX < r[4])
            r2[2][3] = 1;
        if(1.0*rand()/RAND_MAX < r[4])
            r2[3][2] = 1;
        if(1.0*rand()/RAND_MAX < r[5])
            r2[3][4] = 1;
        if(1.0*rand()/RAND_MAX < r[5])
            r2[4][3] = 1;
        if(1.0*rand()/RAND_MAX < r[6])
            r2[2][4] = 1;
        if(1.0*rand()/RAND_MAX < r[6])
            r2[4][2] = 1;
        if(1.0*rand()/RAND_MAX < r[7])
            r2[2][5] = 1;
        if(1.0*rand()/RAND_MAX < r[8])
            r2[4][5] = 1;

        if(is_connected())
            SUCCESS++;
    }
    return (double)SUCCESS/simulation_replication;
}

double cost_function(double a, double b, double c)
{
    return a-b*log(1-c);
}

double cost_sum(const double *r)  //  输入每条边的稳定性，输出总的cost
{
    double tot = 0;
    for(int i = 0; i < dim; i++)
        tot += cost_function(Alpha[i], Beta[i], r[i]);
    double Rr = MCS(r, M);
    if(Rr < Rc)
        tot *= pow((double)Rc/Rr, Lambda);
    return tot;
}

void init()
{
    for(int i = 0; i < particle_num; i++)
    {
        for(int j = 0; j < dim; j++)
		{
			p[i][j] = Pmin + (Pmax - Pmin) * 1.0 * rand() / RAND_MAX;
			pbest[i][j] = p[i][j];
			v[i][j] = Vmin + (Vmax - Vmin) * 1.0 * rand() / RAND_MAX;
		}
		p_value[i] = cost_sum(p[i]);
		pbest_value[i] = p_value[i];
    }
    gbest_value = INFINITY;
	for(int i = 0; i < particle_num; i++)
	{
		if(gbest_value > pbest_value[i])
		{
			gbest_value = pbest_value[i];
			for(int j = 0; j < dim; j++)
				gbest[j] = pbest[i][j];
		}
	}
	fitness[count1++] = gbest_value;
}

void renew_partical()
{
	for(int i = 0; i < particle_num; i++)
	{
		for(int j = 0; j < dim; j++)
		{
			v[i][j] = w*v[i][j]+c1*1.0*rand()/RAND_MAX*(pbest[i][j]-p[i][j])+c2*1.0*rand()/RAND_MAX*(gbest[j]-p[i][j]);
			if(v[i][j] > Vmax)
				v[i][j] = Vmax;
			else if(v[i][j] < Vmin)
				v[i][j] = Vmin;

			p[i][j] += v[i][j];
			if(p[i][j] > Pmax)
				p[i][j] = Pmax;
			else if(p[i][j] < Pmin)
				p[i][j] = Pmin;
		}
		p_value[i] = cost_sum(p[i]);
	}
}

void renew_fitness()
{
	for(int i = 0; i < particle_num; i++)
	{
		if(pbest_value[i] > p_value[i])
        {
			pbest_value[i] = p_value[i];
			for(int j = 0; j < dim; j++)
				pbest[i][j]=p[i][j];
		}
        if(gbest_value > pbest_value[i])
        {
            gbest_value = pbest_value[i];
            for(int j = 0; j < dim; j++)
               gbest[j]=pbest[i][j];
        }
	}
	fitness[count1++] = gbest_value;
}

void output()
{
    for(int i = 0; i < iteration; i++)  //  输出每一代的gbest的取值
        printf("%d\t%g\n", i, fitness[i]);

    printf("Each dimension of gbest:\n");   //  找到的最好的粒子的每一维的取值
    for(int i=0; i<dim; i++)
		printf("%g ", gbest[i]);
	printf("\n");
}


int main()
{
    srand(time(NULL));
    init();
    for(int i = 1; i < iteration; i++)
    {
        renew_partical();
        renew_fitness();
    }
    output();
    return 0;
}
