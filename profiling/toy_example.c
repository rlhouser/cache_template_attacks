#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <stdint.h>
#include <unistd.h>

#define ROWS 10
#define COLUMNS 1024
int map[ROWS][COLUMNS] = {{1},{2},{3},{4},{5},{6},{7},{8},{9},{10}};

int main(int argc, char**argv){
        printf("Running toy example...\n");
	for (int i = 0; i < ROWS; i++){
                int k = 0;
		printf("%i %p\n", i, map[i]);
        }
	FILE *fp;
        char buff[1];
        while(1){
                fp = fopen("char.txt", "r");
                int i = fscanf(fp, "%c", buff);
                int c;
                if (!sscanf(buff, "%i", &c))
                        continue;
		if(map[(c%ROWS)][0] == 0){
                        exit(-1);
                }
		usleep(1000);
                for (int i = 0; i < 25; ++i){
                        sched_yield();
                }
                fclose(fp);
        }
}
