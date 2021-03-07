#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N_ITERATIONS 100000
#define T_MAX 100
#define THR_ITERATIONS 10
#define alpha 0.9
#define penalty 100

int main(){

    srand(time(NULL));

    FILE *myFile;
    myFile = fopen("data/gap1.txt", "r"); //open txt file to read

    if (myFile == NULL){
        printf("Error Reading File\n"); //handle reading file error
        exit (0);
    }

    int numOfProblems; //number of problems in file
    fscanf(myFile, "%d ", &numOfProblems);
    int i, j;
    for (int p = 0; p < numOfProblems; p++) {
        int n, m; //number of agents, number of jobs
        fscanf(myFile, "%d ", &n);
        fscanf(myFile, "%d ", &m);

        int costs[n][m]; //cost of allocating job j to agent i
        for (i = 0; i < n; i++) {
            for (j = 0; j < m; j++) {
                fscanf(myFile, "%d ", &costs[i][j]);
            }
        }

        int resources[n][m]; //resource consumed in allocating job j to agent i
        for (i = 0; i < n; i++) {
            for (j = 0; j < m; j++) {
                fscanf(myFile, "%d ", &resources[i][j]);
            }
        } 

        int capacityPerAgent[m]; //resource capacity of agent i
        for (i = 0; i < n; i++) {
            fscanf(myFile, "%d ", &capacityPerAgent[i]);
        }     

        //SOLVE
        float T = T_MAX;
        int assignment[m];
        for (i = 0; i < m; i++) {
            assignment[i] = 0;
        }
        int cost = 0;
        int freeResources[n];
        for (i = 0; i < n; i++) {
            freeResources[i] = capacityPerAgent[i];
        }
        int costForSecondChance;
        //initial solve problem
        //make each job be performed by the cheapest agent that has enough free resources
        for (j = 0; j < m; j++) {
            int bestCost = 1000;
            int costOfJob;
            for (i = 0; i < n; i++) {
                if (costs[i][j] < bestCost && freeResources[i] >= resources[i][j]) {
                    bestCost = costs[i][j];
                    assignment[j] = i + 1;
                    freeResources[i] = freeResources[i] - resources[i][j];
                    costOfJob = costs[i][j];
                } else if (costs[i][j] < bestCost) {
                    costForSecondChance = costs[i][j];
                }
            }
            if (assignment[j] == 0) {  //ean den uparxei agent me arketa free resources, epilekse ton prwto me to mikrotero kostos
                for (i = 0; i < n; i++) {
                    if (costs[i][j] == costForSecondChance) {
                        assignment[j] = i + 1;
                        costOfJob = costs[i][j] + abs(freeResources[i] - resources[i][j]) * penalty;
                        freeResources[i] = 0;
                    }
                }
            }
            //upologismos cost
            cost += costOfJob;
        }
        int starCost = cost;
        int initialAssignment[m];
        for (i = 0; i < m; i++) {
            initialAssignment[i] = assignment[i];
        }
        int iter;
        for (iter = 0; iter < N_ITERATIONS; iter++) {
            int thr_iter;
            for (thr_iter = 0; thr_iter < THR_ITERATIONS; thr_iter++) {
                //create a new perturbed solution
                int perturbedAssignment[m];
                for (i = 0; i < m; i++) {
                    perturbedAssignment[i] = assignment[i];
                }
                int perturbedFreeResources[n];
                for (i = 0; i < n; i++) {
                    perturbedFreeResources[i] = capacityPerAgent[i];
                }
                int costOfPerturbed = 0;
                int agentsToChange = (rand() % n) + 1; 
                while (agentsToChange > 0) {
                    int agent = (rand() % n) + 1;
                    int job = rand() % m;
                    perturbedAssignment[job] = agent;
                    agentsToChange -= 1;
                }
                //calculate cost of perturbated solution
                for (j = 0; j < m; j++) {
                    i = perturbedAssignment[j]-1;
                    if (perturbedFreeResources[i] >= resources[i][j]) {
                        perturbedFreeResources[i] -= resources[i][j];
                        costOfPerturbed += costs[i][j];
                    } else {
                        costOfPerturbed += costs[i][j] + abs(perturbedFreeResources[i] - resources[i][j]) * penalty;
                    }
                }
                //acceptance criterion
                if (costOfPerturbed - cost < T) {
                    cost = costOfPerturbed;
                    for (i = 0; i < m; i++) {
                        assignment[i] = perturbedAssignment[i];
                    }
                }
            }
            T = alpha * T;
        }

        printf("cost: %d\n", cost);
        printf("assignment: ");
        for (i = 0; i < m; i++) {
            printf("%d ", assignment[i]);
        }
        printf("\n");
    }    

    fclose(myFile);

    return 0;
}