#include "taskqueue.h"

void TaskQueue::addTask(Task task)
{
    m_taskQ.push(task);
}

void TaskQueue::addTask(callback func, void *arg)
{
    m_taskQ.push(Task(func, arg));
}

Task TaskQueue::takeTask()
{
    Task task;
    if (!m_taskQ.empty())
    {
        task = m_taskQ.front();
        m_taskQ.pop();
    }
    return task;
}

int TaskQueue::taskNum()
{
    return static_cast<int>(m_taskQ.size());
}
