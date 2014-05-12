// RouteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UAVtest.h"
#include "RouteDlg.h"

#include <../pugixml/pugixml.hpp>

using namespace std;
using namespace pugi;



// CRouteDlg dialog

IMPLEMENT_DYNAMIC(CRouteDlg, CDialog)

CRouteDlg::CRouteDlg(DataXchg *dataXchg, CWnd* pParent /*=NULL*/)
	: CDialog(CRouteDlg::IDD, pParent)
	, dataXchg_(dataXchg)
{
	memset(m_wptState, 0, sizeof(m_wptState));
}

CRouteDlg::~CRouteDlg()
{
}

void CRouteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTWPT, m_ListWpt);
	DDX_Control(pDX, IDC_EDNUM, m_WptNum);
	DDX_Control(pDX, IDC_EDLAT, m_WptLat);
	DDX_Control(pDX, IDC_EDLON, m_WptLon);
	DDX_Control(pDX, IDC_EDALT, m_WptAlt);
}


BEGIN_MESSAGE_MAP(CRouteDlg, CDialog)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LISTWPT, &CRouteDlg::OnLvnItemActivateListwpt)
	ON_BN_CLICKED(IDC_BTNREAD, &CRouteDlg::OnBnClickedBtnread)
	ON_BN_CLICKED(IDC_BTNWRITE, &CRouteDlg::OnBnClickedBtnwrite)
	ON_BN_CLICKED(IDC_BTNLOADKML, &CRouteDlg::OnBnClickedBtnloadkml)
	ON_BN_CLICKED(IDC_BTNSAVEKML, &CRouteDlg::OnBnClickedBtnsavekml)
	ON_BN_CLICKED(IDC_BTNSETWPT, &CRouteDlg::OnBnClickedBtnsetwpt)
	ON_BN_CLICKED(IDC_BTNGOTOWPT, &CRouteDlg::OnBnClickedBtngotowpt)
END_MESSAGE_MAP()


// RouteDlg message handlers


BOOL CRouteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListWpt.InsertColumn(0, "Num", 0, 40);
	m_ListWpt.InsertColumn(1, "Lat", 0, 120);
	m_ListWpt.InsertColumn(2, "Lon", 0, 120);
	m_ListWpt.InsertColumn(3, "Alt", 0, 90);

	for (int i = 0; i < NUMOFWPT; ++i)
	{
		CString str;
		str.Format("%d", i);
		m_ListWpt.InsertItem(i, str);
		m_ListWpt.SetItemText(i, 1, "<undefined>");
		m_ListWpt.SetItemText(i, 2, "<undefined>");
		m_ListWpt.SetItemText(i, 3, "<undefined>");
	}

	m_ListWpt.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	SetTimer(0, 300, NULL);

	return TRUE;
}


void CRouteDlg::OnTimer(UINT_PTR nIDEvent)
{
	for (int i = 0; i < NUMOFWPT; ++i)
	{
		Waypoint wpt;
		BYTE state;
		
		dataXchg_->GetWpt(i, wpt, state);

		if (m_wptState[i] != state && m_wptState[i] != 3)
		{
			m_wptState[i] = state;

			if (state == 0)
			{
				m_ListWpt.SetItemText(i, 1, "<undefined>");
				m_ListWpt.SetItemText(i, 2, "<undefined>");
				m_ListWpt.SetItemText(i, 3, "<undefined>");
			}
			else
			{
				if (wpt.altitude != EMPTY_WPT)
				{
					CString str;
					str.Format("%.6f", wpt.latitude);
					m_ListWpt.SetItemText(i, 1, str);
					str.Format("%.6f", wpt.longitude);
					m_ListWpt.SetItemText(i, 2, str);
					str.Format("%d", wpt.altitude);
					m_ListWpt.SetItemText(i, 3, str);
				}
				else
				{
					m_ListWpt.SetItemText(i, 1, "-----");
					m_ListWpt.SetItemText(i, 2, "-----");
					m_ListWpt.SetItemText(i, 3, "-----");
				}
			}
		}
	}
}

void CRouteDlg::OnLvnItemActivateListwpt(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (m_wptState[pNMIA->iItem] != 0)
	{
		m_WptNum.SetWindowText(m_ListWpt.GetItemText(pNMIA->iItem, 0));
		m_WptLat.SetWindowText(m_ListWpt.GetItemText(pNMIA->iItem, 1));
		m_WptLon.SetWindowText(m_ListWpt.GetItemText(pNMIA->iItem, 2));
		m_WptAlt.SetWindowText(m_ListWpt.GetItemText(pNMIA->iItem, 3));
	}

	*pResult = 0;
}

void CRouteDlg::OnBnClickedBtnread()
{
	for (int i = 0; i < NUMOFWPT; ++i)
	{
		m_wptState[i] = 0;
		m_ListWpt.SetItemText(i, 1, "<undefined>");
		m_ListWpt.SetItemText(i, 2, "<undefined>");
		m_ListWpt.SetItemText(i, 3, "<undefined>");
	}

	dataXchg_->ClearWptState(0x00, NUMOFWPT);
}

