#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define HALF_ITERATIONS 50000
#define N_ITER_UPDATE_TEMPERATURE 1000
#define alpha 0.9
#define INITIAL_TEMPERATURE 1000
#define vns_penalty 1000

void vns_initial_simulated_annealing (char dataPath[], FILE *resultsFile) {
    printf("VARIABLE NEIGHBORHOOD SEARCH\n");
    fprintf(resultsFile, "VARIABLE NEIGHBORHOOD SEARCH\n");
    srand(time(NULL));

    FILE *myFile;
    myFile = fopen(dataPath, "r"); //open txt file to read

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
        clock_t t; 
        t = clock();
        float T = INITIAL_TEMPERATURE;
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
                        costOfJob = costs[i][j] + abs(freeResources[i] - resources[i][j]) * vns_penalty;
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
        for (iter = 0; iter < HALF_ITERATIONS; iter++) {
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
                    costOfPerturbed += costs[i][j] + abs(perturbedFreeResources[i] - resources[i][j]) * vns_penalty;
                }
            }
            //acceptance criterion
            if (costOfPerturbed < cost) {
                cost = costOfPerturbed;
                for (i = 0; i < m; i++) {
                    assignment[i] = perturbedAssignment[i];
                }
            } else {
                double p = exp((- costOfPerturbed + cost) / T);
                if ((double) rand() / (RAND_MAX) < p) {
                    cost = costOfPerturbed;
                    for (i = 0; i < m; i++) {
                        assignment[i] = perturbedAssignment[i];
                    }
                }
            }
            
            if (cost < starCost) {
                starCost = cost;
                for (i = 0; i < m; i++) {
                    initialAssignment[i] = assignment[i];
                }
            }
            if ((iter + 1) % N_ITER_UPDATE_TEMPERATURE == 0) {
                T *= alpha;
            }
        }
        // printf("SIMULATED ANNEALING INITIAL SOLUTION\n");
        // printf("cost: %d\n", cost);
        // printf("assignment: ");
        // for (i = 0; i < m; i++) {
        //     printf("%d ", assignment[i]);
        // }
        // printf("\n");
        ////////////////////////VNS START
        
        int selectedAgent;
        int costOfJob;
        for (i = 0; i < n; i++)
        {
            freeResources[i] = capacityPerAgent[i];
        }
        int costPerAgent[n];
        for (i = 0; i < n; i++)
        {
            costPerAgent[i] = 0;
        }
        for (i = 0; i < m; i++) {
            initialAssignment[i] = assignment[i];
        }
        for (i = 0; i < m; i++) {
            selectedAgent = initialAssignment[i] - 1;
            if (freeResources[selectedAgent] >= resources[selectedAgent][i]) {
                freeResources[selectedAgent] -= resources[selectedAgent][i];
                costOfJob = costs[selectedAgent][i];
            } else {
                costOfJob = costs[selectedAgent][i] +  abs(freeResources[selectedAgent] - resources[selectedAgent][i]) * vns_penalty;
            }
            cost += costOfJob;
            costPerAgent[selectedAgent] += costOfJob;
        }
        starCost = cost;
        for (i = 0; i < m; i++)
        {
            assignment[i] = initialAssignment[i];
        }
        int maxK; //ean einai megalo problima meiwnei tis prakseis
        if (m > 200) {
            maxK = 2;
        } else {
            maxK = 3;
        }
        for (int k = 0; k < maxK; k++) {
            int changedAssignment[m];
            for (i = 0; i < m; i++) {
                changedAssignment[i] = assignment[i];
            }
            int changedCostPerAgent[n];
            for (i = 0; i < n; i++) {
                changedCostPerAgent[i] = costPerAgent[i];
            }
            int changedAssignmentCost = cost;
            if (k == 0) { //1-CHANGE. Returns a new solution leaving the original intact
            // change the agent that performs a job. If the solution improves, pick the new one.
                for (int j1 = 0; j1 < m; j1++) {
                    for (int a1 = 0; a1 < n; a1++) {
                        int costToBeRemoved = changedCostPerAgent[a1];
                        int costToBeAdded = 0;
                        int resourcesOfA1 = capacityPerAgent[a1];
                        int previousAgent = changedAssignment[j1] - 1;
                        changedAssignment[j1] = a1 + 1;
                        for (j = 0; j < m; j++) {
                            if (changedAssignment[j] == a1 + 1) {
                                if (resourcesOfA1 >= resources[a1][j]) {
                                    resourcesOfA1 -= resources[a1][j];
                                    costToBeAdded += costs[a1][j];
                                } else {
                                    costToBeAdded += costs[a1][j] + abs(resourcesOfA1 - resources[a1][j]) * vns_penalty;
                                    resourcesOfA1 = 0;
                                }
                            }
                        }
                        int newChangedAssignmentCost = 0;
                        for (i = 0; i < n; i++) {
                            newChangedAssignmentCost += changedCostPerAgent[i];
                        }
                        newChangedAssignmentCost += costToBeAdded - costToBeRemoved;
                        if (newChangedAssignmentCost < changedAssignmentCost) {
                            changedAssignmentCost = newChangedAssignmentCost;
                            changedCostPerAgent[a1] = costToBeAdded;
                        } else {
                            changedAssignment[j1] = previousAgent + 1;   
                        }
                    }
                }
            } else if (k == 1) { //2-OPT. Returns a new solution leaving the original intact
            // swap the agents for each job pairs.
                for (int j1 = 0; j1 < m; j1++) {
                    for (int j2 = 0; j2 < m; j2++) {
                        int a1 = changedAssignment[j1] - 1;
                        int a2 = changedAssignment[j2] - 1;
                        int costToBeRemoved;
                        if (a1 == a2) {
                            costToBeRemoved = changedCostPerAgent[a1];
                        } else {
                            costToBeRemoved = changedCostPerAgent[a1] + changedCostPerAgent[a2];
                        }
                        int costToBeAdded1 = 0;
                        int costToBeAdded2 = 0;
                        int resourcesOfA1 = capacityPerAgent[a1];
                        int resourcesOfA2 = capacityPerAgent[a2];
                        changedAssignment[j1] = a2 + 1;
                        changedAssignment[j2] = a1 + 1;
                        for (j = 0; j < m; j++) {
                            if (changedAssignment[j] == a1 + 1) {
                                if (resourcesOfA1 >= resources[a1][j]) {
                                    resourcesOfA1 -= resources[a1][j];
                                    costToBeAdded1 += costs[a1][j];
                                } else {
                                    costToBeAdded1 += costs[a1][j] + abs(resourcesOfA1 - resources[a1][j]) * vns_penalty;
                                    resourcesOfA1 = 0;
                                }
                            } else if (changedAssignment[j] == a2 + 1) {
                                if (resourcesOfA2 >= resources[a2][j]) {
                                    resourcesOfA2 -= resources[a2][j];
                                    costToBeAdded2 += costs[a2][j];
                                } else {
                                    costToBeAdded2 += costs[a2][j] + abs(resourcesOfA2 - resources[a2][j]) * vns_penalty;
                                    resourcesOfA2 = 0;
                                }
                            }
                        }
                        int newChangedAssignmentCost = 0;
                        for (i = 0; i < n; i++) {
                            // if (i != a1 && i != a2){
                                newChangedAssignmentCost += changedCostPerAgent[i];
                            // }
                        }
                        newChangedAssignmentCost += costToBeAdded1 + costToBeAdded2 - costToBeRemoved;
                        if (newChangedAssignmentCost < changedAssignmentCost) {
                            changedAssignmentCost = newChangedAssignmentCost;
                                changedCostPerAgent[a2] = costToBeAdded2;
                                changedCostPerAgent[a1] = costToBeAdded1;
                        } else {
                            changedAssignment[j1] = a1 + 1;   
                            changedAssignment[j2] = a2 + 1;
                        }
                    }
                }
            } else if (k == 2) { //2-CHANGE. Returns a new solution leaving the original intact
            // change the agents that perform two jobs.
                for (int j1 = 0; j1 < m; j1++) {
                    for (int j2 = 0; j2 < m; j2++) {
                        int oldA1 = changedAssignment[j1] - 1;
                        int oldA2 = changedAssignment[j2] - 1;
                        for (int a1 = 0; a1 < n; a1++) {
                            for (int a2 = 0; a2 < n; a2++) {
                                int costToBeRemoved;
                                if (a1 == a2) {
                                    costToBeRemoved = changedCostPerAgent[a1];
                                } else {
                                    costToBeRemoved = changedCostPerAgent[a1] + changedCostPerAgent[a2];
                                }
                                int costToBeAdded1 = 0;
                                int costToBeAdded2 = 0;
                                int resourcesOfA1 = capacityPerAgent[a1];
                                int resourcesOfA2 = capacityPerAgent[a2];
                                changedAssignment[j1] = a1 + 1;
                                changedAssignment[j2] = a2 + 1;
                                for (j = 0; j < m; j++) {
                                    if (changedAssignment[j] == a1 + 1) {
                                        if (resourcesOfA1 >= resources[a1][j]) {
                                            resourcesOfA1 -= resources[a1][j];
                                            costToBeAdded1 += costs[a1][j];
                                        } else {
                                            costToBeAdded1 += costs[a1][j] + abs(resourcesOfA1 - resources[a1][j]) * vns_penalty;
                                            resourcesOfA1 = 0;
                                        }
                                    } else if (changedAssignment[j] == a2 +1) {
                                        if (resourcesOfA2 >= resources[a2][j]) {
                                            resourcesOfA2 -= resources[a2][j];
                                            costToBeAdded2 += costs[a2][j];
                                        } else {
                                            costToBeAdded2 += costs[a2][j] + abs(resourcesOfA2 - resources[a2][j]) * vns_penalty;
                                            resourcesOfA2 = 0;
                                        }
                                    }
                                }
                                int newChangedAssignmentCost = 0;
                                for (i = 0; i < n; i++) {
                                    // if (i != a1 && i != a2) {
                                        newChangedAssignmentCost += changedCostPerAgent[i];
                                    // }
                                }
                                newChangedAssignmentCost += costToBeAdded1 + costToBeAdded2 - costToBeRemoved;
                                if (newChangedAssignmentCost < changedAssignmentCost) {
                                    changedAssignmentCost = newChangedAssignmentCost;
                                    changedCostPerAgent[a1] = costToBeAdded1;
                                    changedCostPerAgent[a2] = costToBeAdded2;
                                } else {
                                    changedAssignment[j1] = oldA1 + 1;   
                                    changedAssignment[j2] = oldA2 + 1;
                                }
                            }
                        }
                        //TODO: an xreiazontai, ksana kane copy ta arxika costs by agent
                        // changedAssignment[j1] = oldA1 + 1;
                        // changedAssignment[j2] = oldA2 + 1;
                    }
                }
            } else if (k == 3) { //3-OPT. Returns a new solution leaving the original intact
            // swap the agents for each job triple.
                for (int j1 = 0; j1 < m; j1++) {
                    for (int j2 = 0; j2 < m; j2++) {
                        for (int j3 = 0; j3 < m; j3++) {
                            int a1 = changedAssignment[j1] - 1;
                            int a2 = changedAssignment[j2] - 1;
                            int a3 = changedAssignment[j3] - 1;
                            int costToBeRemoved;
                            if (a1 == a2 && a1 == a3) {
                                costToBeRemoved = changedCostPerAgent[a1];
                            } else if (a1 == a2 && a1 != a3) {
                                costToBeRemoved = changedCostPerAgent[a1] + changedCostPerAgent[a3];
                            } else if (a1 != a2 && a2 == a3) {
                                costToBeRemoved = changedCostPerAgent[a1] + changedCostPerAgent[a3];
                            } else if (a1 == a3 && a1 != a2) {
                                costToBeRemoved = changedCostPerAgent[a1] + changedCostPerAgent[a2];
                            } else {
                                costToBeRemoved = changedCostPerAgent[a1] + changedCostPerAgent[a2] + changedCostPerAgent[a3];
                            }
                            // int costToBeRemoved = changedCostPerAgent[a1] + changedCostPerAgent[a2] + changedCostPerAgent[a3];
                            int costToBeAdded1 = 0;
                            int costToBeAdded2 = 0;
                            int costToBeAdded3 = 0;
                            int resourcesOfA1 = capacityPerAgent[a1];
                            int resourcesOfA2 = capacityPerAgent[a2];
                            int resourcesOfA3 = capacityPerAgent[a3];
                            changedAssignment[j1] = a2 + 1;
                            changedAssignment[j2] = a3 + 1;
                            changedAssignment[j3] = a1 + 1;
                            for (j = 0; j < n; j++) {
                                if (changedAssignment[j] == a1 + 1) {
                                    if (resourcesOfA1 >= resources[a1][j]) {
                                        resourcesOfA1 -= resources[a1][j];
                                        costToBeAdded1 += costs[a1][j];
                                    } else {
                                        costToBeAdded1 += costs[a1][j] + abs(resourcesOfA1 - resources[a1][j]) * vns_penalty;
                                        resourcesOfA1 = 0;
                                    }
                                } else if (changedAssignment[j] == a2 + 1) {
                                    if (resourcesOfA2 >= resources[a2][j]) {
                                        resourcesOfA2 -= resources[a2][j];
                                        costToBeAdded2 += costs[a2][j];
                                    } else {
                                        costToBeAdded2 += costs[a2][j] + abs(resourcesOfA2 - resources[a2][j]) * vns_penalty;
                                        resourcesOfA2 = 0;
                                    }
                                } else if (changedAssignment[j] == a3 + 1) {
                                    if (resourcesOfA3 >= resources[a3][j]) {
                                        resourcesOfA3 -= resources[a3][j];
                                        costToBeAdded3 += costs[a3][j];
                                    } else {
                                        costToBeAdded3 += costs[a3][j] + abs(resourcesOfA3 - resources[a3][j]) * vns_penalty;
                                        resourcesOfA3 = 0;
                                    }
                                }
                            }
                            int newChangedAssignmentCost = 0;
                            for (i = 0; i < n; i++) {
                                // if (i!= a1 && i != a2 && i != a3) {
                                    newChangedAssignmentCost += changedCostPerAgent[i];
                                // }
                            }
                            newChangedAssignmentCost += costToBeAdded1 + costToBeAdded2 + costToBeAdded3 - costToBeRemoved;
                            if (newChangedAssignmentCost < changedAssignmentCost) {
                                changedAssignmentCost = newChangedAssignmentCost;
                                changedCostPerAgent[a1] = costToBeAdded1;
                                changedCostPerAgent[a2] = costToBeAdded2;
                                changedCostPerAgent[a3] = costToBeAdded3;
                            } else {
                                changedAssignment[j1] = a1 + 1;   
                                changedAssignment[j2] = a2 + 1;
                                changedAssignment[j3] = a3 + 1;
                            }
                        }
                    }
                }
            }
            if (changedAssignmentCost < cost) {
                cost = changedAssignmentCost;
                for (i = 0; i < m; i++) {
                    assignment[i] = changedAssignment[i];
                }
                k = -1;
            }
        }
        t = clock() - t; 
        double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
        printf("cost: %d\n", cost);
        printf("assignment: ");
        for (i = 0; i < m; i++)
        {
            printf("%d ", assignment[i]);
        }
        printf("\n");
        printf("time: %f\n", time_taken);
        fprintf(resultsFile, "cost: %d\n", cost);
        fprintf(resultsFile, "assignment: ");
        for (i = 0; i < m; i++)
        {
            fprintf(resultsFile, "%d ", assignment[i]);
        }
        fprintf(resultsFile, "\n");
        fprintf(resultsFile, "time: %f\n", time_taken);
    }    
    fprintf(resultsFile, "################################################\n");
    fclose(myFile);
}