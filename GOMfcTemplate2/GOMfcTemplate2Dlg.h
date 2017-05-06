
//////////////////////////////////////////////////////////////////////////////
//名称：GOMfcTemplate2
//功能：opencv+mfc 框架
//作者：jsxyhelu(1755311380@qq.com http://jsxyhelu.cnblogs.com)
//组织：GREENOPEN
//日期：2016-10-30
/////////////////////////////////////////////////////////////////////////////
// GOMfcTemplate2Dlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "camerads.h"
using namespace std;
using namespace cv;

// CGOMfcTemplate2Dlg 对话框
class CGOMfcTemplate2Dlg : public CDialogEx
{
// 构造
public:
	CGOMfcTemplate2Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GOMFCTEMPLATE2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	int m_nCamCount;
public:
	CComboBox m_CBNCamList;
	afx_msg void OnBnClickedBtnOpencam();
	Mat m_mainframe;
	Mat m_takepic;
	CCameraDS cameraDs;
	int m_iCamNum;
	afx_msg void OnCbnSelchangeComboCam();
	void showImage(Mat& src, UINT ID);
	afx_msg void OnBnClickedBtnTakepic();
	afx_msg void OnBnClickedBtnConfig();
	afx_msg void OnBnClickedBtnClosecam();
	bool b_closeCam;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtnRatio();
	bool b_takeApic;
};