void CRouteDlg::OnBnClickedBtnwrite()
{
	for (int i = 0; i < NUMOFWPT; ++i)
	{
		if (m_wptState[i] == 3)
		{
			m_wptState[i] = 2;

			Waypoint wpt;

			if (m_ListWpt.GetItemText(i, 3) == "-----")
			{
				wpt.latitude = 0.0;
				wpt.longitude = 0.0;
				wpt.altitude = EMPTY_WPT;
			}
			else
			{
				wpt.latitude = atof(m_ListWpt.GetItemText(i, 1).GetBuffer());
				wpt.longitude = atof(m_ListWpt.GetItemText(i, 2).GetBuffer());
				wpt.altitude = atoi(m_ListWpt.GetItemText(i, 3).GetBuffer());
			}
			
			dataXchg_->SetWpt(i, wpt);
		}
	}
}

void CRouteDlg::OnBnClickedBtnloadkml()
{
	CFileDialog ofd(TRUE, "kml", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "KML(*.kml)|*.kml\0");
	
	if (ofd.DoModal() == IDOK)
	{
		xml_document doc;
		doc.load_file(ofd.GetOFN().lpstrFile);

		xml_node node = doc.first_element_by_path("kml/Placemark/LineString/coordinates");
		
		if (node.empty())
			node = doc.first_element_by_path("kml/Document/Placemark/LineString/coordinates");
		
		if (node.empty())
			node = doc.first_element_by_path("kml/Folder/Placemark/LineString/coordinates");
		
		CString coords(node.first_child().value());

		int i = 0;
		int pos = 0;

		CString delim("\n\r\t, ");
		CString token = coords.Tokenize(delim, pos);

		while (i < 250 && (token != ""))
		{
			m_wptState[i] = 3;
			
			m_ListWpt.SetItemText(i, 2, token);
			if ((token = coords.Tokenize(delim, pos)) == "")
				continue;
			m_ListWpt.SetItemText(i, 1, token);
			if ((token = coords.Tokenize(delim, pos)) == "")
				continue;
			m_ListWpt.SetItemText(i, 3, token);

			++i;
			
			token = coords.Tokenize(delim, pos);
		}

		if (i < 250)
		{
			m_wptState[i] = 3;
			m_ListWpt.SetItemText(i, 1, "-----");
			m_ListWpt.SetItemText(i, 2, "-----");
			m_ListWpt.SetItemText(i, 3, "-----");
		}
	}
}

void CRouteDlg::OnBnClickedBtnsavekml()
{
	CFileDialog ofd(FALSE, "kml", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "KML(*.kml)|*.kml\0");
	
	if (ofd.DoModal() == IDOK)
	{
		xml_document doc;
		xml_node kml = doc.append_child();
		kml.set_name("kml");
		kml.append_attribute("xmlns") = "http://earth.google.com/kml/2.2";

		xml_node placemark = kml.append_child();
		placemark.set_name("Placemark");

		placemark.append_child().set_name("LineString");
		placemark.child("LineString").append_child().set_name("extrude");
		placemark.child("LineString").child("extrude").append_child(node_pcdata).set_value("1");
		placemark.child("LineString").append_child().set_name("altitudeMode");
		placemark.child("LineString").child("altitudeMode").append_child(node_pcdata).set_value("relativeToGround");

		placemark.child("LineString").append_child().set_name("coordinates");

		CString str;
		for (int i = 0; i < 250; ++i)
		{
			CString t;
			t = m_ListWpt.GetItemText(i, 2);
			if (t == "-----" || t == "<undefined>")
				break;
			str += t;
			str += ",";
			str += m_ListWpt.GetItemText(i, 1);
			str += ",";
			str += m_ListWpt.GetItemText(i, 3);
			str += " ";
		}
		placemark.child("LineString").child("coordinates").append_child(node_pcdata).set_value(str.GetBuffer());

		doc.save_file(ofd.GetOFN().lpstrFile, "  ");
	}
}

void CRouteDlg::OnBnClickedBtnsetwpt()
{
	CString str, str1, str2, str3;
	
	m_WptNum.GetWindowText(str);
	m_WptLat.GetWindowText(str1);
	m_WptLon.GetWindowText(str2);
	m_WptAlt.GetWindowText(str3);

	m_WptNum.SetWindowText("");
	m_WptLat.SetWindowText("");
	m_WptLon.SetWindowText("");
	m_WptAlt.SetWindowText("");


	int i = atoi(str.GetBuffer());

	if (str == "" || i >= NUMOFWPT)
		return;

	m_wptState[i] = 3;

	if (str1.Find("--") != -1 || 
		str2.Find("--") != -1 || 
		str3.Find("--") != -1 || 
		str1 == "" || str2 == "" || str3 == "")
	{
		m_ListWpt.SetItemText(i, 1, "-----");
		m_ListWpt.SetItemText(i, 2, "-----");
		m_ListWpt.SetItemText(i, 3, "-----");
	}
	else
	{
		m_ListWpt.SetItemText(i, 1, str1);
		m_ListWpt.SetItemText(i, 2, str2);
		m_ListWpt.SetItemText(i, 3, str3);
	}
}


void CRouteDlg::OnBnClickedBtngotowpt()
{
	CString str;
	
	m_WptNum.GetWindowText(str);
	dataXchg_->GotoWpt(atoi(str.GetBuffer()));
}
