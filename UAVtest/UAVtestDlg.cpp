// UAVtestDlg.cpp : implementation file
//

#include "stdafx.h"
#include <sstream>
#include <iomanip>
#include <../SDL/include/SDL.h>
#include "UAVtest.h"
#include "UAVtestDlg.h"
#include "manualctrl.h"
#include "ConfigDlg.h"
#include "RouteDlg.h"
#include "JoyCfgDlg.h"
#include "AdsSetDlg.h"
#include <config.h>
#include <dataxchg.h>
#include <tds.h>
#include <joystick.h>


using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


enum {
	  HK_MODE_AUTO = 101
	, HK_MODE_FUTABA
	, HK_MODE_MANUAL
	, HK_DATALINK_OFF
	, HK_DATALINK_ON
	, HK_ABORT
	, HK_EMERGENCY
};

// CUAVtestDlg dialog

CUAVtestDlg::CUAVtestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUAVtestDlg::IDD, pParent)
	, dataXchg_(NULL)
	, tds_(NULL)
	, m_bConnect(false)
	, m_bResetAlt(false)
	, m_nSpdSrcIndex(0)
	, m_nHdgSrcIndex(0)
	, m_nAltSrcIndex(0)
	, oisLock_(false)
	, oisIR_(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	states_[0x01] = "Idle: wait";
	states_[0x02] = "Idle: ready";
	states_[0x11] = "Engine: start";
	states_[0x12] = "Engine: warmup";
	states_[0x13] = "Engine: test";
	states_[0x21] = "Ready for takeoff";
	states_[0x31] = "Takeoff: rolling";
	states_[0x32] = "Takeoff: rotate";
	states_[0x41] = "Climb: initial";
	states_[0x42] = "Climb: accelerate";
	states_[0x43] = "Climb: retract flaps";
	states_[0x51] = "Route";
	states_[0x61] = "Pattern";
	states_[0x71] = "Landing: baro";
	states_[0x72] = "Landing: sonar";
	states_[0x73] = "Landing: flare";
	states_[0x81] = "Rolling out";
	states_[0xA1] = "EMERGENCY";
	states_[0xC0] = "Manual: init";
	states_[0xC1] = "Manual: datalink";
	states_[0xC2] = "Manual: futaba";

	char szAppPath[MAX_PATH] = "";
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	std::string filename = szAppPath;
	appPath_ = filename.substr(0, filename.rfind("\\")) + "\\";
}


CUAVtestDlg::~CUAVtestDlg()
{
	if (dataXchg_ != NULL)
		delete dataXchg_;
	if (tds_ != NULL)
		delete tds_;
}



void CUAVtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ADDR, m_Addr);
	DDX_Control(pDX, IDC_RECVERR, m_RecvErr);
	DDX_Control(pDX, IDC_BTNCONNECT, m_Connect);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_INS_ROLL, m_Roll);
	DDX_Control(pDX, IDC_INS_PITCH, m_Pitch);
	DDX_Control(pDX, IDC_INS_YAW, m_Yaw);
	DDX_Control(pDX, IDC_ADS_INDSPEED, m_IndSpeed);
	DDX_Control(pDX, IDC_ADS_BAROALT, m_BaroAlt);
	DDX_Control(pDX, IDC_ADS_VSPEED, m_VSpeed);
	DDX_Control(pDX, IDC_GPS_LATITUDE, m_Latitude);
	DDX_Control(pDX, IDC_GPS_LONGITUDE, m_Longitude);
	DDX_Control(pDX, IDC_GPS_ALTITUDE, m_GpsAltitude);
	DDX_Control(pDX, IDC_ENG_RPM, m_Rpm);
	DDX_Control(pDX, IDC_ENG_TEMP1, m_Temp1);
	DDX_Control(pDX, IDC_ENG_FUELLEVEL, m_FuelLevel);
	DDX_Control(pDX, IDC_SPDSRC, m_SpdSrc);
	DDX_Control(pDX, IDC_HDGSRC, m_HdgSrc);
	DDX_Control(pDX, IDC_ALTSRC, m_AltSrc);
	DDX_Control(pDX, IDC_ADS_INDSPEED2, m_GpsSpeed);
	DDX_Control(pDX, IDC_INS_YAW2, m_GpsCourse);
	DDX_Control(pDX, IDC_CMBFLAPS, m_cmbFlaps);
	DDX_Control(pDX, IDC_CHKIGNITION, m_chkIgnition);
	DDX_Control(pDX, IDC_SLIDER_ELEVATOR, m_sldElevator);
	DDX_Control(pDX, IDC_SLIDER_THROTTLE, m_sldThrottle);
	DDX_Control(pDX, IDC_SLIDER_RUDDER, m_sldRudder);
	DDX_Control(pDX, IDC_SLIDER_AILERONS, m_sldAilerons);
	DDX_Control(pDX, IDC_CONTROL, m_Control);
	DDX_Control(pDX, IDC_TRACK, m_Track);
	DDX_Control(pDX, IDC_OISSTATE, m_OisState);
	DDX_Control(pDX, IDC_STATE, m_State);
	DDX_Control(pDX, IDC_SERVOU, m_ServoU);
	DDX_Control(pDX, IDC_SERVOI, m_ServoI);
	DDX_Control(pDX, IDC_AUX_DIRECTALT, m_DirectAlt);
	DDX_Control(pDX, IDC_LISTPOWERMODULES, powermodules_);
	DDX_Control(pDX, IDC_LISTFAILURES, failures_);
	DDX_Control(pDX, IDC_NSAT, nSat_);
	DDX_Control(pDX, IDC_LISTSNLEVELS, snlevels_);
}

