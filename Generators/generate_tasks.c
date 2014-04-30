#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>


#define DISK_TYPE 2

FILE *fx;
int timer;

//ARRAY MUST TERMINATE IN A VALUE >=100, start array with 0 for values bigger than 0
int dist_rand(int dist,...)
{
  va_list ptr;
  int arg;
  int number=rand()%100,i=0;
  va_start(ptr,dist);
  arg=dist;//start with first arg
  while (arg<=number){
    i++;
    arg=va_arg(ptr,int);
  }
  va_end(ptr);
  return i;
}

int disk_rand()
{
  int num=dist_rand(10,50,80,95,100);
  switch(num)
  {
    case 0:return 2048;
    case 1:return 50*1024;
    case 2:return 200*1024;
    case 3:return 500*1024;
    case 4:return 1000*1024;
  }
  return 2048;
}

void task_line(int i)
{
  timer+=dist_rand(40,60,80,100)*(rand()%5+1);
  double computation_amount=1E9*(dist_rand(0,5,10,20,35,50,65,80,90,95,100));
  int cores=dist_rand(0,75,95,95,100);
  int ram=512*dist_rand(0,50,80,80,100);
  int disk=disk_rand();
  char type;
  if (disk>=500*1024) type=DISK_TYPE;
  else if (disk<=2048) type=dist_rand(50,100);
  else {
    type=dist_rand(40,80,100);
  }
  int net_cap=128*dist_rand(30,50,85,85,100);
  fprintf(fx, "%d,task%d,%c,%f,%d,%d,%d,%d,%d\n",timer,i,(type+'a'),computation_amount,cores,ram,disk,net_cap);
}

int main(int argc,char *argv[])
{
  if (argc<3)
  {
    printf("Insufficient args. Usage: <filename> <nr of tasks>");
    return 0;
  }
  srand(time(NULL));
  timer=0;

  fx=fopen(argv[1],"w+");
  int i; 
  int ntasks=atoi(argv[2]);
  for (i=0;i<ntasks;i++)
  {
    task_line(i);
  }
  fclose(fx);
}