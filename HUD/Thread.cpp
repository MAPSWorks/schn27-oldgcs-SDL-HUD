// thread.cpp

#include <cassert>
#include <Thread.h>

Thread::Thread(void) : handle_(NULL)
{
}

Thread::~Thread(void)
{
	Stop();
}

int Thread::Start(void)
{
	if (handle_ != NULL)
		return 1;

	threadSem_ = SDL_CreateSemaphore(0);
	handle_ = SDL_CreateThread(Thread::Execute, this);
	return handle_ != NULL;
}

void Thread::Stop(void)
{
	if (handle_ == NULL)
		return;

	assert(threadSem_ != NULL);
	SDL_SemPost(threadSem_);
	SDL_WaitThread(handle_, NULL);
	handle_ = NULL;
	SDL_DestroySemaphore(threadSem_);
}

bool Thread::NeedToStop(void)
{
	assert(threadSem_ != NULL);
	return SDL_SemValue(threadSem_) != 0;
}