BEGIN_MESSAGE_MAP(CUAVtestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTNCONNECT, &CUAVtestDlg::OnBnClickedBtnconnect)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_COMMAND_AUTO, &CUAVtestDlg::OnCommandAuto)
	ON_COMMAND(ID_COMMAND_FUTABA, &CUAVtestDlg::OnCommandFutaba)
	ON_COMMAND(ID_COMMAND_MANUAL, &CUAVtestDlg::OnCommandManual)
	ON_COMMAND(ID_COMMAND_ENGSTART, &CUAVtestDlg::OnCommandEngstart)
	ON_COMMAND(ID_COMMAND_TAKEOFF, &CUAVtestDlg::OnCommandTakeoff)
	ON_COMMAND(ID_COMMAND_ABORT, &CUAVtestDlg::OnCommandAbort)
	ON_COMMAND(ID_COMMAND_EMERGENCY, &CUAVtestDlg::OnCommandEmergency)
	ON_COMMAND(ID_WINDOW_CONFIG, &CUAVtestDlg::OnWindowConfig)
	ON_COMMAND(ID_WINDOW_ROUTE, &CUAVtestDlg::OnWindowRoute)
	ON_COMMAND(ID_WINDOW_JOYSTICKCONFIG, &CUAVtestDlg::OnWindowJoystickconfig)
	ON_COMMAND(ID_ADS_RESET, &CUAVtestDlg::OnAdsReset)
	ON_COMMAND(ID_ADS_SETNULLPOINT, &CUAVtestDlg::OnAdsSetnullpoint)
	ON_CBN_SELCHANGE(IDC_SPDSRC, &CUAVtestDlg::OnCbnSelchangeSpdsrc)
	ON_CBN_SELCHANGE(IDC_HDGSRC, &CUAVtestDlg::OnCbnSelchangeHdgsrc)
	ON_CBN_SELCHANGE(IDC_ALTSRC, &CUAVtestDlg::OnCbnSelchangeAltsrc)
END_MESSAGE_MAP()


// CUAVtestDlg message handlers

