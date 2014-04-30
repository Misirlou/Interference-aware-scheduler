#ifndef _INTERFERENCE_TYPES_H_
#define _INTERFERENCE_TYPES_H_


#define MAXTASKS 20

typedef struct{
  msg_task_t task;
  int task_number;
  char type;
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
} s_task_data_t, *task_data_t;

typedef struct{
	int ntasks;
  task_data_t tasks[MAXTASKS];
} s_host_data_t, *host_data_t;

#define DP_INTENSITY 100
#define MIG_SPEED 300

#endif