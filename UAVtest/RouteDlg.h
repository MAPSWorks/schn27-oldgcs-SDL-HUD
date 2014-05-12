#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "DataXchg.h"
#include "waypoint.h"


// RouteDlg dialog

class CRouteDlg : public CDialog
{
	DECLARE_DYNAMIC(CRouteDlg)

public:
	CRouteDlg(DataXchg *dataXchg, CWnd* pParent = NULL);   // standard constructor
	virtual ~CRouteDlg();

// Dialog Data
	enum { IDD = IDD_ROUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
private:
	DataXchg *dataXchg_;

	BYTE m_wptState[NUMOFWPT];

	CListCtrl m_ListWpt;
	CEdit m_WptNum;
	CEdit m_WptLat;
	CEdit m_WptLon;
	CEdit m_WptAlt;
public:
	afx_msg void OnLvnItemActivateListwpt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnread();
	afx_msg void OnBnClickedBtnwrite();
	afx_msg void OnBnClickedBtnloadkml();
	afx_msg void OnBnClickedBtnsavekml();
	afx_msg void OnBnClickedBtnsetwpt();
	afx_msg void OnBnClickedBtngotowpt();
};
