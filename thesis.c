#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#include "simulated_annealing.h"
#include "tabu_search.h"
#include "iterated_local_search.h"
#include "grasp.h"
#include "threshold_accepting.h"
#include "vns_initial_simulated_annealing.h"

int main() {
    DIR *d;
    struct dirent *dir;
    char path[20]="data";
    d = opendir(path);
    char full_path[1000];
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            //Condition to check regular file.
            if(dir->d_type==DT_REG){
                full_path[0]='\0';
                strcat(full_path,path);
                strcat(full_path,"/");
                strcat(full_path,dir->d_name);
                printf("%s\n",full_path);

                char dataPath[50];
                strcpy(dataPath, full_path);

                char delim[] = "/";
                char *ptr = strtok(full_path, delim);
                ptr = strtok(NULL, delim);
                //kanw to path tou results
                char resultsFileName[50] = "results/";
                int length, j;
                length = 0;
                while (resultsFileName[length] != '\0') {
                    ++length;
                }
                for (j = 0; ptr[j] != '\0'; ++j, ++length) {
                    resultsFileName[length] = ptr[j];
                }
                resultsFileName[length] = '\0';

                FILE *resultsFile;
                resultsFile = fopen(resultsFileName, "w");

                if (resultsFile == NULL) {
                    printf("Error Writing File\n");
                    exit (0);
                }

                simulated_annealing(dataPath, resultsFile);
                tabu_search(dataPath, resultsFile);
                iterated_local_search(dataPath, resultsFile);
                grasp(dataPath, resultsFile);
                threshold_accepting(dataPath, resultsFile);
                vns_initial_simulated_annealing(dataPath, resultsFile);
                fclose(resultsFile);
            }
        }
        closedir(d);
    }


    return 0;
}