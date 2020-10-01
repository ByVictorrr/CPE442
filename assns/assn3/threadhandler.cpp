#include "threadhandler.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INIT_SEQ_CHECK 0xac

size_t ThreadHandler::get_next_thread(void)
{
    int retval = this->t_handles_idx;
    this->t_handles_idx++;
    if (this->t_handles_idx > this->t_count)
        return 0;

    return retval;
}

ThreadHandler::ThreadHandler(size_t max_threads)
{
    /* init thread handles and stuff */
    this->t_handles_idx = 0;
    this->t_count = 0;

    this->t_handles = (pthread_t *)calloc(max_threads, sizeof(pthread_t));
    this->t_args = (t_arg *)calloc(max_threads, sizeof(t_arg));
    this->t_count = max_threads;
}
ThreadHandler::~ThreadHandler()
{
    free(this->t_handles);
    free(this->t_args);
}

int ThreadHandler::create(void *data_in, t_func func)
{
    size_t t_i = this->get_next_thread();

    if (this->t_args[t_i].init != 0)
        return -1;

    this->t_args[t_i].init = INIT_SEQ_CHECK;
    this->t_args[t_i].id = t_i;

    pthread_create(&this->t_handles[t_i], NULL, func, data_in);
    return 0;
}

int ThreadHandler::create(size_t id, void *data_in, t_func func)
{
    size_t t_i = id;
    if (t_i >= this->t_count)
        t_i = id % this->t_count;

    if (this->t_args[t_i].init == INIT_SEQ_CHECK)
        return -1;

    this->t_args[t_i].init = INIT_SEQ_CHECK;
    this->t_args[t_i].id = id;

    if (pthread_create(&this->t_handles[t_i], NULL, func, data_in) < 0)
    {
        perror("thread creation error");
        return -1;
    }
    return 0;
}

int ThreadHandler::join(size_t t_id, void **data_out)
{
    if (t_id >= this->t_count)
        t_id = t_id % this->t_count;

    void *temp = 0;
    if (pthread_join(this->t_handles[t_id], data_out) != 0)
    {
        perror("thread join error");
        return -1;
    }

    //clear args
    memset(&this->t_handles[t_id], 0, sizeof(pthread_t));
    memset(&this->t_args[t_id], 0, sizeof(t_arg));

    return 0;
}

int ThreadHandler::joinAll(void *data_out, size_t size)
{
    void *temp = 0;
    for (size_t t = 0; t < this->t_handles_idx; t++)
    {
        if (pthread_join(this->t_handles[t], &temp) != 0)
        {
            perror("thread join error");
        }
        // copy return value in passed array. Note this expects memory to be allocated
        memcpy((u_int8_t *)data_out + size, temp, size);
    }
    memset(this->t_handles, 0, sizeof(this->t_count * sizeof(pthread_t)));
    memset(this->t_args, 0, this->t_count * sizeof(t_arg));
    this->t_handles_idx = 0;
    return 0;
}

/*
inline void *mmapfile(void *addr, size_t offset, size_t length, int fd)
{
    void *retval = mmap(0, length, PROT_READ, MAP_SHARED, fd, offset);
    if ((size_t)retval == -1)
    {
        perror("memory map error");
        return (void *)-1;
    }
    return retval;
}

inline void *umapfile(void *addr, size_t length)
{
    if (munmap(addr, length) < 0)
    {
        return (void *)-1;
    }
    return 0;
}

double dtime(void)
{
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime, (struct timezone *)0);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec * 1.0e-6);
    return (tseconds);
}
*/