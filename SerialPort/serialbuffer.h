// serialbuffer.h

#ifndef SERIALBUFFER_H
#define SERIALBUFFER_H

#include "serialport.h"
#include <string.h>

class SerialBuffer : public SerialPort
{
public:
	SerialBuffer(void *buffer, int size) : buffer_(buffer), pos_(0), size_(size) {}
	virtual ~SerialBuffer(void) {}
	virtual bool lock(void) {return true;}
	virtual void unlock(void) {}

	virtual std::uint16_t read(void *buf, std::uint16_t size) {
		if (pos_ + size > size_)
			size = size_ - pos_;
		memcpy(buf, (char *)buffer_ + pos_, size);
		pos_ += size;
		return size;
	}
	
	virtual void write(const void *buf, std::uint16_t size) {
		if (pos_ + size > size_)
			size = size_ - pos_;
		memcpy((char *)buffer_ + pos_, buf, size);
		pos_ += size;
	}
	
	virtual void clean(void) {}

	int getPos() {return pos_;}

private:
	SerialBuffer();
	SerialBuffer(const SerialBuffer &);
	void *buffer_;
	int pos_;
	const int size_;
};

#endif