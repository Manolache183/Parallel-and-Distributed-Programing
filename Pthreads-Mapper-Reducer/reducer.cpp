#include "reducer.h"

reducer_arg create_reducer_arg(unordered_map<int, vector<vector<int>>*> *toReduce,
	int id, pthread_barrier_t *barrier)
{
	reducer_arg reducerArgs;

	reducerArgs.toReduce = toReduce;
	reducerArgs.id = id;
	reducerArgs.barrier = barrier;

	return reducerArgs;
}

void *thread_function_reducer(void *arg)
{
	reducer_arg *args = (reducer_arg *) arg;
	pthread_barrier_wait(args->barrier);
	
	unordered_map<int, vector<vector<int>>*> toReduce = *(args->toReduce);
	string name = "out" + to_string(args->id + 2) + ".txt";

	ofstream fout(name);
	unordered_set<int> nums;
	vector<int> vec;
	int count = 0;

	for (unordered_map<int, vector<vector<int>>*>::iterator itr = toReduce.begin(); itr != toReduce.end(); itr++) {
		vec = (*(itr->second))[args->id];

		for (int num : vec) {
			if (nums.find(num) == nums.end()) {
				count++;
				nums.insert(num);
			}
		}
	}

	fout << count;
	fout.close();
	pthread_exit(NULL);
}