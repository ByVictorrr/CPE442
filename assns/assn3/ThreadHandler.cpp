
#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include "ThreadHandler.hpp"
#include <pthread.h>


ThreadHandler::ThreadHandler(size_t num_threads)
    : threads(num_threads) {}

pthread_t ThreadHandler::get_next_thread(void){
    static int counter = 0;
    if(counter >= this->threads.size())
        counter=0;
    return this->threads[counter++];
}
void ThreadHandler::create(size_t id, void *(*func)(void*), void *data){
     pthread_create(&threads[id], NULL, func, data);
}
void ThreadHandler::joinAll(void){
    for(int i = 0; i < this->threads.size(); i++)
        pthread_join(this->threads[i], NULL);
}