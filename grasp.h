#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define GRASP_ITERATIONS 1000

void grasp (char dataPath[], FILE *resultsFile) {
    printf("GRASP\n");
    fprintf(resultsFile, "GRASP\n");
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
        int costPerAgent[n];
        for (i = 0; i < n; i++)
        {
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
                    if (costs[i][j] <= costForSecondChance) {
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
        int starCost = cost;
        int initialAssignment[m];
        for (i = 0; i < m; i++) {
            initialAssignment[i] = assignment[i];
        }
        starCost = cost;
        initialAssignment[m];
        for (i = 0; i < m; i++)
        {
            initialAssignment[i] = assignment[i];
        }
        int iter = 0;
        do //find a better solution (cost)
        {
            int costOfChanged;
            int changedCostPerAgent[n];
            int changedAssignment[m];
            int changedFreeResources[n];
            do {
                for (i = 0; i < n; i++)
                {
                    changedCostPerAgent[i] = 0;
                }
                
                for (i = 0; i < m; i++)
                {
                    changedAssignment[i] = initialAssignment[i];
                }
                
                for (i = 0; i < n; i++)
                {
                    changedFreeResources[i] = capacityPerAgent[i];
                }
                costOfChanged = 0;
                int agentsToChange = (rand() % n) + 1;
                while (agentsToChange > 0)
                {
                    int agent = (rand() % n) + 1;
                    int job = rand() % m;
                    changedAssignment[job] = agent;
                    agentsToChange -= 1;
                }
                for (j = 0; j < m; j++)
                {
                    i = changedAssignment[j] - 1;
                    if (changedFreeResources[i] >= resources[i][j])
                    {
                        changedFreeResources[i] -= resources[i][j];
                        costOfChanged += costs[i][j];
                        changedCostPerAgent[i] += costOfChanged;
                    }
                    else
                    {
                        costOfChanged += costs[i][j] + abs(changedFreeResources[i] - resources[i][j]) * penalty;
                        changedFreeResources[i] = 0;
                        changedCostPerAgent[i] += costOfChanged;
                    }
                }
            } while (costOfChanged > starCost);
            

            for (int j1 = 0; j1 < m; j1++)
            {
                for (int j2 = 0; j2 < m; j2++)
                {
                    int localSearchCost = 0; //local search
                    int localCostPerAgent[n];
                    for (i = 0; i < n; i++)
                    {
                        localCostPerAgent[i] = changedCostPerAgent[i];
                    }
                    int a1 = changedAssignment[j1] - 1;
                    int a2 = changedAssignment[j2] - 1;
                    localCostPerAgent[a1] = 0;
                    localCostPerAgent[a2] = 0;
                    //thelw na trexw ola ta jobs na parw ta cost ana agent a1 kai a2
                    int localCapacityA1 = capacityPerAgent[a1];
                    int localCapacityA2 = capacityPerAgent[a2];
                    changedAssignment[j1] = a2 + 1;
                    changedAssignment[j2] = a1 + 1;
                    for (j = 0; j < m; j++)
                    {
                        if ((changedAssignment[j] - 1) == a1)
                        {
                            if (localCapacityA1 >= resources[a1][j])
                            {
                                localCapacityA1 -= resources[a1][j];
                                localCostPerAgent[a1] += costs[a1][j];
                            }
                            else
                            {
                                localCostPerAgent[a1] += costs[a1][j] + abs(localCapacityA1 - resources[a1][j]) * penalty;
                                localCapacityA1 = 0;
                            }
                        }
                        else if ((changedAssignment[j] - 1) == a2)
                        {
                            if (localCapacityA2 >= resources[a2][j])
                            {
                                localCapacityA2 -= resources[a2][j];
                                localCostPerAgent[a2] += costs[a2][j];
                            }
                            else
                            {
                                localCostPerAgent[a2] += costs[a2][j] + abs(localCapacityA2 - resources[a2][j]) * penalty;
                                localCapacityA2 = 0;
                            }
                        }
                    }
                    for (i = 0; i < n; i++)
                    {
                        localSearchCost += localCostPerAgent[i];
                    }
                    if (localSearchCost < costOfChanged)
                    {
                        changedAssignment[j1] = a2 + 1;
                        changedAssignment[j2] = a1 + 1;
                        costOfChanged = localSearchCost;
                    } else {
                        changedAssignment[j1] = a1 + 1;
                        changedAssignment[j2] = a2 + 1;
                    }
                }
            }
            cost = costOfChanged;
            if (cost < starCost)
            {
                starCost = cost;
                for (i = 0; i < m; i++)
                {
                    initialAssignment[i] = changedAssignment[i];
                }
            }
            iter++;
        } while (iter <= GRASP_ITERATIONS);
        t = clock() - t; 
        double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
        printf("cost: %d\n", starCost);
        printf("assignment: ");
        for (i = 0; i < m; i++)
        {
            printf("%d ", initialAssignment[i]);
        }
        printf("\n");
        printf("time: %f\n", time_taken);
        fprintf(resultsFile, "cost: %d\n", starCost);
        fprintf(resultsFile, "assignment: ");
        for (i = 0; i < m; i++)
        {
            fprintf(resultsFile, "%d ", initialAssignment[i]);
        }
        fprintf(resultsFile, "\n");
        fprintf(resultsFile, "time: %f\n", time_taken);
    }
    fprintf(resultsFile, "################################################\n");
    fclose(myFile);
}