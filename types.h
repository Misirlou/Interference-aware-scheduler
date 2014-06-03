#ifndef _INTERFERENCE_TYPES_H_
#define _INTERFERENCE_TYPES_H_


#define MAXTASKS 20

#define DP_INTENSITY 100
#define MIG_SPEED 300


typedef struct{
  msg_task_t task;
  int task_number;
  unsigned char type;
  /* TYPES!
  0->cpu
  1->mem
  2->disk
  */
  double clock_created;
  double clock_start;
  double clock_end;

  int ncpus;
  int ramsize;
  int net_cap;
  char disk_path[5];
  int disksize;
  const char *hostname;
} s_task_data_t, *task_data_t;

typedef struct{
	int ntasks;
  int avail_ram;
  int avail_net;
  int avail_cpus;
  int avail_disk;
  task_data_t tasks[MAXTASKS];
} s_host_data_t, *host_data_t;



#endif