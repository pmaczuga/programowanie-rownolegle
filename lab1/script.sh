#!/bin/bash
mpiexec -machinefile ./single-node -np 2 ./task-1 single-node.txt
mpiexec -machinefile ./single-node -np 2 -env MPIR_CVAR_CH3_NOLOCAL 1 ./task-1 single-node-nolocal.txt
mpiexec -machinefile ./two_nodes_same_host -np 2 ./task-1 two_nodes_same_host.txt
mpiexec -machinefile ./two_nodes_other_hosts -np 2 ./task-1 two_nodes_other_hosts.txt
