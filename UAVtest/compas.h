#pragma once


// CCompas

class CCompas : public CWnd
{
	DECLARE_DYNAMIC(CCompas)

public:
	CCompas();
	virtual ~CCompas();

protected:
	//{{AFX_MSG(CCompas)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	const int baseSize;

	// курс
	double m_heading;

	void CCompas::Draw(CDC *pDC);
public:
	// Установка курса
	void SetValue(double heading);
};


