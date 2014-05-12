#include "StdAfx.h"
#include "Thread.h"

Thread::Thread(void)
	: created_(false)
	, event_(0)
	, thread_(0)
	, id_(0)
{
	event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
}

Thread::~Thread(void)
{
	Close();
	CloseHandle(event_);
}

bool Thread::Create(void)
{
	ResetEvent(event_);
	thread_ = CreateThread(NULL, 0, Execute, static_cast<void *>(this), 0, &id_);
	created_ = thread_ != 0;
	return created_;
}

void Thread::Close(void)
{
	if (!created_)
		return;

	SetEvent(event_);
	if (WaitForSingleObject(thread_, 1000) == WAIT_TIMEOUT)
		TerminateThread(thread_, 0);
	ResetEvent(event_);
	created_ = false;
}

HANDLE Thread::GetEvent(void)
{
	return event_;
}