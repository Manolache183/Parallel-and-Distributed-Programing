#include "helpers.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    int N = atoi(argv[1]);
    int err = atoi(argv[2]);
    
    vector<vector<int>> data(MAIN_CLUSTER_SIZE);

    int numtasks, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int lastProc;
    int left, right, faultyNode;

    faultyNode = err != 2 ? -1 : 1;

    if (err == 0) {
        lastProc = 3;
        left = rank == 0 ? MAIN_CLUSTER_SIZE - 1 : rank - 1;
        right = rank == MAIN_CLUSTER_SIZE - 1 ? ROOT : rank + 1;
    } else if (err == 1 || err == 2) {
        lastProc = err;

        if (rank == 0) {
            right = MAIN_CLUSTER_SIZE - 1;
            left = right;
        } else {
            right = rank - 1;
            left = rank + 1;

            if (rank == MAIN_CLUSTER_SIZE - 1) {
                left = ROOT;
            }
            
            if (rank == lastProc) {
                right = left;
            }
        }
    }

    int parent = -1;
    vector<long> V;

    ifstream fin;
    string filename;
    int numChildren;
    int sourceRank;

    // Ring Topology (no err)
    if (err == 0) {
        if (rank < MAIN_CLUSTER_SIZE) {
            // Main Cluster

            // Read the input folders
            filename = "cluster" + to_string(rank) + ".txt";
            fin.open(filename);

            fin >> numChildren;

            int child;
            for (int i = 0; i < numChildren; i++) {
                fin >> child;
                data[rank].push_back(child);
            }

            fin.close();
            
            if (rank == ROOT) {
                // First process starts the circle.
                
                // Send to process 1 the child workers and my id.
                send_topology(rank, rank + 1, data, ROOT, ROOT);
                
                // Receive the final topology from the last process.
                sourceRank = MAIN_CLUSTER_SIZE - 1;
                receive_topology(sourceRank, data, 1, MAIN_CLUSTER_SIZE - 1);

                // Send the remaining worker information to process 1.
                send_topology(rank, rank + 1, data, rank + 2, MAIN_CLUSTER_SIZE - 1);
                
            } else if (rank == MAIN_CLUSTER_SIZE - 1) {
                // Last process closes the circle.

                // Receive the workers from the previous process.
                sourceRank = rank - 1;
                receive_topology(sourceRank, data, ROOT, rank - 1);

                // Send the workers to the first process.
                send_topology(rank, ROOT, data, 1, rank);

            } else if (rank == MAIN_CLUSTER_SIZE - 2) {
                // Penultimate process, the final topology circle closes here.

                // Receive the workers from the previous process.
                sourceRank = rank - 1;

                receive_topology(sourceRank, data, ROOT, rank - 1);

                // Send the workers to the next process.
                send_topology(rank, rank + 1, data, ROOT, rank);

                // Receive the final topology from the previous process.
                receive_topology(sourceRank, data, rank + 1, MAIN_CLUSTER_SIZE - 1);

            } else {
                
                // Receive the workers from the previous process.
                sourceRank = rank - 1;

                receive_topology(sourceRank, data, ROOT, rank - 1);

                // Send the workers to the next process.
                send_topology(rank, rank + 1, data, ROOT, rank);

                // Receive the final topology from the previous process.
                receive_topology(sourceRank, data, rank + 1, MAIN_CLUSTER_SIZE - 1);

                // Send the final topology to the next process.
                send_topology(rank, rank + 1, data, rank + 2, MAIN_CLUSTER_SIZE - 1);
                
            }

            // Send to children the topology and assign the work
            for (int i = 0; i < numChildren; i++) {
                child = data[rank][i];
                send_topology(rank, child, data, ROOT, MAIN_CLUSTER_SIZE - 1);
            }

        } else {
            receive_topology(parent, data, ROOT, MAIN_CLUSTER_SIZE - 1);
        }
    }
    

    // Line topology
    if (err == 1 || err == 2) {
        if (rank < MAIN_CLUSTER_SIZE) {
            // Main Cluster

            // Read the input folders
            filename = "cluster" + to_string(rank) + ".txt";
            fin.open(filename);

            fin >> numChildren;

            int child;
            for (int i = 0; i < numChildren; i++) {
                fin >> child;
                data[rank].push_back(child);
            }

            fin.close();
            
            if (rank == ROOT) {
                // First process starts the circle.
                
                // Send to process right the child workers and my id.
                send_topology(rank, right, data, ROOT, ROOT);
                
                // Receive the final topology from process right process.
                receive_topology(right, data, lastProc, MAIN_CLUSTER_SIZE - 1);
                
            } else if (rank == lastProc) {
                // Last process closes the circle.
                
                // Receive the workers from the left process.
                receive_topology(left, data, ROOT, ROOT); //0's kids
                receive_topology(left, data, rank + 1, MAIN_CLUSTER_SIZE - 1);

                // Send the workers to the left process.
                send_topology(rank, left, data, lastProc, lastProc);

            } else if (!(err == 2 && rank == faultyNode)) {
                // Receive the workers from the left process.
                receive_topology(left, data, ROOT, ROOT); //0's kids

                receive_topology(left, data, rank + 1, MAIN_CLUSTER_SIZE - 1);

                // Send the workers to the right process.
                send_topology(rank, right, data, ROOT, ROOT); // 0's kids

                send_topology(rank, right, data, rank, MAIN_CLUSTER_SIZE - 1);

                // Receive the final topology from the rigth process.
                receive_topology(right, data, lastProc, rank - 1);

                // Send the final topology to the left process.
                send_topology(rank, left, data, lastProc, rank);
                
            }

            // Send to children the topology and assign the work
            for (int i = 0; i < numChildren; i++) {
                child = data[rank][i];
                send_topology(rank, child, data, ROOT, MAIN_CLUSTER_SIZE - 1);
            }

        } else {
            receive_topology(parent, data, ROOT, MAIN_CLUSTER_SIZE - 1);
        }
    }

    // Print topology
    cout << rank << " -> ";

    // Faulty node
    if (err == 2 && (rank == faultyNode || parent == faultyNode)) {
        cout << faultyNode << ":";
        for (int j = 0; j < data[faultyNode].size() - 1; j++) {
            cout << data[faultyNode][j] << ",";
        }

        if (data[faultyNode].size() > 0) {
            cout << data[faultyNode][data[faultyNode].size() - 1] << " ";
        }
        
        cout << endl;

        MPI_Finalize();
        return 0;
    }

    for (int i = 0; i < MAIN_CLUSTER_SIZE; i++) {
        if (i == faultyNode) {
            continue;
        }
        
        cout << i << ":";

        for (int j = 0; j < ((int) data[i].size() - 1); j++) {
            cout << data[i][j] << ",";
        }

        if (data[i].size() > 0) {
            cout << data[i][data[i].size() - 1] << " ";
        }
    }

    cout << endl;

    // Send and collect to / from workers
    if (rank < MAIN_CLUSTER_SIZE) {
        // Main Cluster

        if (rank == ROOT) {
            // Generate vector
            
            for (int k = 0; k < N; k++) {
                V.push_back(N - k - 1);
            }

            classic_cluster_member_work(rank, right, data, V, lastProc);

            // Print the final vector
            cout << "Rezultat: ";
            for (int i = 0; i < V.size(); i++) {
                cout << V[i] << " ";
            }
            cout << endl;
            
        } else {
            // Receive from left
            receive_vector(left, V);

            classic_cluster_member_work(rank, right, data, V, lastProc);

            // Send back to left
            send_vector(rank, left, V, 0, N);
        }

    } else {
        // Receive from parent
        receive_vector(parent, V);

        // Do the work
        for (int i = 0; i < V.size(); i++) {
            V[i] *= 5;
        }

        // Send to parent
        send_vector(rank, parent, V, 0, V.size());
    }

    MPI_Finalize();
    return 0;
}