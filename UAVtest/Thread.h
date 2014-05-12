#pragma once

#include <windows.h>

class Thread
{
public:
	typedef DWORD (*THREADFUNC)(void *);

	Thread(void);
	virtual ~Thread(void);

	bool Create(void);
	void Close(void);

protected:
	HANDLE GetEvent(void);

	static DWORD __stdcall Execute(void *lpThis)
		{return static_cast<Thread *>(lpThis)->ThreadProc(static_cast<Thread *>(lpThis)->param_);};

	void *param_;

private:
	virtual DWORD ThreadProc(void *p) = 0;

	bool created_;
	HANDLE event_;
	HANDLE thread_;
	DWORD id_;
};
