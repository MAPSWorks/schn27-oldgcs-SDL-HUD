#pragma once


// CAviaHorizon

class CAviaHorizon : public CWnd
{
	DECLARE_DYNAMIC(CAviaHorizon)

public:
	CAviaHorizon();
	virtual ~CAviaHorizon();

protected:
	//{{AFX_MSG(CAviaHorizon)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	const int baseSize;
	const int pitchWidth;
	const int pitchHeight;
	const double pitchScale;

	// Тангаж
	double m_pitch;
	// Крен
	double m_roll;

	void CAviaHorizon::Draw(CDC *pDC);
public:
	// Установка крена и тангажа
	void SetValue(double pitch, double roll);
};


