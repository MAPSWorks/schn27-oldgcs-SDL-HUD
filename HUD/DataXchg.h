// dataxchg.h

#ifndef DATAXCHG_H
#define DATAXCHG_H

#include <thread.h>
#include <uavdata.h>
#include <../SerialPort/serialport.h>
#include <sdlwrap.h>


class DataXchg : public Thread
{
public:
	DataXchg(SerialPort *rs);
	virtual ~DataXchg(void);

	void GetData(UAVDATA &uavData);

private:
	virtual int ThreadProc(void *p);
	void ProcessData(void);

	SerialPort *rs_;
	UAVDATA uavData_;

	SDL_mutex *uavDataMut;
};

#endif



