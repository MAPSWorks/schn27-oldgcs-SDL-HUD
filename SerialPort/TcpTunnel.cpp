// tcptunnel.cpp

#include <cassert>
#include <iostream>
#include "TcpTunnel.h"

#pragma comment(lib, "ws2_32.lib")

bool TcpTunnel::wsastartup_ = false;


TcpTunnel::TcpTunnel(const char *ip, std::uint16_t port, const char *handshake, bool server)
	: initialized_(false)
	, ip_(ip)
	, port_(port)
	, handshake_(handshake)
	, server_(server)
	, sck_(NULL)
	, serverSck_(NULL)
	, bufferSize_(1024)
	, buffer_(new uint8_t[bufferSize_])
	, bufferPos_(0)
	, dataInBuffer_(0)
	, timeout_(200)
{
	assert(bufferSize_);
}


TcpTunnel::~TcpTunnel()
{
	deinit();
	delete [] buffer_;
}


bool TcpTunnel::lock()
{
	return true;
}

void TcpTunnel::unlock()
{
}


std::uint16_t TcpTunnel::read(void *data, std::uint16_t size)
{
	init();
	checkClient();

	std::uint32_t result = 0;

	if (dataInBuffer_ >= size)
	{
		// если данных в буфере больше чем надо
		memcpy(data, buffer_ + bufferPos_, size);
		result = size;
		bufferPos_ += size;
		dataInBuffer_ -= size;
	}
	else
	{
		// если данных в буфере меньше чем надо
		
		std::uint8_t *dataPtr = (uint8_t *)data;

		if (dataInBuffer_)
		{
			// прочитать остатки

			memcpy(dataPtr, buffer_ + bufferPos_, dataInBuffer_);
			result = dataInBuffer_;
			dataPtr += dataInBuffer_;
			size -= dataInBuffer_;
		}

		while (size)
		{
			dataInBuffer_ = 0;
			bufferPos_ = 0;

			// принять новую датаграму

			fd_set set = {0};
			FD_SET(sck_, &set);
			TIMEVAL tv = {0, timeout_ * 1000}; // seconds, microseconds
			
			int select_res = select(0, &set, NULL, NULL, &tv);
			if (select_res > 0)
			{
				sockaddr_in sa = {0};
				int sa_size = sizeof(sa);
				int n = recv(sck_, (char *)buffer_, bufferSize_, 0);

				if (n > 0)
				{
					dataInBuffer_ = n;
				
					n = min(dataInBuffer_, size);	// читаем меньше или равно того что приняли
					memcpy(dataPtr, buffer_, n);
					dataPtr += n;
					result += n;
					bufferPos_ = n;
					dataInBuffer_ -= n;
					size -= n;
				}
				else
				{
					deinit();
					return result;
				}
			}
			else if (select_res == SOCKET_ERROR)
			{
				deinit();
				return result;
			}
			else
				return result;
		}
	}
	
	return result;
}


void TcpTunnel::write(const void *data, std::uint16_t size)
{
	init();
	checkClient();

	if (send(sck_, (const char *)data, size, 0) == SOCKET_ERROR)
		deinit();
}


void TcpTunnel::clean()
{
	init();
	checkClient();

	dataInBuffer_ = 0;
	bufferPos_ = 0;

	fd_set set = {0};
	FD_SET(sck_, &set);
	TIMEVAL tv = {0, 0}; // seconds, microseconds

	while (select(0, &set, NULL, NULL, &tv) > 0)
	{
		sockaddr_in sa = {0};
		int sa_size = sizeof(sa);
		if (recv(sck_, (char *)buffer_, bufferSize_, 0) <= 0) 
		{
			deinit();
			return;
		}
	}
}


void TcpTunnel::init()
{
	if (initialized_)
		return;

	if (!wsastartup_)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			throw std::runtime_error("WSAStartup failed");
		wsastartup_ = true;
	}

	if (server_)
		serverSck_ = createServerSocket();
	else
	{
		sck_ = createClientSocket();
		send(sck_, handshake_.c_str(), handshake_.length(), 0);
	}

	initialized_ = true;
}


SOCKET TcpTunnel::createServerSocket()
{
	SOCKET sck;

	sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sck == INVALID_SOCKET)
		throw std::runtime_error("Socket creation failed");

	sockaddr_in sa = {0};
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(port_);

	if (bind(sck, (sockaddr *)&sa, sizeof(sa)) < 0)
		throw std::runtime_error("Bind failed");

	listen(sck, 1);

	return sck;
}

SOCKET TcpTunnel::createClientSocket()
{
	SOCKET sck;

	sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sck == INVALID_SOCKET)
		throw std::runtime_error("Socket creation failed");

	sockaddr_in sa = {0};
	sa.sin_family = AF_INET;

	hostent* addr = gethostbyname(ip_.c_str());
	if (addr == NULL)
		sa.sin_addr.s_addr = inet_addr(ip_.c_str());
	else
		sa.sin_addr.s_addr = *(u_long *)addr->h_addr_list[0];

	sa.sin_port = htons(port_);
	
	while (connect(sck, (sockaddr *)&sa, sizeof(sa)) < 0)
	{}

	return sck;
}


void TcpTunnel::deinit()
{
	if (server_)
	{
		closesocket(serverSck_);
		serverSck_ = NULL;
	}

	if (sck_ != NULL)
	{
		closesocket(sck_);
		sck_ = NULL;
	}

	initialized_ = false;
}


void TcpTunnel::checkClient()
{
	if (!server_ || sck_ != NULL)
		return;

	sockaddr_in sa;
	int n = sizeof (sa);
	sck_ = accept(serverSck_, (sockaddr *)&sa, &n);
	
	if (sck_ == INVALID_SOCKET)
		throw std::runtime_error("Accept failed");
}
