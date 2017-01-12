#pragma once
#include "afxwin.h"
class mySplash :
	public CWnd
{
	DECLARE_DYNAMIC(mySplash)

protected:

	DECLARE_MESSAGE_MAP()

public:

	CBitmap m_bitmap;

	void Create(UINT nBitmapID);

	afx_msg void OnPaint();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	mySplash(void);
	~mySplash(void);
};

