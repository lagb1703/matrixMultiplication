#include "stdint.h"
#include "malloc.h"
#include <stdlib.h>
#include "time.h"
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#define I32 int32_t
#define UI32 uint32_t
#define MATRIX I32 **
#define ARGSNUM 2
#define MAXNUM 10

void freeMatrix(MATRIX a, I32 n)
{
    if (!a)
        return;
    // if (n == 0)
    // {
    //     free(a);
    //     return;
    // }
    // if (n == 1)
    // {
    //     free(a[0]);
    //     free(a);
    //     return;
    // }
    // if (a[1] == a[0] + n)
    // {
    //     free(a[0]);
    //     free(a);
    //     return;
    // }
    for (I32 i = 0; i < n; i++)
    {
        free(a[i]);
    }
    free(a);
}

void print(MATRIX a, I32 n)
{
    for (UI32 i = 0; i < n; i++)
    {
        for (UI32 j = 0; j < n; j++)
        {
            printf("%i ", a[i][j]);
        }
        printf("\n");
    }
}

MATRIX multCuadratica(MATRIX a, MATRIX b, UI32 n)
{
    MATRIX result = (MATRIX)malloc(sizeof(I32 *) * n);
    for (I32 i = 0; i < n; i++)
    {
        result[i] = (I32 *)malloc(sizeof(I32) * n);
        for (I32 j = 0; j < n; j++)
        {
            I32 partial = 0;
            for (I32 k = 0; k < n; k++)
            {
                partial += a[i][k] * b[k][j];
                // printf("%i a%i,%i %i b%i, %i\n", a[i][k], i+1, k+1, b[k][j], k+1, j+1);
            }
            // printf("\n");
            result[i][j] = partial;
        }
    }
    return result;
}

MATRIX randomMatrix(UI32 n)
{
    MATRIX result = (MATRIX)malloc(sizeof(I32 *) * n);
    I32 *data = (I32 *)malloc(sizeof(I32) * n * n);
    for (UI32 i = 0; i < n; i++)
    {
        result[i] = data + i * n;
        for (UI32 j = 0; j < n; j++)
        {
            result[i][j] = rand() % MAXNUM;
        }
    }
    return result;
}

void broadcastMatrix(MATRIX a, UI32 n)
{
    for (int i = 0; i < n; i++)
    {
        printf("Esto es una mala idea");
        MPI_Bcast(a[i], n, MPI_INT, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char **argv)
{
    int processId, size_Of_Cluster;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    if (argc != ARGSNUM)
    {
        MPI_Finalize();
        return 1;
    }
    UI32 n = (UI32)atoi(argv[1]);
    MATRIX a;
    MATRIX b;
    struct timespec start, end;
    if (processId == 0)
    {
        srand(time(NULL));
        a = randomMatrix(n);
        b = randomMatrix(n);
        clock_gettime(CLOCK_MONOTONIC, &start);
    }
    else
    {
        a = (MATRIX)malloc(sizeof(I32 *) * n);
        b = (MATRIX)malloc(sizeof(I32 *) * n);
        for (UI32 i = 0; i < n; i++)
        {
            a[i] = (I32 *)malloc(sizeof(I32) * n);
            b[i] = (I32 *)malloc(sizeof(I32) * n);
        }
    }
    printf("process %i of %i\n", processId, size_Of_Cluster);
    printf("matrix a\n");
    print(a, n);
    MPI_Barrier(MPI_COMM_WORLD);
    if (processId == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &end);
    }
    freeMatrix(a, n);
    freeMatrix(b, n);
    // freeMatrix(c, n);
    MPI_Finalize();
    return 0;
}