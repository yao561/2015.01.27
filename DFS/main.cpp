#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>

#define point_num 6
int r2[point_num*point_num];    //  r2[i*point_num+j]表示的是i与j两点之间的距离
bool visited[point_num];
using namespace std;

void DFS(int a)
{
    visited[a] = true;
    if(a == point_num-1)
        return;
    for(int i = 0; i < point_num; i++)
    {
        if(visited[i] == false && r2[a*point_num+i])
            DFS(i);
    }
}


int main()
{
    memset(r2, 0, sizeof(r2));
    memset(visited, false, sizeof(visited));

    r2[0*point_num+1] = 1;
    r2[0*point_num+3] = 1;
    r2[1*point_num+3] = r2[3*point_num+1] = 1;
    r2[1*point_num+2] = r2[2*point_num+1] = 1;
    r2[2*point_num+3] = r2[3*point_num+2] = 1;
    r2[4*point_num+3] = r2[3*point_num+4] = 1;
    r2[2*point_num+4] = r2[4*point_num+2] = 1;
    r2[2*point_num+5] = 1;
    r2[4*point_num+5] = 1;
    DFS(0);

    if(visited[point_num-1])
        cout << "Hello world!" << endl;
    return 0;
}
