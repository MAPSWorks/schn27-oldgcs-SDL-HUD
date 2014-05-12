// Speed.cpp : implementation file
//

#include "stdafx.h"
#include "UAVtest.h"
#include "Speed.h"
#include <math.h>


// CSpeed

IMPLEMENT_DYNAMIC(CSpeed, CWnd)

CSpeed::CSpeed()
: baseSize(256)
, m_speed(0)
{

}

CSpeed::~CSpeed()
{
}


BEGIN_MESSAGE_MAP(CSpeed, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// Установка высоты
void CSpeed::SetValue(double speed)
{
	if (m_speed != speed)
	{
		m_speed = speed;

		CDC *pDC = GetDC();
		Draw(pDC);
		ReleaseDC(pDC);
	}
}


void CSpeed::OnPaint()
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



void CSpeed::Draw(CDC *pDC)
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
	gaugeBg.LoadBitmap(IDB_SPEED);
	gaugeBgMask.LoadBitmap(IDB_SPEED_MSK);
	dcImage.SelectObject(&gaugeBg);
	dcBuf.MaskBlt(0, 0, baseSize, baseSize, &dcImage, 0, 0, gaugeBgMask, 0, 0, MAKEROP4(SRCAND, SRCCOPY));

	// вывод стрелочки
	CPen pen(PS_SOLID, 4, RGB(200, 200, 200));
	CPen *oldPen = dcBuf.SelectObject(&pen);

	const double pi = 3.14159;
	const int arrowSize = baseSize - 70;
	double a = (m_speed / 200.0) * 2 * pi;
	double rsin = sin(a);
	double rcos = cos(a);

	dcBuf.MoveTo((baseSize) / 2 + xcorr, (baseSize) / 2 - ycorr);
	dcBuf.LineTo((baseSize + rsin * arrowSize) / 2 + xcorr, (baseSize - rcos * arrowSize) / 2 - ycorr);

	dcBuf.SelectObject(oldPen);

	// вывод прибора на экран
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(r.left, r.top, r.Width(), r.Height(), &dcBuf, 0, 0, baseSize, baseSize, SRCCOPY);
}