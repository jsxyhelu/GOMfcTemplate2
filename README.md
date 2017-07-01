# GOMfcTemplate2
      http://www.cnblogs.com/jsxyhelu/p/GOMFCTemplate2.html
      编写带界面的图像处理程序，选择opencv+mfc是一种很好的选择；在读取摄像头数据方面，网上的方法很多，其中shiqiyu的camerads的方法是较好的。
      基于现有资料，通过在实际项目中的积累，我总结出来一套结合opencv和mfc的摄像头采集框架。具有以下特点：
      1、基于directshow，兼容性好，速度快。到目前为止，无论是工业相机还是普通相机，没发现不兼容的；
      2、摄像头部分通过线程读取，保证界面的运行流畅；
      3、框架经过多次打磨，已经比较稳定，不会出现异常错误；代码简洁明了，方便复用。
 
一、代码解析
       框架为对话框模式代码生成，加入CameraDS类和CvvImage类。
      
CameraDS是shiqiyu编写的，主要完成directshow的引入，提供了以下函数。能够获得目前相机总数，读取相机名称，打开相机以及获得当前帧的数据等
 

//打开摄像头，nCamID指定打开哪个摄像头，取值可以为0,1,2,...
//bDisplayProperties指示是否自动弹出摄像头属性页
//nWidth和nHeight设置的摄像头的宽和高，如果摄像头不支持所设定的宽度和高度，则返回false
boolCCameraDS::OpenCamera(int nCamID,bool bDisplayProperties=true,int nWidth=320,int nHeight=240);
//关闭摄像头，析构函数会自动调用这个函数
voidCloseCamera();
//返回摄像头的数目
//可以不用创建CCameraDS实例，采用int c=CCameraDS::CameraCount();得到结果。
staticintCameraCount();
//根据摄像头的编号返回摄像头的名字
//nCamID: 摄像头编号
//sName: 用于存放摄像头名字的数组
//nBufferSize: sName的大小
//可以不用创建CCameraDS实例，采用CCameraDS::CameraName();得到结果。
staticintCCameraDS::CameraName(int nCamID,char* sName,int nBufferSize);
//返回图像宽度
intGetWidth(){return m_nWidth;}
//返回图像高度
intGetHeight(){return m_nHeight;}
//抓取一帧，返回的IplImage不可手动释放！
//返回图像数据的为RGB模式的Top-down(第一个字节为左上角像素)，即IplImage::origin=0(IPL_ORIGIN_TL)
IplImage*QueryFrame();
voidDisplayPinProperties(void);

 
CvvImage类是Opencv自己提供的，这里使用它的主要目的是讲mat对象画到mfc的控件中去

CvvImage cimg;
IplImage cpy = dst;
cimg.CopyOf(&cpy );// 复制图片
cimg.DrawToHDC( hDC,&rect );// 将图片绘制到显示控件的指定区域内

在GOMfcTemplate2Dlg中是主要代码，分为以下几个部分。这块的东西主要是我自己总结的。
1、摄像头显示循环，是单独的线程

//摄像头显示循环
DWORD WINAPI CaptureThread(LPVOID lpParameter)
{
CGOMfcTemplate2Dlg* pDlg =(CGOMfcTemplate2Dlg*)lpParameter;
while(true)
{
IplImage* queryframe = pDlg->cameraDs.QueryFrame();
Mat matframe(queryframe);//iplimage到Mat转化
if(pDlg->b_closeCam)//退出循环
break;
if(pDlg->b_takeApic )
{
pDlg->b_takeApic =false;
pDlg->m_mainframe = matframe;
Sleep(500);
}
pDlg->showImage(matframe,IDC_CAM);
}
return0;
}

这个线程函数，在创建的时候读取主Dlg的指针为参数，这样能够进行线程间通信。它主要完成两项工作，一个是通过camerads的QueryFrame函数读取当前的图像并传递给主线程;一个是判断b_closeCam和b_taleApic两个控制变量是否为true并进行相关操作。
目前的线程间通信采用的变量共享的方式，由于在摄像头线程中是写变量，在主线程中是读变量，一般不会冲突。但是如果摄像头很多或者实时性非常高，还是应该采用postmessage的方式通信。
2、initdialog中，对界面控件进行初始化

