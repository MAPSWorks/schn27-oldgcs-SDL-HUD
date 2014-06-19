#include "StdAfx.h"
#include "DataXchg.h"
#include <protocol.h>
#include <buffer.h>
#include <../SerialPort/serialport.h>
#include <../SerialPort/serialbuffer.h>

using namespace std;

namespace {
enum {
	  ID_INFO = 0x00
	, ID_TELEMETRY = 0x10
	, ID_HWSTATE = 0x12
	, ID_POWERMODULES = 0x14
	, ID_ACTIVELEG = 0x16
	, ID_SATSTATE = 0x18
	, ID_MANUAL = 0x20
	, ID_COMMAND = 0x22
	, ID_GETPAR = 0x30
	, ID_SETPAR = 0x32
	, ID_GETWPT = 0x40
	, ID_SETWPT = 0x42
	, ID_GOTOWPT = 0x48
	, ID_ADSSET = 0x50
	, ID_ADSRESET = 0x52
	, ID_OISCTRL = 0x80
	, ID_DATALINKCTRL = 0x82
	, ID_GROUP = 0xF0
};
}


DataXchg::DataXchg(SerialPort *rs, int addr, bool datalinkMod, const std::string &logName)
	: rs_(rs)
	, addr_(addr)
	, logName_(logName)
	, m_reqActiveWpt(-1)
	, manual_mode_(false)
	, datalinkmod_(datalinkMod)
	, cmd_(-1)
	, adsreset_(false)
	, adsset_(false)
	, adspressure_(101.3f)
	, adstemp_(15.0f)
	, powermodule_index_(0)

{
	memset(&uavData_, 0, sizeof(uavData_));
	memset(&manualCtrl_, 0, sizeof(manualCtrl_));
	memset(&oisCtrl_, 0, sizeof(oisCtrl_));
	memset(&m_par, 0, sizeof(m_par));
	memset(&m_parState, IDLE, sizeof(m_parState));
	memset(&m_wpt, 0, sizeof(m_wpt));
	memset(&m_wptState, 3, sizeof(m_wptState));
	
	Create();	// создание потока
}

DataXchg::~DataXchg(void)
{
	Close();
	if (rs_ != NULL)
		delete rs_;
}


////////////////////////////////////////////////////////////////////////////////////////////


// передача данных телеметрии
void DataXchg::GetData(UavData &uavData)
{
	csUavData.Lock();
	uavData = uavData_;
	csUavData.Unlock();
}


// установка режима автопилота
void DataXchg::Command(int cmd)
{
	cmd_ = cmd;	
}


// установка значений ручного управлени€
void DataXchg::SetManualCtrl(const ManualCtrl &manualCtrl)
{
	csManualCtrl.Lock();
	manualCtrl_ = manualCtrl;
	csManualCtrl.Unlock();
}


// установка значений управлени€ ќ“√
void DataXchg::SetOisCtrl(const OisCtrl &oisCtrl)
{
	csOisCtrl.Lock();
	oisCtrl_ = oisCtrl;
	csOisCtrl.Unlock();
}


// чтение параметра
bool DataXchg::GetPar(int index, DWORD &val, BYTE &state)
{
	if (index >= NUMOFPAR)
		return false;

	csPar.Lock();
	
	val = m_par[index];
	state = m_parState[index];
	
	if (state == READRESP)
		m_parState[index] = IDLE;

	csPar.Unlock();

	return true;
}


// установка параметра
bool DataXchg::SetPar(int index, DWORD val)
{
	if (index >= NUMOFPAR)
		return false;

	csPar.Lock();
	m_par[index] = val;
	m_parState[index] = WRITEREQ;
	csPar.Unlock();

	return true;
}


// запрос на чтение параметра
void DataXchg::ReqPar(int index)
{
	if (index >= NUMOFPAR)
		return;

	csPar.Lock();
	m_parState[index] = READREQ;
	csPar.Unlock();
}



