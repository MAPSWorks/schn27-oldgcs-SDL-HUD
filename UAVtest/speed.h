#pragma once


// CSpeed

class CSpeed : public CWnd
{
	DECLARE_DYNAMIC(CSpeed)

public:
	CSpeed();
	virtual ~CSpeed();

protected:
	//{{AFX_MSG(CSpeed)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	const int baseSize;

	// ��������
	double m_speed;

	void CSpeed::Draw(CDC *pDC);
public:
	// ��������� ��������
	void SetValue(double speed);
};


