#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N_ITERATIONS 500
#define penalty 1

int getAgentPerformingJob(int assignment[], int n, int job);

int main(){

    srand(time(NULL));

    FILE *myFile;
    myFile = fopen("data/gap4.txt", "r"); //open txt file to read

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
        int assignment[m];
        for (i = 0; i < m; i++) {
            assignment[i] = 0;
        }
        int cost = 0;
        int freeResources[n];
        for (i = 0; i < n; i++) {
            freeResources[i] = capacityPerAgent[i];
        }
        int costPerAgent[n];
        for (i = 0; i < n; i++) {
            costPerAgent[i] = 0;
        }
        int costForSecondChance;
        //initial solve problem
        //make each job be performed by the cheapest agent that has enough free resources
        for (j = 0; j < m; j++) {
            int bestCost = 1000;
            int costOfJob;
            int selectedAgent;
            for (i = 0; i < n; i++) {
                if (costs[i][j] < bestCost && freeResources[i] >= resources[i][j]) {
                    bestCost = costs[i][j];
                    assignment[j] = i + 1;
                    freeResources[i] = freeResources[i] - resources[i][j];
                    costOfJob = costs[i][j];
                    selectedAgent = i;
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
                        selectedAgent = i;
                    }
                }
            }
            //upologismos cost
            cost += costOfJob;
            //upologismos cost per Agent
            costPerAgent[selectedAgent] += costOfJob;
        }
        for (i = 0; i < n; i++) {
            printf("%d\t",costPerAgent[i]);
        }
        int starCost = cost;
        int initialAssignment[m];
        for (i = 0; i < m; i++) {
            initialAssignment[i] = assignment[i];
        }
        int iter = 0;
        do {

            /*      TODO: check better the cost
             */
            for (int j1 = 0; j1 <m; j1++) {
                for (int j2 = 0; j2 < m; j2++) {
                    if (j1 != j2) {
                         int localSearchCost = 0;  //local search
                        int localCostPerAgent[n];
                        for (i = 0; i < n; i++) {
                            localCostPerAgent[i] = costPerAgent[i];
                        }
                        int a1 = getAgentPerformingJob(initialAssignment, n, j1);
                        int a2 = getAgentPerformingJob(initialAssignment, n, j2);
                        localCostPerAgent[a1] = 0;
                        localCostPerAgent[a2] = 0;
                        //thelw na trexw ola ta jobs na parw ta cost ana agent a1 kai a2
                        int localCapacityA1 = capacityPerAgent[a1];
                        int localCapacityA2 = capacityPerAgent[a2];
                        for (j = 0; j < m; j++) {
                            if (getAgentPerformingJob(initialAssignment, n, j) == a1) {
                                if (localCapacityA1 >= resources[a1][j]) {
                                    localCapacityA1 -= resources[a1][j];
                                    localCostPerAgent[a1] += costs[a1][j];
                                } else {
                                    localCostPerAgent[a1] += costs[a1][j] + abs(localCapacityA1 - resources[a1][j]) * penalty;
                                    localCapacityA1 = 0;
                                }
                                // localCapacityA1 -= resources[a1][j];
                            } else if (getAgentPerformingJob(initialAssignment, n, j) == a2) {
                                if (localCapacityA2 >= resources[a2][j]) {
                                    localCapacityA2 -= resources[a2][j];
                                    localCostPerAgent[a2] += costs[a2][j];
                                } else {
                                    localCostPerAgent[a2] += costs[a2][j] + abs(localCapacityA2 - resources[a2][j]) * penalty;
                                    localCapacityA2 = 0;
                                }
                                // localCapacityA2 -= resources[a2][j];
                            }
                        }
                        for (i = 0; i < n; i++) {
                            localSearchCost += localCostPerAgent[i];
                        }
                        // localSearchCost -= (costs[a1][j1] + costs[a2][j2]);
                        // localSearchCost += (costs[a1][j2] + costs[a2][j1]);
                        // printf("localSearch: %d", localSearchCost);
                        // printf("\nstar: %d\n", starCost);
                        if (localSearchCost < starCost) {
                            initialAssignment[a1] = j2 + 1;
                            initialAssignment[a2] = j1 + 1;
                            starCost = localSearchCost;
                        }
                    }
                }
            }
            //create a new perturbed solution
            int perturbedCostPerAgent[n];
            for (i = 0; i < n; i++) {
                perturbedCostPerAgent[i] = 0;
            }
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
                    perturbedCostPerAgent[i] += costOfPerturbed;
                } else {
                    costOfPerturbed += costs[i][j] + abs(perturbedFreeResources[i] - resources[i][j]) * penalty;
                    perturbedFreeResources[i] = 0;
                    perturbedCostPerAgent[i] += costOfPerturbed;
                }
            }
            //TO DO: LOCAL SEARCH GIA TO PERTURBED
            for (int j1 = 0; j1 <m; j1++) {
                for (int j2 = 0; j2 < m; j2++) {
                    if (j1 != j2) {
                         int localSearchCost = 0;  //local search
                        int localCostPerAgent[n];
                        for (i = 0; i < n; i++) {
                            localCostPerAgent[i] = perturbedCostPerAgent[i];
                        }
                        int a1 = getAgentPerformingJob(perturbedAssignment, n, j1);
                        int a2 = getAgentPerformingJob(perturbedAssignment, n, j2);
                        localCostPerAgent[a1] = 0;
                        localCostPerAgent[a2] = 0;
                        //thelw na trexw ola ta jobs na parw ta cost ana agent a1 kai a2
                        int localCapacityA1 = capacityPerAgent[a1];
                        int localCapacityA2 = capacityPerAgent[a2];
                        for (j = 0; j < m; j++) {
                            if (getAgentPerformingJob(perturbedAssignment, n, j) == a1) {
                                if (localCapacityA1 >= resources[a1][j]) {
                                    localCapacityA1 -= resources[a1][j];
                                    localCostPerAgent[a1] += costs[a1][j];
                                } else {
                                    localCostPerAgent[a1] += costs[a1][j] + abs(localCapacityA1 - resources[a1][j]) * penalty;
                                    localCapacityA1 = 0;
                                }
                                // localCapacityA1 -= resources[a1][j];
                            } else if (getAgentPerformingJob(perturbedAssignment, n, j) == a2) {
                                if (localCapacityA2 >= resources[a2][j]) {
                                    localCapacityA2 -= resources[a2][j];
                                    localCostPerAgent[a2] += costs[a2][j];
                                } else {
                                    localCostPerAgent[a2] += costs[a2][j] + abs(localCapacityA2 - resources[a2][j]) * penalty;
                                    localCapacityA2 = 0;
                                }
                                // localCapacityA2 -= resources[a2][j];
                            }
                        }
                        for (i = 0; i < n; i++) {
                            localSearchCost += localCostPerAgent[i];
                        }
                        // localSearchCost -= (costs[a1][j1] + costs[a2][j2]);
                        // localSearchCost += (costs[a1][j2] + costs[a2][j1]);
                        // printf("localSearch: %d", localSearchCost);
                        // printf("\nstar: %d\n", starCost);
                        if (localSearchCost < costOfPerturbed) {
                            perturbedAssignment[a1] = j2 + 1;
                            perturbedAssignment[a2] = j1 + 1;
                            costOfPerturbed = localSearchCost;
                        }
                    }
                }
            }            
            cost = costOfPerturbed;
            if (cost < starCost) {
                starCost = cost;
                for (i = 0; i < m; i++) {
                    initialAssignment[i] = perturbedAssignment[i];
                }
            }
            iter++;
        } while(iter <= N_ITERATIONS);
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

int getAgentPerformingJob(int assignment[], int n, int job) {
    int i = 0;
    if (assignment[i] == job + 1) {
        return 0;
    }
    while (assignment[i] != job +1 && i < n) {
        i++;
    }
    return i;
}