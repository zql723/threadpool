#include "threadpool.h"
#include <iostream>
#include <unistd.h>
using namespace std;

void taskFunc(void *arg)
{
    int num = *(int *)arg;
    cout << "thread " << pthread_self() << " is working, number = " << num << endl;
    sleep(1);
    delete (int *)arg;
}

int main()
{
    // 创建线程池
    ThreadPool tp(3, 10);

    for (int i = 0; i < 100; i++)
    {
        int *num = new int(i + 1);
        tp.addTask(Task(taskFunc, num));
    }

    sleep(20);

    cout << "busy thread number = " << tp.getBusyNum() << endl;
    cout << "alive thread number = " << tp.getAliveNum() << endl;

    return 0;
}