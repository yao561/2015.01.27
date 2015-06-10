#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <cmath>
//下面是蒙特卡洛相关设置
#define point_num 32 //节点的数目
#define Lambda 10
#define M 1e3
#define Rc 0.99 //想要达到的整体结构的稳定性
#define rc 0.8  //每条边的稳定性的下界
#define maxFE 1000

#define particle_num 100    //粒子的数目
#define dim 48  //粒子的维度

using namespace std;
const double Alpha[] = { 120, 120, 90, 100, 65, 100, 90, 160, 160,
120, 120, 90, 100, 65, 100, 90, 160, 160,
120, 120, 90, 100, 65, 100, 90, 160, 160,
120, 120, 90, 100, 65, 100, 90, 160, 160,
120, 120, 90, 100, 65, 100, 90, 160, 160,
120, 120, 90 };
const double Beta[] = { 14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
14.7, 14.7, 8.75, 9.9, 5.64, 9.9, 8.75, 12, 12,
14.7, 14.7, 8.75 };
double r[dim];
int original_r[point_num][point_num];  //  表示原始的连通图
int r2[point_num][point_num];    //  r2[i][j]用1表示i,j两点连通的，用0表示不连通
bool visited[point_num];


void DFS(int a)
{
	visited[a] = true;
	if (a == point_num - 1)
		return;
	for (int i = 0; i < point_num; i++)
	{
		if (visited[i] == false && r2[a][i])
			DFS(i);
	}
}
bool is_connected()
{
	memset(visited, false, sizeof(visited));
	DFS(0);
	if (visited[point_num - 1])
		return true;
	return false;
}
double MCS(const double *r, int simulation_replication) //  输入每条边的稳定性和蒙特卡洛的迭代次数，输出整个结构的稳定性
{
	int SUCCESS = 0;
	for (int i = 0; i < simulation_replication; i++)
	{
		memset(r2, 0, sizeof(r2));
		int temp = 0;
		for (int j = 0; j < point_num; j++)
			for (int k = 0; k < point_num; k++)
			{
				if (original_r[j][k])
					if (1.0 * rand() / RAND_MAX < r[temp++])
						r2[j][k] = 1;
			}

		if (is_connected())
			SUCCESS++;
	}
	return (double)SUCCESS / simulation_replication;
}
double cost_function(double a, double b, double c)
{
	return a - b*log(1 - c);
}
double cost_sum(const double *r)  //  输入每条边的稳定性，输出总的cost
{
	double tot = 0;
	for (int i = 0; i < dim; i++)
		tot += cost_function(Alpha[i], Beta[i], r[i]);
	double Rr = MCS(r, M);
	if (Rr < Rc)
		tot *= pow((double)Rc / Rr, Lambda);
	return tot;
}

#define MAXPOP 5000
#define MAXDIM 10000

//parameter definition
int maxgen, bestidx;
int randlist[MAXPOP];
double p[MAXPOP][MAXDIM], v[MAXPOP][MAXDIM], fitness[MAXPOP];
double best, bestp[MAXDIM], results[100];
double center[MAXDIM];
double phi, upper,lower;
int FEs, maxrun;

//function definition
int input();
int initialize();
int getrandlist(int randlist[], int length);
int getcenter();
int evolve(int winidx, int loseidx);
int compete(int r1, int r2);
int boundary(double a[], int temp);
int procedure();

int rnd(int low, int high)
{
	int res;
	if (low >= high)
	{
		res = low;
	}
	else
	{
		res = low + (int)(rand() / RAND_MAX *(high - low + 1));
		if (res > high)
		{
			res = high;
		}
	}
	return (res);
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


	procedure();
	return 0;
}

//main procedure
int procedure()
{
	initialize();

	while(FEs < maxFE)
	{
		int winidx, loseidx;
		//generate a random index list in preparation for pairwise competition
		getrandlist(randlist, particle_num);
		//calculate the center (mean position) of the swarm
		getcenter();
		//pairwise competition
		for(int i = 0; i < particle_num/2; i++)
		{
			int r1 = randlist[i], r2 = randlist[i + particle_num/2];
			winidx = compete(r1, r2);
			loseidx = r1 + r2 - winidx;
			evolve(winidx,loseidx);
		}
		memcpy(bestp, p[bestidx], sizeof(bestp));
		FEs++;
		printf("FEs = %d: %f\n", FEs, best);
	}
	//output result
	printf("Best result: %.6e\n", best);
	return 0;
}




int initialize()
{
	//boundary setting
	lower = rc; upper = 1;
	//phi setting (the only parameter in CSO, please SET PROPERLY)
	phi = 0;
	//population size setting

	for(int i = 0; i < particle_num; i++)
		for(int j = 0; j < dim; j++)
			{
				p[i][j] = lower + (upper - lower) * 1.0 * rand() / RAND_MAX;
				v[i][j] = 0;
			}
	best = 1e20;


	FEs = 0;
	for(int i = 0; i < particle_num; i++){
		fitness[i] = cost_sum(p[i]);
			if(fitness[i]  < best)
            {
				best = fitness[i] ;
				bestidx = i;
			}
	}
	memcpy(bestp, p[bestidx], sizeof(bestp));


	return 0;
}

//loser learn from winner
int evolve(int winidx, int loseidx){
	//position and velocity update
	double c1, c2, c3;
	for(int j = 0; j < dim; j++){
		c1 = 1.0 * rand() / RAND_MAX;
		c2 = 1.0 * rand() / RAND_MAX;
		c3 = 1.0 * rand() / RAND_MAX;
		v[loseidx][j] = c1*v[loseidx][j]
										+ c2*( p[winidx][j] - p[loseidx][j])
										+ c3*phi*(center[j] - p[loseidx][j]);
		p[loseidx][j] += v[loseidx][j];
	}
	//boundary control
	boundary(p[loseidx], dim);
	//fitness evaluation
	fitness[loseidx] = cost_sum(p[loseidx]);
	if(fitness[loseidx]  < best){
		best = fitness[loseidx] ;
		bestidx = loseidx;
	}
	return 0;
}


//calculate the population center
int getcenter(){
	for(int j = 0; j < dim; j++){
		center[j] = 0;
		for(int i = 0; i < particle_num; i++){
			center[j] += p[i][j];
		}
		center[j] /= particle_num;
	}
	return 0;
}


//generat a random index list ranging from 1 to length
int getrandlist(int randlist[], int length){
	int end1,k;
	int templist[MAXPOP];
		end1 = length - 1;
		for(int i = 0; i < length; i++){
			templist[i] = i;
		}
		for(int i = 0; i < length; i++){
			k = rnd(0,end1);
			randlist[i] = templist[k];
			templist[k] = templist[end1];
			end1--;
		}
	return 0;
}


// compete two selected particles
int compete(int r1, int r2){
	if(fitness[r1] < fitness[r2])
		return r1;
	return r2;
}


int boundary(double a[], int temp){
	for(int i = 0; i < temp; i++){
		if(a[i] < lower)
			a[i] = lower;
		if(a[i] > upper)
			a[i] = upper;
	}
	return 0;
}
