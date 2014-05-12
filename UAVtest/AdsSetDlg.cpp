// AdsSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UAVtest.h"
#include "AdsSetDlg.h"


// CAdsSetDlg dialog

IMPLEMENT_DYNAMIC(CAdsSetDlg, CDialog)

CAdsSetDlg::CAdsSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdsSetDlg::IDD, pParent)
	, pressure_(760)
	, temp_(15)
{

}

CAdsSetDlg::~CAdsSetDlg()
{
}

void CAdsSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDPRESSURE, pressure_);
	DDX_Text(pDX, IDC_EDTEMP, temp_);
	DDX_Control(pDX, IDC_CMBVALUE, value_);
}


BEGIN_MESSAGE_MAP(CAdsSetDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAdsSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAdsSetDlg message handlers


BOOL CAdsSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	value_.Clear();
	value_.InsertString(0, "mm Hg");
	value_.InsertString(1, "kPa");
	value_.SetCurSel(0);

	UpdateData(FALSE);

	return TRUE;
}

void CAdsSetDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	pressure_kPa_ = pressure_;
	
	if (value_.GetCurSel() == 0)
		pressure_kPa_ *= 0.133289f;

	OnOK();
}

