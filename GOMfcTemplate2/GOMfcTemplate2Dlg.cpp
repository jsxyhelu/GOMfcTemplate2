
//////////////////////////////////////////////////////////////////////////////
//名称：GOMfcTemplate2
//功能：opencv+mfc 框架
//作者：jsxyhelu(1755311380@qq.com http://jsxyhelu.cnblogs.com)
//组织：GREENOPEN
//日期：2016-10-30
/////////////////////////////////////////////////////////////////////////////

// GOMfcTemplate2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GOMfcTemplate2.h"
#include "GOMfcTemplate2Dlg.h"
#include "afxdialogex.h"
using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//摄像头显示循环,所有关于采集的操作是通过主线程传递控制变量到采集线程，而后由采集线程完成的
DWORD WINAPI CaptureThread(LPVOID lpParameter)
{
	CGOMfcTemplate2Dlg* pDlg = (CGOMfcTemplate2Dlg*)lpParameter;
	while (true)
	{
		if (pDlg->b_closeCam)//退出循环
			break;
		//从directX中获得当前图像并显示出来
		IplImage* queryframe  = pDlg->cameraDs.QueryFrame();
		//在2.0版本中可以强转，在3.0中需要使用函数
		Mat camframe = cvarrToMat(queryframe);
		pDlg->showImage(camframe,IDC_CAM);
		//采集并显示图像?
		//注意！这里我将采集到的图像的显示写在了线程中，这样能够强制窗口重画 jsxyhelu 2016年10月30日 
		if (pDlg->b_takeApic )
		{
			pDlg->m_mainframe = camframe.clone();
			pDlg->b_takeApic = false;
		}
		if (pDlg->m_mainframe.rows > 0)
		{
			pDlg->showImage(pDlg->m_mainframe,IDC_PIC);
		}
		 
	}

	return 0;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGOMfcTemplate2Dlg 对话框



CGOMfcTemplate2Dlg::CGOMfcTemplate2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGOMfcTemplate2Dlg::IDD, pParent)
	, m_nCamCount(0)
	, m_iCamNum(0)
	, b_takeApic(false)
	, b_closeCam(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGOMfcTemplate2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAM, m_CBNCamList);
}

BEGIN_MESSAGE_MAP(CGOMfcTemplate2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPENCAM, &CGOMfcTemplate2Dlg::OnBnClickedBtnOpencam)
	ON_CBN_SELCHANGE(IDC_COMBO_CAM, &CGOMfcTemplate2Dlg::OnCbnSelchangeComboCam)
	ON_BN_CLICKED(IDC_BTN_TAKEPIC, &CGOMfcTemplate2Dlg::OnBnClickedBtnTakepic)
	ON_BN_CLICKED(IDC_BTN_CONFIG, &CGOMfcTemplate2Dlg::OnBnClickedBtnConfig)
	ON_BN_CLICKED(IDC_BTN_CLOSECAM, &CGOMfcTemplate2Dlg::OnBnClickedBtnClosecam)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_RATIO, &CGOMfcTemplate2Dlg::OnBnClickedBtnRatio)
END_MESSAGE_MAP()


// CGOMfcTemplate2Dlg 消息处理程序

BOOL CGOMfcTemplate2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_nCamCount = CCameraDS::CameraCount();//摄像头总数
	//获得摄像头数目
	char camera_name[1024];
	char istr[25];
	for(int i=0; i < m_nCamCount; i++)
	{  
		int retval = CCameraDS::CameraName(i, camera_name, sizeof(camera_name) );
		sprintf_s(istr, " # %d", i);
		strcat_s(camera_name,istr );  
		CString camstr(camera_name);
		if(retval >0)
			m_CBNCamList.AddString(camstr);
		else
			AfxMessageBox(_T("不能获取摄像头的名称"));
	} 
	//初始化显示控件
	CRect rect;
	GetDlgItem(IDC_CAM)->GetClientRect(&rect);
	m_mainframe = Mat::zeros(rect.Height(),rect.Width(),CV_8UC3);
	GetDlgItem(IDC_PIC)->GetClientRect(&rect);
	m_takepic  = Mat::zeros(rect.Height(),rect.Width(),CV_8UC3);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGOMfcTemplate2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGOMfcTemplate2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//绘制图片显示区域
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGOMfcTemplate2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGOMfcTemplate2Dlg::OnBnClickedBtnOpencam()
{
	if (m_nCamCount>=1)//开视频捕获线程
	{	
		 HANDLE hThread = NULL;
		 DWORD dwThreadID = 0;
		 OnBnClickedBtnClosecam();//首先关闭现有摄像头
		 bool bret = cameraDs.OpenCamera(m_iCamNum,false,640,480); //尝试打开摄像头
		 if (bret)
		 {  
			 b_closeCam = false;
			 hThread = CreateThread(NULL, 0, CaptureThread, this, 0, &dwThreadID);
		 }
		
	}
	else
	{
		AfxMessageBox(_T("请确认至少有摄像头连上了"));
	}
}


void CGOMfcTemplate2Dlg::OnCbnSelchangeComboCam()
{
	m_iCamNum = m_CBNCamList.GetCurSel();
}


void CGOMfcTemplate2Dlg::showImage(Mat& src, UINT ID)
{
	if (src.empty())
		return;
	CRect rect;
	Mat dst = src.clone();
	GetDlgItem(ID) ->GetClientRect( &rect );	// 获取控件尺寸位置
	if (dst.channels() == 1)
		cvtColor(dst, dst, CV_GRAY2BGR);
	CDC* pDC = GetDlgItem( ID ) ->GetDC();   
	HDC hDC = pDC ->GetSafeHdc();				// 获取 HDC(设备句柄) 来进行绘图操作
	CvvImage cimg;
	IplImage cpy = dst;
	cimg.CopyOf( &cpy );						// 复制图片
	cimg.DrawToHDC( hDC, &rect );				// 将图片绘制到显示控件的指定区域内
	ReleaseDC( pDC );
}


void CGOMfcTemplate2Dlg::OnBnClickedBtnTakepic()
{
	b_takeApic = true;
	Sleep(100);//等待采集线程返回 jsxyhelu
	//imwrite("abc.jpg",m_mainframe);
}


void CGOMfcTemplate2Dlg::OnBnClickedBtnConfig()
{
	cameraDs.DisplayFilterProperties();
}


void CGOMfcTemplate2Dlg::OnBnClickedBtnClosecam()
{
	 //尝试关闭摄像头
	b_closeCam = true;
	Sleep(100);
	cameraDs.CloseCamera();
}


void CGOMfcTemplate2Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	OnBnClickedBtnClosecam();//窗体销毁之前尝试关闭所有摄像头和线程
}



void CGOMfcTemplate2Dlg::OnBnClickedBtnRatio()
{
	OnBnClickedBtnClosecam();
	if (m_nCamCount>=1)//开视频捕获线程
	{	
		HANDLE hThread = NULL;
		DWORD dwThreadID = 0;
		OnBnClickedBtnClosecam();//首先关闭现有摄像头
		bool bret = cameraDs.OpenCamera(m_iCamNum,true,640,480); //尝试打开摄像头
		if (bret)
		{  
			b_closeCam = false;
			hThread = CreateThread(NULL, 0, CaptureThread, this, 0, &dwThreadID);
		}

	}
	else
	{
		AfxMessageBox(_T("请确认至少有摄像头连上了"));
	}
}
