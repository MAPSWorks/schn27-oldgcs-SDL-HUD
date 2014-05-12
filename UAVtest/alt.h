#pragma once


// CAlt

class CAlt : public CWnd
{
	DECLARE_DYNAMIC(CAlt)

public:
	CAlt();
	virtual ~CAlt();

protected:
	//{{AFX_MSG(CAlt)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	const int baseSize;

	// высота
	double m_altitude;

	void CAlt::Draw(CDC *pDC);
public:
	// Установка высоты
	void SetValue(double altitude);
};


