// Compas.cpp : implementation file
//

#include "stdafx.h"
#include "UAVtest.h"
#include "Compas.h"
#include <math.h>


// CCompas

IMPLEMENT_DYNAMIC(CCompas, CWnd)

CCompas::CCompas()
: baseSize(256)
, m_heading(0)
{

}

CCompas::~CCompas()
{
}


BEGIN_MESSAGE_MAP(CCompas, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// Установка курса
void CCompas::SetValue(double heading)
{
	if (heading >= 360.0)
		heading -= 0.0;
	if (heading < 0.0)
		heading += 360.0;	
	
	if (m_heading != heading)
	{
		m_heading = heading;

		CDC *pDC = GetDC();
		Draw(pDC);
		ReleaseDC(pDC);
	}
}


void CCompas::OnPaint()
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



void CCompas::Draw(CDC *pDC)
{
	const int xcorr = 3;	// магическая коррекция центра
	const int ycorr = 3;	// магическая коррекция центра

	CDC dcImage;	// дополнительный контекст для битмапов
	dcImage.CreateCompatibleDC(pDC);

	// область вывода
	CRect r;
	GetClientRect(r);

	// контекст+битмап, в который будет вывод конечного изображения
	CDC dcBuf;
	CBitmap bmBuf;
	dcBuf.CreateCompatibleDC(pDC);
	bmBuf.CreateCompatibleBitmap(pDC, baseSize, baseSize);
	dcBuf.SelectObject(&bmBuf);

	// заполнение цветом фона
	CBrush brFrame(GetSysColor(COLOR_3DFACE));
	CRect r2(0, 0, baseSize, baseSize);
	dcBuf.FillRect(&r2, &brFrame);

	// рамка и фон прибора
	CBitmap gaugeBg;
	CBitmap gaugeBgMask;
	gaugeBg.LoadBitmap(IDB_COMPAS);
	gaugeBgMask.LoadBitmap(IDB_COMPAS_MSK);
	dcImage.SelectObject(&gaugeBg);
	dcBuf.MaskBlt(0, 0, baseSize, baseSize, &dcImage, 0, 0, gaugeBgMask, 0, 0, MAKEROP4(SRCAND, SRCCOPY));

	// вывод указателя курса
	CPen pen1(PS_SOLID, 4, RGB(200, 200, 200));
	CPen pen2(PS_SOLID, 4, RGB(100, 100, 100));
	CPen *oldPen = dcBuf.SelectObject(&pen1);

	const double pi = 3.14159;
	const int arrowSize = baseSize - 70;
	double hsin = sin(m_heading * pi / 180.0);
	double hcos = cos(m_heading * pi / 180.0);
	dcBuf.MoveTo(baseSize / 2 + xcorr, baseSize / 2 - ycorr);
	dcBuf.LineTo((baseSize + hsin * arrowSize) / 2 + xcorr, (baseSize - hcos * arrowSize) / 2 - ycorr);

	dcBuf.SelectObject(&pen2);
	dcBuf.MoveTo(baseSize / 2 + xcorr, baseSize / 2 - ycorr);
	dcBuf.LineTo((baseSize - hsin * arrowSize) / 2 + xcorr, (baseSize + hcos * arrowSize) / 2 - ycorr);

	dcBuf.SelectObject(oldPen);

	// вывод прибора на экран
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(r.left, r.top, r.Width(), r.Height(), &dcBuf, 0, 0, baseSize, baseSize, SRCCOPY);
	
}