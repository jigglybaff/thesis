#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(){
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
        int assignment[m];
        for (i = 0; i < m; i++)
        {
            assignment[i] = 0;
        }
        int cost = 0;
        int freeResources[n];
        for (i = 0; i < n; i++)
        {
            freeResources[i] = capacityPerAgent[i];
        }
        int costPerAgent[n];
        for (i = 0; i < n; i++)
        {
            costPerAgent[i] = 0;
        }
        int costForSecondChance;
        //initial solve problem
        //make each job be performed by the cheapest agent that has enough free resources
        int penalty = 100;
        // for (j = 0; j < m; j++)
        // {
            int bestCost = 1000;
            int costOfJob;
            int selectedAgent;
        //     for (i = 0; i < n; i++)
        //     {
        //         if (costs[i][j] < bestCost && freeResources[i] >= resources[i][j])
        //         {
        //             bestCost = costs[i][j];
        //             assignment[j] = i + 1;
        //             freeResources[i] = freeResources[i] - resources[i][j];
        //             costOfJob = costs[i][j];
        //             selectedAgent = i;
        //         }
        //         else if (costs[i][j] < bestCost)
        //         {
        //             costForSecondChance = costs[i][j];
        //         }
        //     }
        //     if (assignment[j] == 0)
        //     { //ean den uparxei agent me arketa free resources, epilekse ton prwto me to mikrotero kostos
        //         for (i = 0; i < n; i++)
        //         {
        //             if (costs[i][j] <= costForSecondChance)
        //             {
        //                 assignment[j] = i + 1;
        //                 costOfJob = costs[i][j] + abs(freeResources[i] - resources[i][j]) * penalty;
        //                 freeResources[i] = 0;
        //                 selectedAgent = i;
        //             }
        //         }
        //     }
        //     //upologismos cost
        //     cost += costOfJob;
        //     //upologismos cost per Agent
        //     costPerAgent[selectedAgent] += costOfJob;
        // }

        // int starCost = cost;
        
        int initialAssignment[15] = {3,2,1,3,3,2,2,1,1,5,3,4,2,4,5};
        cost = 0;
        for (i = 0; i < m; i++) {
            selectedAgent = initialAssignment[i] - 1;
            if (freeResources[selectedAgent] >= resources[selectedAgent][i]) {
                freeResources[selectedAgent] -= resources[selectedAgent][i];
                costOfJob = costs[selectedAgent][i];
            } else {
                costOfJob = costs[selectedAgent][i] +  abs(freeResources[selectedAgent] - resources[selectedAgent][i]) * penalty;
            }
            cost += costOfJob;
            costPerAgent[selectedAgent] += costOfJob;
            printf("cost of job! : %d\n", costOfJob);
        }
        printf("cost! : %d\n", cost);
        // cost = 993;
        int starCost = cost;
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
                                    costToBeAdded += costs[a1][j] + abs(resourcesOfA1 - resources[a1][j]) * penalty;
                                    resourcesOfA1 = 0;
                                }
                            }
                        }
                        int newChangedAssignmentCost = 0;
                        for (i = 0; i < n; i++) {
                            newChangedAssignmentCost += changedCostPerAgent[i];
                        }
                        // printf("1cost: %d\n", costToBeAdded);
                        // printf("cost: %d\n", costToBeRemoved);
                        // printf("cost: %d\n", newChangedAssignmentCost);
                        newChangedAssignmentCost += costToBeAdded - costToBeRemoved;
                        // printf("costtt: %d\n", cost);
                        // printf("cost!: %d\n\n\n", newChangedAssignmentCost);
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
                                    costToBeAdded1 += costs[a1][j] + abs(resourcesOfA1 - resources[a1][j]) * penalty;
                                    resourcesOfA1 = 0;
                                }
                            } else if (changedAssignment[j] == a2 + 1) {
                                if (resourcesOfA2 >= resources[a2][j]) {
                                    resourcesOfA2 -= resources[a2][j];
                                    costToBeAdded2 += costs[a2][j];
                                } else {
                                    costToBeAdded2 += costs[a2][j] + abs(resourcesOfA2 - resources[a2][j]) * penalty;
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
                        // printf("2cost: %d\n", costToBeAdded1);
                        //         printf("cost: %d\n", costToBeAdded2);
                        //         printf("cost: %d\n", costToBeRemoved);
                        //         printf("cost: %d\n", newChangedAssignmentCost);
                        newChangedAssignmentCost += costToBeAdded1 + costToBeAdded2 - costToBeRemoved;
                        // printf("costtt: %d\n", cost);
                        // printf("cost!: %d\n\n\n", newChangedAssignmentCost);
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
                                            costToBeAdded1 += costs[a1][j] + abs(resourcesOfA1 - resources[a1][j]) * penalty;
                                            resourcesOfA1 = 0;
                                        }
                                    } else if (changedAssignment[j] == a2 +1) {
                                        if (resourcesOfA2 >= resources[a2][j]) {
                                            resourcesOfA2 -= resources[a2][j];
                                            costToBeAdded2 += costs[a2][j];
                                        } else {
                                            costToBeAdded2 += costs[a2][j] + abs(resourcesOfA2 - resources[a2][j]) * penalty;
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
                                // printf("3cost: %d\n", costToBeAdded1);
                                // printf("cost: %d\n", costToBeAdded2);
                                // printf("cost: %d\n", costToBeRemoved);
                                // printf("cost: %d\n", newChangedAssignmentCost);
                                newChangedAssignmentCost += costToBeAdded1 + costToBeAdded2 - costToBeRemoved;
                                // printf("costtt: %d\n", cost);
                                // printf("cost!: %d\n\n\n", newChangedAssignmentCost);
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
                                        costToBeAdded1 += costs[a1][j] + abs(resourcesOfA1 - resources[a1][j]) * penalty;
                                        resourcesOfA1 = 0;
                                    }
                                } else if (changedAssignment[j] == a2 + 1) {
                                    if (resourcesOfA2 >= resources[a2][j]) {
                                        resourcesOfA2 -= resources[a2][j];
                                        costToBeAdded2 += costs[a2][j];
                                    } else {
                                        costToBeAdded2 += costs[a2][j] + abs(resourcesOfA2 - resources[a2][j]) * penalty;
                                        resourcesOfA2 = 0;
                                    }
                                } else if (changedAssignment[j] == a3 + 1) {
                                    if (resourcesOfA3 >= resources[a3][j]) {
                                        resourcesOfA3 -= resources[a3][j];
                                        costToBeAdded3 += costs[a3][j];
                                    } else {
                                        costToBeAdded3 += costs[a3][j] + abs(resourcesOfA3 - resources[a3][j]) * penalty;
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
                            // printf("4cost: %d\n", costToBeAdded1);
                            //     printf("cost: %d\n", costToBeAdded2);
                            //     printf("cost: %d\n", costToBeAdded3);
                            //     printf("cost: %d\n", costToBeRemoved);
                            //     printf("cost: %d\n", newChangedAssignmentCost);
                            newChangedAssignmentCost += costToBeAdded1 + costToBeAdded2 + costToBeAdded3 - costToBeRemoved;
                            // printf("costtt: %d\n", cost);
                            // printf("cost!: %d\n\n\n", newChangedAssignmentCost);
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
        printf("cost: %d\n", cost);
        printf("assignment: ");
        for (i = 0; i < m; i++)
        {
            printf("%d ", assignment[i]);
        }
        printf("\n");
        p = 100;
    }
    fclose(myFile);
    return 0;
}