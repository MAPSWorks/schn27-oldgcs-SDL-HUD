// udptunnel.h

#ifndef UDPTUNNEL_H
#define UDPTUNNEL_H

#include <string>
#include <winsock.h>
#include "serialport.h"

class UdpTunnel : public SerialPort
{
public:
	UdpTunnel(const char *selfIp, uint16_t selfPort, const char *remoteIp, std::uint16_t remotePort);
	virtual ~UdpTunnel(void);

	virtual bool lock(void);
	virtual void unlock(void);
	virtual std::uint16_t read(void *buf, std::uint16_t size);
	virtual void write(const void *buf, std::uint16_t size);
	virtual void clean(void);

private:
	UdpTunnel(void);	// ctor disabled
	UdpTunnel(const UdpTunnel &);

	void init(void);
	void deinit(void);

	bool initialized_;

	std::string selfIp_;
	std::uint16_t selfPort_;
	std::string remoteIp_;
	uint16_t remotePort_;
	in_addr remote_addr_;
	bool update_remote_addr_;

	SOCKET sck_;

	const std::uint32_t bufferSize_;
	std::uint8_t *buffer_;
	std::uint32_t bufferPos_;
	std::uint32_t dataInBuffer_;

	const std::uint32_t timeout_;

	static bool wsastartup_;
};

#endif