BOOL CUAVtestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Init SDL library
	if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
    {
		MessageBox("Couldn't initialize SDL!");
        exit(1);
    }

	atexit(SDL_Quit);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_Addr.SetWindowText("0");
	
	// read joystick.xml and open joystick
	joystick_ = new Joystick(appPath_ + "joystick.xml");
	joystick_->readCfg();

	// Gauges
	m_GaugeAviaHorizon.SubclassDlgItem(IDC_AVIAHORIZON, this);
	m_GaugeCompas.SubclassDlgItem(IDC_COMPAS, this);
	m_GaugeAlt.SubclassDlgItem(IDC_ALT, this);
	m_GaugeVar.SubclassDlgItem(IDC_VAR, this);
	m_GaugeIndSpeed.SubclassDlgItem(IDC_SPEED, this);

	// Switches

	m_SpdSrc.AddString("IAS");
	m_SpdSrc.AddString("GS");
	m_SpdSrc.SetCurSel(0);

	m_HdgSrc.AddString("Heading");
	m_HdgSrc.AddString("Course");
	m_HdgSrc.SetCurSel(0);

	m_AltSrc.AddString("Baro");
	m_AltSrc.AddString("GPS");
	m_AltSrc.SetCurSel(0);

	// Manual Control
	
	m_sldElevator.SetRange(-127, 127, 1);
	m_sldElevator.SetPageSize(0);
	m_sldElevator.SetPos(0);

	m_sldAilerons.SetRange(-127, 127, 1);
	m_sldAilerons.SetPageSize(0);
	m_sldAilerons.SetPos(0);

	m_sldRudder.SetRange(-127, 127, 1);
	m_sldRudder.SetPageSize(0);
	m_sldRudder.SetPos(0);

	m_sldThrottle.SetRange(0, 127, 1);
	m_sldThrottle.SetPageSize(0);
	m_sldThrottle.SetPos(127);

	m_cmbFlaps.AddString("Zero");
	m_cmbFlaps.AddString("Take off");
	m_cmbFlaps.AddString("Landing");
	m_cmbFlaps.SetCurSel(0);

	m_chkIgnition.SetCheck(1);

	RegisterHotkeys(true);

	// Power modules listview
	InitPowerModules();

	InitSat();

	// Failures listview
	InitFailures();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CUAVtestDlg::InitPowerModules(void)
{
	powermodules_.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	powermodules_.InsertColumn(0, "", LVCFMT_LEFT, 53);

	for (int i = 0; i < UavData::MAX_POWERMODULES; ++i)
	{
		std::stringstream ss;
		ss << i + 1;
		powermodules_.InsertColumn(i + 1, ss.str().c_str(), LVCFMT_LEFT, 40);
		powermodules_.SetColumnColors(i + 1, 0x8080FF, 0x000000);
	}

	char *fields[] = {"Flags", "Uin (V)", "Uout (V)", "Ibat (A)", "Iout (A)", "t (°C)", "tbat (°C)"};

	for (int i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i)
	{
		powermodules_.InsertItem(i, fields[i]);
		
		for (int j = 0; j < UavData::MAX_POWERMODULES; ++j)
			powermodules_.SetItemText(i, j + 1, "");
	}
}


void CUAVtestDlg::InitFailures(void)
{
	failures_.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	failures_.InsertColumn(0, "", LVCFMT_LEFT, 0);

	for (int i = 0; i < 4; ++i)
		failures_.InsertColumn(i + 1, "", LVCFMT_CENTER, 80);

	const char *names[] = {
		"ИНС", "ИНС*", "СВС", "ИВ",		// 31..28
		"СНС", "---",  "ОШ",  "ДУС",	// 27..24
		"ДУА", "ПИТ2", "ПИТ1", "ГЕН",	// 23..20
		"ПН", "КПН", "КСУ2", "КСУ",		// 19..16
		"ДВИГ", "ТРМ", "ПОШ", "БУД",	// 15..12
		"ГАЗ", "ЗКРП", "ЗКРЛ", "РНП",	// 11..8
		"РНЛ", "РВП", "РВЛ", "ЭП",		// 7..4
		"ЭЛ", "FRAM", "LOG", "FUT"		// 3..0
	};

	for (int i = 0; i < 8; ++i)
	{
		failures_.InsertItem(i, "");

		for (int j = 0; j < 4; ++j)
			failures_.SetItemText(i, j + 1, names[j + i * 4]);
	}
}


