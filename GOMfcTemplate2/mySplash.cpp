#include "stdafx.h"

#include "mySplash.h"

IMPLEMENT_DYNAMIC(mySplash, CWnd)

mySplash::mySplash()

{

}

mySplash::~mySplash()

{

}

BEGIN_MESSAGE_MAP(mySplash, CWnd)

	ON_WM_PAINT()

	ON_WM_TIMER()

END_MESSAGE_MAP()

void mySplash::Create(UINT nBitmapID)

{

	m_bitmap.LoadBitmap(nBitmapID);

	BITMAP bitmap;

	m_bitmap.GetBitmap(&bitmap);

	CreateEx(0,AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),NULL, WS_POPUP | WS_VISIBLE, 0, 0, bitmap.bmWidth, bitmap.bmHeight, NULL, NULL);

}

void mySplash::OnPaint()

{

	CPaintDC dc(this); 

	BITMAP bitmap;

	m_bitmap.GetBitmap(&bitmap);

	CDC dcComp;

	dcComp.CreateCompatibleDC(&dc);

	dcComp.SelectObject(&m_bitmap);

	dc.BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcComp, 0, 0, SRCCOPY);

}

void mySplash::OnTimer(UINT_PTR nIDEvent)

{
	DestroyWindow(); //销毁初始画面窗口
}
