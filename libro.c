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
#define MAXNUM 5

typedef struct
{
    MATRIX a;
    MATRIX b;
    MATRIX c;
    UI32 n;
} Data;

void freeMatrix(MATRIX a, UI32 n)
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

void rigthVector(void *d)
{
    Data *data = (Data *)d;
    UI32 n = data->n;
    MATRIX a = data->a;
    MATRIX b = data->b;
    MATRIX c = data->c;
    for (UI32 i = 0; i < n; i++)
    {
        for (UI32 j = 0; j < n; j++)
        {
            c[i][n] += a[i][j] * b[j][n];
        }
        c[i][n] += a[i][n] * b[n][n];
    }
    killMyThread(NULL);
}

void leftVector(void *d)
{
    Data *data = (Data *)d;
    UI32 n = data->n;
    MATRIX a = data->a;
    MATRIX b = data->b;
    MATRIX c = data->c;
    for (UI32 i = 0; i < n; i++)
    {
        for (UI32 j = 0; j < n; j++)
        {
            c[n][i] += a[n][j] * b[j][i];
        }
        c[n][i] += a[n][n] * b[n][i];
    }
    killMyThread(NULL);
}

void scalar(void *d)
{
    Data *data = (Data *)d;
    UI32 n = data->n;
    MATRIX a = data->a;
    MATRIX b = data->b;
    MATRIX c = data->c;
    for (UI32 i = 0; i <= n; i++)
    {
        c[n][n] += a[n][i] * b[i][n];
    }
    killMyThread(NULL);
}

void centralMatrix(void *d)
{
    Data *data = (Data *)d;
    UI32 n = data->n;
    MATRIX a = data->a;
    MATRIX b = data->b;
    MATRIX c = data->c;
    for (UI32 i = 0; i < n; i++)
    {
        for (UI32 j = 0; j < n; j++)
        {
            c[i][j] += a[i][n] * b[n][j];
        }
    }
}

void multCuadratica(MATRIX a, MATRIX b, MATRIX c, UI32 n, UI32 numThreads)
{
    Thread *threads = (Thread *)malloc(sizeof(Thread) * numThreads);
    Data *data = (Data *)malloc(sizeof(Data));
    data->a = a;
    data->b = b;
    data->c = c;
    for (UI32 i = n - 1; i > 1; i--)
    {
        if ((4 * (n - i)) % numThreads == 0 && i != 0)
        {
            for (UI32 j = 0; j < numThreads; j++)
            {
                joinThread(threads[j]);
            }
        }
        data->n = i;
        createThread(rigthVector, data, &threads[(4 * i) % numThreads]);
        createThread(leftVector, data, &threads[(4 * i + 1) % numThreads]);
        createThread(scalar, data, &threads[(4 * i + 2) % numThreads]);
        createThread(centralMatrix, data, &threads[(4 * i + 3) % numThreads]);
    }
    for (UI32 j = 0; j < numThreads; j++)
    {
        joinThread(threads[j]);
    }
    free(data);
    c[0][0] += a[0][0] * b[0][0] + a[0][1] * b[1][0];
    c[0][1] += a[0][0] * b[0][1] + a[0][1] * b[1][1];
    c[1][0] += a[1][0] * b[0][0] + a[1][1] * b[1][0];
    c[1][1] += a[1][0] * b[0][1] + a[1][1] * b[1][1];
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

MATRIX init(UI32 n)
{
    MATRIX result = (MATRIX)malloc(sizeof(I32 *) * n);
    for (UI32 i = 0; i < n; i++)
    {
        result[i] = (I32 *)malloc(sizeof(I32) * n);
        for (UI32 j = 0; j < n; j++)
        {
            result[i][j] = 0;
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
    UI32 numThreads = (UI32)atoi(argv[2]);
    srand(time(NULL));
    MATRIX a = randomMatrix(n);
    // printf("matrix a\n");
    // print(a, n);
    MATRIX b = randomMatrix(n);
    // printf("matrix b\n");
    // print(b, n);
    MATRIX c = init(n);
    multCuadratica(a, b, c, n, numThreads);
    // printf("matrix c\n");
    // print(c, n);
    freeMatrix(a, n);
    freeMatrix(b, n);
    freeMatrix(c, n);
    return 0;
}