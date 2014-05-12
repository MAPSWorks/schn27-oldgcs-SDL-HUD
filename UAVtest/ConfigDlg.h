#pragma once

#include "DataXchg.h"
#include "afxcmn.h"
#include "ListCtrlEx.h"
#include "afxwin.h"
#include <vector>
#include <string>

// CConfigDlg dialog

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(DataXchg *dataXchg, CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedBtnread();
	afx_msg void OnBnClickedBtnwrite();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnload();
	afx_msg void OnBnClickedBtnsave();

	DECLARE_MESSAGE_MAP()

private:
	void FillParamInfo(void);
	int GetParamIndexByName(const std::string &name);

	struct ParamInfo
	{
		ParamInfo(int id_, const std::string &name_, int color_, bool isInt_)
			: id(id_), name(name_), color(color_), isInt(isInt_)
		{}
		int id;
		std::string name;
		int color;
		bool isInt;
	};

	typedef std::vector<ParamInfo> ParamInfoVec;


	ParamInfoVec paramInfo_;

	CImageList imgList_;
	CListCtrlEx paramList_;
	CEdit valueEditor_;
	CProgressCtrl paramProgress_;

	DataXchg *dataXchg_;
};