void CUAVtestDlg::InitSat(void)
{
	snlevels_.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	snlevels_.InsertColumn(0, "", LVCFMT_LEFT, 0);
	snlevels_.InsertColumn(1, "", LVCFMT_CENTER, 55);
	snlevels_.InsertColumn(2, "", LVCFMT_CENTER, 55);

	for (int i = 0; i < 6; ++i)
	{
		snlevels_.InsertItem(i, "");
		snlevels_.SetItemText(i, 1, "");
		snlevels_.SetItemText(i, 2, "");
	}
}



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUAVtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
		CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUAVtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CUAVtestDlg::OnBnClickedBtnconnect()
{
	m_bConnect = !m_bConnect;
	m_Connect.SetWindowText(m_bConnect ? "Disconnect" : "Connect");
	m_Addr.EnableWindow(!m_bConnect);
	
	if (m_bConnect)
	{
		CString addr_str;
		m_Addr.GetWindowText(addr_str);

		Config *config = new Config(appPath_ + "uavtest.xml");
		config->Read();

		dataXchg_ = new DataXchg(config->GetFcsIf(), _ttoi(addr_str), config->GetDatalinkMod(), config->getLogName());
		tds_ = new TDS(config->GetTdsIf());

		delete config;

		SetTimer(0, 20, NULL);
		SetTimer(1, 100, NULL);
		SetTimer(2, 500, NULL);
	}
	else
	{
		KillTimer(0);
		KillTimer(1);
		KillTimer(2);

		if (dataXchg_ != NULL)
		{
			delete dataXchg_;
			dataXchg_ = NULL;
		}
		
		if (tds_ != NULL)
		{
			delete tds_;
			tds_ = NULL;
		}
	}
}


void CUAVtestDlg::OnClose()
{
	SDL_Quit();
	
	// unregister hotkeys
	RegisterHotkeys(false);

	CDialog::OnClose();
}


void CUAVtestDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0)
	{
		KillTimer(0);

		UpdateTds();			// обновление данных в TDS
		UpdateGauges();			// обновление приборов

		UpdateJoystick();		// чтение состояния джойстика
		UpdateManualCtrl();		// ручное управление
		UpdateOisCtrl();		// управление ОТГ

		SetTimer(0, 20, NULL);
	}

	else if (nIDEvent == 1)
		UpdateTextValues();		// обновление текстовых данных

	else if (nIDEvent == 2)
	{
		UpdatePowerModules();	// обновление состояния модулей питания
		UpdateFailures();		// обновление отказов
		UpdateSat();
	}

	CDialog::OnTimer(nIDEvent);
}


void CUAVtestDlg::UpdateTds(void)
{
	if (tds_ == NULL)
		return;
	
	UavData uavData = {0};
	
	if (dataXchg_ != NULL)
		dataXchg_->GetData(uavData);

	tds_->update(uavData);
}


void CUAVtestDlg::UpdateTextValues(void)
{
	UavData uavData = {0};
	
	if (dataXchg_ != NULL)
		dataXchg_->GetData(uavData);

	CString str;

	str.Format("%d/%d", uavData.nGoodCnt, uavData.nErrCnt);
	m_RecvErr.SetWindowText(str);

	str.Format("%s", uavData.info);
	m_Info.SetWindowText(str);

	statemap::const_iterator it = states_.find(uavData.state);
	if (it != states_.end())
		str.Format("%s", it->second);
	else
		str = "Unknown state";
	m_State.SetWindowText(str);

	str.Format("%.1f", uavData.orientation.roll);
	m_Roll.SetWindowText(str);

	str.Format("%.1f", uavData.orientation.pitch);
	m_Pitch.SetWindowText(str);

	str.Format("%.1f", uavData.orientation.heading);
	m_Yaw.SetWindowText(str);

	str.Format("%.1f", uavData.air_data.speed);
	m_IndSpeed.SetWindowText(str);

	str.Format("%.0f", uavData.air_data.altitude);
	m_BaroAlt.SetWindowText(str);

	str.Format("%.1f", uavData.air_data.v_speed);
	m_VSpeed.SetWindowText(str);

	str.Format("%.1f", uavData.position.speed);
	m_GpsSpeed.SetWindowText(str);

	str.Format("%.1f", uavData.position.course);
	m_GpsCourse.SetWindowText(str);

	str.Format("%.5f", uavData.position.latitude);
	m_Latitude.SetWindowText(str);

	str.Format("%.5f", uavData.position.longitude);
	m_Longitude.SetWindowText(str);

	str.Format("%.0f", uavData.position.altitude);
	m_GpsAltitude.SetWindowText(str);

	str.Format("%d", uavData.engine.rpm);
	m_Rpm.SetWindowText(str);

	str.Format("%d / %d", uavData.engine.temp1, uavData.engine.temp2);
	m_Temp1.SetWindowText(str);

	str.Format("%d", uavData.engine.fuel_level);
	m_FuelLevel.SetWindowText(str);

	str.Format("%.1f", uavData.hardwarestate.servoU);
	m_ServoU.SetWindowText(str);

	str.Format("%.1f", uavData.hardwarestate.servoI);
	m_ServoI.SetWindowText(str);

	str.Format("%.2f", uavData.sonar.altitude);
	m_DirectAlt.SetWindowText(str);

	str.Format("%d) S=%.0f Z=%.0f", 
		uavData.track.activeWpt,
		uavData.track.distance,
		uavData.track.error
		);
	m_Track.SetWindowText(str);

	str.Format("A=%5.2f E=%5.2f R=%5.2f T=%5.2f F=%d", 
		uavData.control.aileron,
		uavData.control.elevator,
		uavData.control.rudder,
		uavData.control.throttle,
		uavData.control.flaps
		);
	m_Control.SetWindowText(str);

	str.Format("A=%.0f E=%.0f Z=%.1f", 
		uavData.oisstate.azimuth,
		uavData.oisstate.elevation,
		uavData.oisstate.zoom
		);
	m_OisState.SetWindowText(str);
}


