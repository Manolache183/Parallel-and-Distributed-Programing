#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <climits>

using namespace std;

typedef struct mapper_arg {
	unordered_set<string> *remainingFiles;
	unordered_map<int, vector<vector<int>>*> *toReduce;
	pthread_mutex_t *mutex;
	pthread_barrier_t *barrier;
	int R;
	int id;
} mapper_arg;

void working_function_mapper(ifstream &dataIn, mapper_arg *args);

void *thread_function_mapper(void *arg);

mapper_arg create_mapper_arg(unordered_set<string> *remainingFiles,
	unordered_map<int, vector<vector<int>>*> *toReduce,
	pthread_mutex_t *mutex, pthread_barrier_t *barrier,
	int id, int R);
