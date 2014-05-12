// Alt.cpp : implementation file
//

#include "stdafx.h"
#include "UAVtest.h"
#include "Alt.h"
#include <math.h>


// CAlt

IMPLEMENT_DYNAMIC(CAlt, CWnd)

CAlt::CAlt()
	: baseSize(256)
	, m_altitude(0)
{
}

CAlt::~CAlt()
{
}


BEGIN_MESSAGE_MAP(CAlt, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// Установка высоты
void CAlt::SetValue(double altitude)
{
	if (m_altitude == altitude)
		return;

	m_altitude = altitude;

	CDC *pDC = GetDC();
	Draw(pDC);
	ReleaseDC(pDC);
}


void CAlt::OnPaint()
{
	PAINTSTRUCT ps;
	CDC *pDC = BeginPaint(&ps);

	if (pDC == NULL)
		return;

	Draw(pDC);

	EndPaint(&ps);
}



void CAlt::Draw(CDC *pDC)
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
	gaugeBg.LoadBitmap(IDB_ALT);
	gaugeBgMask.LoadBitmap(IDB_ALT_MSK);
	dcImage.SelectObject(&gaugeBg);
	dcBuf.MaskBlt(0, 0, baseSize, baseSize, &dcImage, 0, 0, gaugeBgMask, 0, 0, MAKEROP4(SRCAND, SRCCOPY));

	// вывод стрелочек
	CPen pen1(PS_SOLID, 8, RGB(200, 200, 200));
	CPen *oldPen = dcBuf.SelectObject(&pen1);

	const double pi = 3.14159;
	const int arrow1Size = baseSize - 70;
	const int arrow2Size = baseSize - 140;
	int alth = m_altitude / 1000;
	int altl = m_altitude - alth;
	double r1sin = sin((altl / 1000.0) * 2 * pi);
	double r1cos = cos((altl / 1000.0) * 2 * pi);
	double r2sin = sin((m_altitude / 10000.0) * 2 * pi);
	double r2cos = cos((m_altitude / 10000.0) * 2 * pi);

	dcBuf.MoveTo((baseSize) / 2 + xcorr, (baseSize) / 2 - ycorr);
	dcBuf.LineTo((baseSize + r2sin * arrow2Size) / 2 + xcorr, (baseSize - r2cos * arrow2Size) / 2 - ycorr);

	CPen pen2(PS_SOLID, 4, RGB(200, 200, 200));
	dcBuf.SelectObject(&pen2);


	dcBuf.MoveTo((baseSize) / 2 + xcorr, (baseSize) / 2 - ycorr);
	dcBuf.LineTo((baseSize + r1sin * arrow1Size) / 2 + xcorr, (baseSize - r1cos * arrow1Size) / 2 - ycorr);

	dcBuf.SelectObject(oldPen);

	// вывод прибора на экран
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(r.left, r.top, r.Width(), r.Height(), &dcBuf, 0, 0, baseSize, baseSize, SRCCOPY);
	
}