// AviaHorizon.cpp : implementation file
//

#include "stdafx.h"
#include "UAVtest.h"
#include "AviaHorizon.h"
#include <math.h>


// CAviaHorizon

IMPLEMENT_DYNAMIC(CAviaHorizon, CWnd)

CAviaHorizon::CAviaHorizon()
: baseSize(256)
, pitchHeight(820)
, pitchWidth(186)
, pitchScale(3.55)
, m_pitch(0)
, m_roll(0)
{

}

CAviaHorizon::~CAviaHorizon()
{
}


BEGIN_MESSAGE_MAP(CAviaHorizon, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// Установка крена и тангажа
void CAviaHorizon::SetValue(double pitch, double roll)
{
	if (pitch > 90.0)
		pitch = 90.0;
	if (pitch < -90.0)
		pitch = -90.0;
	if (roll > 180.0)
		pitch = 180.0;
	if (pitch < -180.0)
		pitch = -180.0;
	
	if (m_pitch != pitch || m_roll != roll)
	{
		m_pitch = pitch;
		m_roll = roll;

		CDC *pDC = GetDC();
		Draw(pDC);
		ReleaseDC(pDC);
	}
}


void CAviaHorizon::OnPaint()
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



void CAviaHorizon::Draw(CDC *pDC)
{
	const int ycorr = 6;	// магическая коррекция центра

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

	// вывод шкалы тангажа
	CBitmap gaugePitch;
	CBitmap gaugePitchMask;
	gaugePitch.LoadBitmap(IDB_HORIZON_PITCH);
	dcImage.SelectObject(&gaugePitch);
	int y = (pitchHeight - baseSize) / 2 - (int)(m_pitch * pitchScale + 0.5) + ycorr - 1;
	dcBuf.BitBlt((baseSize - pitchWidth) / 2, 0, baseSize, baseSize, &dcImage, 0, y, SRCCOPY);

	// вывод указателя крена
	CPen pen(PS_SOLID, 3, RGB(200, 200, 200));
	CPen *oldPen = dcBuf.SelectObject(&pen);

	const int fuz = 45;	// размер "фюзеляжа" на указателе

	const double pi = 3.14159;
	double a = -m_roll * pi / 180.0;
	double rsin = sin(a);
	double rcos = cos(a);
	dcBuf.MoveTo((baseSize - rcos * pitchWidth) / 2, (baseSize + rsin * pitchWidth) / 2 - ycorr);
	dcBuf.LineTo((baseSize - rcos * fuz) / 2, (baseSize + rsin * fuz) / 2 - ycorr);
	dcBuf.MoveTo((baseSize + rcos * fuz) / 2, (baseSize - rsin * fuz) / 2 - ycorr);
	dcBuf.LineTo((baseSize + rcos * pitchWidth) / 2, (baseSize - rsin * pitchWidth) / 2 - ycorr);

	dcBuf.Arc(CRect((baseSize - fuz) / 2, (baseSize - fuz) / 2 - ycorr, (baseSize + fuz) / 2, (baseSize + fuz) / 2 - ycorr), 
		CPoint((baseSize - fuz * rcos) / 2, (baseSize + fuz * rsin) / 2 - ycorr), 
		CPoint((baseSize + fuz * rcos) / 2, (baseSize - fuz * rsin) / 2 - ycorr));

	dcBuf.SelectObject(oldPen);

	// рамка прибора
	CDC dcBufFrame;	// дополнительный контекст для формирования рамки
	dcBufFrame.CreateCompatibleDC(pDC);
	CBitmap bmBufFrame;
	bmBufFrame.CreateCompatibleBitmap(pDC, baseSize, baseSize);
	dcBufFrame.SelectObject(&bmBufFrame);


	// заполнение цветом фона
	CBrush brFrame(GetSysColor(COLOR_3DFACE));
	CRect r2(0, 0, baseSize, baseSize);
	dcBufFrame.FillRect(&r2, &brFrame);

	// формирование рамки
	CBitmap gaugeFrame;
	CBitmap gaugeFrameMask;
	gaugeFrame.LoadBitmap(IDB_HORIZON);
	gaugeFrameMask.LoadBitmap(IDB_HORIZON_MSK);
	dcImage.SelectObject(&gaugeFrame);
	dcBufFrame.MaskBlt(0, 0, baseSize, baseSize, &dcImage, 0, 0, gaugeFrameMask, 0, 0, MAKEROP4(SRCAND, SRCCOPY));

	// композиция рамки с прибором
	CBitmap gaugeMask;
	gaugeMask.LoadBitmap(IDB_HORIZON_MSK2);
	dcBuf.MaskBlt(0, 0, baseSize, baseSize, &dcBufFrame, 0, 0, gaugeMask, 0, 0, MAKEROP4(SRCAND, SRCCOPY));

	// вывод прибора на экран
	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(r.left, r.top, r.Width(), r.Height(), &dcBuf, 0, 0, baseSize, baseSize, SRCCOPY);
	
}