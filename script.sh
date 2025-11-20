#!/bin/sh
mpicc -Wall -O2 -o tradicional.a tradicional.c
mkdir -p csv
echo "500,1000,1500,2000,2500,3000" > csv/tradicional.csv
for j in 1 2 3 4 5 6 7 8 9 10; do
  for i in 1000 1500 2000 2500 3000 3500 4000; do
    mpirun -np 2 ./tradicional.a $i >> csv/tradicional.csv
    if [ "$i" != "3000" ]; then
        echo -n "," >> csv/tradicional.csv
    fi
  done
  echo "" >> csv/tradicional.csv
done