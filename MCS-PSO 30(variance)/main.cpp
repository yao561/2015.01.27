//MCS-PSO
#include <iostream>
#include <ctime>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>
#include "var.h"

//下面是蒙特卡洛相关设置
#define point_num 32 //  节点的数目
#define Lambda 10
#define M 1e3
//#define M2 5000
#define Rc 0.95 //  想要达到的整体结构的稳定性
#define rc 0.8  //  每条边的稳定性的下界

//下面是PSO相关设置
#define particle_num 80 //  粒子的数目
#define dim 48   //  粒子的维度
#define Vmax 1
#define Vmin -1
#define Pmax 1
#define Pmin rc
#define c1 0.8
#define c2 0.8
#define w 0.7
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


//下面是PSO相关的数组
double p[particle_num][dim];
double p_value[particle_num];
double v[particle_num][dim];
double pbest[particle_num][dim];
double pbest_value[particle_num];
double gbest[dim];
double gbest_value;
double fitness[iteration];  //  用来记录每一轮的最佳适应值


//下面是用来输出数据然后画图的
double MCS_value[iteration][particle_num];              //每个粒子用MCS方法算出来的稳定性估计值
double fitness_value[iteration][particle_num];          //记录每一代每一个粒子算出来的cost值（加上惩罚函数之后）
double original_fitness_value[iteration][particle_num]; //记录每一代每一个粒子原始cost值
double MCS_VAR[iteration];                              //记录每一轮MCS的标准差
double fitness_var[iteration];                          //记录每一轮fitness的标准差
double original_fitness_var[iteration];                 //记录每一轮原始fitness的标准差

//下面是蒙特卡洛判断的相关设置
double r[dim];
int original_r[point_num][point_num];  //  表示原始的连通图
int r2[point_num][point_num];    //  r2[i][j]用1表示i,j两点连通的，用0表示不连通
bool visited[point_num];

int cur_iteration;
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
double cost_sum(const double *r, int idx)  //  输入每条边的稳定性，输出总的cost
{
    double tot = 0;
    for(int i = 0; i < dim; i++)
        tot += cost_function(Alpha[i], Beta[i], r[i]);
    original_fitness_value[cur_iteration][idx] = tot;
    MCS_value[cur_iteration][idx] = MCS(r, M);
    if(MCS_value[cur_iteration][idx] < Rc)
        tot *= pow((double)Rc/MCS_value[cur_iteration][idx], Lambda);
    fitness_value[cur_iteration][idx] = tot;
    return tot;
}
/*void heuristic()    //  用启发式方法产生第一个粒子
{
    for(int i = 0; i < dim; i++)
        p[0][i] = rc;           //STEP 1    //rc是下界
    double Rr1 = MCS(p[0], M2); //STEP 2    //p[0]表示的第一个粒子通过M2次蒙特卡洛计算得到的系统稳定性
    while(Rr1 < Rc)
    {
        double R_temp[dim];                 //用来记录STEP4.2中的系统稳定性
        double Cost_temp[dim];              //记录与上面相对应的cost function
        for(int i = 0; i < dim; i++)
        {
            double temp1 = p[0][i];
            p[0][i] = 0.25 * (1 - p[0][i]) + p[0][i];
            R_temp[i] = MCS(p[0], M2);
            Cost_temp[i] = cost_sum(p[0]);
            p[0][i] = temp1;
        }
        int temp2 = 0;                      //用来记录这些值中 大于Rc的个数
        for(int i = 0; i < dim; i++)
            if(R_temp[i] >= Rc)
                temp2++;
        int j;                              //STEP5中想要寻找的点
        if(!temp2)                          //STEP5.1
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
        else                                 //STEP5.2
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
        double temp4 = p[0][j];                //STEP5.3
        for(int i = 0; i < dim; i++)
            p[0][i] = 0.25 * (1 - temp4) + temp4;
        Rr1 = MCS(p[0], M2);
    }

    for(int i = 0; i < dim; i++)
    {
        pbest[0][i] = p[0][i];
        v[0][i] = Vmin + (Vmax - Vmin) * 1.0 * rand() / RAND_MAX;
    }
    p_value[0] = cost_sum(p[0]);
    pbest_value[0] = p_value[0];
}*/

void init()
{
    //heuristic();
    cur_iteration = 0;
    for(int i = 0; i < particle_num; i++)
    {
        for(int j = 0; j < dim; j++)
		{

			p[i][j] = Pmin + (Pmax - Pmin) * 1.0 * rand() / RAND_MAX;
			pbest[i][j] = p[i][j];
			v[i][j] = Vmin + (Vmax - Vmin) * 1.0 * rand() / RAND_MAX;
		}
		p_value[i] = cost_sum(p[i], i);
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
	fitness[cur_iteration++] = gbest_value;
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
		p_value[i] = cost_sum(p[i], i);
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
	fitness[cur_iteration++] = gbest_value;
}

void output()
{
    for(int i = 0; i < iteration; i++)  //  输出每一代的gbest的取值
        printf("%d\t%g\n", i, fitness[i]);

    printf("Each dimension of gbest:\n");   //  找到的最好的粒子的每一维的取值
    for(int i=0; i<dim; i++)
		printf("%g ", gbest[i]);
	printf("\n");

	//输出每一代的各种值的情况
    out.open("MCS.txt");
    out << "iteration" << "\t" << "particle_num" << "\t" << "MCS_value" << "\t" << "fitness_value" << "\t" << "original_fitness_value" << endl;
    for(int i = 0; i < iteration; i++)
    {
        for(int j = 0; j < particle_num; j++)
            out << i << "\t" << j << "\t" << MCS_value[i][j] << "\t" << fitness_value[i][j] << "\t" << original_fitness_value[i][j] << endl;
    }
    out.close();
}

void outvar()
{
    for(int i = 0; i < iteration; i++)
    {
        MCS_VAR[i] = cal_var(MCS_value[i], particle_num);
        fitness_var[i] = cal_var(fitness_value[i], particle_num);
        original_fitness_var[i] = cal_var(original_fitness_value[i], particle_num);
    }

    //输出各种值的标准差的情况
    out.open("var.txt");
    out << "i" << "\t" << "MCS_VAR" << "\t" << "fitness_var" << "\t" << "original_fitness_var" << endl;
    for(int i = 0; i < iteration; i++)
    {
        out << i << "\t" << MCS_VAR[i] << "\t" << fitness_var[i] << "\t" << original_fitness_var[i] << endl;
    }
    out.close();
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

    init();
    for(int i = 1; i < iteration; i++)
    {
        renew_partical();
        renew_fitness();
    }
    output();
    outvar();
    return 0;
}
