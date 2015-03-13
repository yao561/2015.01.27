//#include "stdafx.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>

#define bianshu 9
#define point_num 6
#define M 100000//蒙特卡洛方法的迭代次数

int r[point_num][point_num];
bool visited[point_num];//用来记录某点是否连通
double a[bianshu];

using namespace std;

void DFS(int a)
{
	visited[a] = true;
	if (a == point_num - 1)
		return;
	for (int i = 0; i < point_num; i++)
	{
		if (visited[i] == false && r[a][i])
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


int main()
{
	srand(time(NULL));
	int s = 0, t = 5,
		k = 0, SUCCESS = 0;
	for (int i = 0; i < bianshu; i++)
		cin >> a[i];
	while (k++ < M)
	{
		for (int i = 0; i < point_num; i++)
			for (int j = 0; j < point_num; j++)
				r[i][j] = 0;
		if (1.0 * rand() / RAND_MAX < a[0])
			r[s][1] = 1;
		if (1.0 * rand() / RAND_MAX < a[1])
			r[s][3] = 1;
		if (1.0 * rand() / RAND_MAX < a[2])
			r[1][3] = 1;
		if (1.0 * rand() / RAND_MAX < a[2])
			r[3][1] = 1;
		if (1.0 * rand() / RAND_MAX < a[3])
			r[1][2] = 1;
        if (1.0 * rand() / RAND_MAX < a[3])
			r[2][1] = 1;
		if (1.0 * rand() / RAND_MAX < a[4])
			r[2][3] = 1;
        if (1.0 * rand() / RAND_MAX < a[4])
			r[3][2] = 1;
		if (1.0 * rand() / RAND_MAX < a[5])
			r[3][4] = 1;
        if (1.0 * rand() / RAND_MAX < a[5])
			r[4][3] = 1;
		if (1.0 * rand() / RAND_MAX < a[6])
			r[2][4] = 1;
        if (1.0 * rand() / RAND_MAX < a[6])
			r[4][2] = 1;
		if (1.0 * rand() / RAND_MAX < a[7])
			r[2][t] = 1;
		if (1.0 * rand() / RAND_MAX < a[8])
			r[4][t] = 1;

		if (is_connected())
			SUCCESS++;
	}
	printf("%f\n", (double)1.0*SUCCESS / M);

	return 0;
}
