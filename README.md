Interference-aware-scheduler
============================

Scheduling algorithm that takes into account perfomance slowdowns in nodes to better assign tasks on a cloud using SimGrid. 
The slowdowns are considered to be caused by virtualization technology being unable to share equally some resources (like cache memory and disk access), so it attemps to schedule tasks that depend on different resources to the same node, to avoid conflicting acess to shared resources. 
