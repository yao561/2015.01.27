#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>

//下面是蒙特卡洛相关设置
#define point_num 32 //  节点的数目
#define Lambda 10
#define M 1e3
#define Rc 0.99 //  想要达到的整体结构的稳定性
#define rc 0.8  //  每条边的稳定性的下界

//下面是DE相关设置
#define particle_num 80 //  粒子的数目
#define dim 48 //粒子的维度
#define xmax 1
#define xmin rc
#define CR 0.7
#define F 0.5
#define iteration 100

using namespace std;
ofstream out;

const double Alpha[] = {120, 120, 90, 100, 65, 100, 90, 160, 160,
                        120, 120, 90, 100, 65, 100, 90, 160, 160,
                        120, 120, 90, 100, 65, 100, 90, 160, 160,
                        120, 120, 90, 100, 65, 100, 90, 160, 160,
                        120, 120, 90, 100, 65, 100, 90, 160, 160,
                        120, 120, 90};
const double Beta[] =  {14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
                        14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
                        14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
                        14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
                        14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
                        14.7, 14.7, 8.75};

double x[particle_num][dim];
double v[particle_num][dim];
double u[particle_num][dim];
double gbest[dim];
double x_fitness[particle_num]; //每个粒子当前的适应值，记录以避免重复计算

double r[dim];
int original_r[point_num][point_num];  //  表示原始的连通图
int r2[point_num][point_num];    //  r2[i][j]用1表示i,j两点连通的，用0表示不连通
bool visited[point_num];

int cur_interation;
double gbest_value;
double KEEP[30];

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
        memset(r2, 0, sizeof(r2));
        int temp = 0;
        for(int j = 0; j < point_num; j++)
            for(int k = 0; k < point_num; k++)
            {
                if(original_r[j][k])
                    if(1.0*rand()/RAND_MAX < r[temp++])
                        r2[j][k] = 1;
            }

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
    cur_interation = 0;
    for(int i = 0; i < particle_num; i++)
        for(int j = 0; j < dim; j++)
            x[i][j] = xmin + (xmax - xmin) * 1.0 * rand() / RAND_MAX;
    gbest_value = INFINITY;
    for(int i = 0; i < particle_num; i++)
    {
        x_fitness[i] = cost_sum(x[i]);
        if(x_fitness[i] < gbest_value)
        {
            gbest_value = x_fitness[i];
            for(int j = 0; j < dim; j++)
                gbest[j] = x[i][j];
        }
    }
    printf("%d\t%g\n", ++cur_interation, gbest_value);
    out << cur_interation << "\t" << gbest_value << endl;
}
void main_loop()
{
    for(int i = 0; i < particle_num; i++)
    {
        int r1, r2, r3;
        do
        {
        r1 = rand() * particle_num / RAND_MAX;
        } while(r1==i);
        do
        {
        r2 = rand() * particle_num / RAND_MAX;
        } while( r2==i || r2==r1);
        do
        {
        r3 = rand() * particle_num / RAND_MAX;
        } while( r3==i || r3==r1 || r3==r2 );

        int J = rand() * dim / RAND_MAX;
        for(int j = 0; j < dim; j++)
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
            J = (J + 1) % dim;
        }



        double score = cost_sum(u[i]);
        if(score <= x_fitness[i])
        {
            for(int j = 0; j < dim; j++)
            {
                x[i][j] = u[i][j];
            }
            x_fitness[i] = cost_sum(x[i]);
        }
    }
    for(int i = 0; i < particle_num; i++)
    {
        if(x_fitness[i] < gbest_value)
        {
            gbest_value = x_fitness[i];
            for(int j = 0; j < dim; j++)
                gbest[j] = x[i][j];
        }

    }
    printf("%d\t%g\n", ++cur_interation, gbest_value);
    out << cur_interation << "\t" << gbest_value << endl;
}

void output()
{
    printf("Each dimension of gbest:\n");   //  找到的最好的粒子的每一维的取值
    for(int i=0; i<dim; i++)
		printf("%g ", gbest[i]);

	printf("\ngbest_value = %f\n", gbest_value);
}



int main()
{
    srand(time(NULL));

    ifstream in;
    in.open("j301_1.txt");
    for (int i = 0; i < point_num; i++)
    {
        for (int j = 0; j < point_num; j++)
        {
            in >> original_r[i][j];
        }
    }

    out.open("output0.99.txt");
    out << Rc << endl;
    for(int xunhuan = 0; xunhuan < 30; xunhuan++)
    {
        init();
        for(int i = 1; i < iteration; i++)
        {
            main_loop();
        }
        output();
        KEEP[xunhuan] = gbest_value;
        out << gbest_value << endl << endl << endl;
    }
    for(int i = 0; i < 30; i++)
        out << KEEP[i] << endl;
    out.close();
    return 0;
}
