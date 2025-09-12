#!/bin/sh
# Crear la carpeta csv si no existe
gcc ./libro.c -o libro.out
gcc ./tradicional.c -o tradicional.out
mkdir -p csv
echo "500,1000,1500,2000,2500,3000" >csv/libro.csv
for j in 1; do
  for i in 500 1000 1500 2000 2500 3000; do
    ./libro.out $i 4 >>csv/libro.csv
    if [ $i != 3000 ]; then
        echo -n "," >>csv/libro.csv
    fi
  done
  echo "" >>csv/libro.csv
done

echo "500,1000,1500,2000,2500,3000" >csv/tradicional.csv
for j in 1; do
  for i in 500 1000 1500 2000 2500 3000; do
    ./tradicional.out $i 4 >>csv/tradicional.csv
    if [ $i != 3000 ]; then
        echo -n "," >>csv/tradicional.csv
    fi
  done
  echo "" >>csv/tradicional.csv
done