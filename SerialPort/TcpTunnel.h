// tcptunnel.h

#ifndef TCPTUNNEL_H
#define TCPTUNNEL_H

#include <string>
#include <winsock.h>
#include "serialport.h"

class TcpTunnel : public SerialPort
{
public:
	enum {CLIENT = false, SERVER = true};

	TcpTunnel(const char *ip, std::uint16_t port, const char *handshake, bool server);
	virtual ~TcpTunnel();

	virtual bool lock();
	virtual void unlock();
	virtual std::uint16_t read(void *buf, std::uint16_t size);
	virtual void write(const void *buf, std::uint16_t size);
	virtual void clean();

private:
	TcpTunnel();	// ctor disabled
	TcpTunnel(const TcpTunnel &);

	void init();
	void deinit();
	SOCKET createServerSocket();
	SOCKET createClientSocket();
	void checkClient();

	bool initialized_;

	const std::string ip_;
	const uint16_t port_;
	const std::string handshake_;
	const bool server_;

	SOCKET sck_;
	SOCKET serverSck_;

	const std::uint32_t bufferSize_;
	std::uint8_t *buffer_;
	std::uint32_t bufferPos_;
	std::uint32_t dataInBuffer_;

	const std::uint32_t timeout_;

	static bool wsastartup_;
};

#endif
