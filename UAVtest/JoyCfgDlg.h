#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <stdint.h>

class Joystick;

// CJoyCfgDlg dialog

class CJoyCfgDlg : public CDialog
{
	DECLARE_DYNAMIC(CJoyCfgDlg)

public:
	CJoyCfgDlg(Joystick *joystick, CWnd* pParent = NULL);   // standard constructor
	virtual ~CJoyCfgDlg();
	void InitJoystick(void);
	bool GetJoystickData(int flapsPos, int &ailerons, int &elevator, int &rudder, int &throttle, int &buttons);


// Dialog Data
	enum { IDD = IDD_JOYCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeCmbjoy();
	afx_msg void OnClose();
	afx_msg void OnCbnSelchange();
	afx_msg void OnBnClickedChkInv();

	DECLARE_MESSAGE_MAP()

private:
	Joystick *joystick_;

	CComboBox m_cmbJoy;
	CComboBox m_cmbAilerons;
	CComboBox m_cmbElevator;
	CComboBox m_cmbRudder;
	CComboBox m_cmbThrottle;

	CProgressCtrl m_prgAilerons;
	CProgressCtrl m_prgElevator;
	CProgressCtrl m_prgRudder;
	CProgressCtrl m_prgThrottle;
	
	CButton m_chkAilerons;
	CButton m_chkElevator;
	CButton m_chkRudder;
	CButton m_chkThrottle;

	CComboBox m_cmbFlaps0;
	CEdit m_edFlaps0;
	CComboBox m_cmbFlaps1;
	CEdit m_edFlaps1;
	CComboBox m_cmbFlaps2;
	CEdit m_edFlaps2;
	CComboBox m_cmbOisLock;
	CEdit m_edOisLock;
	CComboBox m_cmbOisIR;
	CEdit m_edOisIR;

	CEdit m_edDeadzone;
};