// чтение точки
bool DataXchg::GetWpt(int index, Waypoint &wpt, BYTE &state)
{
	if (index >= NUMOFWPT)
		return false;

	csWpt.Lock();
	wpt = m_wpt[index];
	state = m_wptState[index];
	csWpt.Unlock();

	return true;
}


// установка точки
bool DataXchg::SetWpt(int index, const Waypoint &wpt)
{
	if (index >= NUMOFWPT)
		return false;

	csWpt.Lock();
	m_wpt[index] = wpt;
	m_wptState[index] = 2;
	csWpt.Unlock();

	return true;
}


// сброс состо€ни€ точки
void DataXchg::ClearWptState(int index, int n)
{
	csWpt.Lock();

	while (n-- && (index < NUMOFWPT))
	{
		m_wptState[index] = 0;
		++index;
	}

	csWpt.Unlock();
}


// лететь на точку
void DataXchg::GotoWpt(int index)
{
	m_reqActiveWpt = index;
}


// сброс высоты и скорости —¬—
void DataXchg::AdsReset(void)
{
	adsreset_ = true;
}


// установка нулевой точки высоты —¬—
void DataXchg::AdsSet(float pressure, float temp)
{
	adspressure_ = pressure;
	adstemp_ = temp;
	adsset_ = true;
}


////////////////////////////////////////////////////////////////////////////////////////////


DWORD DataXchg::ThreadProc(void *p)
{
	while (WaitForSingleObject(GetEvent(), 0) != WAIT_OBJECT_0)
	{
		try {
			ProcessData();
		} catch (...) {
			// suppress all exceptions (it is bad)
		}
	}
	return 0;
}


void DataXchg::ProcessData(void)
{
	static BYTE cnt = 0;
	static ULARGE_INTEGER oldtime = {0};

	ULARGE_INTEGER newtime = {0};
	
	do
	{
		Sleep(1);
		SYSTEMTIME st;
		GetSystemTime(&st);
		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);
		newtime.LowPart = ft.dwLowDateTime;
		newtime.HighPart = ft.dwHighDateTime;
	}
	while (newtime.QuadPart - oldtime.QuadPart < 200000);	// = 20 ms * 10000 ticks/ms

	oldtime.QuadPart = newtime.QuadPart;

	if (manual_mode_)
	{
		UpdateManual();
		UpdatePars();
		UpdateWpt();
		UpdateAds();

		if (cnt == 5)
			UpdateTelemetry();
		else if (cnt == 10)
			UpdateHardwareState();
		else if (cnt == 15)
			UpdatePowerModules();

		if (++cnt > 20)
			cnt = 0;
	}
	else
	{
		if (cnt == 1)
			UpdateInfo();
		UpdateTelemetry();
		if (cnt == 5 || cnt == 15)
			UpdateHardwareState();
		if (cnt == 7 || cnt == 17)
			UpdatePowerModules();
		if (cnt == 9 || cnt == 19)
			UpdateSat();
		UpdateOis();
		UpdatePars();
		UpdateWpt();
		UpdateAds();

		if (++cnt > 20)
			cnt = 0;
	}

	UpdateCommand();
}