void CUAVtestDlg::UpdateGauges(void)
{
	UavData uavData = {0};
	
	if (dataXchg_ != NULL)
		dataXchg_->GetData(uavData);

	m_GaugeAviaHorizon.SetValue(uavData.orientation.pitch, uavData.orientation.roll);

	switch (m_nHdgSrcIndex)
	{
	case 0: m_GaugeCompas.SetValue(uavData.orientation.heading); break;
	case 1: m_GaugeCompas.SetValue(uavData.position.course); break;
	default: m_GaugeCompas.SetValue(0.0); break;
	}

	switch (m_nAltSrcIndex)
	{
	case 0: m_GaugeAlt.SetValue(uavData.air_data.altitude); break;
	case 1: m_GaugeAlt.SetValue(uavData.position.altitude); break;
	default: m_GaugeAlt.SetValue(0.0); break;
	}

	m_GaugeVar.SetValue(uavData.air_data.v_speed);

	switch (m_nSpdSrcIndex)
	{
	case 0: m_GaugeIndSpeed.SetValue(uavData.air_data.speed * 3.6); break;
	case 1: m_GaugeIndSpeed.SetValue(uavData.position.speed * 3.6); break;
	default: m_GaugeIndSpeed.SetValue(0.0); break;
	}
}



void CUAVtestDlg::UpdatePowerModules(void)
{
	UavData uavData = {0};
	
	if (dataXchg_ != NULL)
	dataXchg_->GetData(uavData);

	for (int i = 0; i < UavData::MAX_POWERMODULES; ++i)
	{
		const UavData::powermodule_t &module = uavData.powermodules[i];

		powermodules_.SetColumnColors(i + 1, module.ok ? 0x80FF80 : 0x8080FF, 0x000000);

		std::stringstream ss;
		
		ss << (int)module.flags;
		powermodules_.SetItemText(0, i + 1, ss.str().c_str());

		ss.str("");
		ss << std::fixed << std::setprecision(1) << module.uin;
		powermodules_.SetItemText(1, i + 1, ss.str().c_str());

		ss.str("");
		ss << std::fixed << std::setprecision(1) << module.uout;
		powermodules_.SetItemText(2, i + 1, ss.str().c_str());

		ss.str("");
		ss << std::fixed << std::setprecision(2) << module.ibat;
		powermodules_.SetItemText(3, i + 1, ss.str().c_str());

		ss.str("");
		ss << std::fixed << std::setprecision(2) << module.iout;
		powermodules_.SetItemText(4, i + 1, ss.str().c_str());

		ss.str("");
		ss << std::fixed << std::setprecision(0) << module.tmod;
		powermodules_.SetItemText(5, i + 1, ss.str().c_str());

		ss.str("");
		ss << std::fixed << std::setprecision(0) << module.tbat;
		powermodules_.SetItemText(6, i + 1, ss.str().c_str());
	}
}


void CUAVtestDlg::UpdateFailures(void)
{
	UavData uavData = {0};
	
	if (dataXchg_ != NULL)
		dataXchg_->GetData(uavData);

	uint32_t fail = uavData.hardwarestate.failures;

	for (int i = 0; i < 32; ++i)
	{
		failures_.SetCellColors(i / 4, (i % 4) + 1, (fail & (1UL << 31)) ? 0x8080FF : 0xFFFFFF, 0);
		fail <<= 1;
	}

	failures_.Invalidate();
}


