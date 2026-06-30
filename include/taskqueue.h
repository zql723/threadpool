#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <queue>
#include <pthread.h>

using callback = void (*)(void *arg);

// 任务结构体
struct Task
{
    Task()
    {
        function = nullptr;
        arg = nullptr;
    }

    Task(callback func, void *arg)
    {
        this->function = func;
        this->arg = arg;
    }

    callback function;
    void *arg;
};

// 任务队列类
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();

public:
    // 添加任务
    void addTask(Task task);
    void addTask(callback func, void *arg);

    // 取出任务
    Task takeTask();

    // 获取任务个数
    int taskNum();

private:
    std::queue<Task> m_taskQ;
    pthread_mutex_t m_mutex;
};
#endif