#pragma once
#include "afxwin.h"


// CAdsSetDlg dialog

class CAdsSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CAdsSetDlg)

public:
	CAdsSetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAdsSetDlg();

	float GetPressure(void) const {return pressure_kPa_;}
	float GetTemp(void) const {return temp_;}

// Dialog Data
	enum { IDD = IDD_ADSSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();

private:
	float pressure_;
	float pressure_kPa_;
	float temp_;
	CComboBox value_;
};
