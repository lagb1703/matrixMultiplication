#include "stdint.h"
#include "malloc.h"
#include <stdlib.h>
#include "time.h"
#include "thread.c"
#include <stdio.h>
#define I32 int32_t
#define UI32 uint32_t
#define MATRIX I32 **
#define ARGSNUM 2
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

void multCuadratica(MATRIX a, MATRIX b, MATRIX c, UI32 n)
{
    n--;
    Thread threads[2];
    for (; n > 1; n--)
    {
        Data *d = (Data *)malloc(sizeof(Data));
        d->a = a;
        d->b = b;
        d->c = c;
        d->n = n;
        createThread(rigthVector, d, &threads[0]);
        createThread(leftVector, d, &threads[1]);
        for (UI32 i = 0; i <= n; i++)
        {
            c[n][n] += a[n][i] * b[i][n];
        }
        for (UI32 i = 0; i < n; i++)
        {
            for (UI32 j = 0; j < n; j++)
            {
                c[i][j] += a[i][n] * b[n][j];
            }
        }
        joinThread(threads[0]);
        joinThread(threads[1]);
        free(d);
    }
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
    srand(time(NULL));
    MATRIX a = randomMatrix(n);
    // printf("matrix a\n");
    // print(a, n);
    MATRIX b = randomMatrix(n);
    // printf("matrix b\n");
    // print(b, n);
    MATRIX c = init(n);
    multCuadratica(a, b, c, n);
    // printf("matrix c\n");
    // print(c, n);
    freeMatrix(a, n);
    freeMatrix(b, n);
    freeMatrix(c, n);
    return 0;
}