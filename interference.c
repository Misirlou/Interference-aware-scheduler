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

static void schedule(task_data_t task_data)
{
  int best=10000,bestpos=-1;
  unsigned int i;
  msg_host_t host;
  host_data_t host_data;

  xbt_dynar_foreach(hosts_dynar,i,host)
  {
    host_data=MSG_host_get_data(host);
    //filter
    if (filter_host(host_data,task_data))
    {
      //check score
      if (host_data->ntasks<best)
      {
        bestpos=i;
        best=host_data->ntasks;
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
  xbt_assert(argc == 3);
  MSG_create_environment(argv[1]);

  hosts_dynar = MSG_hosts_as_dynar();
  tasks_data = xbt_dynar_new(sizeof(task_data_t),&xbt_free_ref);
  msg_host_t pm0 = xbt_dynar_get_as(hosts_dynar, 0, msg_host_t);
  create_host_data(hosts_dynar);

  char **argv2 = xbt_new(char *, 2);
  argv2[0] = xbt_strdup(argv[2]);
  argv2[1] = NULL;
  MSG_process_create_with_arguments("master", master_main, NULL, pm0,1,argv2);

  int res = MSG_main();
  
  //host_data_t p =xbt_dynar_get_as(hosts_data,1,host_data_t);
  //XBT_DEBUG("memcheck hack %d",p[0]);
  xbt_dynar_free(&hosts_dynar);
  xbt_dynar_free(&hosts_data);
  xbt_dynar_free(&tasks_data);
  //XBT_DEBUG("memcheck hack %d",p[0]);
  XBT_INFO("Bye (simulation time %g)", MSG_get_clock());
  return !(res == MSG_OK);
}
