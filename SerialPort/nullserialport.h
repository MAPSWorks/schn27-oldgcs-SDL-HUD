// nullserialport.h

#ifndef NULLSERIALPORT_H
#define NULLSERIALPORT_H

#include "serialport.h"

class NullSerialPort : public SerialPort
{
public:
	NullSerialPort(void) {}
	virtual ~NullSerialPort(void) {}
	virtual bool lock(void) {return true;}
	virtual void unlock(void) {}
	virtual std::uint16_t read(void *buf, std::uint16_t size) {return 0;}
	virtual void write(const void *buf, std::uint16_t size) {}
	virtual void clean(void) {}

private:
	NullSerialPort(const NullSerialPort &);
};

#endif
