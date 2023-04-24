#include "helpers.h"

using namespace std;

void receive_topology(int &sourceRank, vector<vector<int>> &data, int start, int end) {
    int size;

    if (sourceRank == -1) {
        MPI_Status status;

        for (int i = start; i <= end; i++) {
            MPI_Recv(&size, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (size != 0) {
                data[i].resize(size);
                MPI_Recv(&data[i][0], size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            }
        }
        
        sourceRank = status.MPI_SOURCE;
    } else {
        for (int i = start; i <= end; i++) {
            MPI_Recv(&size, 1, MPI_INT, sourceRank, sourceRank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (size != 0) {
                data[i].resize(size);
                MPI_Recv(&data[i][0], size, MPI_INT, sourceRank, sourceRank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
}

void send_topology(int sourceRank, int destRank, vector<vector<int>> &data, int start, int end) {
    vector<int> toSend;
    int size;
    for (int i = start; i <= end; i++) {
        toSend = data[i];

        size = toSend.size();
        MPI_Send(&size, 1, MPI_INT, destRank, sourceRank, MPI_COMM_WORLD);
        cout << "M(" << sourceRank << "," << destRank << ")" << endl;

        if (size != 0) {
            MPI_Send(&toSend[0], size, MPI_INT, destRank, sourceRank, MPI_COMM_WORLD);
            cout << "M(" << sourceRank << "," << destRank << ")" << endl;
        }
    }
}

void send_vector(int sourceRank, int destRank, vector<long> &V, int start, int size) {
    
    // Send the size of the vector
    MPI_Send(&size, 1, MPI_INT, destRank, sourceRank, MPI_COMM_WORLD);
    cout << "M(" << sourceRank << "," << destRank << ")" << endl;

    // Send the vector, but only the part that wasn't computed by me
    MPI_Send(&V[start], size, MPI_LONG, destRank, sourceRank, MPI_COMM_WORLD);
    cout << "M(" << sourceRank << "," << destRank << ")" << endl;
}

void receive_vector(int sourceRank, vector<long> &V) {
    int size;
    MPI_Recv(&size, 1, MPI_INT, sourceRank, sourceRank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    V.resize(size);
    
    MPI_Recv(&V[0], size, MPI_LONG, sourceRank, sourceRank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void classic_cluster_member_work(int rank, int right, vector<vector<int>> data, vector<long> &V, int last) {
    int child, start, end, N = V.size();

    int numOfWorkers = 0;

    if (last == MAIN_CLUSTER_SIZE - 1 || rank == 0) {
        for (int i = rank; i < MAIN_CLUSTER_SIZE; i++) {
            numOfWorkers += data[i].size();
        }
    } else {
        for (int i = rank; i > 0; i--) {
            numOfWorkers += data[i].size();
        }
    }

    double split = (double) N / numOfWorkers;

    // Send to children their part

    for (int i = 0; i < data[rank].size(); i++) {
        child = data[rank][i];

        start = i * split;
        end = min((int) (i + 1) * split, N);

        send_vector(rank, child, V, start, end - start);
    }
    
    // Send to right his part
    if (rank != last) {
        send_vector(rank, right, V, end, N - end);
    }

    // Receive from children
    vector<long> temp;
    for (int i = 0; i < data[rank].size(); i++) {
        child = data[rank][i];

        start = i * split;
        end = min((i + 1) * split, N);

        receive_vector(child, temp);

        for (int j = start; j < end; j++) {
            V[j] = temp[j - start];
        }
    }

    // Receive from right
    if (rank != last) {
        receive_vector(right, temp);
        for (int i = end; i < N; i++) {
            V[i] = temp[i - end];
        }
    }
    
}

int min(int a, int b) {
    return a < b ? a : b;
}