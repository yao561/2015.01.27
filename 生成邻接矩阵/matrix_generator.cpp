/* 生成网络的邻接矩阵。*/
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

int matrix[1001][1001];

int main() {
	int network_size;
	cout << "请输入节点个数：";
	cin >> network_size;
	cout << "请输入网络数据：" << endl;
	memset(matrix, sizeof(matrix), 0);
	for (int cc = 0; cc < network_size; cc++) {
		int nodeNum, temp, successors_size;
		cin >> nodeNum >> temp >> successors_size;
		for (int i = 0; i < successors_size; i++) {
			int childNum;
			cin >> childNum;
			matrix[nodeNum][childNum] = 1;
		}
	}
	ofstream out;

	//这里改输出文件的名字。
	out.open("j301_1.txt", ofstream::out);
	for (int i = 1; i <= network_size; i++) {
		for (int j = 1; j < network_size; j++) {
			out << matrix[i][j] << " ";
		}
		out << matrix[i][network_size] << endl;
	}
	out.close();

	return 0;
}
