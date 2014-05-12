// dataxchg.cpp

#include <memory.h>
#include <buffer.h>
#include <DataXchg.h>
#include <crc.h>

using namespace std;


DataXchg::DataXchg(SerialPort *rs)
	: rs_(rs)
{
	memset(&uavData_, 0, sizeof(uavData_));

	uavDataMut = SDL_CreateMutex();
	assert(uavDataMut != NULL);
	
	Thread::Start();
}

DataXchg::~DataXchg(void)
{
	Thread::Stop();
	SDL_DestroyMutex(uavDataMut);
}


// передача данных телеметрии
void DataXchg::GetData(UAVDATA &uavData)
{
	SDL_mutexP(uavDataMut);
	uavData = uavData_;
	SDL_mutexV(uavDataMut);
}


////////////////////////////////////////////////////////////////////////////////////////////


int DataXchg::ThreadProc(void *p)
{
	while (!NeedToStop())
		ProcessData();

	return 0;
}


void DataXchg::ProcessData(void)
{
	uint8_t buf[255];
	const uint8_t datasize = sizeof(buf);

	if (rs_->read(buf, 3) != 3)
		return;

	if (buf[1] < 3)
		return;

	if (rs_->read(buf + 3, buf[1] - 3) != buf[1] - 3)
		return;

	if (buf[0] != 0x55 || buf[buf[1] - 1] != Crc8(buf, buf[1] - 1))
		return;

	// разбор пакета
	SDL_mutexP(uavDataMut);

	Buffer buffer(buf, datasize);

	uavData_.timeStamp = buffer.getUint16(2);
	uavData_.flags = buffer.getUint16();

	uavData_.Ins.pitch = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
	uavData_.Ins.roll = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
	uavData_.Ins.heading = buffer.getUint16() * 0.01f;

	uavData_.AutoCtrl.elevator = static_cast<int8_t>(buffer.getUint8()) / 20.0f;
	uavData_.AutoCtrl.aileron_l = static_cast<int8_t>(buffer.getUint8()) / 20.0f;
	uavData_.AutoCtrl.aileron_r = static_cast<int8_t>(buffer.getUint8()) / 20.0f;
	uavData_.AutoCtrl.rudder = static_cast<int8_t>(buffer.getUint8()) / 20.0f;
	uavData_.AutoCtrl.throttle = buffer.getUint8() * 0.01f;

	uavData_.Gps.trackS = buffer.getUint8() * 10.0f;

	uavData_.Ads.ind_speed = buffer.getUint16() * 0.01f;
	uavData_.Ads.true_speed = buffer.getUint16() * 0.01f;
	uavData_.Ads.altitude = static_cast<int16_t>(buffer.getUint16());
	uavData_.Ads.pressure = buffer.getUint16() * 0.1f;
	uavData_.Ads.v_speed = static_cast<int16_t>(buffer.getUint16()) * 0.01f;

	uavData_.Gps.course = buffer.getUint16() * 0.01f;
	uavData_.Gps.speed = buffer.getUint16() * 0.01f;
	uavData_.Gps.trackZ = static_cast<int16_t>(buffer.getUint16());

	uavData_.Gps.latitude = buffer.getFloat32();
	uavData_.Gps.longitude = buffer.getFloat32();
	uavData_.Gps.altitude = buffer.getUint16();

	uavData_.Gps.utctime.year = buffer.getUint16();
	uavData_.Gps.utctime.month = buffer.getUint8();
	uavData_.Gps.utctime.day = buffer.getUint8();
	uavData_.Gps.utctime.hours = buffer.getUint8();
	uavData_.Gps.utctime.minutes = buffer.getUint8();
	uavData_.Gps.utctime.seconds = buffer.getUint8();

	uavData_.Eng.rpm = buffer.getUint16();
	uavData_.Eng.temp1 = static_cast<int16_t>(buffer.getUint16());
	uavData_.Eng.temp2 = static_cast<int16_t>(buffer.getUint16());

	SDL_mutexV(uavDataMut);
}



