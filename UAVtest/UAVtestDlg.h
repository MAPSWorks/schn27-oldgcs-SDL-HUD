// UAVtestDlg.h : header file
//

#pragma once
#include <vector>
#include <string>
#include <map>
#include "afxwin.h"
#include "ListCtrlEx.h"
#include "AviaHorizon.h"
#include "Compas.h"
#include "Alt.h"
#include "Var.h"
#include "Speed.h"
#include "afxcmn.h"
#include <stdint.h>

class DataXchg;
class TDS;
class Joystick;


// CUAVtestDlg dialog
class CUAVtestDlg : public CDialog
{
// Construction
public:
	CUAVtestDlg(CWnd* pParent = NULL);
	virtual ~CUAVtestDlg();

// Dialog Data
	enum { IDD = IDD_UAVTEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnconnect();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeSpdsrc();
	afx_msg void OnCbnSelchangeHdgsrc();
	afx_msg void OnCbnSelchangeAltsrc();
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnCommandAuto();
	afx_msg void OnCommandFutaba();
	afx_msg void OnCommandManual();
	afx_msg void OnCommandEngstart();
	afx_msg void OnCommandTakeoff();
	afx_msg void OnCommandAbort();
	afx_msg void OnCommandEmergency();
	afx_msg void OnDatalink(bool value);
	afx_msg void OnWindowConfig();
	afx_msg void OnWindowRoute();
	afx_msg void OnWindowJoystickconfig();
	afx_msg void OnAdsReset();
	afx_msg void OnAdsSetnullpoint();
	DECLARE_MESSAGE_MAP()

private:
	void InitPowerModules(void);
	void InitFailures(void);
	void InitSat(void);

	void UpdateTds(void);
	void UpdateGauges(void);
	void UpdateJoystick(void);
	void UpdateManualCtrl(void);
	void UpdateOisCtrl(void);
	void UpdateTextValues(void);
	void UpdatePowerModules(void);
	void UpdateFailures(void);
	void UpdateSat(void);
	
	DataXchg *dataXchg_;
	TDS *tds_;
	Joystick *joystick_;

	bool m_bConnect;
	bool m_bResetAlt;

	DWORD m_nRecv;
	DWORD m_nErr;

	int m_nSpdSrcIndex;
	int m_nHdgSrcIndex;
	int m_nAltSrcIndex;

	bool CheckSum(BYTE *buf);
	void RegisterHotkeys(bool bRegister);

	CEdit m_Addr;
	CEdit m_RecvErr;
	CButton m_Connect;
	CEdit m_Info;
	CEdit m_Roll;
	CEdit m_Pitch;
	CEdit m_Yaw;
	CEdit m_IndSpeed;
	CEdit m_BaroAlt;
	CEdit m_VSpeed;
	CEdit m_Latitude;
	CEdit m_Longitude;
	CEdit m_GpsAltitude;
	CEdit m_GpsSpeed;
	CEdit m_GpsCourse;
	CEdit m_Rpm;
	CEdit m_Temp1;
	CEdit m_FuelLevel;

	CAviaHorizon m_GaugeAviaHorizon;
	CCompas m_GaugeCompas;
	CAlt m_GaugeAlt;
	CVar m_GaugeVar;
	CSpeed m_GaugeIndSpeed;
	CComboBox m_SpdSrc;
	CComboBox m_HdgSrc;
	CComboBox m_AltSrc;
	CComboBox m_cmbMode;
	CComboBox m_cmbFlaps;
	CButton m_chkIgnition;
	CSliderCtrl m_sldElevator;
	CSliderCtrl m_sldThrottle;
	CSliderCtrl m_sldRudder;
	CSliderCtrl m_sldAilerons;
	CListCtrlEx powermodules_;
	CListCtrlEx failures_;
	CEdit nSat_;
	CListCtrlEx snlevels_;

	CEdit m_Control;
	CEdit m_Track;
	CEdit m_OisState;
	
	CEdit m_State;
	CEdit m_ServoU;
	CEdit m_ServoI;
	CEdit m_DirectAlt;

	typedef std::map<uint8_t, char *> statemap;
	statemap states_;

	std::string appPath_;

	bool oisLock_;
	bool oisIR_;
};
