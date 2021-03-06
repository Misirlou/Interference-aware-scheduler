#ifndef _INTERFERENCE_COMMON_H_
#define _INTERFERENCE_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "msg/msg.h"
#include "xbt/sysdep.h"         
#include "xbt/log.h"
#include "xbt/asserts.h"

#include "types.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(interference,
 "Messages specific for this msg example");




xbt_dynar_t hosts_data;
xbt_dynar_t tasks_data;
xbt_dynar_t hosts_dynar;
int vmnum=0;

double interference_matrix[3][3]={{0.97,0.95,0.93},{0.90,0.85,0.83},{0.85,0.82,0.70}};

static void create_host_data(xbt_dynar_t hosts)
{
  unsigned long len=xbt_dynar_length(hosts);
  hosts_data=xbt_dynar_new(sizeof(host_data_t),&xbt_free_ref);
  host_data_t data;
  msg_host_t pm;
  unsigned long i,j;
  for (i=0;i<len;i++)
  {
    data=xbt_new(s_host_data_t,1);
    data->ntasks=0;
    for (j=0;j<MAXTASKS;j++)
    {
      data->tasks[j]=NULL;
    }
    
    pm=xbt_dynar_get_as(hosts, i, msg_host_t);
    data->avail_ram=1024*16;
    data->avail_net=1024;
    data->avail_cpus=MSG_host_get_core_number(pm);
    data->avail_disk=1024*1024*4;
    
    MSG_host_set_data(pm,data);
    xbt_dynar_push(hosts_data,&data);
  }
}

task_data_t create_task_data(msg_task_t task, char type)
{
  task_data_t ret= xbt_new(s_task_data_t,1);
  ret->task=task;
  ret->type=type;
  ret->task_number=0;
  ret->clock_created=MSG_get_clock();
  ret->clock_start=0;
  ret->clock_end=-1;
  ret->ncpus=1;
  ret->ramsize=2048;
  ret->net_cap=100;
  ret->disksize=1024*100;//100GB
  sprintf(ret->disk_path,"/s/");
  xbt_dynar_push(tasks_data,&ret);
  MSG_task_set_data(task, ret);
  return ret;
}

static double get_pm_interference(host_data_t host_data, unsigned char task_type)
{
  double acc=1.0/interference_matrix[task_type][task_type];
  int i;
  for (i=0;i<MAXTASKS;i++)
  {
    if (host_data->tasks[i]!=NULL) acc*=interference_matrix[task_type][host_data->tasks[i]->type];
  }
  XBT_DEBUG("Interference %f",acc);
  return acc;
}

static int task_computation()
{
  //XBT_INFO("task");
  task_data_t task_data = MSG_process_get_data(MSG_process_self());
  msg_task_t task=task_data->task;
  msg_vm_t vm=MSG_process_get_host(MSG_process_self());
  msg_host_t host=MSG_vm_get_pm(vm);
  host_data_t host_data=MSG_host_get_data(host);
  int cores=MSG_host_get_core_number(vm);
  double flops=MSG_get_host_speed(vm);
  double amount,remaining;
  task_data->hostname=MSG_host_get_name(host);
  task_data->clock_start=MSG_get_clock();
  remaining=MSG_task_get_remaining_computation(task);
  host_data->tasks[task_data->task_number]=task_data;
  host_data->ntasks++;

  XBT_DEBUG("task has %d core(s), %0.0f flops/s per each, computation %0.0f",cores, flops,remaining);

  while (remaining>0)
  {
    MSG_process_sleep(5);
    amount=5*cores*flops*get_pm_interference(host_data,task_data->type);
    if (remaining-amount<=10) 
    {
      amount=remaining;
    }
    MSG_task_set_compute_duration(task,remaining-amount);
    remaining=MSG_task_get_remaining_computation(task);  
    XBT_DEBUG("task did %0.0f ops, remaining %0.0f",amount,remaining);

  }

  XBT_DEBUG("Task terminated");
  //XBT_DEBUG("Task pointers %p %p %p %p %p",host,MSG_host_get_data(host),vm,task,task_data);
  task_data->clock_end=MSG_get_clock();
  host_data->tasks[task_data->task_number]=NULL;
  host_data->ntasks--;
  MSG_task_destroy(task);
  task_data->task=NULL;
  MSG_vm_destroy(vm);
  
  return 0;
}

static void launch_task_computation(msg_host_t host,task_data_t task_data)
{
  char pr_name[35];
  sprintf(pr_name,"comp_proc%s",MSG_host_get_name(host));
  MSG_process_create(pr_name, task_computation, task_data, host);
}

static void allocate_vm(msg_host_t host,task_data_t task_data)
{
  char vm_name[25];
  sprintf(vm_name,"vm%d@%s",vmnum,MSG_host_get_name(host));
  vmnum++;
  int i;
  host_data_t host_data=MSG_host_get_data(host);
  for (i=0;i<MAXTASKS;i++)
    if (host_data->tasks[i]==NULL) break;
  task_data->task_number=i;
  msg_vm_t vm = MSG_vm_create(host, vm_name,task_data->ncpus,task_data->ramsize,task_data->net_cap,task_data->disk_path,task_data->disksize,MIG_SPEED,DP_INTENSITY);
  MSG_vm_start(vm);
  launch_task_computation(vm,task_data);
}


void schedule(task_data_t task);

static task_data_t read_task(char *line)
{
  const char* tok;
  const char* str[15];
  int i=0;
  for (tok=strtok(line,",");tok&&*tok;tok=strtok(NULL,",\n"))
  {
    str[i]=tok;
    i++;
  }
  //write function "%d,task%d,%c,%f,%d,%d,%d,%d,%d\n",timer,i,type,computation_amount,cores,ram,disk,net_cap
  msg_task_t task=MSG_task_create(str[1], atof(str[3]), 0, NULL);
  task_data_t task_data=create_task_data(task,str[2][0]-'a');
  task_data->clock_created=atoi(str[0]);
  task_data->ncpus=atoi(str[4]);
  task_data->ramsize=atoi(str[5]);
  task_data->disksize=atoi(str[6]);
  task_data->net_cap=atoi(str[7]);
  return task_data;
}

static int master_main(int argc, char *argv[])
{
  
  char *taskfile=argv[0];
  FILE *fx;
  task_data_t task_data;
  fx=fopen(taskfile,"r");
  char line[1024];
  while(fgets(line,1024,fx))
  {
    XBT_DEBUG("read line %s ",line);
    task_data=read_task(line);
    MSG_process_sleep(task_data->clock_created-MSG_get_clock());
    schedule(task_data);
  }

  XBT_INFO("scheduling complete");
  return 0;
}

void print_data(char* type,char *hostfile,char *taskfile,char *printfile)
{
  unsigned int i;
  FILE *fx;
  //setlocale(LC_NUMERIC, "French_Canada.1252");
  fx=fopen(printfile,"w+");
  task_data_t task_data;

  fprintf(fx,"%s\t%s\t%s\n\n\n",type,hostfile,taskfile);
  xbt_dynar_foreach(tasks_data,i,task_data)
  {
    fprintf(fx,"%c\t%5.0f\t%5.0f\t%5.0f\t%s\t%d\t%d\t%d\n",task_data->type+'0',task_data->clock_created,task_data->clock_start,task_data->clock_end,task_data->hostname,task_data->ncpus,task_data->disksize,task_data->ramsize);
  }
  fprintf(fx, "\n" );
  fclose(fx);
  //printf("coiso\n");
}


#endif