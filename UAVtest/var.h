#pragma once


// CVar

class CVar : public CWnd
{
	DECLARE_DYNAMIC(CVar)

public:
	CVar();
	virtual ~CVar();

protected:
	//{{AFX_MSG(CVar)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	const int baseSize;

	// вертикальная скорость
	double m_vspeed;

	void CVar::Draw(CDC *pDC);
public:
	// Установка вертикальной скорости
	void SetValue(double vspeed);
};


