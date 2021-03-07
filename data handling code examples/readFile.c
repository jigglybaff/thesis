#include <stdio.h>
#include <stdlib.h>

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

        //print 
        printf("num of problem: %d\n", p+1);
        printf("%d %d\n", n, m);
        for (i = 0; i < n; i++) {
            for (j = 0; j < m-1; j++) {
                printf("%d ", costs[i][j]);
            }
            printf("%d\n", costs[i][m-1]);
        }
        for (i = 0; i < n; i++) {
            for (j = 0; j < m-1; j++) {
                printf("%d ", resources[i][j]);
            }
            printf("%d\n", resources[i][m-1]);
        }
        for (i = 0; i < n-1; i++) {
            printf("%d ", capacityPerAgent[i]);
        }
        printf("%d\n", capacityPerAgent[n-1]);
    }    

    fclose(myFile);

    return 0;
}