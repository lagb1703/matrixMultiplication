#!/bin/sh
mpicc -O2 -o tradicional.a tradicional.c
mkdir -p csv
echo "workers,500,1000,1500,2000,2500,3000,3500,4000" >csv/tradicional.csv
for z in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30; do
    for j in 2 4 8 12 16 32; do
        echo -n $j >>csv/tradicional.csv
        echo -n "," >>csv/tradicional.csv
    for i in 1000 1500 2000 2500 3000 3500 4000; do
        mpiexec --bind-to core:overload-allowed -hostfile hosts -n $j ./tradicional.a $i >>csv/tradicional.csv
        if [ $i != 4000 ]; then
            echo -n "," >>csv/tradicional.csv
        fi
    done
    echo "" >>csv/tradicional.csv
    done
done