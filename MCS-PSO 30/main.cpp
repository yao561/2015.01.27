//MCS-PSO
#include <iostream>
#include <ctime>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>

//���������ؿ����������
#define point_num 32 //  �ڵ����Ŀ
#define Lambda 10
#define M 1e6
//#define M2 5000
#define Rc 0.925 //  ��Ҫ�ﵽ������ṹ���ȶ���
#define rc 0.8  //  ÿ���ߵ��ȶ��Ե��½�

//������PSO�������
#define particle_num 80 //  ���ӵ���Ŀ
#define dim 48   //  ���ӵ�ά��
#define Vmax 1
#define Vmin -1
#define Pmax 1
#define Pmin rc
#define c1 0.8
#define c2 0.8
#define w 0.7
#define iteration 100

using namespace std;

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


double p[particle_num][dim];
double p_value[particle_num];
double v[particle_num][dim];
double pbest[particle_num][dim];
double pbest_value[particle_num];
double gbest[dim];
double gbest_value;

double fitness[iteration];  //  ������¼ÿһ�ֵ������Ӧֵ

double r[dim];
int original_r[point_num][point_num];  //  ��ʾԭʼ����ͨͼ
int r2[point_num][point_num];    //  r2[i][j]��1��ʾi,j������ͨ�ģ���0��ʾ����ͨ
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

double MCS(const double *r, int simulation_replication) //  ����ÿ���ߵ��ȶ��Ժ����ؿ���ĵ�����������������ṹ���ȶ���
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

double cost_sum(const double *r)  //  ����ÿ���ߵ��ȶ��ԣ�����ܵ�cost
{
    double tot = 0;
    for(int i = 0; i < dim; i++)
        tot += cost_function(Alpha[i], Beta[i], r[i]);
    double Rr = MCS(r, M);
    if(Rr < Rc)
        tot *= pow((double)Rc/Rr, Lambda);
    return tot;
}

/*void heuristic()    //  ������ʽ����������һ������
{
    for(int i = 0; i < dim; i++)
        p[0][i] = rc;           //STEP 1    //rc���½�
    double Rr1 = MCS(p[0], M2); //STEP 2    //p[0]��ʾ�ĵ�һ������ͨ��M2�����ؿ������õ���ϵͳ�ȶ���
    while(Rr1 < Rc)
    {
        double R_temp[dim];                 //������¼STEP4.2�е�ϵͳ�ȶ���
        double Cost_temp[dim];              //��¼���������Ӧ��cost function
        for(int i = 0; i < dim; i++)
        {
            double temp1 = p[0][i];
            p[0][i] = 0.25 * (1 - p[0][i]) + p[0][i];
            R_temp[i] = MCS(p[0], M2);
            Cost_temp[i] = cost_sum(p[0]);
            p[0][i] = temp1;
        }
        int temp2 = 0;                      //������¼��Щֵ�� ����Rc�ĸ���
        for(int i = 0; i < dim; i++)
            if(R_temp[i] >= Rc)
                temp2++;
        int j;                              //STEP5����ҪѰ�ҵĵ�
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
    for(int i = 0; i < iteration; i++)  //  ���ÿһ����gbest��ȡֵ
        printf("%d\t%g\n", i, fitness[i]);

    printf("Each dimension of gbest:\n");   //  �ҵ�����õ����ӵ�ÿһά��ȡֵ
    for(int i=0; i<dim; i++)
		printf("%g ", gbest[i]);
	printf("\n");
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
    return 0;
}
