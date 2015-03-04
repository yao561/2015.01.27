#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>

#define n 4
#define M 1000//���ؿ��巽���ĵ�������


int x[n];//������¼ĳ���Ƿ���ͨ
double r[n][n];//������¼��֮���


using namespace std;

void dfs(int ok)
{
    for(int i = 0; i < n; i++)
    {

        if(r[ok][i]==1)
        {
            if(x[i]==1)
                return;
            else
                x[i] = 1;
            if(i==n-1)
                return;
            else
                dfs(i);
        }
    }
}




int main()
{
    srand(time(NULL));
    int s = 0, t = 3,
        k = 0, SUCCESS = 0;
    while(k++ < M)
    {
        for(int i = 0; i < n; i++)  for(int j = 0; j < n; j++)  r[i][j] = 0;
        if(1.0 * rand()/RAND_MAX < 0.91)r[s][1] = 1;
        if(1.0 * rand()/RAND_MAX < 0.85) r[s][2] = 1;
        if(1.0 * rand()/RAND_MAX < 0.9) r[1][2] = 1;
        if(1.0 * rand()/RAND_MAX < 0.89) r[2][1] = 1;
        if(1.0 * rand()/RAND_MAX < 0.88) r[1][t] = 1;
        if(1.0 * rand()/RAND_MAX < 0.9) r[2][t] = 1;
        memset(x, 0, sizeof(x));    x[0] = 1;
        dfs(0);
        if(x[n-1]==1)   SUCCESS++;
    }
    printf("%.9f",(double)1.0*SUCCESS/M);
    return 0;
}
