#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <stdio.h>
#include "../cacheutils.h"

#define ARRAY_SIZE 5120
#define PROBE_OFFSET 2048
#define N_PROBES 4194304
#define N_BINS 80

size_t array[ARRAY_SIZE];

size_t hit_histogram[N_BINS];
size_t miss_histogram[N_BINS];

size_t onlyreload(void* addr)
//Access a given address and return the number of CPU cycles taken to access that address
{
  size_t time = rdtsc();
  maccess(addr);
  size_t delta = rdtsc() - time;
  return delta;
}

size_t flushandreload(void* addr)
//Access a given address, flush the address, and return the number of CPU cycles taken to access that address
{
  size_t time = rdtsc();
  maccess(addr);
  size_t delta = rdtsc() - time;
  flush(addr);
  return delta;
}

int main(int argc, char** argv)
{
  //Initialization
  memset(array,-1,ARRAY_SIZE*sizeof(size_t)); 
  
  //Get timing for cache hits
  maccess(array + PROBE_OFFSET); 
  sched_yield();
  for (int i = 0; i < N_PROBES; ++i)
  {
    size_t d = onlyreload(array+PROBE_OFFSET); 
    hit_histogram[MIN((N_BINS - 1),d/5)]++;
    sched_yield();
  }

  //Get timing for cache misses
  flush(array+PROBE_OFFSET);
  for (int i = 0; i < N_PROBES; ++i)
  {
    size_t d = flushandreload(array+PROBE_OFFSET);
    miss_histogram[MIN((N_BINS - 1),d/5)]++;
    sched_yield();
  }

  //printing
  printf(".\n");
  size_t hit_max = 0;
  size_t hit_max_i = 0;
  size_t miss_min_i = 0;
  FILE *outfile;
  outfile = fopen("histogram.csv", "w");
  for (int i = 0; i < N_BINS; ++i)
  {
    fprintf(outfile, "%3d\t%10zu\t%10zu\n",i*5,hit_histogram[i],miss_histogram[i]);
    if (hit_max < hit_histogram[i])
    {
      hit_max = hit_histogram[i];
      hit_max_i = i;
    }
    if (miss_histogram[i] > 3 && miss_min_i == 0)
      miss_min_i = i;
  }
  if (miss_min_i > hit_max_i+4)
    printf("Flush+Reload possible!\n");
  else if (miss_min_i > hit_max_i+2)
    printf("Flush+Reload probably possible!\n");
  else if (miss_min_i < hit_max_i+2)
    printf("Flush+Reload maybe not possible!\n");
  else
    printf("Flush+Reload not possible!\n");
  size_t min = -1UL;
  size_t min_i = 0;
  for (int i = hit_max_i; i < miss_min_i; ++i)
  {
    if (min > (hit_histogram[i] + miss_histogram[i]))
    {
      min = hit_histogram[i] + miss_histogram[i];
      min_i = i;
    }
  }
  fclose(outfile);
  printf("The lower the threshold, the lower the number of false positives.\n");
  printf("Suggested cache hit/miss threshold: %zu\n",min_i * 5);
  return min_i * 5;
}