// чтение полной телеметрии
void DataXchg::UpdateTelemetry(void)
{
	BYTE buf[255];
	BYTE datasize = sizeof(buf);

	if (Request(ID_TELEMETRY, NULL, 0, buf, &datasize))
	{
		// разбор пакета
		csUavData.Lock();

		++uavData_.nGoodCnt;

		Buffer buffer(buf, datasize);

		uavData_.state = buffer.getUint8();

		uavData_.orientation.roll = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		uavData_.orientation.pitch = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		uavData_.orientation.heading = buffer.getUint16() * 0.01f;

		uavData_.air_data.speed = buffer.getUint16() * 0.01f;
		uavData_.air_data.altitude = static_cast<int16_t>(buffer.getUint16());
		uavData_.air_data.v_speed = static_cast<int16_t>(buffer.getUint16()) * 0.01f;

		uavData_.sonar.altitude = buffer.getUint16() * 0.01f;

		uavData_.position.course = buffer.getUint16() * 0.01f;
		uavData_.position.speed = buffer.getUint16() * 0.01f;
		uavData_.position.latitude = buffer.getFloat32();
		uavData_.position.longitude = buffer.getFloat32();
		uavData_.position.altitude = static_cast<int16_t>(buffer.getUint16());

		uavData_.engine.rpm = buffer.getUint16();
		uavData_.engine.temp1 = static_cast<int16_t>(buffer.getUint16());
		uavData_.engine.temp2 = static_cast<int16_t>(buffer.getUint16());

		uavData_.control.aileron = static_cast<int8_t>(buffer.getUint8()) / 127.0f;
		uavData_.control.elevator = static_cast<int8_t>(buffer.getUint8()) / 127.0f;
		uavData_.control.rudder = static_cast<int8_t>(buffer.getUint8()) / 127.0f;
		uavData_.control.throttle = static_cast<int8_t>(buffer.getUint8()) / 127.0f;
		uavData_.control.flaps = buffer.getUint8();

		uavData_.track.activeWpt = buffer.getUint8();
		uavData_.track.error = static_cast<int16_t>(buffer.getUint16());
		uavData_.track.distance = static_cast<int16_t>(buffer.getUint16()) * 10.0f;
		
		csUavData.Unlock();
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}
}



// чтение дополнительной телеметрии
void DataXchg::UpdateHardwareState(void)
{
	BYTE buf[255];
	BYTE datasize = sizeof(buf);

	if (Request(ID_HWSTATE, NULL, 0, buf, &datasize))
	{
		// разбор пакета
		csUavData.Lock();

		++uavData_.nGoodCnt;

		Buffer buffer(buf, datasize);

		uavData_.hardwarestate.failures = buffer.getUint32();
		uavData_.hardwarestate.servoU = buffer.getUint16() * 0.01f;
		uavData_.hardwarestate.servoI = buffer.getUint16() * 0.01f;
		uavData_.engine.fuel_level = buffer.getUint8();

		csUavData.Unlock();
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}
}



// чтение состо€ни€ модулей питани€
void DataXchg::UpdatePowerModules(void)
{
	BYTE buf[255];
	BYTE datasize = sizeof(buf);

	buf[0] = powermodule_index_;

	if (Request(ID_POWERMODULES, buf, 1, buf, &datasize))
	{
		// разбор пакета
		csUavData.Lock();

		++uavData_.nGoodCnt;

		Buffer buffer(buf, datasize);

		UavData::powermodule_t &module = uavData_.powermodules[powermodule_index_];

		module.ok = buffer.getUint8() != 0;
		module.flags = buffer.getUint8();
		module.uin = buffer.getUint16() * 0.1f;
		module.uout = buffer.getUint16() * 0.1f;
		module.ibat = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		module.iout = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		module.tmod = static_cast<int16_t>(buffer.getUint16());
		module.tbat = static_cast<int16_t>(buffer.getUint16());

		csUavData.Unlock();

		if (++powermodule_index_ >= UavData::MAX_POWERMODULES)
			powermodule_index_ = 0;
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}
}


// чтение состо€ни€ —Ќ—
void DataXchg::UpdateSat(void)
{
	BYTE buf[255];
	BYTE datasize = sizeof(buf);

	if (Request(ID_SATSTATE, NULL, 0, buf, &datasize))
	{
		// разбор пакета
		csUavData.Lock();

		++uavData_.nGoodCnt;

		Buffer buffer(buf, datasize);

		uavData_.position.nsat = buffer.getUint8();
		for (int i = 0; i < 12; ++i)
			uavData_.position.snlevels[i] = static_cast<int16_t>(buffer.getUint16());
		
		csUavData.Unlock();
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}
}



