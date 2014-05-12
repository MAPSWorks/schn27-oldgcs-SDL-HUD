// rs232.h

#ifndef RS232_H
#define RS232_H

#include <cstdint>
#include <string>
#include <windows.h>
#include "serialport.h"

class Rs232 : public SerialPort
{
public:
	Rs232(const char *name, std::uint32_t baudRate, std::uint16_t timeOut);
	virtual ~Rs232(void);
	virtual bool lock(void);
	virtual void unlock(void);
	virtual uint16_t read(void *buf, std::uint16_t size);
	virtual void write(const void *buf, std::uint16_t size);
	virtual void clean(void);

private:
	Rs232(void);
	Rs232(const Rs232 &);
	
	void init(void);
	void deinit(void);

	std::string name_;
	std::uint32_t baudRate_;
	std::uint16_t timeOut_;
	bool initialized_;
	HANDLE port_;	// Port handle, INVALID_HANDLE_VALUE if port is not opened
};

#endif
