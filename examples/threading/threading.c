#include "threading.h"

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void sleep_ms(int msec) {
    struct timespec ts;
    ts.tv_sec = msec / 1000;                // Convering msec to sec
    ts.tv_nsec = (msec % 1000) * 1000000;   // Convering the remainder to nanosec
    nanosleep(&ts, NULL);
}

void* threadfunc(void* thread_param)
{
    // DONE: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    sleep_ms(thread_func_args->waitToObtain_ms);

    int mtxStatus;
    mtxStatus = pthread_mutex_lock(thread_func_args->mtx);
    if (mtxStatus) {
        thread_func_args->thread_complete_success = false;
        return  (void *) thread_func_args;
    }

    sleep_ms(thread_func_args->waitToRelease_ms);

    pthread_mutex_unlock(thread_func_args->mtx);

    thread_func_args->thread_complete_success = true;

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * DONE: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    struct thread_data* threadData = (struct thread_data*)malloc(sizeof(struct thread_data));

    threadData->mtx = mutex;
    threadData->waitToObtain_ms = wait_to_obtain_ms;
    threadData->waitToRelease_ms = wait_to_release_ms;

    int ret = pthread_create(thread, NULL, threadfunc, threadData);
    if (ret) {
        errno = ret;
        perror("Pthread creation");
        return false;
    }

    return true;
}

