#include "mapper.h"

void figure_powers(vector<vector<int>> *lists, unordered_set<int> nums, int max, int R) {
	int num, count;

	for (int i = 2; i < max; i++) {
		if (max / i < i) {
			break;
		}

		num = i;
		for (count = 2; num < max && count <= R + 1; count++) {
			if (num > INT_MAX / i) {
				break;
			}
			num *= i;
			if (nums.find(num) != nums.end()) {
				lists->at(count - 2).push_back(num);
			}
		}
	}

	if (nums.find(1) != nums.end()) {
		for (int i = 0; i < R; i++) {
			lists->at(i).push_back(1);
		}
	}
}

mapper_arg create_mapper_arg(unordered_set<string> *remainingFiles,
	unordered_map<int, vector<vector<int>>*> *toReduce,
	pthread_mutex_t *mutex, pthread_barrier_t *barrier,
	int id, int R)
{
	mapper_arg mapperArgs;

	mapperArgs.remainingFiles = remainingFiles;
	mapperArgs.toReduce = toReduce;
	mapperArgs.mutex = mutex;
	mapperArgs.id = id;
	mapperArgs.R = R;
	mapperArgs.barrier = barrier;

	return mapperArgs;
}

void working_function_mapper(ifstream &data_in, mapper_arg *args)
{
	int size;
	data_in >> size;
	unordered_set<int> nums;
	vector<vector<int>> *lists;

	if (args->toReduce->find(args->id) == args->toReduce->end()) {
		lists = new vector<vector<int>>(args->R);
		(*(args->toReduce))[args->id] = lists;
	} else {
		lists = (*(args->toReduce))[args->id];
	}

	int max = -1;
	int target;

	for (int i = 0; i < size; i++) {
		data_in >> target;
		
		if (target > max) {
			max = target;
		}

		nums.insert(target);
	}

	figure_powers(lists, nums, max, args->R);
}

void *thread_function_mapper(void *arg)
{
	mapper_arg *args = (mapper_arg *) arg;
	int r;
	while (true) {
		r = pthread_mutex_lock(args->mutex);
		if (r) {
			printf("Eroare la blocarea mutex-ului\n");
			exit(-1);
		}

		bool toWork = false;
		string fileName;
		if (!args->remainingFiles->empty()) {
			unordered_set<string> :: iterator itr = args->remainingFiles->begin();
			fileName = *itr;
			args->remainingFiles->erase(fileName);
			toWork = true;
		}

		r = pthread_mutex_unlock(args->mutex);
		if (r) {
			printf("Eroare la deblocarea mutex-ului\n");
			exit(-1);
		}
		
		if (toWork) {
			ifstream data_in(fileName);
			working_function_mapper(data_in, args);
			data_in.close();
		} else {
			break;
		}
	
	}

	pthread_barrier_wait(args->barrier);
	pthread_exit(NULL);
}
