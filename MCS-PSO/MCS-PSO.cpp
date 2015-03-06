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
#define M 1000
#define Rc 0.95 //  想要达到的整体结构的稳定性
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
int r2[point_num*point_num];    //  r2[i*point_num+j]表示的是i与j两点之间的距离

bool Floyd()    //  用Floyd方法判断r2这个数组代表的结构是否连通
{
    for(int k = 0; k < point_num; k++)
        for(int i = 0; i < point_num; i++)
            for(int j = 0; j < point_num; j++)
                if(r2[i*point_num+j]>r2[i*point_num+k]+r2[k*point_num+j])
                {
                    r2[i*point_num+j]=r2[i*point_num+k]+r2[k*point_num+j];
                    if(i==0&&j==5)
                        return true;
                }
    //if(r2[0*point_num+point_num-1]<1000)
        //return true;
    return false;
}

double MCS(const double *r, int simulation_replication) //  输入每条边的稳定性和蒙特卡洛的迭代次数，输出整个结构的稳定性
{
    int SUCCESS = 0;
    for(int i = 0; i < simulation_replication; i++)
    {
        for(int j = 0; j < point_num*point_num; j++)
            r2[j] = 1000;
        if(1.0*rand()/RAND_MAX < r[0])
            r2[0*point_num+1] = 1;
        if(1.0*rand()/RAND_MAX < r[1])
            r2[0*point_num+3] = 1;
        if(1.0*rand()/RAND_MAX < r[2])
            r2[1*point_num+3] = r2[3*point_num+1] = 1;
        if(1.0*rand()/RAND_MAX < r[3])
            r2[1*point_num+2] = r2[2*point_num+1] = 1;
        if(1.0*rand()/RAND_MAX < r[4])
            r2[2*point_num+3] = r2[3*point_num+2] = 1;
        if(1.0*rand()/RAND_MAX < r[5])
            r2[4*point_num+3] = r2[3*point_num+4] = 1;
        if(1.0*rand()/RAND_MAX < r[6])
            r2[2*point_num+4] = r2[4*point_num+2] = 1;
        if(1.0*rand()/RAND_MAX < r[7])
            r2[2*point_num+5] = 1;
        if(1.0*rand()/RAND_MAX < r[8])
            r2[4*point_num+5] = 1;

        if(Floyd())
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

void heuristic()    //  用启发式方法产生第一个粒子
{
    for(int i = 0; i < dim; i++)
        p[0][i] = rc;
    double Rr1 = MCS(p[0], 5000);
    while(Rr1 < Rc)
    {
        double R_temp[dim];
        double Cost_temp[dim];
        for(int i = 0; i < dim; i++)
        {
            double temp1 = p[0][i];
            p[0][i] = 0.25 * (1 - p[0][i]) + p[0][i];
            R_temp[i] = MCS(p[0], 5000);
            Cost_temp[i] = cost_sum(p[0]);
            p[0][i] = temp1;
        }
        int temp2 = 0;  //  用来记录这些值中大于Rc的个数
        for(int i = 0; i < dim; i++)
            if(R_temp[i] >= Rc)
                temp2++;
        int j;  //  STEP5中想要寻找的点
        if(!temp2)  //  STEP5.1
        {
            double temp3 = 0;
            for(int i = 0; i < dim; i++)
            {
                if(temp3 < R_temp[i])
                {
                    temp3 = R_temp[i];
                    j = i;
                }
            }
        }
        else    //  STEP5.2
        {
            double temp3 = INFINITY;
            for(int i = 0; i < dim; i++)
            {
                if(temp3 > Cost_temp[i] && R_temp[i] >= Rc)
                {
                    temp3 = Cost_temp[i];
                    j = i;
                }
            }
        }
        double temp4 = p[0][j]; //  STEP5.3
        for(int i = 0; i < dim; i++)
            p[0][i] = 0.25 * (1 - temp4) + temp4;
        Rr1 = MCS(p[0], 5000);
    }

    for(int i = 0; i < dim; i++)
    {
        pbest[0][i] = p[0][i];
        v[0][i] = Vmin + (Vmax - Vmin) * 1.0 * rand() / RAND_MAX;
    }
    p_value[0] = cost_sum(p[0]);
    pbest_value[0] = p_value[0];
}

void init()
{
    heuristic();
    for(int i = 1; i < particle_num; i++)
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
	fitness[0] = gbest_value;
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
			if(gbest_value > pbest_value[i])
			{
				gbest_value = pbest_value[i];
				for(int j = 0; j < dim; j++)
					gbest[j]=pbest[i][j];
			}
		}
	}
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
		fitness[i]=gbest_value;
    }
    output();
    return 0;
}