void CUAVtestDlg::UpdateSat(void)
{
	UavData uavData = {0};
	
	if (dataXchg_ != NULL)
		dataXchg_->GetData(uavData);

	
	std::stringstream ss;
	ss << (int)uavData.position.nsat;
	nSat_.SetWindowText(ss.str().c_str());

	for (int i = 0; i < 12; ++i)
	{
		std::stringstream ss;
		ss << uavData.position.snlevels[i];
		snlevels_.SetItemText(i % 6, i / 6 + 1, ss.str().c_str());
	}
}


void CUAVtestDlg::UpdateJoystick(void)
{
	if (!joystick_->update())
		return;

	m_sldAilerons.SetPos((int)(joystick_->getAxis(Joystick::Aileron) * 127.0));
	m_sldElevator.SetPos((int)(joystick_->getAxis(Joystick::Elevator) * 127.0));
	m_sldRudder.SetPos((int)(joystick_->getAxis(Joystick::Rudder) * 127.0));
	m_sldThrottle.SetPos((int)((0.5 - 0.5 * joystick_->getAxis(Joystick::Throttle)) * 127.0));

	if (joystick_->getButton(Joystick::Flaps0))
		m_cmbFlaps.SetCurSel(0);
	if (joystick_->getButton(Joystick::Flaps1))
		m_cmbFlaps.SetCurSel(1);
	if (joystick_->getButton(Joystick::Flaps2))
		m_cmbFlaps.SetCurSel(2);

	if (joystick_->getButtonEvt(Joystick::OisLock))
		oisLock_ = !oisLock_;

	if (joystick_->getButtonEvt(Joystick::OisIR))
		oisIR_ = !oisIR_;
}


void CUAVtestDlg::UpdateManualCtrl(void)
{
	if (dataXchg_ == NULL)
		return;

	ManualCtrl manualCtrl;
	manualCtrl.ailerons = m_sldAilerons.GetPos() / 127.0f;
	manualCtrl.elevator = m_sldElevator.GetPos() / 127.0f;
	manualCtrl.rudder = m_sldRudder.GetPos() / 127.0f;
	manualCtrl.throttle = m_chkIgnition.GetCheck() ? (127 - m_sldThrottle.GetPos()) / 127.0f : -1.0f;
	manualCtrl.flaps = m_cmbFlaps.GetCurSel();
	dataXchg_->SetManualCtrl(manualCtrl);
}


void CUAVtestDlg::UpdateOisCtrl(void)
{
	if (dataXchg_ == NULL)
		return;

	OisCtrl oisCtrl;
	oisCtrl.azimuth = m_sldAilerons.GetPos() / 127.0f;
	oisCtrl.elevation = m_sldElevator.GetPos() / 127.0f;
	oisCtrl.zoom = m_sldRudder.GetPos() / 127.0f;
	oisCtrl.flags =
		(oisLock_ ? (1 << 0) : 0) | 
		(oisIR_ ? (1 << 1) : 0);
	dataXchg_->SetOisCtrl(oisCtrl);
}


LRESULT CUAVtestDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case HK_MODE_AUTO: OnCommandAuto(); break;
	case HK_MODE_MANUAL: OnCommandManual(); break;
	case HK_MODE_FUTABA: OnCommandFutaba(); break;
	case HK_DATALINK_OFF: OnDatalink(false); break;
	case HK_DATALINK_ON: OnDatalink(true); break;
	case HK_ABORT: OnCommandAbort(); break;
	case HK_EMERGENCY: OnCommandEmergency(); break;
	}

	return 0;
}


void CUAVtestDlg::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CDialog::OnActivateApp(bActive, dwThreadID);
	RegisterHotkeys(bActive == TRUE);
}


