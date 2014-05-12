#pragma once

#include "uavdata.h"
#include "stdint.h"

class SerialPort;

class TDS
{
public:
	TDS(SerialPort *port);
	~TDS(void);
	void update(const UavData &uavdata);

private:
	TDS(void);
	SerialPort *port_;
};
