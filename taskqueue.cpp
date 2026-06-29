#include "taskqueue.h"

TaskQueue::TaskQueue()
{
    pthread_mutex_init(&m_mutex, nullptr);
}

TaskQueue::~TaskQueue()
{
    pthread_mutex_destroy(&m_mutex);
}

void TaskQueue::addTask(Task task)
{
    pthread_mutex_lock(&m_mutex);
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mutex);
}

void TaskQueue::addTask(callback func, void *arg)
{
    pthread_mutex_lock(&m_mutex);
    m_taskQ.push(Task(func, arg));
    pthread_mutex_unlock(&m_mutex);
}

Task TaskQueue::takeTask()
{
    pthread_mutex_lock(&m_mutex);
    Task task;
    if (!m_taskQ.empty())
    {
        task = m_taskQ.front();
        m_taskQ.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return task;
}

int TaskQueue::taskNum()
{
    pthread_mutex_lock(&m_mutex);
    int num = m_taskQ.size();
    pthread_mutex_unlock(&m_mutex);
    return num;
}
