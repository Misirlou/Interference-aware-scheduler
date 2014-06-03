#include "common.h"

//XBT_LOG_DEFAULT_CATEGORY(interference);


static int filter_host(host_data_t host_data,task_data_t task_data)
{
  if (host_data->avail_ram<task_data->ramsize) return 0;
  if (host_data->avail_disk<task_data->disksize) return 0;
  if (host_data->avail_net<task_data->net_cap) return 0;
  if (host_data->avail_cpus<task_data->ncpus) return 0;
  return 1;
}

void schedule(task_data_t task_data)
{
  int bestpos=-1;
  unsigned int i;
  double score;
  double best=10000;
  msg_host_t host;
  host_data_t host_data;

  xbt_dynar_foreach(hosts_dynar,i,host)
  {
    host_data=MSG_host_get_data(host);
    //filter
    if (filter_host(host_data,task_data))
    {
      //check score
      score=host_data->ntasks*1000000000.0/MSG_get_host_speed(host);
      //XBT_INFO("score %f %f",score,MSG_get_host_speed(host));
      if (score<best)
      {
        bestpos=i;
        best=score;
      }
    }
  }

  host=xbt_dynar_get_as(hosts_dynar, bestpos, msg_host_t);
  allocate_vm(host,task_data);
}



int main(int argc, char *argv[])
{
  /* Get the arguments */
  MSG_init(&argc, argv);

  /* load the platform file */
  xbt_assert(argc == 4);
  MSG_create_environment(argv[1]);

  hosts_dynar = MSG_hosts_as_dynar();
  tasks_data = xbt_dynar_new(sizeof(task_data_t),&xbt_free_ref);
  msg_host_t pm0 = xbt_dynar_get_as(hosts_dynar, 0, msg_host_t);
  create_host_data(hosts_dynar);

  char **argv2 = xbt_new(char *, 50);
  argv2[0] = xbt_strdup(argv[2]);
  argv2[1] = NULL;
  MSG_process_create_with_arguments("master", master_main, NULL, pm0,1,argv2);

  int res = MSG_main();
  print_data("normal",argv[1],argv[2],argv[3]);

  XBT_INFO("Bye (simulation time %g)", MSG_get_clock());
  //host_data_t p =xbt_dynar_get_as(hosts_data,1,host_data_t);
  task_data_t td =xbt_dynar_get_as(tasks_data,1,task_data_t);
  XBT_DEBUG("memcheck hack %f",td->clock_created);
  xbt_dynar_free(&tasks_data);
  xbt_dynar_free(&hosts_dynar);
  xbt_dynar_free(&hosts_data);
  XBT_DEBUG("memcheck hack %f",td->clock_created);
  
  return !(res == MSG_OK);
}
