// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include <sstream>
#include <../pugixml/pugixml.hpp>
#include "UAVtest.h"
#include "ConfigDlg.h"


using namespace std;
using namespace pugi;


// CConfigDlg dialog

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(DataXchg *dataXchg, CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
	, dataXchg_(dataXchg)
{
	imgList_.Create(1, 20, ILC_COLOR, 0, 1);
}

CConfigDlg::~CConfigDlg()
{
	imgList_.Detach();
	imgList_.DeleteImageList();
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, paramList_);
	DDX_Control(pDX, IDC_EDIT1, valueEditor_);
	DDX_Control(pDX, IDC_PARAMPROGRESS, paramProgress_);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_BTNREAD, &CConfigDlg::OnBnClickedBtnread)
	ON_BN_CLICKED(IDC_BTNWRITE, &CConfigDlg::OnBnClickedBtnwrite)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTNLOAD, &CConfigDlg::OnBnClickedBtnload)
	ON_BN_CLICKED(IDC_BTNSAVE, &CConfigDlg::OnBnClickedBtnsave)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CConfigDlg message handlers

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	FillParamInfo();

	paramList_.Reset();
	paramList_.SetImageList(&imgList_, LVSIL_SMALL);
	paramList_.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	paramList_.InsertColumn(0, "ID", LVCFMT_LEFT, 50);
	paramList_.InsertColumn(1, "Name", LVCFMT_LEFT, 150);
	paramList_.InsertColumn(2, "Value", LVCFMT_LEFT, 100);

	paramList_.SetColumnEditor(2, &valueEditor_);

	int i = 0;
	for (ParamInfoVec::const_iterator it = paramInfo_.begin(), end = paramInfo_.end(); it != end; ++it)
	{
		stringstream ss;
		ss << it->id;
		paramList_.InsertItem(i, ss.str().c_str());
		paramList_.SetItemText(i, 1, it->name.c_str());
		paramList_.SetItemText(i, 2, "");
		paramList_.SetRowColors(i, it->color, 0);
		++i;
	}
	
	paramProgress_.SetRange(0, 100);

	OnBnClickedBtnread();
	SetTimer(0, 300, NULL);

	return TRUE;
}


void CConfigDlg::OnOK()
{
}


void CConfigDlg::OnBnClickedBtnread()
{
	for (int i = 0, n = paramInfo_.size(); i < n; ++i)
	{
		dataXchg_->ReqPar(paramInfo_[i].id);
		paramList_.SetItemText(i, 2, "");
	}
}


void CConfigDlg::OnBnClickedBtnwrite()
{
	for (int i = 0, n = paramInfo_.size(); i < n; ++i)
	{
		std::istringstream ss(std::string(paramList_.GetItemText(i, 2)));

		DWORD newval = 0;

		if (paramInfo_[i].isInt)
			ss >> newval;
		else
		{
			float v = 0.0;
			ss >> v;
			newval = *reinterpret_cast<DWORD *>(&v);
		}

		int id = paramInfo_[i].id;

		DWORD val;
		BYTE state;
		dataXchg_->GetPar(id, val, state);

		if (val != newval)
			dataXchg_->SetPar(id, newval);
	}
}


void CConfigDlg::OnTimer(UINT_PTR nIDEvent)
{
	const int n = paramInfo_.size();
	int nready = 0;

	for (int i = 0; i < n; ++i)
	{
		DWORD val;
		BYTE state;
		dataXchg_->GetPar(paramInfo_[i].id, val, state);

		if (state == DataXchg::READRESP)
		{
			stringstream ss;
			ss << (paramInfo_[i].isInt ? val : *reinterpret_cast<float *>(&val));
			paramList_.SetItemText(i, 2, ss.str().c_str());
			++nready;
		} 
		else if (state == DataXchg::IDLE)
			++nready;
	}

	paramProgress_.SetPos(nready * 100 / n);
	paramProgress_.ShowWindow(nready == n ? SW_HIDE : SW_SHOW);
}


// чтение параметров из файла
void CConfigDlg::OnBnClickedBtnload()
{
	CFileDialog ofd(TRUE, "xml", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "XML(*.xml)|*.xml\0");
	
	if (ofd.DoModal() != IDOK)
		return;

	xml_document doc;
	doc.load_file(ofd.GetOFN().lpstrFile);

	xml_node node = doc.first_element_by_path("FCSConfig/param");

	while (!node.empty())
	{
		std::string name = node.attribute("name").value();
		int index = GetParamIndexByName(name);
		
		if (index != -1)
		{
			xml_attribute value = node.attribute("value");
			stringstream ss;
			ss << (paramInfo_[index].isInt ? value.as_int() : value.as_double());
			paramList_.SetItemText(index, 2, ss.str().c_str());
		}

		node = node.next_sibling();
	}
}


// запись параметров в файл
void CConfigDlg::OnBnClickedBtnsave()
{
	CFileDialog ofd(FALSE, "xml", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "XML(*.xml)|*.xml\0");
	
	if (ofd.DoModal() != IDOK)
		return;

	xml_document doc;
	xml_node root = doc.append_child();
	root.set_name("FCSConfig");

	for (int i = 0, n = paramList_.GetItemCount(); i < n; ++i)
	{
		xml_node node = root.append_child();
		node.set_name("param");
		node.append_attribute("name").set_value(paramList_.GetItemText(i, 1));
		node.append_attribute("value").set_value(paramList_.GetItemText(i, 2));
	}

	doc.save_file(ofd.GetOFN().lpstrFile, "  ");
}


void CConfigDlg::FillParamInfo(void)
{
	char szAppPath[MAX_PATH] = "";
	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	std::string filename = szAppPath;
	filename = filename.substr(0, filename.rfind("\\"));
	filename += "\\params.xml";
	
	xml_document doc;

	if (!doc.load_file(filename.c_str()))
		return;

	xml_node node = doc.first_element_by_path("params/param");

	paramInfo_.clear();

	while (!node.empty())
	{
		int id = node.attribute("id").as_int();
		std::string name = node.attribute("name").value();
		bool isInt = node.attribute("intvalue").as_bool();

		xml_attribute color_attr = node.attribute("color");
		
		int color = 0xFFFFFF;
		if (!color_attr.empty())
		{
			std::stringstream ss;
			ss << std::hex << color_attr.value();
			ss >> color;
		}

		paramInfo_.push_back(ParamInfo(id, name, color, isInt));

		node = node.next_sibling();
	}
}


int CConfigDlg::GetParamIndexByName(const std::string &name)
{
	for (int i = 0, n = paramInfo_.size(); i < n; ++i)
		if (paramInfo_[i].name == name)
			return i;
	
	return -1;
}
