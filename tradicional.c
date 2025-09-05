#include "stdint.h"
#include "malloc.h"
#include <stdlib.h>
#include "time.h"
#include <stdio.h>
#define I32 int32_t
#define UI32 uint32_t
#define MATRIX I32 **
#define ARGSNUM 2
#define MAXNUM 10

void freeMatrix(MATRIX a, I32 n)
{
    for (UI32 i = 0; i < n; i++)
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
    srand(time(NULL));
    MATRIX a = randomMatrix(n);
    // printf("matrix a\n");
    // print(a, n);
    MATRIX b = randomMatrix(n);
    // printf("matrix b\n");
    // print(b, n);
    MATRIX c = multCuadratica(a, b, n);
    // printf("matrix c\n");
    // print(c, n);
    freeMatrix(a, n);
    freeMatrix(b, n);
    freeMatrix(c, n);
    return 0;
}