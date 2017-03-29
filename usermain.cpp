#include <iostream.h>
#include "thread.h"
#include "semaphore.h"
#include "event.h"

#include <stdlib.h>
#include <time.h>

#include "ts_io.h"
#include "ts_alloc.h"

Semaphore* bufferSlots;
Semaphore* bufferItems;
Semaphore* mutex;
int buffer[100];
int c = 0;

class Producer: public Thread 
{
public:
	
	virtual ~Producer() { waitToComplete(); }
	
	virtual void run()
	{
		for (int i = 0 ; i < 10; i++) 
		{
			Thread::sleep(rand() % 20 + 1);
			bufferSlots->wait();
			mutex->wait();
			buffer[c++] = i;
			bufferItems->signal();
			ThreadsafeIO(cout << id << " produced " << i << endl)
			mutex->signal();
		}
	}
	
	int id;
};

class Consumer: public Thread 
{
public:
	
	virtual ~Consumer() { waitToComplete(); }
	
	virtual void run()
	{
		for (int i = 0 ; i < 10; i++) 
		{
			bufferItems->wait();
			mutex->wait();
			ThreadsafeIO(cout << id << " consumed " << buffer[--c] << endl)
			bufferSlots->signal();
			mutex->signal();
			Thread::sleep(rand() % 20 + 1);
		}
	}
	
	int id;
};

class Sleeper: public Thread
{
public:

	virtual ~Sleeper() { waitToComplete(); }
	
	virtual void run()
	{
		ThreadsafeIO(cout << id << " is now sleeping" << endl);
		sleep(sleeptime);
		ThreadsafeIO(cout << id << " has woken up" << endl);
	}
	
	int sleeptime;
	int id;
};

int userMain(int argc, char* argv[])
{
	ThreadsafeIO(cout << "start main" << endl)
	
	bufferSlots = new Semaphore(100);
	bufferItems = new Semaphore(0);
	mutex = new Semaphore(1);
	
	int i;
	int count = 100;
	
	Sleeper** sleepers = new Sleeper*[10];
	for(i = 0; i < 10; i++)
	{
		sleepers[i] = new Sleeper();
		sleepers[i]->sleeptime = 50 + 10*i;
		sleepers[i]->id = i;
	}
	for(i = 0; i < 10; i++)
	{
		sleepers[i]->start();
	}
	for(i = 0; i < 10; i++)
	{
		delete sleepers[i];
	}
	
	/*Producer** producers = new Producer*[count];
	Consumer** consumers = new Consumer*[count];
	
	
	for(i = 0; i < count; i++)
	{
		producers[i] = new Producer();
		producers[i]->id = i;
		consumers[i] = new Consumer();
		consumers[i]->id = i;
	}
	
	for(i = 0; i < count; i++)
	{
		producers[i]->start();
		consumers[i]->start();
	}
	
	for(i = 0; i < count; i++)
	{
		delete producers[i];
		delete consumers[i];
	}
	
	delete[] producers;
	delete[] consumers;*/
	
	ThreadsafeIO(cout << "end main" << endl)
	
	while (1);
	
	return 0;
}