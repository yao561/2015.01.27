#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>

using namespace std;

int main()
{
    int count1 = 0;
    int original_r[150][150];
    ifstream in;
    in.open("j1201_1.txt");
    for (int i = 0; i < 122; i++)
    {
        for (int j = 0; j < 122; j++)
        {
            in >> original_r[i][j];
            if(original_r[i][j])
                count1++;
        }
    }
    cout << count1;
    return 0;
}
