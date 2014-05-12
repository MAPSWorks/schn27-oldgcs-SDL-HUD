// serialport.h

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <cstdint>

class SerialPort
{
public:
	virtual ~SerialPort() {}
	
	virtual bool lock(void) = 0;
	virtual void unlock(void) = 0;
	virtual std::uint16_t read(void *buf, std::uint16_t size) = 0;
	virtual void write(const void *buf, std::uint16_t size) = 0;
	virtual void clean(void) = 0;
};

#endif
