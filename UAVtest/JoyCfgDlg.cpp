// JoyCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UAVtest.h"
#include "JoyCfgDlg.h"
#include <joystick.h>


// CJoyCfgDlg dialog

IMPLEMENT_DYNAMIC(CJoyCfgDlg, CDialog)

CJoyCfgDlg::CJoyCfgDlg(Joystick *joystick, CWnd* pParent /*=NULL*/)
	: CDialog(CJoyCfgDlg::IDD, pParent)
	, joystick_(joystick)
{
}

CJoyCfgDlg::~CJoyCfgDlg()
{
}

void CJoyCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CMBJOY, m_cmbJoy);

	DDX_Control(pDX, IDC_CMBAILERONS, m_cmbAilerons);
	DDX_Control(pDX, IDC_PRGAILERONS, m_prgAilerons);
	DDX_Control(pDX, IDC_CHKAILERONS, m_chkAilerons);

	DDX_Control(pDX, IDC_CMBELEVATOR, m_cmbElevator);
	DDX_Control(pDX, IDC_PRGELEVATOR, m_prgElevator);
	DDX_Control(pDX, IDC_CHKELEVATOR, m_chkElevator);

	DDX_Control(pDX, IDC_CMBRUDDER, m_cmbRudder);
	DDX_Control(pDX, IDC_PRGRUDDER, m_prgRudder);
	DDX_Control(pDX, IDC_CHKRUDDER, m_chkRudder);

	DDX_Control(pDX, IDC_CMBTHROTTLE, m_cmbThrottle);
	DDX_Control(pDX, IDC_PRGTHROTTLE, m_prgThrottle);
	DDX_Control(pDX, IDC_CHKTHROTTLE, m_chkThrottle);

	DDX_Control(pDX, IDC_CMBFLAPS0, m_cmbFlaps0);
	DDX_Control(pDX, IDC_EDFLAPS0, m_edFlaps0);

	DDX_Control(pDX, IDC_CMBFLAPS1, m_cmbFlaps1);
	DDX_Control(pDX, IDC_EDFLAPS1, m_edFlaps1);

	DDX_Control(pDX, IDC_CMBFLAPS2, m_cmbFlaps2);
	DDX_Control(pDX, IDC_EDFLAPS2, m_edFlaps2);

	DDX_Control(pDX, IDC_CMBOISLOCK, m_cmbOisLock);
	DDX_Control(pDX, IDC_EDOISLOCK, m_edOisLock);

	DDX_Control(pDX, IDC_CMBOISIR, m_cmbOisIR);
	DDX_Control(pDX, IDC_EDOISIR, m_edOisIR);

	DDX_Control(pDX, IDC_EDDEADZONE, m_edDeadzone);
}


BEGIN_MESSAGE_MAP(CJoyCfgDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_CMBJOY, &CJoyCfgDlg::OnCbnSelchangeCmbjoy)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_CMBAILERONS, &CJoyCfgDlg::OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_CMBELEVATOR, &CJoyCfgDlg::OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_CMBRUDDER, &CJoyCfgDlg::OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_CMBTHROTTLE, &CJoyCfgDlg::OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_CMBFLAPS0, &CJoyCfgDlg::OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_CMBFLAPS1, &CJoyCfgDlg::OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_CMBFLAPS2, &CJoyCfgDlg::OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_CMBOISLOCK, &CJoyCfgDlg::OnCbnSelchange)
	ON_CBN_SELCHANGE(IDC_CMBOISIR, &CJoyCfgDlg::OnCbnSelchange)
	ON_BN_CLICKED(IDC_CHKAILERONS, &CJoyCfgDlg::OnBnClickedChkInv)
	ON_BN_CLICKED(IDC_CHKELEVATOR, &CJoyCfgDlg::OnBnClickedChkInv)
	ON_BN_CLICKED(IDC_CHKRUDDER, &CJoyCfgDlg::OnBnClickedChkInv)
	ON_BN_CLICKED(IDC_CHKTHROTTLE, &CJoyCfgDlg::OnBnClickedChkInv)
END_MESSAGE_MAP()


// CJoyCfgDlg message handlers


BOOL CJoyCfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cmbJoy.AddString("<none>");

	std::vector<std::string> list = joystick_->getList();

	for (std::vector<std::string>::const_iterator it = list.begin(), end = list.end(); it != end; ++it)
		m_cmbJoy.AddString(it->c_str());

	m_cmbJoy.SetCurSel((joystick_->getDevIndex() < list.size()) ? joystick_->getDevIndex() + 1 : 0);
	OnCbnSelchangeCmbjoy();

	m_prgAilerons.SetRange32(0, 65535);
	m_prgElevator.SetRange32(0, 65535);
	m_prgRudder.SetRange32(0, 65535);
	m_prgThrottle.SetRange32(0, 65535);

	m_chkAilerons.SetCheck(joystick_->getAxisInv(Joystick::Aileron));
	m_chkElevator.SetCheck(joystick_->getAxisInv(Joystick::Elevator));
	m_chkRudder.SetCheck(joystick_->getAxisInv(Joystick::Rudder));
	m_chkThrottle.SetCheck(joystick_->getAxisInv(Joystick::Throttle));

	CString str;
	str.Format("%d", joystick_->getDeadZone());
	m_edDeadzone.SetWindowText(str);

	SetTimer(0, 50, NULL);

	return TRUE;
}


void CJoyCfgDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (joystick_->update())
	{
		m_prgAilerons.SetPos(joystick_->getAxis(Joystick::Aileron) * 32767 + 32768);
		m_prgElevator.SetPos(joystick_->getAxis(Joystick::Elevator) * 32767 + 32768);
		m_prgRudder.SetPos(joystick_->getAxis(Joystick::Rudder) * 32767 + 32768);
		m_prgThrottle.SetPos(joystick_->getAxis(Joystick::Throttle) * 32767 + 32768);

		m_edFlaps0.SetWindowText((joystick_->getButton(Joystick::Flaps0)) ? "1" : "0");
		m_edFlaps1.SetWindowText((joystick_->getButton(Joystick::Flaps1)) ? "1" : "0");
		m_edFlaps2.SetWindowText((joystick_->getButton(Joystick::Flaps2)) ? "1" : "0");
		m_edOisLock.SetWindowText((joystick_->getButton(Joystick::OisLock)) ? "1" : "0");
		m_edOisIR.SetWindowText((joystick_->getButton(Joystick::OisIR)) ? "1" : "0");
	}
}


void CJoyCfgDlg::OnCbnSelchangeCmbjoy()
{
	m_cmbAilerons.Clear();
	m_cmbElevator.Clear();
	m_cmbRudder.Clear();
	m_cmbThrottle.Clear();
	m_cmbFlaps0.Clear();
	m_cmbFlaps1.Clear();
	m_cmbFlaps2.Clear();
	m_cmbOisLock.Clear();
	m_cmbOisIR.Clear();

	joystick_->setDevIndex(m_cmbJoy.GetCurSel() - 1);

	int numAxes = joystick_->getNumOfAxes();
	int numButtons = joystick_->getNumOfButtons();

	for (int i = 0; i < numAxes; ++i)
	{
		CString str;
		str.Format("Axis N%d", i);

		m_cmbAilerons.AddString(str);
		m_cmbElevator.AddString(str);
		m_cmbRudder.AddString(str);
		m_cmbThrottle.AddString(str);
	}

	for (int i = 0; i < numButtons; ++i)
	{
		CString str;
		str.Format("Button N%d", i);
		
		m_cmbFlaps0.AddString(str);
		m_cmbFlaps1.AddString(str);
		m_cmbFlaps2.AddString(str);
		m_cmbOisLock.AddString(str);
		m_cmbOisIR.AddString(str);
	}

	m_cmbAilerons.SetCurSel(joystick_->getAxisIndex(Joystick::Aileron));
	m_cmbElevator.SetCurSel(joystick_->getAxisIndex(Joystick::Elevator));
	m_cmbRudder.SetCurSel(joystick_->getAxisIndex(Joystick::Rudder));
	m_cmbThrottle.SetCurSel(joystick_->getAxisIndex(Joystick::Throttle));
	m_cmbFlaps0.SetCurSel(joystick_->getButtonIndex(Joystick::Flaps0));
	m_cmbFlaps1.SetCurSel(joystick_->getButtonIndex(Joystick::Flaps1));
	m_cmbFlaps2.SetCurSel(joystick_->getButtonIndex(Joystick::Flaps2));
	m_cmbOisLock.SetCurSel(joystick_->getButtonIndex(Joystick::OisLock));
	m_cmbOisIR.SetCurSel(joystick_->getButtonIndex(Joystick::OisIR));
}

void CJoyCfgDlg::OnClose()
{
	CDialog::OnClose();
}


void CJoyCfgDlg::OnCbnSelchange()
{
	joystick_->setAxisIndex(Joystick::Aileron, m_cmbAilerons.GetCurSel());
	joystick_->setAxisIndex(Joystick::Elevator, m_cmbElevator.GetCurSel());
	joystick_->setAxisIndex(Joystick::Rudder, m_cmbRudder.GetCurSel());
	joystick_->setAxisIndex(Joystick::Throttle, m_cmbThrottle.GetCurSel());
	joystick_->setButtonIndex(Joystick::Flaps0, m_cmbFlaps0.GetCurSel());
	joystick_->setButtonIndex(Joystick::Flaps1, m_cmbFlaps1.GetCurSel());
	joystick_->setButtonIndex(Joystick::Flaps2, m_cmbFlaps2.GetCurSel());
	joystick_->setButtonIndex(Joystick::OisLock, m_cmbOisLock.GetCurSel());
	joystick_->setButtonIndex(Joystick::OisIR, m_cmbOisIR.GetCurSel());
}


void CJoyCfgDlg::OnOK()
{
	CString str;
	
	m_edDeadzone.GetWindowText(str);
	joystick_->setDeadZone(atoi(str));

	joystick_->writeCfg();

	CDialog::OnOK();
}


void CJoyCfgDlg::OnBnClickedChkInv()
{
	joystick_->setAxisInv(Joystick::Aileron, m_chkAilerons.GetCheck() != 0);
	joystick_->setAxisInv(Joystick::Elevator, m_chkElevator.GetCheck() != 0);
	joystick_->setAxisInv(Joystick::Rudder, m_chkRudder.GetCheck() != 0);
	joystick_->setAxisInv(Joystick::Throttle, m_chkThrottle.GetCheck() != 0);
}

