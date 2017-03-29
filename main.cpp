#include "init.h"
#include "thread.h"

int userMain(int argc, char* argv[]);

class UserMainThread: public Thread
{
public:
	
	UserMainThread(int argc_, char** argv_)
	{
		argc = argc_;
		argv = argv_;
	}
	
	virtual ~UserMainThread() { waitToComplete(); }
	
	virtual void run()
	{
		exitcode = userMain(argc, argv);
	}
	
	int argc;
	char** argv;
	int exitcode;
};

int main(int argc, char* argv[])
{
	initialize();
	
	UserMainThread userMainThread(argc, argv);
	userMainThread.start();
	userMainThread.waitToComplete();

	int exitcode = userMainThread.exitcode;
	
	finalize();
	
	return exitcode;
}