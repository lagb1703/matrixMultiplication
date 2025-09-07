#include "stdint.h"
#include "malloc.h"
#include <stdlib.h>
#include "time.h"
#include "thread.c"
#include <stdio.h>
#define I32 int32_t
#define UI32 uint32_t
#define MATRIX I32 **
#define ARGSNUM 3
#define MAXNUM 10

typedef struct
{
    MATRIX a;
    MATRIX b;
    MATRIX c;
    UI32 i, n;
} Data;

void freeMatrix(MATRIX a, I32 n)
{
    for (UI32 i = 0; i < n; i++)
    {
        free(a[i]);
    }
    free(a);
}

void print(MATRIX a, UI32 n)
{
    printf("{");
    for (UI32 i = 0; i < n; i++)
    {
        printf("{");
        for (UI32 j = 0; j < n; j++)
        {
            printf("%i", a[i][j]);
            if (j < n - 1)
            {
                printf(",");
            }
        }
        printf("}");
        if (i < n - 1)
        {
            printf(",");
        }
    }
    printf("}\n");
    for (UI32 i = 0; i < n; i++)
    {
        for (UI32 j = 0; j < n; j++)
        {
            printf("%i ", a[i][j]);
        }
        printf("\n");
    }
}

void inter(void *d)
{
    Data *data = (Data *)d;
    UI32 n = data->n;
    UI32 i = data->i;
    MATRIX result = data->c;
    MATRIX a = data->a;
    MATRIX b = data->b;
    for (I32 j = 0; j < n; j++)
    {
        result[i][j] = 0;
        for (I32 k = 0; k < n; k++)
        {
            result[i][j] += a[i][k] * b[k][j];
        }
    }
    killMyThread(NULL);
}

MATRIX multCuadratica(MATRIX a, MATRIX b, UI32 n, UI32 numTreads)
{
    MATRIX result = (MATRIX)malloc(sizeof(I32 *) * n);
    Data *data = (Data *)malloc(sizeof(Data) * numTreads);
    for (UI32 i = 0; i < numTreads; i++)
    {
        data[i].a = a;
        data[i].b = b;
        data[i].c = result;
        data[i].n = n;
    }
    Thread *thread = (Thread *)malloc(sizeof(Thread) * numTreads);
    for (I32 i = 0; i < n; i++)
    {
        result[i] = (I32 *)malloc(sizeof(I32) * n);
        data[i % numTreads].i = i;
        // printf("crear hilo %i\n", i % numTreads);
        createThread(inter, &data[i % numTreads], &thread[i % numTreads]);
        if ((i + 1) % numTreads == 0 || i == n - 1)
        {
            for (UI32 j = 0; j < numTreads; j++)
            {
                // printf("esperar %i\n", j);
                if (thread[j])
                    joinThread(thread[j]);
            }
        }
    }
    // for (UI32 j = 0; j < (n % numTreads == 0 ? numTreads : n % numTreads); j++)
    // {
    //     printf("esperar %i\n", j);
    //     joinThread(&thread[j]);
    // }
    free(data);
    return result;
}

MATRIX randomMatrix(UI32 n)
{
    MATRIX result = (MATRIX)malloc(sizeof(I32 *) * n);
    for (UI32 i = 0; i < n; i++)
    {
        result[i] = (I32 *)malloc(sizeof(I32) * n);
        for (UI32 j = 0; j < n; j++)
        {
            result[i][j] = rand() % MAXNUM;
        }
    }
    return result;
}

int main(int argc, char **argv)
{
    if (argc != ARGSNUM)
    {
        return 1;
    }
    UI32 n = (UI32)atoi(argv[1]);
    UI32 numTreads = (UI32)atoi(argv[2]);
    // UI32 n = 4;
    // UI32 numTreads = 20;
    srand(time(NULL));
    MATRIX a = randomMatrix(n);
    // printf("matrix a\n");
    // print(a, n);
    MATRIX b = randomMatrix(n);
    // printf("matrix b\n");
    // print(b, n);
    MATRIX c = multCuadratica(a, b, n, numTreads);
    // printf("matrix c\n");
    // print(c, n);
    freeMatrix(a, n);
    freeMatrix(b, n);
    freeMatrix(c, n);
    return 0;
}