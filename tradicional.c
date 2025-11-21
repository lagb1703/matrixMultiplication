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
    if (n <= 0)
    {
        free(a);
        return;
    }
    I32 *first = a[0];
    if (!first)
    {
        for (I32 i = 0; i < n; i++)
        {
            if (a[i])
                free(a[i]);
        }
        free(a);
        return;
    }
    int is_contiguous = 1;
    for (I32 i = 0; i < n; i++)
    {
        if (a[i] != first + (I32)((I32)i * (I32)n))
        {
            is_contiguous = 0;
            break;
        }
    }
    if (is_contiguous)
    {
        free(first);
    }
    else
    {
        for (I32 i = 0; i < n; i++)
        {
            if (a[i])
                free(a[i]);
        }
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

MATRIX multCuadratica(MATRIX a, MATRIX b, UI32 n, UI32 processId, UI32 size_Of_Cluster)
{
    I32 work = n / size_Of_Cluster;
    I32 loseWork = n % size_Of_Cluster;
    I32 beginMatrix = work * processId + min(processId, loseWork);
    I32 endMatrix = beginMatrix + work;
    if (loseWork - processId > 0)
        endMatrix += 1;
    UI32 total = endMatrix - beginMatrix;
    printf("2\n");
    MATRIX c = (MATRIX)malloc(sizeof(I32 *) * n);
    printf("2.1, begin = %i\n", beginMatrix);
    for (I32 i = 0; i < total; i++)
    {
        c[i] = (I32 *)malloc(sizeof(I32) * n);
        for (I32 j = 0; j < n; j++)
        {
            c[i][j] = 0;
            for (I32 k = 0; k < n; k++)
            {
                c[i][j] += a[i + beginMatrix][k] * b[k][j];
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (processId == 0)
    {
        printf("3\n");
        for (int pid = 1; pid < size_Of_Cluster; pid++)
        {
            I32 pBegin = work * pid + min(pid, loseWork);
            I32 pEnd = pBegin + work;
            if (loseWork - pid > 0)
                pEnd += 1;
            UI32 pTotal = pEnd - pBegin;
            if (pTotal <= 0)
                continue;
            for (UI32 i = total; i < total + pTotal; i++)
            {
                printf("%i\n", i);
                c[i] = (I32 *)malloc(sizeof(I32) * n);
                MPI_Recv(c[i], n, MPI_INT, pid, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            total += pTotal;
        }
        printf("pepe el mago\n");
        return c;
    }
    for (UI32 i = 0; i < total; i++)
    {
        MPI_Send(c[i], n, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    return c;
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
    struct timespec start, end;
    if (processId == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        printf("1\n");
        srand(time(NULL));
        a = randomMatrix(n);
        b = randomMatrix(n);
        // printf("matrix a\n");
        // print(a, n);
        // printf("matrix b\n");
        // print(b, n);
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
    printf("1.5\n");
    MATRIX c = multCuadratica(a, b, n, processId, size_Of_Cluster);
    printf("pepe el mago\n");
    // print(c, n);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f", elapsed);
    clock_gettime(CLOCK_MONOTONIC, &end);
    freeMatrix(a, n);
    freeMatrix(b, n);
    // free(c);
    // freeMatrix(c, n);
    MPI_Finalize();
    return 0;
}