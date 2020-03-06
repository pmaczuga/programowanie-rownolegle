#!/bin/bash
echo -e "\nOn single node"
mpiexec -machinefile ./single-node -np 2 ./task_1.o single-node.txt

echo -e "\nOn single node, not shared memory"
mpiexec -machinefile ./single-node -np 2 -env MPIR_CVAR_CH3_NOLOCAL 1 ./task_1.o single-node-nolocal.txt

echo -e "\nDifferent nodes, same host"
mpiexec -machinefile ./two_nodes_same_host -np 2 ./task_1.o two_nodes_same_host.txt

echo -e "\nOn different hosts"
mpiexec -machinefile ./two_nodes_other_hosts -np 2 ./task_1.o two_nodes_other_hosts.txt
