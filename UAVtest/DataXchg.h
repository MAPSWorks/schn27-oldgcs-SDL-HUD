#pragma once
#include "thread.h"
#include "uavdata.h"
#include "manualctrl.h"
#include "oisctrl.h"
#include "waypoint.h"
#include <afxmt.h>
#include <string>
#include <vector>

class SerialPort;

#define NUMOFPAR	0x100
#define NUMOFWPT	0x100

enum {
	  CMD_AUTO = 0x00
	, CMD_ENGSTART = 0x01
	, CMD_TAKEOFF = 0x02
	, CMD_ABORT = 0x0F
	, CMD_FUTABA = 0x55
	, CMD_EMERGENCY = 0xAA
	, CMD_MANUAL = 0xFF
	, CMD_DATALINK_OFF = 0x100
	, CMD_DATALINK_ON = 0x101
};


class DataXchg : public Thread
{
public:
	enum {IDLE, READREQ, READRESP, WRITEREQ};

	DataXchg(SerialPort *rs, int addr, bool datalinkMod, const std::string &logName);
	virtual ~DataXchg(void);

	void GetData(UavData &uavData);
	void Command(int cmd);
	void SetManualCtrl(const ManualCtrl &manualCtrl);
	void SetOisCtrl(const OisCtrl &oisCtrl);
	bool GetPar(int index, DWORD &val, BYTE &state);
	bool SetPar(int index, DWORD val);
	void ReqPar(int index);
	bool GetWpt(int index, Waypoint &wpt, BYTE &state);
	bool SetWpt(int index, const Waypoint &wpt);
	void GotoWpt(int index);
	void ClearWptState(int index, int n);
	void AdsReset(void);
	void AdsSet(float pressure, float temp);

private:
	DataXchg(void);
	virtual DWORD ThreadProc(void *p);
	void ProcessData(void);
	void UpdateInfo(void);
	void UpdateTelemetry(void);
	void UpdateManual(void);
	void UpdatePars();
	void UpdateWpt();
	void UpdateHardwareState(void);
	void UpdatePowerModules(void);
	void UpdateCommand(void);
	void UpdateAds(void);
	void UpdateOis(void);
	void UpdateSat(void);

	struct requestDescriptor {
		requestDescriptor(BYTE id_, BYTE *sendBuf_, BYTE sendLen_, BYTE *recvBuf_, BYTE recvLen_) :
			id(id_), sendBuf(sendBuf_), sendLen(sendLen_), recvBuf(recvBuf_), recvLen(recvLen_) {}
		BYTE id;
		BYTE *sendBuf;
		BYTE sendLen;
		BYTE *recvBuf;
		BYTE recvLen;
		bool result;
	};

	bool Request(BYTE id, BYTE *sendBuf, BYTE sendLen, BYTE *recvBuf, BYTE *recvLen);
	bool Request(std::vector<requestDescriptor> &requests);

	SerialPort *rs_;
	int addr_;
	std::string logName_;

	UavData uavData_;
	ManualCtrl manualCtrl_;
	OisCtrl oisCtrl_;

	BYTE m_parState[NUMOFPAR];
	DWORD m_par[NUMOFPAR];

	BYTE m_wptState[NUMOFWPT];
	Waypoint m_wpt[NUMOFWPT];

	int m_reqActiveWpt;

	bool manual_mode_;
	bool datalinkmod_;
	int cmd_;

	bool adsreset_;
	bool adsset_;
	float adspressure_;
	float adstemp_;

	int powermodule_index_;

	CCriticalSection csUavData;
	CCriticalSection csManualCtrl;
	CCriticalSection csOisCtrl;
	CCriticalSection csPar;
	CCriticalSection csWpt;
};




