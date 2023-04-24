#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <fstream>
#include <unordered_set>
#include "mapper.h"
#include "reducer.h"

using namespace std;

void get_args(int argc, char **argv, int &M, int &R, string &name)
{
	if (argc < 4)
	{
		printf("Numar insuficient de parametri: ./tema1 M R filename\n");
		exit(1);
	}

	M = atoi(argv[1]);
	R = atoi(argv[2]);
	name = argv[3];
}

int main(int argc, char *argv[])
{
    int M;
    int R;
    string filename;

	unordered_set<string> remainingFiles;
	unordered_map<int, vector<vector<int>>*> toReduce;
	pthread_mutex_t mutex;

	int numOfFiles;

	get_args(argc, argv, M, R, filename);
	ifstream fin(filename);

	fin >> numOfFiles;
	for (int i = 0; i < numOfFiles; i++) {
		fin >> filename;
		remainingFiles.insert(filename);
	}

	fin.close();

	pthread_barrier_t barrier;
	int r = pthread_barrier_init(&barrier, NULL, M + R);
	if (r) {
		cout << "Probleme la crearea barierei";
		exit(-1);
	}

	vector<pthread_t> mapperThreads(M);
	vector<int> mapperThreadsIds(M);

	vector<pthread_t> reducerThreads(R);
	vector<int> reducerThreadsIds(R);

	r = pthread_mutex_init(&mutex, NULL);
	if (r) {
			printf("Eroare la crearea mutex-ului\n");
			exit(-1);
	}

	vector<mapper_arg> mapperArgs(M);
	vector<reducer_arg> reducerArgs(R);

	int len = max(R, M);
	for (int i = 0; i < len; i++) {
		if (i < M) {
			mapperThreadsIds[i] = i;

			mapperArgs[i] = create_mapper_arg(&remainingFiles, &toReduce, &mutex, &barrier, i, R);

			r = pthread_create(&mapperThreads[i], NULL, thread_function_mapper, &mapperArgs[i]);
			if (r) {
				cout << "Probleme la crearea de mapper thread";
				exit(-1);
			}
		}

		if (i < R) {
			reducerThreadsIds[i] = i;

			reducerArgs[i] = create_reducer_arg(&toReduce, i, &barrier);

			r = pthread_create(&reducerThreads[i], NULL, thread_function_reducer, &reducerArgs[i]);
			if (r) {
				cout << "Probleme la crearea de reducer thread";
				exit(-1);
			}
		}
	}

	for (int i = 0; i < len; i++) {
		if (i < M) {
			pthread_join(mapperThreads[i], NULL);
			if (r) {
				cout << "Probleme la join de mapper thread";
				exit(-1);
			}
		}

		if (i < R) {
			pthread_join(reducerThreads[i], NULL);
			if (r) {
				cout << "Probleme la join de reducer thread";
				exit(-1);
			}
		}
	}

	r = pthread_mutex_destroy(&mutex);
	if (r) {
			printf("Eroare la distrugerea mutex-ului\n");
			exit(-1);
	}

	r = pthread_barrier_destroy(&barrier);
	if (r) {
			printf("Eroare la distrugerea barierei-ului\n");
			exit(-1);
	}

	for (unordered_map<int, vector<vector<int>>*>::iterator itr = toReduce.begin(); itr != toReduce.end(); itr++) {
		delete itr->second;
	}

    return 0;
}