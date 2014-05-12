#include "StdAfx.h"
#include "TDS.h"
#include "buffer.h"
#include "crc.h"
#include <../SerialPort/serialport.h>

TDS::TDS(SerialPort *port)
	: port_(port)
{
}

TDS::~TDS(void)
{
	if (port_ != NULL)
		delete port_;
}

void TDS::update(const UavData &uavdata)
{
	if (port_ == NULL)
		return;

	uint8_t buffer[100];

	Buffer buf(buffer, sizeof(buffer));

	buf.putUint8(0x55);
	buf.putUint8(0x3b);
	buf.putUint16(0);	// метка времени fake
	
	uint16_t flags = 0;
	
	if (uavdata.state == 0xC1)
		flags |= 0x0001;
	else if (uavdata.state == 0xC2)
		flags |= 0x0002;
	else if (uavdata.state == 0xA1)
		flags |= 0x0200;

	if (uavdata.hardwarestate.failures & 1)
		flags |= 0x0004;
	if (uavdata.hardwarestate.failures & 0xC0000000UL)
		flags |= 0x0020;
	
	buf.putUint16(flags);
	
	buf.putUint16(static_cast<int16_t>(uavdata.orientation.pitch * 100.0f));
	buf.putUint16(static_cast<int16_t>(uavdata.orientation.roll * 100.0f));
	buf.putUint16(static_cast<uint16_t>(uavdata.orientation.heading * 100.0f));
	
	buf.putUint8(static_cast<int8_t>(uavdata.control.elevator * 20.0f));
	buf.putUint8(static_cast<int8_t>(uavdata.control.aileron * 20.0f));
	buf.putUint8(static_cast<int8_t>(-uavdata.control.aileron * 20.0f));
	buf.putUint8(static_cast<int8_t>(uavdata.control.rudder * 20.0f));
	buf.putUint8(static_cast<uint8_t>(uavdata.control.throttle >= 0 ? uavdata.control.throttle * 100.0 : 0));
	
	float d = uavdata.track.distance * 0.1f;
	if (d < 0)
		d = 0;
	else if (d > 255.0f)
		d = 255.0f;

	buf.putUint8(static_cast<uint8_t>(d));

	buf.putUint16(static_cast<uint16_t>(uavdata.air_data.speed * 100.0f));
	buf.putUint16(static_cast<uint16_t>(uavdata.air_data.speed * 100.0f));
	buf.putUint16(static_cast<int16_t>(uavdata.air_data.altitude));
	buf.putUint16(7600);	// давление fake
	buf.putUint16(static_cast<int16_t>(uavdata.air_data.v_speed * 100.0f));

	buf.putUint16(static_cast<uint16_t>(uavdata.position.course * 100.0f));
	buf.putUint16(static_cast<uint16_t>(uavdata.position.speed * 100.0f));
	buf.putUint16(static_cast<int16_t>(uavdata.track.error));
	buf.putFloat32(uavdata.position.latitude);
	buf.putFloat32(uavdata.position.longitude);
	buf.putUint16(static_cast<uint16_t>(uavdata.position.altitude));

	buf.putUint16(2010);	// Год fake
	buf.putUint8(1);		// Месяц fake
	buf.putUint8(1);		// День fake
	buf.putUint8(0);		// Часы fake
	buf.putUint8(0);		// Минуты fake
	buf.putUint8(0);		// Секунды fake

	buf.putUint16(uavdata.engine.rpm);
	buf.putUint16(uavdata.engine.temp1);
	buf.putUint16(uavdata.engine.temp1);

	buf.putUint8(uavdata.track.activeWpt);

	buf.putUint8(Crc8(buffer, buf.GetIndexForPut()));

	port_->write(buffer, buf.GetIndexForPut());
}
