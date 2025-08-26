#include <stdint.h>
#include <stdlib.h>

typedef void (*ThreadFunc)(void *);

#ifdef _WIN32
#include <windows.h>
typedef HANDLE Thread;

typedef struct
{
    ThreadFunc func;
    void *arg;
} ThreadStart;

static DWORD WINAPI thread_start_wrapper(LPVOID lpParameter)
{
    ThreadStart *ts = (ThreadStart *)lpParameter;
    if (ts && ts->func)
        ts->func(ts->arg);
    free(ts);
    return 0;
}

uint32_t createThread(ThreadFunc func, void *arg, Thread *thread)
{
    ThreadStart *ts = (ThreadStart *)malloc(sizeof *ts);
    if (!ts)
        return 0;
    ts->func = func;
    ts->arg = arg;

    HANDLE h = CreateThread(NULL, 0, thread_start_wrapper, ts, 0, NULL);
    if (!h)
    {
        free(ts);
        return 0;
    }
    *thread = h;
    return 1;
}

void joinThread(Thread thread)
{
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}

void killMyThread(void *value_ptr)
{
    (void)value_ptr;
    ExitThread(0);
}

#elif defined(__linux__)
#include <pthread.h>
typedef pthread_t Thread;

typedef struct
{
    ThreadFunc func;
    void *arg;
} ThreadStart;

static void *thread_start_wrapper(void *p)
{
    ThreadStart *ts = (ThreadStart *)p;
    if (ts && ts->func)
        ts->func(ts->arg);
    free(ts);
    return NULL;
}

uint32_t createThread(ThreadFunc func, void *arg, Thread *thread)
{
    ThreadStart *ts = (ThreadStart *)malloc(sizeof *ts);
    if (!ts)
        return 0;
    ts->func = func;
    ts->arg = arg;

    int rc = pthread_create(thread, NULL, thread_start_wrapper, ts);
    if (rc != 0)
    {
        free(ts);
        return 0;
    }
    return 1;
}

void joinThread(Thread thread)
{
    pthread_join(thread, NULL);
}

void killMyThread(void *value_ptr)
{
    pthread_exit(value_ptr);
}
#else
#error "Unsupported platform"
#endif