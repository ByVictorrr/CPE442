
#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <vector>

class ThreadHandler{
    private:
        std::vector<pthread_t> threads;
    public:
        ThreadHandler(size_t num_threads);

        pthread_t get_next_thread(void);
        void create(size_t id, void *(*func)(void*), void * data);
        void joinAll(void );
        
};