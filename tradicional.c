#include "stdint.h"
#include "malloc.h"
#include <stdlib.h>
#include "time.h"
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <string.h>
#define I32 int32_t
#define UI32 uint32_t
#define MATRIX I32 **
#define ARGSNUM 2
#define MAXNUM 10
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

void freeMatrix(MATRIX a, I32 n)
{
    if (!a)
        return;
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

MATRIX multCuadratica(MATRIX a, MATRIX b, UI32 n, UI32 beginMatrix, UI32 endMatrix)
{
    UI32 total = endMatrix - beginMatrix;
    MATRIX result = (MATRIX)malloc(sizeof(I32 *) * total);
    for (I32 i = 0; i < total; i++)
    {
        result[i] = (I32 *)malloc(sizeof(I32) * n);
        for (I32 j = 0; j < n; j++)
        {
            result[i][j] = 0;
            for (I32 k = 0; k < n; k++)
            {
                result[i][j] += a[i + beginMatrix][k] * b[k][j];
            }
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
    I32 work = n / size_Of_Cluster;
    I32 loseWork = n % size_Of_Cluster;
    I32 beginMatrix = work * processId + min(processId, loseWork);
    I32 endMatrix = beginMatrix + work;
    if (loseWork - processId > 0)
        endMatrix += 1;
    struct timespec start, end;
    if (processId == 0)
    {
        srand(time(NULL));
        a = randomMatrix(n);
        b = randomMatrix(n);
        clock_gettime(CLOCK_MONOTONIC, &start);
        printf("matrix a\n");
        print(a, n);
        printf("matrix b\n");
        print(b, n);
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
    broadcastMatrix(a, n);
    broadcastMatrix(b, n);
    MATRIX c = multCuadratica(a, b, n, beginMatrix, endMatrix);
    MPI_Barrier(MPI_COMM_WORLD);
    if (processId == 0)
    {
        // MATRIX response = (MATRIX)malloc(sizeof(I32 *) * n);
        // UI32 total0 = endMatrix - beginMatrix;
        // printf("process %i of %i\n", processId, size_Of_Cluster);
        // for (UI32 i = 0; i < total0; i++)
        // {
        //     response[i] = (I32 *)malloc(sizeof(I32) * n);
        //     memcpy(response[i], c[i], sizeof(I32) * n);
        //     for(UI32 j = 0; j < n; j++){
        //         printf("%i ", response[i][j]);
        //     }
        //     printf("\n");
        // }
        for (int pid = 1; pid < size_Of_Cluster; pid++)
        {
            I32 pBegin = work * pid + min(pid, loseWork);
            I32 pEnd = pBegin + work;
            if (loseWork - pid > 0)
                pEnd += 1;
            UI32 pTotal = pEnd - pBegin;
            printf("%i\n", pTotal);
            if (pTotal <= 0)
                continue;
            for (UI32 i = 0; i < pTotal; i++)
            {
                response[pBegin + i] = (I32 *)malloc(sizeof(I32) * n);
                // MPI_Recv(response[pBegin + i], n, MPI_INT, pid, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        // clock_gettime(CLOCK_MONOTONIC, &end);
        // printf("result: \n");
        // print(response, n);
        // free(response);
    }
    else
    {
        UI32 total = endMatrix - beginMatrix;
        for (UI32 i = 0; i < total; i++)
        {
            // MPI_Send(c[i], n, MPI_INT, 0, 1, MPI_COMM_WORLD);
            // printf("process %i of %i\n", processId, size_Of_Cluster);
            for(UI32 j = 0; j < n; j++){
                printf("%i ", c[i][j]);
            }
        }
    }
    // freeMatrix(a, n);
    // freeMatrix(b, n);
    // freeMatrix(c, n);
    MPI_Finalize();
    return 0;
}