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

	// ������
	double m_altitude;

	void CAlt::Draw(CDC *pDC);
public:
	// ��������� ������
	void SetValue(double altitude);
};