void CUAVtestDlg::RegisterHotkeys(bool bRegister)
{
	if (bRegister)
	{
		RegisterHotKey(GetSafeHwnd(), HK_MODE_AUTO, 0, VK_F1);
		RegisterHotKey(GetSafeHwnd(), HK_MODE_FUTABA, 0, VK_F2);
		RegisterHotKey(GetSafeHwnd(), HK_MODE_MANUAL, 0, VK_F3);
		RegisterHotKey(GetSafeHwnd(), HK_DATALINK_OFF, 0, VK_F5);
		RegisterHotKey(GetSafeHwnd(), HK_DATALINK_ON, 0, VK_F6);
		RegisterHotKey(GetSafeHwnd(), HK_ABORT, 0, VK_F8);
		RegisterHotKey(GetSafeHwnd(), HK_EMERGENCY, MOD_CONTROL, VK_F12);
	}
	else
	{
		UnregisterHotKey(GetSafeHwnd(), HK_MODE_AUTO);
		UnregisterHotKey(GetSafeHwnd(), HK_MODE_FUTABA);
		UnregisterHotKey(GetSafeHwnd(), HK_MODE_MANUAL);
		UnregisterHotKey(GetSafeHwnd(), HK_DATALINK_OFF);
		UnregisterHotKey(GetSafeHwnd(), HK_DATALINK_ON);
		UnregisterHotKey(GetSafeHwnd(), HK_ABORT);
		UnregisterHotKey(GetSafeHwnd(), HK_EMERGENCY);
	}
}


void CUAVtestDlg::OnAdsReset()
{
	if (dataXchg_ != NULL)
		dataXchg_->AdsReset();
}

void CUAVtestDlg::OnAdsSetnullpoint()
{
	if (dataXchg_ == NULL)
		return;

	CAdsSetDlg *dlg = new CAdsSetDlg();

	if (dlg->DoModal() == IDOK)
		dataXchg_->AdsSet(dlg->GetPressure(), dlg->GetTemp());

	delete dlg;
}


void CUAVtestDlg::OnCommandAuto()
{
	if (dataXchg_ != NULL)
		dataXchg_->Command(CMD_AUTO);
}

void CUAVtestDlg::OnCommandFutaba()
{
	if (dataXchg_ != NULL)
		dataXchg_->Command(CMD_FUTABA);
}

void CUAVtestDlg::OnCommandManual()
{
	if (dataXchg_ != NULL)
		dataXchg_->Command(CMD_MANUAL);
}

void CUAVtestDlg::OnCommandEngstart()
{
	if (dataXchg_ != NULL)
		dataXchg_->Command(CMD_ENGSTART);
}

void CUAVtestDlg::OnCommandTakeoff()
{
	if (dataXchg_ != NULL)
		dataXchg_->Command(CMD_TAKEOFF);
}

void CUAVtestDlg::OnCommandAbort()
{
	if (dataXchg_ != NULL)
		dataXchg_->Command(CMD_ABORT);
}

void CUAVtestDlg::OnCommandEmergency()
{
	if (dataXchg_ != NULL)
		dataXchg_->Command(CMD_EMERGENCY);
}

void CUAVtestDlg::OnDatalink(bool value)
{
	if (dataXchg_ != NULL)
		dataXchg_->Command(value ? CMD_DATALINK_ON : CMD_DATALINK_OFF);
}

void CUAVtestDlg::OnWindowConfig()
{
	if (dataXchg_ == NULL)
		return;
	
	CConfigDlg *dlg = new CConfigDlg(dataXchg_);
	dlg->DoModal();
	delete dlg;
}

void CUAVtestDlg::OnWindowRoute()
{
	if (dataXchg_ == NULL)
		return;

	CRouteDlg *dlg = new CRouteDlg(dataXchg_);
	dlg->DoModal();
	delete dlg;
}

void CUAVtestDlg::OnWindowJoystickconfig()
{
	CJoyCfgDlg *dlg = new CJoyCfgDlg(joystick_);
	dlg->DoModal();
	delete dlg;
	joystick_->readCfg();
}


void CUAVtestDlg::OnCbnSelchangeSpdsrc()
{
	CString str;
	m_SpdSrc.GetWindowText(str);
	m_nSpdSrcIndex = m_SpdSrc.FindString(-1, str);
}

void CUAVtestDlg::OnCbnSelchangeHdgsrc()
{
	CString str;
	m_HdgSrc.GetWindowText(str);
	m_nHdgSrcIndex = m_HdgSrc.FindString(-1, str);
}

void CUAVtestDlg::OnCbnSelchangeAltsrc()
{
	CString str;
	m_AltSrc.GetWindowText(str);
	m_nAltSrcIndex = m_AltSrc.FindString(-1, str);
}


