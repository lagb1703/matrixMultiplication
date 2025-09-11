#include "stdint.h"
#include "malloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdalign.h>
#include <semaphore.h>
#include "time.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#define SHM_NAME "/arrays"
#define SEM_NAME "/semC"
#define I32 int32_t
#define UI32 uint32_t
#define ARGSNUM 3
#define MAXNUM 5

typedef struct
{
    size_t a_off, b_off, c_off, div_off, total_size;
    UI32 n, processCount;
} Data;

size_t round_up(size_t x, size_t a) { return (x + a - 1) & ~(a - 1); }

typedef struct
{
    UI32 start, end;
} range;

range *getWorkingRange(Data *d)
{
    sem_t *semC = sem_open(SEM_NAME, 0);
    if (semC == SEM_FAILED)
    {
        perror("Error abriendo semáforo en getWorkingRange");
        return NULL;
    }

    sem_wait(semC);
    UI32 *init = (I32 *)((char *)d + d->div_off);
    d->processCount--;
    range *r = (range *)malloc(sizeof(range));
    r->start = *init;
    if (!d->processCount && r->start + *(init + 1) != d->n)
    {
        r->end = d->n;
        sem_post(semC);
        sem_close(semC);
        return r;
    }
    r->end = *init + *(init + 1);
    *init = r->end;
    sem_post(semC);
    sem_close(semC);
    return r;
}

void print(I32 *a, I32 n)
{
    printf("{");
    for (UI32 i = 0; i < n; i++)
    {
        printf("{");
        for (UI32 j = 0; j < n; j++)
        {
            printf("%i", a[i * n + j]);
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
            printf("%i ", a[i * n + j]);
        }
        printf("\n");
    }
}

void multCuadratica(Data *data)
{
    I32 *a = (I32 *)((char *)data + data->a_off);
    I32 *b = (I32 *)((char *)data + data->b_off);
    I32 *c = (I32 *)((char *)data + data->c_off);
    UI32 n = data->n, start = 0, end = 0;
    range *r = getWorkingRange(data);
    start = r->start;
    end = r->end;
    // printf("start: %i, end: %i\n", start, end);
    free(r);
    for (; start < end; start++)
    {
        for (UI32 i = 0; i <= start; i++)
        {
            c[start * n + start] += a[start * n + i] * b[i * n + start];
        }
        for (UI32 i = 0; i < start; i++)
        {
            for (UI32 j = 0; j < start; j++)
            {
                c[i * n + start] += a[i * n + j] * b[j * n + start];
            }
            c[i * n + start] += a[i * n + start] * b[start * n + start];
        }
        for (UI32 i = 0; i < start; i++)
        {
            for (UI32 j = 0; j < start; j++)
            {
                c[start * n + i] += a[start * n + j] * b[j * n + i];
            }
            c[start * n + i] += a[start * n + start] * b[start * n + i];
        }
        for (UI32 i = 0; i < start; i++)
        {
            for (UI32 j = 0; j < start; j++)
            {
                c[i * n + j] += a[i * n + start] * b[start * n + j];
            }
        }
    }
}

void randomMatrix(I32 *matrix, UI32 n)
{
    for (UI32 i = 0; i < n; i++)
    {
        for (UI32 j = 0; j < n; j++)
        {
            matrix[i * n + j] = rand() % MAXNUM;
        }
    }
}

void init(I32 *matrix, UI32 n)
{
    for (UI32 i = 0; i < n; i++)
    {
        for (UI32 j = 0; j < n; j++)
        {
            matrix[i * n + j] = 0;
        }
    }
}

Data *createSharedMemory(UI32 n, UI32 nProcess)
{
    int shm_fd;
    Data *ptr;
    size_t bytesA = n * n * sizeof(I32), bytesB = bytesA, bytesC = bytesA;
    size_t baseData = round_up(sizeof(Data), alignof(I32));
    size_t offA = baseData;
    size_t offB = round_up(offA + bytesA, alignof(I32));
    size_t offC = round_up(offB + bytesB, alignof(I32));
    size_t offDiv = round_up(offC + bytesC, alignof(I32));
    size_t total = round_up(offDiv + sizeof(UI32) * 2, sysconf(_SC_PAGESIZE));
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, total);
    ptr = mmap(NULL, total, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);
    ptr->n = n;
    ptr->a_off = offA;
    ptr->b_off = offB;
    ptr->c_off = offC;
    ptr->processCount = nProcess + 1;
    ptr->div_off = offDiv;
    ptr->total_size = total;
    I32 *a = (I32 *)((char *)ptr + ptr->a_off);
    I32 *b = (I32 *)((char *)ptr + ptr->b_off);
    I32 *c = (I32 *)((char *)ptr + ptr->c_off);
    UI32 *i = (I32 *)((char *)ptr + ptr->div_off);
    *i = 1;
    *(i + 1) = (n-1) / (nProcess + 1);
    // printf("space: %i\n", *(i + 1));
    randomMatrix(a, n);
    randomMatrix(b, n);
    init(c, n);
    return ptr;
}

void deleteSharedMemory(Data *d)
{
    sem_unlink(SEM_NAME);
    munmap(d, d->total_size);
    shm_unlink(SHM_NAME);
}

int main(int argc, char **argv)
{
    if (argc != ARGSNUM)
    {
        return 1;
    }
    UI32 n = (UI32)atoi(argv[1]);
    UI32 nProcess = (UI32)atoi(argv[2]);
    // UI32 n = 10;
    // UI32 nProcess = 10;
    struct timespec start, end;
    int parentId = getpid();
    srand(1);
    sem_unlink(SEM_NAME);
    sem_t *s = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0600, 1);
    if (s == SEM_FAILED)
    {
        perror("Error creando semáforo s");
        return 1;
    }
    // sem_close(s);
    // sem_unlink(SEM_NAME);
    // return 0;
    Data *data = createSharedMemory(n, nProcess);
    // I32 *a = (I32 *)((char *)data + data->a_off);
    // I32 *b = (I32 *)((char *)data + data->b_off);
    // printf("matrix a\n");
    // print(a, n);
    // printf("matrix b\n");
    // print(b, n);
    I32 *c = (I32 *)((char *)data + data->c_off);
    pid_t *p = (pid_t *)malloc(sizeof(pid_t) * nProcess);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < nProcess && parentId == getpid(); i++)
        p[i] = fork();
    multCuadratica(data);
    if (parentId == getpid())
    {
        for (int i = 0; i < nProcess; i++)
            waitpid(p[i], NULL, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("%.6f,\n", elapsed);
        // print(c, n);
        sem_post(s);
        sem_close(s);
        free(p);
        deleteSharedMemory(data);
        return 0;
    }
    return 0;
}