m_nCamCount =CCameraDS::CameraCount();//摄像头总数
//获得摄像头数目
char camera_name[1024];
char istr[25];
for(int i=0; i < m_nCamCount; i++)
{
int retval =CCameraDS::CameraName(i, camera_name,sizeof(camera_name));
sprintf_s(istr," # %d", i);
strcat_s(camera_name,istr );
CString camstr = camera_name;
if(retval >0)
m_CBNCamList.AddString(camstr);
else
AfxMessageBox(_T("不能获取摄像头的名称"));
}
//初始化显示控件
CRect rect;
GetDlgItem(IDC_CAM)->GetClientRect(&rect);
m_mainframe =Mat::zeros(rect.Height(),rect.Width(),CV_8UC3);
GetDlgItem(IDC_PIC)->GetClientRect(&rect);
m_takepic =Mat::zeros(rect.Height(),rect.Width(),CV_8UC3);
return TRUE;// 除非将焦点设置到控件，否则返回 TRUE

包括填写combolist控件，为两个用于显示的static控件生成对应大小的mat变量等。
3、打开摄像头，主要就是根据选择的摄像头名称，创建摄像头线程

voidCGOMfcTemplate2Dlg::OnBnClickedBtnOpencam()
{
if(m_nCamCount>=1)//开视频捕获线程
{
HANDLE hThread = NULL;
DWORD dwThreadID =0;
OnBnClickedBtnClosecam();//首先关闭现有摄像头
bool bret = cameraDs.OpenCamera(m_iCamNum,false,640,480);//尝试打开摄像头
if(bret)
{
b_closeCam =false;
hThread =CreateThread(NULL,0,CaptureThread,this,0,&dwThreadID);
}
}
else
{
AfxMessageBox(_T("请确认至少有摄像头连上了"));
}
}

稍作修改，可以用于多摄像头，这个是完全没有问题并且做过实际项目的。
4、关闭摄像头

voidCGOMfcTemplate2Dlg::OnBnClickedBtnClosecam()
{
//尝试关闭摄像头
b_closeCam =true;
Sleep(100);
cameraDs.CloseCamera();
}

传递控制变量到摄像头线程，并且调用camerads的closecamera函数关闭摄像头；
5、采集图片

voidCGOMfcTemplate2Dlg::OnBnClickedBtnTakepic()
{
b_takeApic =true;
Sleep(100);
if(m_mainframe.rows >0)
{
showImage(m_mainframe,IDC_PIC);
}
}

传递控制变量到摄像头线程,并且显示图片到控件。
6、显示图像函数，为了方便地讲mat对象显示到mfc的控件上，编写图像实现函数

voidCGOMfcTemplate2Dlg::showImage(Mat& src, UINT ID)
{
if(src.empty())
return;
CRect rect;
Mat dst = src.clone();
GetDlgItem(ID)->GetClientRect(&rect );// 获取控件尺寸位置
if(dst.channels()==1)
cvtColor(dst, dst, CV_GRAY2BGR);
CDC* pDC =GetDlgItem( ID )->GetDC();
HDC hDC = pDC ->GetSafeHdc();// 获取 HDC(设备句柄) 来进行绘图操作
CvvImage cimg;
IplImage cpy = dst;
cimg.CopyOf(&cpy );// 复制图片
cimg.DrawToHDC( hDC,&rect );// 将图片绘制到显示控件的指定区域内
ReleaseDC( pDC );
}

主要就是调用cvvimage的drawtohdc函数，并进行相关的错误控制。
二、存在问题
      由于directshow本身是com架构，学习曲线陡峭。目前这个框架还存在至少两个问题：
      1、摄像头熟悉配置问题，以及配置的保持问题。目前框架中的摄像头配置没有实现，就是下图的这种能够调整摄像头参数的对话框如何出来。并且上一次的配置数据要能够保存下来。
      
      2、视频数据采集问题。
      目前我采用的是xvid的摄像头数据采集方式，能够解决问题。但是我认为directshow应该本身就能够才是摄像头数据，并且保存为.avi。这个方面还需要继续研究。
      我会在下一步工作中继续总结这方面资料，同时非常希望能够得到高人的指点，感谢！
 
