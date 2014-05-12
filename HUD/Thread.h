#pragma once

#include <sdlwrap.h>

class Thread
{
public:
	Thread(void);
	virtual ~Thread(void);
	int Start(void);
	void Stop(void);

protected:
	bool NeedToStop(void);

private:
	virtual int ThreadProc(void *p) = 0;
	SDL_semaphore *threadSem_;
	SDL_Thread *handle_;
	void *param_;
	static int Execute(void *ptr)
		{return static_cast<Thread *>(ptr)->ThreadProc(static_cast<Thread *>(ptr)->param_);};
};
