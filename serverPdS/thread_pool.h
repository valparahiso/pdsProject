//
// Created by simo on 06/12/20.
//

#ifndef SERVERPDS_THREAD_POOL_H
#define SERVERPDS_THREAD_POOL_H


#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "tcp_connection.h"

class thread_pool {
public:
    thread_pool():max_threads(2), num_threads(0){
    };

    void push(tcp_connection::pointer new_connection){
        std::unique_lock<std::mutex> ul(sm);
        if(num_threads == max_threads-1) pop();
        cv.wait(ul,[this](){return (num_threads != max_threads-1); });
        std::cout<<"DOPO LA WAIT: num, max: "<<num_threads<<", "<<max_threads<<std::endl;
        num_threads++;
        pool.push(std::thread(std::bind(&tcp_connection::start, new_connection)));
    }

    void pop(){

        std::lock_guard<std::mutex> lg(sm);
        pool.front().join();
        pool.pop();
        num_threads--;
        cv.notify_one();
    }



private:
    int max_threads;
    int num_threads;
    std::queue<std::thread> pool;
    std::mutex sm;
    std::condition_variable cv;


};


#endif //SERVERPDS_THREAD_POOL_H
