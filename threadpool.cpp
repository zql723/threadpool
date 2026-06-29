#include "threadpool.h"
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
using namespace std;

ThreadPool::ThreadPool(int min, int max)
{
    // 创建任务队列
    taskQ = new TaskQueue;

    // 创建工作线程ID数组
    threadIDs = new pthread_t[max];
    if (threadIDs == nullptr)
    {
        cout << "new threadIDs failed..." << endl;
        return;
    }

    // 初始化线程ID数组
    memset(threadIDs, 0, sizeof(pthread_t) * max);

    // 初始化线程池参数
    minNum = min;
    maxNum = max;
    busyNum = 0;
    liveNum = min;
    exitNum = 0;
    shutdown = false;

    // 初始化互斥锁和条件变量
    if (pthread_mutex_init(&mutexPool, nullptr) != 0 ||
        pthread_cond_init(&notEmpty, nullptr) != 0)
    {
        cout << "mutex or condition init failed..." << endl;
        return;
    }

    // 创建线程
    pthread_create(&managerID, nullptr, manager, this);
    for (int i = 0; i < min; i++)
    {
        pthread_create(&threadIDs[i], nullptr, worker, this);
    }
}

ThreadPool::~ThreadPool()
{
    // 关闭线程池
    shutdown = true;

    // 回收管理线程
    pthread_join(managerID, nullptr);

    // 唤醒所有工作线程
    pthread_cond_broadcast(&notEmpty);

    // 回收所有工作线程
    for (int i = 0; i < maxNum; i++)
    {
        if (threadIDs[i] != 0)
        {
            pthread_join(threadIDs[i], nullptr);
        }
    }

    // 释放任务队列
    if (taskQ)
    {
        delete taskQ;
        taskQ = nullptr;
    }

    // 释放线程数组
    if (threadIDs)
    {
        delete[] threadIDs;
        threadIDs = nullptr;
    }

    // 销毁互斥锁
    pthread_mutex_destroy(&mutexPool);

    // 销毁条件变量
    pthread_cond_destroy(&notEmpty);
}

void *ThreadPool::worker(void *arg)
{
    ThreadPool *pool = static_cast<ThreadPool *>(arg);

    while (true)
    {
        pthread_mutex_lock(&pool->mutexPool);
        // 若任务队列为空且线程池没有关闭，工作线程等待
        while (pool->taskQ->taskNum() == 0 && !pool->shutdown)
        {
            // 阻塞工作线程
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);
            // 判断是否需要退出线程
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    pool->threadExit();
                }
            }
        }

        // 判断线程池是否关闭
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->mutexPool);
            pool->threadExit();
        }

        // 从任务队列取任务
        Task task = pool->taskQ->takeTask();

        // 当前线程开始忙碌
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexPool);

        cout << "thread " << to_string(pthread_self()) << "start working..." << endl;

        if (task.function != nullptr)
        {
            task.function(task.arg);
        }

        // delete task.arg;
        // task.arg = nullptr;

        cout << "thread " << to_string(pthread_self()) << "end working..." << endl;

        // 任务执行完
        pthread_mutex_lock(&pool->mutexPool);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexPool);
    }
    return nullptr;
}

void *ThreadPool::manager(void *arg)
{
    ThreadPool *pool = static_cast<ThreadPool *>(arg);

    while (!pool->shutdown)
    {
        sleep(3);

        // 查看任务数量、忙的线程数量、存活的线程数量
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->taskQ->taskNum();
        int busyNum = pool->busyNum;
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mutexPool);

        // 添加线程：任务数量>存活的线程数量 && 存活的线程数量 < 最大线程数
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; i++)
            {
                if (pool->threadIDs[i] == 0)
                {
                    pthread_create(&pool->threadIDs[i], nullptr, worker, pool);
                    counter++;
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }

        // 销毁线程：忙碌线程 * 2 < 存活线程 && 存活线程 > 最小线程数
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);
            // 唤醒空闲线程，让它们自己退出
            for (int i = 0; i < NUMBER; i++)
            {
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return nullptr;
}

void ThreadPool::threadExit()
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < maxNum; i++)
    {
        if (pthread_equal(threadIDs[i], tid))
        {
            threadIDs[i] = 0;
            cout << "threadExit() called: " << to_string(tid) << " exiting..." << endl;
            break;
        }
    }
    pthread_exit(nullptr);
}

void ThreadPool::addTask(Task task)
{
    if (shutdown)
        return;

    taskQ->addTask(task);
    pthread_cond_signal(&notEmpty);
}

int ThreadPool::getBusyNum()
{
    pthread_mutex_lock(&mutexPool);
    int busy = this->busyNum;
    pthread_mutex_unlock(&mutexPool);
    return busy;
}

int ThreadPool::getAliveNum()
{
    pthread_mutex_lock(&mutexPool);
    int alive = this->liveNum;
    pthread_mutex_unlock(&mutexPool);
    return alive;
}