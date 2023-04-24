#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

using namespace std;

typedef struct reducer_arg {
	unordered_map<int, vector<vector<int>>*> *toReduce;
    pthread_barrier_t *barrier;
	int id;
} reducer_arg;


void *thread_function_reducer(void *arg);
reducer_arg create_reducer_arg(unordered_map<int, vector<vector<int>>*> *toReduce,
    int id, pthread_barrier_t *barrier);
