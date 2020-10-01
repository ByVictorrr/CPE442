#ifndef __THREAD_HANDLER__
#define __THREAD_HANDLER__

#include <sys/types.h>

typedef void *(*t_func)(void *);
struct t_arg
{
    u_int8_t init;
    size_t id;
};

class ThreadHandler
{
private:
    pthread_t *t_handles;
    struct t_arg *t_args;
    size_t t_handles_idx;
    size_t t_count;

    size_t get_next_thread(void);

public:
    ThreadHandler(size_t max_threads);
    ~ThreadHandler();

    int create(void *data_in, t_func func);
    int create(size_t id, void *data_in, t_func func);
    int join(size_t t_id, void **data_out, size_t size);
    int joinAll(void *data_out, size_t size);
};

/*
inline void* mmapfile(void *, size_t, size_t, int fd);
inline void* umapfile(void*, size_t);
double dtime(void);
*/

#endif