// передача данных ручного управлени€
void DataXchg::UpdateManual(void)
{
	BYTE buf[16];
	BYTE datasize = sizeof(buf);

	csManualCtrl.Lock();

	Buffer buffer(buf, sizeof(buf));

	buffer.putUint8(static_cast<int8_t>(manualCtrl_.ailerons * 127.0));
	buffer.putUint8(static_cast<int8_t>(manualCtrl_.elevator * 127.0));
	buffer.putUint8(static_cast<int8_t>(manualCtrl_.rudder * 127.0));
	buffer.putUint8(static_cast<int8_t>(manualCtrl_.throttle * 127.0));
	buffer.putUint8(manualCtrl_.flaps);

	csManualCtrl.Unlock();

	if (Request(ID_MANUAL, buf, buffer.GetIndexForPut(), buf, &datasize))
	{
		// разбор пакета
		csUavData.Lock();

		++uavData_.nGoodCnt;

		Buffer buffer(buf, datasize);

		uavData_.orientation.roll = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		uavData_.orientation.pitch = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		uavData_.orientation.heading = buffer.getUint16() * 0.01f;

		uavData_.air_data.speed = buffer.getUint16() * 0.01f;
		uavData_.air_data.altitude = static_cast<int16_t>(buffer.getUint16());
		uavData_.air_data.v_speed = static_cast<int16_t>(buffer.getUint16()) * 0.01f;

		uavData_.sonar.altitude = buffer.getUint16() * 0.01f;

		csUavData.Unlock();
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}
}


// передача данных ручного управлени€
void DataXchg::UpdateOis(void)
{
	BYTE buf[16];
	BYTE datasize = sizeof(buf);

	csOisCtrl.Lock();

	Buffer buffer(buf, sizeof(buf));

	buffer.putUint8(static_cast<int8_t>(oisCtrl_.azimuth * 127.0));
	buffer.putUint8(static_cast<int8_t>(oisCtrl_.elevation * 127.0));
	buffer.putUint8(static_cast<int8_t>(oisCtrl_.zoom * 127.0));
	buffer.putUint8(oisCtrl_.flags);

	csOisCtrl.Unlock();

	if (Request(ID_OISCTRL, buf, buffer.GetIndexForPut(), buf, &datasize))
	{
		// разбор пакета
		csUavData.Lock();

		++uavData_.nGoodCnt;

		Buffer buffer(buf, datasize);

		uavData_.oisstate.azimuth = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		uavData_.oisstate.elevation = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		uavData_.oisstate.zoom = static_cast<int16_t>(buffer.getUint16()) * 0.01f;
		
		csUavData.Unlock();
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}
}


// чтение / запись параметров
void DataXchg::UpdatePars()
{
	const int maxgroup = 8;
	std::vector<requestDescriptor> requests;
	BYTE abuf[8 * maxgroup];

	for (int index = 0; index < NUMOFPAR && requests.size() < maxgroup; ++index)
	{
		int n = requests.size();

		csPar.Lock();

		if (m_parState[index] == READREQ)	// требуетс€ чтение параметра?
		{
			BYTE *buf = abuf + n * 8;
			buf[0] = index;
			requests.push_back(requestDescriptor(ID_GETPAR, buf, 1, buf, 8));
		}
		else if (m_parState[index] == WRITEREQ)
		{
			BYTE *buf = abuf + n * 8;

			Buffer buffer(buf, 8);
			buffer.putUint8(index);
			buffer.putUint32(m_par[index]);

			requests.push_back(requestDescriptor(ID_SETPAR, buf, buffer.GetIndexForPut(), buf, 8));
		}

		csPar.Unlock();
	}

	if (requests.empty())
		return;

	if (Request(requests))
	{
		for (int i = 0; i < requests.size(); ++i)
		{
			if (!requests[i].result)
				continue;
			
			if (requests[i].id == ID_GETPAR)
			{
				Buffer buffer(requests[i].recvBuf, requests[i].recvLen);
				int index = buffer.getUint8();
				csPar.Lock();
				m_par[index] = buffer.getUint32();
				m_parState[index] = READRESP;
				csPar.Unlock();
			}
			else if (requests[i].id == ID_SETPAR)
			{
				Buffer buffer(requests[i].recvBuf, requests[i].recvLen);
				int index = buffer.getUint8();
				csPar.Lock();
				m_par[index] = buffer.getUint32();
				m_parState[index] = IDLE;
				csPar.Unlock();
			}
		}

		csUavData.Lock();
		++uavData_.nGoodCnt;
		csUavData.Unlock();
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}
}




