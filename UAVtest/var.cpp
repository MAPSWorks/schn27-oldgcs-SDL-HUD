// Var.cpp : implementation file
//

#include "stdafx.h"
#include "UAVtest.h"
#include "Var.h"
#include <math.h>


// CVar

IMPLEMENT_DYNAMIC(CVar, CWnd)

CVar::CVar()
: baseSize(256)
, m_vspeed(0)
{

}

CVar::~CVar()
{
}


BEGIN_MESSAGE_MAP(CVar, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// ��������� ������������ ��������
void CVar::SetValue(double vspeed)
{
	if (vspeed > 20.0)
		vspeed = 20.0;

	if (vspeed < -20.0)
		vspeed = -20.0;

	if (m_vspeed != vspeed)
	{
		m_vspeed = vspeed;

		CDC *pDC = GetDC();
		Draw(pDC);
		ReleaseDC(pDC);
	}
}


void CVar::OnPaint()
{
	PAINTSTRUCT ps;
	CDC *pDC = BeginPaint(&ps);

	if (pDC == NULL)
	{
		return;
	}

	Draw(pDC);

	EndPaint(&ps);
}



void CVar::Draw(CDC *pDC)
{
	const int xcorr = 3;	// ���������� ��������� ������
	const int ycorr = 3;	// ���������� ��������� ������

	CDC dcImage;	// �������������� �������� ��� ��������
	dcImage.CreateCompatibleDC(pDC);

	// ������� ������
	CRect r;
	GetClientRect(r);

	// ��������+������, � ������� ����� ����� ��������� �����������
	CDC dcBuf;
	CBitmap bmBuf;
	dcBuf.CreateCompatibleDC(pDC);
	bmBuf.CreateCompatibleBitmap(pDC, baseSize, baseSize);
	dcBuf.SelectObject(&bmBuf);

	// ���������� ������ ����
	CBrush brFrame(GetSysColor(COLOR_3DFACE));
	CRect r2(0, 0, baseSize, baseSize);
	dcBuf.FillRect(&r2, &brFrame);

	// ����� � ��� �������
	CBitmap gaugeBg;
	CBitmap gaugeBgMask;
	gaugeBg.LoadBitmap(IDB_VAR);
	gaugeBgMask.LoadBitmap(IDB_VAR_MSK);
	dcImage.SelectObject(&gaugeBg);
	dcBuf.MaskBlt(0, 0, baseSize, baseSize, &dcImage, 0, 0, gaugeBgMask, 0, 0, MAKEROP4(SRCAND, SRCCOPY));

	// ����� ���������
	CPen pen(PS_SOLID, 4, RGB(200, 200, 200));
	CPen *oldPen = dcBuf.SelectObject(&pen);

	const double pi = 3.14159;
	const int arrowSize = baseSize - 70;
	
	double a = m_vspeed / 23.0 * pi;
	double rsin = sin(a);
	double rcos = cos(a);

	dcBuf.MoveTo((baseSize) / 2 + xcorr, (baseSize) / 2 - ycorr);
	dcBuf.LineTo((baseSize - rcos * arrowSize) / 2 + xcorr, (baseSize - rsin * arrowSize) / 2 - ycorr);

	dcBuf.SelectObject(oldPen);

	// ����� ������� �� �����
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(r.left, r.top, r.Width(), r.Height(), &dcBuf, 0, 0, baseSize, baseSize, SRCCOPY);
	
}