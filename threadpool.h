#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <queue>
#include "taskqueue.h"

// 线程池类
class ThreadPool
{
public:
    ThreadPool(int min, int max);
    ~ThreadPool();

public:
    // 添加任务
    void addTask(Task task);

    // 获取线程池中忙的线程数量
    int getBusyNum();

    // 获取线程池中存活的线程数量
    int getAliveNum();

private:
    // 工作线程任务函数（消费者线程）
    static void *worker(void *arg);

    // 管理线程任务函数
    static void *manager(void *arg);

    // 单个线程退出
    void threadExit();

private:
    TaskQueue *taskQ; // 任务队列

    pthread_t managerID;  // 管理线程ID
    pthread_t *threadIDs; // 工作线程ID

    int minNum;  // 最小线程数量
    int maxNum;  // 最大线程数量
    int busyNum; // 忙的线程数量
    int liveNum; // 存活的线程数量
    int exitNum; // 要销毁的线程数量

    pthread_mutex_t mutexPool; // 线程池互斥锁
    pthread_cond_t notEmpty;   // 任务队列非空条件变量

    bool shutdown; // 销毁线程池？

    static const int NUMBER = 2;
};

#endif