// чтение / запись точек маршрута
void DataXchg::UpdateWpt()
{
	if (m_reqActiveWpt >= 0)
	{
		BYTE datasize = 1;
		BYTE buf = m_reqActiveWpt & 0xFF;
		m_reqActiveWpt = -1;
		Request(ID_GOTOWPT, &buf, 1, &buf, &datasize);
	}

	const int maxgroup = 8;
	std::vector<requestDescriptor> requests;
	BYTE abuf[16 * maxgroup];

	for (int index = 0; index < NUMOFWPT && requests.size() < maxgroup; ++index)
	{
		int n = requests.size();

		csWpt.Lock();

		if (m_wptState[index] == 0)	// требуетс€ чтение точки?
		{
			BYTE *buf = abuf + n * 16;
			buf[0] = index;
			requests.push_back(requestDescriptor(ID_GETWPT, buf, 1, buf, 16));
		}
		else if (m_wptState[index] == 2) // требуетс€ запись точки?
		{
			BYTE *buf = abuf + n * 16;

			Buffer buffer(buf, 16);
			buffer.putUint8(index);
			buffer.putFloat32(m_wpt[index].latitude);
			buffer.putFloat32(m_wpt[index].longitude);
			buffer.putUint16(m_wpt[index].altitude);
			requests.push_back(requestDescriptor(ID_SETWPT, buf, buffer.GetIndexForPut(), buf, 16));
		}

		csWpt.Unlock();
	}

	int eofindex = -1;

	if (Request(requests))
	{
		for (int i = 0; i < requests.size(); ++i)
		{
			if (!requests[i].result)
				continue;
			
			if (requests[i].id == ID_GETWPT)
			{
				Buffer buffer(requests[i].recvBuf, requests[i].recvLen);
				int index = buffer.getUint8();
				csWpt.Lock();
				m_wpt[index].latitude = buffer.getFloat32();
				m_wpt[index].longitude = buffer.getFloat32();
				m_wpt[index].altitude = static_cast<int16_t>(buffer.getUint16());

				m_wptState[index] = 1;
				
				if (m_wpt[index].altitude == -32768)	// end of route
					eofindex = index;

				csWpt.Unlock();
			}
			else if (requests[i].id == ID_SETWPT)
			{
				Buffer buffer(requests[i].recvBuf, requests[i].recvLen);
				int index = buffer.getUint8();
				csWpt.Lock();
				m_wpt[index].latitude = buffer.getFloat32();
				m_wpt[index].longitude = buffer.getFloat32();
				m_wpt[index].altitude = static_cast<int16_t>(buffer.getUint16());
				m_wptState[index] = 1;
				csWpt.Unlock();
			}
		}

		csUavData.Lock();
		++uavData_.nGoodCnt;
		csUavData.Unlock();
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}

	if (eofindex >= 0)
	{
		for (int i = eofindex; i < NUMOFWPT; ++i)
		{
			m_wpt[i].latitude = 0;
			m_wpt[i].longitude = 0;
			m_wpt[i].altitude = -32768;
			m_wptState[i] = 1;
		}
	}
}



void DataXchg::UpdateInfo(void)
{
	BYTE buf[64];
	BYTE datasize = sizeof(buf);

	if (Request(ID_INFO, NULL, 0, buf, &datasize))
	{
		csUavData.Lock();
		++uavData_.nGoodCnt;
		memset(uavData_.info, 0, sizeof(uavData_.info));
		memcpy(uavData_.info, buf + 1, datasize - 1);
		csUavData.Unlock();
	}
	else
	{
		csUavData.Lock();
		++uavData_.nErrCnt;
		csUavData.Unlock();
	}
}


