#!/bin/sh
# Crear la carpeta csv si no existe
gcc ./libro.c -o libro.out
gcc ./tradicional.c -o tradicional.out
mkdir -p csv
echo "0, 500,1000,1500,2000,2500,3000" >csv/libro.csv
for z in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30; do
    for j in 4 8 12 16; do
        echo -n $j >>csv/libro.csv
        echo -n "," >>csv/libro.csv
    for i in 1000 1500 2000 2500 3000 3500 4000; do
        ./libro.out $i $j >>csv/libro.csv
        if [ $i != 4000 ]; then
            echo -n "," >>csv/libro.csv
        fi
    done
    echo "" >>csv/libro.csv
    done
done

echo "0, 500,1000,1500,2000,2500,3000" >csv/tradicional.csv
for z in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30; do
    for j in 4 8 12 16; do
        echo -n $j >>csv/tradicional.csv
        echo -n "," >>csv/tradicional.csv
    for i in 1000 1500 2000 2500 3000 3500 4000; do
        ./tradicional.out $i $j >>csv/tradicional.csv
        if [ $i != 4000 ]; then
            echo -n "," >>csv/tradicional.csv
        fi
    done
    echo "" >>csv/tradicional.csv
    done
done