#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TABU_N_ITERATIONS 4000
#define N_NEIGHBORS 50
#define penalty 100

void tabu_search (char dataPath[], FILE *resultsFile) {
    printf("TABU SEARCH\n");
    fprintf(resultsFile, "TABU SEARCH\n");
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
        //TODO: tabu list
        int neighbors = 0;
        int tabuList[TABU_N_ITERATIONS][m];
        //add the initial assigment in the list
        for (i = 0; i < m; i++) {
            tabuList[neighbors][i] = initialAssignment[i];
        }
        neighbors += 1;
        for (int iter = 0; iter < TABU_N_ITERATIONS; iter++) {
            int bestNeighbor[m];
            int isThereBest = 0;
            int bestNCost = 10000;
            for (int k = 0; k < N_NEIGHBORS; k++) {
                //random neighbor
                int perturbedAssignment[m];
                for (i = 0; i < m; i++) {
                    perturbedAssignment[i] = assignment[i];
                }
                int perturbedFreeResources[n];
                for (i = 0; i < n; i++) {
                    perturbedFreeResources[i] = capacityPerAgent[i];
                }
                int costOfPerturbed = 0;
                int agentsToChange = 2; 
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
                //check if tabu list contains this neighbor
                int equals = 0;
                for (i = 0; i < neighbors; i++) {
                    for (j = 0; j < m; j++) {
                        if (tabuList[i][j] == perturbedAssignment[j]) {
                            equals += 1;
                        }
                    }
                }
                if (equals == m) {
                    continue;
                }
                if (isThereBest == 0 || costOfPerturbed < bestNCost) {
                    isThereBest = 1;
                    bestNCost = costOfPerturbed;
                    for (i = 0; i < m; i++) {
                        bestNeighbor[i] = perturbedAssignment[i];
                    }
                }
            }
            if (isThereBest == 0) {
                continue;
            }
            //add the best neighbor in the tabu list
            for (i = 0; i < m; i++) {
                tabuList[neighbors][i] = bestNeighbor[i];
            }
            if (bestNCost < starCost) {
                starCost = bestNCost;
                for (i = 0; i < m; i++) {
                    assignment[i] = bestNeighbor[i];
                }
            }
        }
        t = clock() - t; 
        double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
        printf("cost: %d\n", starCost);
        printf("assignment: ");
        for (i = 0; i < m; i++) {
            printf("%d ", assignment[i]);
        }
        printf("\n");
        printf("time: %f\n", time_taken);
        fprintf(resultsFile, "cost: %d\n", starCost);
        fprintf(resultsFile, "assignment: ");
        for (i = 0; i < m; i++) {
            fprintf(resultsFile, "%d ", assignment[i]);
        }
        fprintf(resultsFile, "\n");
        fprintf(resultsFile, "time: %f\n", time_taken);
    }    
    fprintf(resultsFile, "################################################\n");
    fclose(myFile);
}