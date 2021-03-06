// rs232.cpp

#include <string>
#include "Rs232.h"

Rs232::Rs232(const char *name, std::uint32_t baudRate, std::uint16_t timeOut)
	: name_(name)
	, baudRate_(baudRate)
	, timeOut_(timeOut)
	, initialized_(false)
	, port_(INVALID_HANDLE_VALUE)
{
}


Rs232::~Rs232(void)
{
	deinit();
}


bool Rs232::lock(void)
{
	return true;
}

void Rs232::unlock(void)
{
}


std::uint16_t Rs232::read(void *buf, std::uint16_t size)
{
	init();
	std::uint32_t result = 0;
	return ReadFile(port_, buf, size, reinterpret_cast<LPDWORD>(&result), NULL) ? result : 0;
}


void Rs232::write(const void *buf, std::uint16_t size)
{
	init();
	std::uint32_t junk = 0;
	WriteFile(port_, buf, size, reinterpret_cast<LPDWORD>(&junk), NULL);
}


void Rs232::clean(void)
{
	init();
	PurgeComm(port_, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
}


void Rs232::init(void)
{
	if (initialized_)
		return;

	std::string tmp("\\\\.\\");
	tmp += name_;

	port_ = CreateFile(tmp.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (port_ == INVALID_HANDLE_VALUE)
		throw std::runtime_error("COM port opening failed");

	DCB dcb = {0};
	dcb.DCBlength = sizeof(dcb);
	
	bool success = GetCommState(port_, &dcb) == TRUE;
	
	dcb.BaudRate = baudRate_;
	dcb.ByteSize = 8;
	dcb.fParity = FALSE;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fBinary = TRUE;
	dcb.fAbortOnError = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fInX = FALSE;
	dcb.fOutX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	
	success &= SetCommState(port_, &dcb) == TRUE;

	COMMTIMEOUTS ct = {0};
	ct.ReadIntervalTimeout = timeOut_;
	ct.ReadTotalTimeoutConstant = 100;
	ct.ReadTotalTimeoutMultiplier = timeOut_;
	ct.WriteTotalTimeoutConstant = 10;
	ct.WriteTotalTimeoutMultiplier = 1;
	
	success &= SetCommTimeouts(port_, &ct) == TRUE;
	success &= SetupComm(port_, 1024, 1024) == TRUE;
	success &= PurgeComm(port_, PURGE_RXCLEAR | PURGE_TXCLEAR) == TRUE;
	success &= ClearCommError(port_, NULL, NULL);

	if (!success)
	{
		deinit();
		throw std::runtime_error("COM port opening failed");
	}
	
	initialized_ = true;
}


void Rs232::deinit(void)
{
	if (port_ != INVALID_HANDLE_VALUE)
	{
		CloseHandle(port_);
		port_ = INVALID_HANDLE_VALUE;
	}
}



