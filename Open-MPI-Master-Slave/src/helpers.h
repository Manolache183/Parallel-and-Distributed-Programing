#include "mpi.h"
#include <fstream>
#include <iostream>
#include <vector>

#define ROOT 0
#define MAIN_CLUSTER_SIZE 4

using namespace std;

void receive_topology(int &sourceRank, vector<vector<int>> &data, int start, int end);
void send_topology(int sourceRank, int destRank, vector<vector<int>> &data, int start, int end);
void send_vector(int sourceRank, int destRank, vector<long> &V, int start, int size);
void receive_vector(int sourceRank, vector<long> &V);
void classic_cluster_member_work(int rank, int right, vector<vector<int>> data, vector<long> &V, int last);
int min(int a, int b);