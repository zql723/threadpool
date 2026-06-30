#include "threadpool.h"
#include <iostream>
#include <unistd.h>
using namespace std;

void taskFunc(void *arg)
{
    int num = *static_cast<int *>(arg);
    cout << "thread" << static_cast<unsigned long>(pthread_self())
         << " is working...  task number = " << num << endl;
    sleep(1);
    delete static_cast<int *>(arg);
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

    cout << "main thread exit..." << endl;

    return 0;
}