void DataXchg::UpdateCommand(void)
{
	if (cmd_ < 0)
		return;

	if (cmd_ < 0x100)
	{
		manual_mode_ = (cmd_ == CMD_MANUAL) && !datalinkmod_;

		BYTE buf = cmd_;
		BYTE datasize = 1;
		Request(ID_COMMAND, &buf, 1, &buf, &datasize);
	}
	else
	{
		BYTE buf = (cmd_ == CMD_DATALINK_OFF) ? 0 : 1;
		BYTE datasize = 1;
		Request(ID_DATALINKCTRL, &buf, 1, &buf, &datasize);
	}

	cmd_ = -1;
}


void DataXchg::UpdateAds(void)
{
	if (adsset_)
	{
		BYTE buf[4];
		Buffer buffer(buf, sizeof(buf));
		buffer.putUint16(static_cast<uint16_t>(adspressure_ * 100));
		buffer.putUint16(static_cast<int16_t>(adstemp_));

		BYTE datasize = 0;
		adsset_ = !Request(ID_ADSSET, buf, buffer.GetIndexForPut(), NULL, &datasize);
	}

	if (adsreset_)
	{
		BYTE datasize = 0;
		adsreset_ = !Request(ID_ADSRESET, NULL, 0, NULL, &datasize);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////

bool DataXchg::Request(BYTE id, BYTE *sendBuf, BYTE sendLen, BYTE *recvBuf, BYTE *recvLen)
{
	std::vector<requestDescriptor> requests;
	requests.push_back(requestDescriptor(id, sendBuf, sendLen, recvBuf, *recvLen));
	if (!Request(requests))
		return false;
	*recvLen = requests[0].recvLen;
	return requests[0].result;
}


bool DataXchg::Request(std::vector<requestDescriptor> &requests)
{
	if (requests.empty())
		return true;

	BYTE buf[256];

	rs_->clean();

	Protocol protocol(rs_, buf, sizeof(buf), logName_);

	BYTE id_req;

	if (requests.size() > 1) 
	{
		BYTE buf2[256];
		SerialBuffer sb(buf2, sizeof(buf2));
		Protocol protocolb(&sb, buf, sizeof(buf), "");
		for (int i = 0; i < requests.size(); ++i)
			protocolb.send(addr_, requests[i].id, requests[i].sendBuf, requests[i].sendLen);

		id_req = ID_GROUP;
		protocol.send(addr_, ID_GROUP, buf2, sb.getPos());
	}
	else
	{
		id_req = requests[0].id;
		protocol.send(addr_, requests[0].id, requests[0].sendBuf, requests[0].sendLen);
	}
	
	uint8_t addr_recv;
	uint8_t id_recv;
	int size;
	
	if (!protocol.receive(addr_recv, id_recv, size))
		return false;

	if ((addr_ != 0 && addr_ != addr_recv) || (id_recv != id_req + 1))
	{
		Sleep(1000);	// ждать гарантированной отработки таймаута в пульте ручного управлени€
		rs_->clean();
		return false;
	}

	if (requests.size() > 1) 
	{
		SerialBuffer sb(protocol.getDataPointer(), size);
		BYTE buf2[256];
		Protocol protocolb(&sb, buf2, sizeof(buf2), "");

		for (int i = 0; i < requests.size(); ++i)
		{
			uint8_t addr_recv;
			uint8_t id_recv;
			int size;

			if (protocolb.receive(addr_recv, id_recv, size) && (addr_ == 0 || addr_ == addr_recv) && (id_recv == requests[i].id + 1))
			{
				if (requests[i].recvLen > size)
					requests[i].recvLen = size;

				if (requests[i].recvBuf != NULL)
					memcpy(requests[i].recvBuf, protocolb.getDataPointer(), requests[i].recvLen);

				requests[i].result = true;
			}
		}
	}
	else
	{
		if (requests[0].recvLen > size)
			requests[0].recvLen = size;

		if (requests[0].recvBuf != NULL)
			memcpy(requests[0].recvBuf, protocol.getDataPointer(), requests[0].recvLen);

		requests[0].result = true;
	}

	return true;
}



