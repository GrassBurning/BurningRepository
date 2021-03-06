
// VTestCheckDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VTestCheck.h"
#include "VTestCheckDlg.h"
#include "afxdialogex.h"
#include "Password.h"
#include "CvvImage.h"
#include "cv.h"
#include "highgui.h"
#include "easysize.h"
#include "tisudshl.h"
#include "General.h"
#include <vector>
#include <deque>
#include <algorithm>
#include <mmsystem.h>
//#include "CmdHelper.h"//这两个头文件是需要包含的，这样才能进行文件的生成
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WMDebug 1
#pragma comment(lib, "winmm")  
// #ifdef _DEBUG
// #pragma comment (lib, "opencv_calib3d231d.lib")
// #pragma comment (lib, "opencv_contrib231d.lib")
// #pragma comment (lib,"opencv_imgproc231d.lib")
// #pragma comment (lib, "opencv_core231d.lib")
// #pragma comment (lib, "opencv_features2d231d.lib")
// #pragma comment (lib, "opencv_flann231d.lib")
// #pragma comment (lib, "opencv_gpu231d.lib")
// #pragma comment (lib, "opencv_highgui231d.lib")
// #pragma comment (lib, "opencv_legacy231d.lib")
// #pragma comment (lib, "opencv_ml231d.lib")
// #pragma comment (lib, "opencv_objdetect231d.lib")
// #pragma comment (lib, "opencv_ts231d.lib")
// #pragma comment (lib, "opencv_video231d.lib")
// //#pragma comment (lib, "opencv_nonfree231d.lib")
// #else
// #pragma comment (lib, "opencv_calib3d231.lib")
// #pragma comment (lib, "opencv_contrib231.lib")
// #pragma comment (lib, "opencv_imgproc231.lib")
// #pragma comment (lib, "opencv_core231.lib")
// #pragma comment (lib, "opencv_features2d231.lib")
// #pragma comment (lib, "opencv_flann231.lib")
// #pragma comment (lib, "opencv_gpu231.lib")
// #pragma comment (lib, "opencv_highgui231.lib")
// #pragma comment (lib, "opencv_legacy231.lib")
// #pragma comment (lib, "opencv_ml231.lib")
// #pragma comment (lib, "opencv_objdetect231.lib")
// #pragma comment (lib, "opencv_ts231.lib")
// #pragma comment (lib, "opencv_video231.lib")
// //#pragma comment (lib, "opencv_nonfree231.lib")
// #endif

// CVTestCheckDlg 对话框
using namespace cv;
using namespace std;
using namespace _DSHOWLIB_NAMESPACE;
using namespace DShowLib;
CPassword password;
CBitmap BitmapPass;
CBitmap BitmapFail;
CBitmap BitmapWait;
CBitmap BitmapStop;
CBitmap BitmapTest;
CVTestCheckDlg * ex = NULL;
Point PrePosBegin(0,0);
Point PrePosEnd(0,0);
Rect SelectRect;
bool SelectFlag;
Mat PShowImg;
BYTE* pBuf[1];
//Grabber grabber;
DShowLib::Grabber* m_pGrabber;
tFrameHandlerSinkPtr pSink;
void onMouse(int Event,int x,int y, int flags,void * param);
UINT TestEntry(LPVOID pParam);
CString strMusicPathOK;
CString strMusicPathNG;
Mutex mTestResult;
HANDLE  hTestResult = NULL;
CString strPicLogPath;

typedef struct SpotInfo
{
	int rows;
	int cols;
}sSpotInfo,*pSpotInfo;

typedef struct RowTestInfo
{
	sSpotInfo sMidPoint;
	sSpotInfo sEnd;
	int iLeftLength;
	int iRightLength;
}RowTestInfo,*pRowTestInfo;
typedef struct SpotState
{
	sSpotInfo Spot;
	int state; //0 黑 //1 白
}sSpotState,*pSpotState;

typedef struct TestPicInfo
{
sSpotInfo NextStartSpot;
sSpotInfo LastStartSpot;
int rows;
int cols;
int channels;
}sTestPicInfo,*pTestPicInfo;

typedef struct TestRectInfo
{
	sSpotInfo StartSpot;  //坐下脚点
	int width;
	int length;
}sTestRectInfo,*pTestRectInfo;


typedef struct PicChangeInfo
{
	int iPosition;
	char cDataR;
	char cDataG;
	char cDataB;
	int state;  /*
				1:背景 2:目标出现 3：目标结束 4：字体出现 5：字体结束

				*/
}sPicChangeInfo,*pPicChangeInfo;
typedef struct TestPicConfig
{
	int iTargetPixMinLength;    //
	int iBackgroundMinLength;     //背景最小长度
	int iBackgroundDeviation;  //多大范围内一直为背景值，则认为背景开始
	int iTargetMinLength;          //目标最小长度
	int iTargetDeviation;       //多大范围内一直为目标值，则认为目标开始


	int iFont1Spec;
	int iFont1MinLength;           //字体最小长度
	int iFont1BandMinWidth;      //字体最小间距
	int iFont1SkipMaxWidth;      //字体跨度最大宽度
	int iFont1Deviation;         //多大范围内一直为字体值，则认为字体开始
	int iFont1MinWidth;

	int iFont2Spec;
	int iFont2MinLength;           //字体最小长度
	int iFont2BandMinWidth;      //字体最小间距
	int iFont2SkipMaxWidth;      //字体跨度最大宽度
	int iFont2Deviation;         //多大范围内一直为字体值，则认为字体开始
	int iFont2MinWidth;

	int iSpotMinLength;            //点最小长度
	int iSpotMinWidth;             //点最小宽度
	int iSpotDeviation;         //多大范围内一直为点值，则认为点开始

	int iBandFirstMinWidth;        //底端第一个白色背景宽度
	int iBandSecondMinWidth ;       //底端第二个白色背景宽度
	int iBandThirdMinWidth;        //底端第三个白色背景宽度



	int iSpotSpec;
	int iSpotSpec1;
	int iTargetSpec1;
	int iTargetSpec2;

	int iBackgroundSpec;

	int iTargetTopBottomMinPlace;         //当目标上下超出该值，则认为目标不存在
	int iTargetLeftRightMinPlace;         //当目标左右超出该值，则认为目标不存在
	
}sTestPicConfig,*pTestPicConfig;

typedef struct TestPicResult
{
	int iEdgeLength;
	int iEdgePosition;
	int iEdgeLeftPosition;
	int iEdgeRightPosition;
	int iSpotFirstLeftPosition;
	int iSpotSecondLeftPosition;
	int iSpotFirstRightPosition;
	int iSpotSecondRightPosition;

	int iBandFirstWidth;
	int iBandSecondWidth;
	int iBandThirdWidth;

	int iFont1BottomPosition;
	int iFont1TopPosition;
	int iFont1BeginPosition;
	int iFont1EndPosition;

	int iFont2BottomPosition;
	int iFont2TopPosition;
	int iFont2BeginPosition;
	int iFont2EndPosition;

	bool bA1CheckResult;
	vector<vector<int>> iA1FontChange;

	bool bRCheckResult;
	vector<vector<int>> iRFontChange;

	bool bUCheckResult;
	vector<vector<int>> iUFontChange;

	bool bBCheckResult;
	vector<vector<int>> iBFontChange;

	bool bA2CheckResult;
	vector<vector<int>> iA2FontChange;

	int iSpotLength;
	int iSpotPosition;
	vector<vector<int>> vFontInfo;

	vector<sSpotInfo> vSpot1;
	vector<sSpotInfo> vSpot2;
} sTestPicResult,*pTestPicResult;

typedef struct SpotLocationLength
{
	sSpotInfo Spot;
	int length;
}sSpotLocationLength,*pSpotLocationLength;

int fLoadConfig(sTestPicConfig& TestConfig);
int CheckPic(Mat * mPic,sTestPicResult& TestResult,sTestPicConfig& TestConfig,Mat* mPicDebug);
int splitcharacter(uchar**pFont,int iFontSkip,int &k,vector<vector<int>>& iFontChange,sTestPicConfig& TestConfig,int & iEdgeRightPosition);
int checkA1(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int checkR(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int checkU(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int checkB(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int checkA2(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
UINT TestBegin(LPVOID pParam);
UINT TestResultHandle(LPVOID pParam);
int FindEndwiseEdge(Mat* mPic,
				sTestPicInfo& TestInfo,
				uchar** ptr[],
				int Skip,
				int Spec,
				deque<sSpotInfo>& EdgeInfo);
int CalculationAngle(sSpotInfo& spot,
	deque<sSpotInfo>& edgeinfo,
	float& angle,
	sSpotInfo& LocationSpot);
int AnalysisData(vector<sSpotLocationLength>& SpotSet,sSpotInfo& LocationSpot);
int CountBandNum(Mat* mPic,
	sTestPicInfo& TestInfo,
	uchar*** pFont,
	float angle,
	int Spec,
	vector<sSpotInfo>& vFontLeftEdge,
	vector<sSpotInfo>& vFontRightEdge,
	int wSpecNum,
	int bSpecNum,
	int bWithSpecNum);
int CheckNewPic(Mat * mPic,sTestPicInfo& TestInfo,uchar** ptr[],int bLength,int wLength,int Spec);
int SkipTargetBand(Mat* mPic,
			sTestPicInfo& TestInfo,
			uchar** ptr[],
			int iTargetSpec,
			int& iTargetTopBottomRange,
			int& iTargetLeftRightRange,
			float angle,
			int iBandWidthSpec,
			int iTargetLengthSpec);
int SkipBandFixFont(Mat* mPic,
	sTestPicInfo& TestInfo,
	uchar** ptr[],
	int iTargetSpec,
	int& iTargetTopBottomRange,
	int& iTargetLeftRightRange,
	float angle,
	int iBandWidthSpec,
	int iTargetSpecLength,
	vector<sSpotInfo>& vFontLeftEdge,
	vector<sSpotInfo>& vFontRightEdge);
CVTestCheckDlg::CVTestCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVTestCheckDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVTestCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_P_PF, m_Picture_PF);
	DDX_Control(pDX, IDC_ListInfo, m_ListBox);
	DDX_Control(pDX, IDC_STATICVIDEO, m_cStaticVideoWindow);
}

BEGIN_MESSAGE_MAP(CVTestCheckDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CVTestCheckDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CVTestCheckDlg::OnBnClickedCancel)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_B_SetPosition, &CVTestCheckDlg::OnBnClickedBSetposition)
	ON_BN_CLICKED(IDC_Config, &CVTestCheckDlg::OnBnClickedConfig)

END_MESSAGE_MAP()
#ifdef EASYSIZEOPEN	
BEGIN_EASYSIZE_MAP(CVTestCheckDlg)
	//EASYSIZE(Control_ID,left,top,right,bottom,options)
	EASYSIZE(IDC_STATICVIDEO,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_P_PF,IDC_STATICVIDEO,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDOK,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDCANCEL,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_Config,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_B_SetPosition,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_S1,ES_KEEPSIZE,IDC_Config,ES_BORDER,ES_KEEPSIZE,0) // test info
	EASYSIZE(IDC_S2,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_Edit_PF,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_ListInfo,IDC_STATICVIDEO,IDC_S1,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP
#endif
// CVTestCheckDlg 消息处理程序

BOOL CVTestCheckDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
#ifdef EASYSIZEOPEN	
	INIT_EASYSIZE;
#endif
	BitmapPass.LoadBitmap(IDB_BITMAP_Pass);
	BitmapFail.LoadBitmap(IDB_BITMAP_Fail);
	BitmapWait.LoadBitmap(IDB_BITMAP_Wait);
	BitmapStop.LoadBitmap(IDB_BITMAP_Stop);
	BitmapTest.LoadBitmap(IDB_BITMAP_Test);
	m_Picture_PF.SetBitmap(BitmapStop);
	DShowLib::InitLibrary();
	atexit( ExitLibrary );
	m_pGrabber = new DShowLib::Grabber();
	ASSERT( m_pGrabber );
	if( !m_pGrabber->loadDeviceStateFromFile( "lastSelectedDeviceState.xml" ) )
	{
		//std::cout << "Device opened from: <" << "lastSelectedDeviceState.xml" << ">." << std::endl;
		if( !m_pGrabber->showDevicePage() || !m_pGrabber->isDevValid() )
		{
			return false;
		}
		m_pGrabber->saveDeviceStateToFile( "lastSelectedDeviceState.xml", true, true, false );
	
	}

	m_pGrabber->setOverlayBitmapPathPosition( ePP_NONE );
//	pSink = FrameHandlerSink::create( eY800, 1 );
	pSink = FrameHandlerSink::create( eRGB24, 3 );

	// We use snap mode.
	pSink->setSnapMode( false );

	// Set the sink.
	m_pGrabber->setSinkType( pSink );
//	m_cStaticVideoWindow.SetWindowPos(NULL,0,0,640,480,SWP_NOMOVE|SWP_NOZORDER);
	CRect rect;
	m_cStaticVideoWindow.GetWindowRect(&rect);
	m_pGrabber->setDefaultWindowPosition(false);   //取消采用Window自带图像映射显示方式
	m_pGrabber->setWindowSize(rect.Width(),rect.Height());
	m_pGrabber->setHWND( m_cStaticVideoWindow.m_hWnd );
//	m_pGrabber->
	
// Prepare the live mode, to get the output size if the sink.
	if( !m_pGrabber->prepareLive( true ) )
	{
		std::cerr << "Could not render the VideoFormat into a eY800 sink.";
		return -1;
	}
#if 0	
	FrameTypeInfo info;
	pSink->getOutputFrameType( info );
	
	pBuf[0] = new BYTE[info.buffersize];

	tMemBufferCollectionPtr pCollection = MemBufferCollection::create(info,1,pBuf);
	pCollection->getBuffer(0)->getSize().cx;
	if( pCollection == 0 || !pSink->setMemBufferCollection( pCollection ) )
	{
		std::cerr << "Could not set the new MemBufferCollection, because types do not match.";
		return -1;
	}
#endif
	
	m_pGrabber->startLive( true );

#if 0
	m_pGrabber->showDevicePage();  //配置对话框
	if( ! m_pGrabber->isDevValid() )
	{
		// Set the window that should display the live video.
		//pGrabber->setHWND( m_hWnd );
		AfxMessageBox( TEXT("No device was selected."));
		return FALSE;
	}
	m_pGrabber->closeDev();
#endif
	char cMusicPath[100];
	CString strMusicPath;
	GetCurrentDirectory(1000,cMusicPath);
	strMusicPath.Format("%s",cMusicPath);
	strMusicPathOK.Format("%s//OK.wav",strMusicPath);
	strMusicPathNG.Format("%s//NG.wav",strMusicPath);
	
	
	strPicLogPath.Format("D:\\Log");
	CreateDirectory(strPicLogPath,NULL);
	UpdateWindow();
	ex = this;
	hTestResult = CreateMutex(NULL,FALSE,"TestResult");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		hTestResult = OpenMutex(MUTEX_ALL_ACCESS,TRUE,"TestResult");
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVTestCheckDlg::OnPaint()
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

// 		//定义并创建一个内存设备环境
// 		CDC  m_dcMemory;
// 		GetDlgItem(IDC_Picture)->GetWindowRect(&rect);  
// 		if( !m_dcMemory.CreateCompatibleDC(&dc) )          // 创建兼容性的DC
// 			return;
// 		SetStretchBltMode(dc.m_hDC,STRETCH_HALFTONE);
		
	}
	else
	{
		CDialogEx::OnPaint();	
// 		m_pGrabber->setDefaultWindowPosition(TRUE);
// 		m_pGrabber->stopLive();
// 		CRect rect1;
// 		GetDlgItem( IDC_Picture )->GetWindowRect(&rect1);
// 		m_pGrabber->setDefaultWindowPosition(false);
// 
// 		m_pGrabber->setWindowSize(rect1.Width(),rect1.Height());
// 		m_pGrabber->setHWND( GetDlgItem( IDC_Picture )->m_hWnd );
// 		m_pGrabber->startLive();

		CRect rect1;
		m_pGrabber->setDefaultWindowPosition(true);
		m_pGrabber->stopLive();
		
		m_cStaticVideoWindow.GetWindowRect(&rect1);
		
		m_pGrabber->setDefaultWindowPosition(false);
		m_pGrabber->setWindowSize(rect1.Width(),rect1.Height());
		m_pGrabber->setHWND( GetDlgItem( IDC_STATICVIDEO )->m_hWnd );
		m_pGrabber->startLive();
		
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVTestCheckDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int ii = 0;

void CVTestCheckDlg::OnBnClickedOk()
{	

// 	AfxBeginThread(TestBegin,NULL);
// 	AfxBeginThread(TestResultHandle,NULL);
// 	

//	return ;
	sTestPicConfig TestConfig;
	sTestPicResult TestResult;
	Mat mPicDebug;
#if 1
	
	Mat srcImage1 = imread("D:\\Log\\2017-11-25-14-33-26_163_164.bmp",1);
//	Mat srcImage1 = imread("E:\\Work\\opencv\\VTestCheck\\VTestCheck\\23445.bmp",1);
//	Mat srcImage1 = imread("E:\\Work\\opencv\\VTestCheck\\VTestCheck\\zz.bmp",1);
//	Mat srcImage1 = imread("D:\\Work\\opencv\\VTestCheck\\BurningRepository\\VTestCheck\\zz.bmp",1);

	fLoadConfig(TestConfig);
#ifdef WMDebug
	srcImage1.copyTo(mPicDebug);
#endif
	CheckPic(&srcImage1,TestResult,TestConfig,&mPicDebug);
	//	imshow("Example1",srcImage1);
	//	cvWaitKey();
	imwrite("cc.bmp",mPicDebug);
	return ;

#else
	long height,width;
	Mat *a;
	int c;
//	width = m_pGrabber->getAcqSizeMaxX();
//	height = m_pGrabber->getAcqSizeMaxY();
//	pSink->setSnapMode( false );
//	m_pGrabber->startLive(false);
//	pSink->snapImages( 1 ); 
	smart_ptr<MemBuffer> pBuffer = pSink->getLastAcqMemBuffer();
	width = pBuffer->getSize().cx;
	height = pBuffer->getSize().cy;
	pBuffer->save("zz.bmp");
//	m_pGrabber->stopLive();

//	a = new Mat(height,width,CV_8UC3,(uchar*)pBuffer->getPtr());

//	pSink->setSnapMode( false );
//	m_pGrabber->startLive(true);
	Mat b;
//	a->copyTo(b);

//	CString ab;
//	ii ++;
//	ab.Format("%d.bmp",ii);
//	imwrite(ab.GetBuffer(ab.GetLength()),b);
//	m_pGrabber->startLive(true);
//	namedWindow("img");
//	imshow("img", *a);
//	waitKey(0);
//	Mat b(height,width,CV_8UC3,(uchar*)pBuffer->getPtr());
//	imwrite("cc.bmp",b);
	b = imread("zz.bmp");
//	imshow("img", b);
//	waitKey(0);
	fLoadConfig(TestConfig);
	c = CheckPic(&srcImage1,TestResult,TestConfig,&mPicDebug);
	if(!c)
	{
		ShowPass();
	}
	else
	{
		CString dd;
		dd.Format("%d",c);
		AfxMessageBox(dd);
		ShowError();
	}
#endif

}

void CVTestCheckDlg::ShowPass()
{
	m_Picture_PF.SetBitmap(BitmapPass);
	PlaySound(strMusicPathOK,NULL,SND_ASYNC|SND_NODEFAULT );
}
void CVTestCheckDlg::ShowError()
{
	m_Picture_PF.SetBitmap(BitmapFail);
	PlaySound(strMusicPathNG,NULL,SND_ASYNC|SND_NODEFAULT );
}
UINT Test(LPVOID pParam)
{
	long height,width;
	Mat *a;
	Mat b;
	Mat mPicDebug;
	smart_ptr<MemBuffer> pBuffer = pSink->getLastAcqMemBuffer();
	width = pBuffer->getSize().cx;
	height = pBuffer->getSize().cy;
//	a = new Mat(height,width,CV_8UC3,(uchar*)pBuffer->getPtr());
	pBuffer->save("zz.bmp");
	b = imread("zz.bmp");
//	a->copyTo(b);
#ifdef WMDebug

	b.copyTo(mPicDebug);
	
#endif
	sTestPicConfig TestConfig;
	sTestPicResult TestResult;
	int c;
	CString sMessage;
	fLoadConfig(TestConfig);
	c = CheckPic(&b,TestResult,TestConfig,&mPicDebug);
	switch (c)
	{
	case 0:
		{
			ex->ShowPass();
			break;
		}
	case 2:
		{
			break;
		}
	default:
		{
			ex->ShowError();
		}
	}

	return 0;
}




void CVTestCheckDlg::OnBnClickedCancel()
{

	
	// Stop the live video.
	m_pGrabber->stopLive();
	
	// Close the device.
	m_pGrabber->closeDev();
	CloseHandle(hTestResult);
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CVTestCheckDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
#ifdef EASYSIZEOPEN	
	UPDATE_EASYSIZE;
#endif

	UpdateWindow();
	// TODO: 在此处添加消息处理程序代码
}


void CVTestCheckDlg::OnBnClickedBSetposition()
{
	// TODO: 在此添加控件通知处理程序代码

	long height,width;
	Mat *a;
	Mat b;
	smart_ptr<MemBuffer> pBuffer = pSink->getLastAcqMemBuffer();
	width = pBuffer->getSize().cx;
	height = pBuffer->getSize().cy;
	a = new Mat(height,width,CV_8UC3,(uchar*)pBuffer->getPtr());
	a->copyTo(PShowImg);
	delete a;
//	PShowImg = imread("D:\\2.bmp",1);
	if(PShowImg.empty() || !PShowImg.data)
	{
		CString tmp;
		tmp.Format("Can not load image !\n");
		AfxMessageBox(tmp);
		return;
	}
	namedWindow("PostionImg");
	imshow("PostionImg",PShowImg);
	setMouseCallback("PostionImg",onMouse,0);
//	waitKey(0);
	return;
}

void onMouse(int Event,int x,int y, int flags,void * param)
{
	char temp[46];
	Mat pShowImgTmp;
	memset(temp,0,sizeof(temp));
	PShowImg.copyTo(pShowImgTmp);
	if(Event==CV_EVENT_LBUTTONDOWN)
	{
		SelectRect.x = x;
		SelectRect.y = y;
		PrePosBegin.x = x;
		PrePosBegin.y = y;
		SelectFlag = true;
	}
#if 1
	else if(SelectFlag &&Event == CV_EVENT_MOUSEMOVE)
	{
		PrePosEnd.x = x;
		PrePosEnd.y = y;
		sprintf(temp, "P(%d,%d)", x, y);
		putText(pShowImgTmp, temp, PrePosEnd, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));//只是实时显示鼠标移动的坐标 
		rectangle(pShowImgTmp,PrePosBegin,PrePosEnd,Scalar(0,255,0),2);
		imshow("PostionImg",pShowImgTmp);
	}
#endif
	else if(SelectFlag && Event == CV_EVENT_LBUTTONUP)
	{
		//显示框出的图
		PrePosEnd.x = x;
		PrePosEnd.y = y;
		SelectRect = Rect(PrePosBegin,PrePosEnd);
		SelectFlag = false;		
	}else if(Event==CV_EVENT_LBUTTONDBLCLK)
	{
		CvScalar s;
		IplImage tmp = PShowImg;
		s = cvGet2D(&tmp,PrePosEnd.y,PrePosEnd.x);
		sprintf(temp, "pixel(%f,%f,%f)",s.val[0],s.val[1],s.val[2]);
		putText(pShowImgTmp, temp, PrePosEnd, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
		imshow("PostionImg",pShowImgTmp);
	}
	return;
}
void AddProcInfo(CString strProcInfo)
{
	int lines;
	lines = ex->m_ListBox.AddString(strProcInfo);
	ex->m_ListBox.SetCurSel(lines);
//	ex->m_ListBox.ResetContent();
	return;
}




void CVTestCheckDlg::OnBnClickedConfig()
{
	// Stop the live video.
	m_pGrabber->stopLive();

	// Close the device.
	m_pGrabber->closeDev();
	// TODO: 在此添加控件通知处理程序代码
	if(password.DoModal() == IDOK)
	{

	}
	CRect rect1;
	m_pGrabber->setDefaultWindowPosition(true);
	m_pGrabber->stopLive();

	m_cStaticVideoWindow.GetWindowRect(&rect1);

	m_pGrabber->setDefaultWindowPosition(false);
	m_pGrabber->setWindowSize(rect1.Width(),rect1.Height());
	m_pGrabber->setHWND( GetDlgItem( IDC_STATICVIDEO )->m_hWnd );
	m_pGrabber->startLive();
}

typedef struct TestThreadInfo
{
	CWinThread *pTestThreadEntry;
	bool bTestResult;
	bool bTestEnd;
	CTime TestTime;
	Mat * mTestData;
	int iTestPosition;
	ULONG iTestItem;
	bool bInvalid;

} *pTestThreadInfo,sTestThreadInfo;

vector<sTestThreadInfo> GlobalTestInfo;
UINT TestBegin(LPVOID pParam)
{
	sTestThreadInfo TestInfo;
	long height,width;
	Mat *pData;
	ULONG iItem;
	ULONG * ipItem;
	Mat b;
	iItem = 0;
	while (true)
	{
		TestInfo.bTestResult = false;
		TestInfo.bTestEnd = false;
		TestInfo.iTestPosition = -1;
		TestInfo.mTestData = NULL;
		TestInfo.pTestThreadEntry = NULL;
		 
// 		width = 0;
// 		height = 0;
		pData = NULL;
		TestInfo.TestTime = CTime::GetCurrentTime();
		TestInfo.iTestItem = iItem;
		TestInfo.mTestData = NULL;
		smart_ptr<MemBuffer> pBuffer = pSink->getLastAcqMemBuffer();
// 		width = pBuffer->getSize().cx;
// 		height = pBuffer->getSize().cy;
//		
		pBuffer->save("zz.bmp");
		b = imread("zz.bmp");
		TestInfo.mTestData  = new Mat(b);
		if (TestInfo.mTestData == NULL)
		{
			return 1;
		}
		b.copyTo(*TestInfo.mTestData);
		TestInfo.bInvalid = false;
		ipItem = NULL;
		ipItem = new ULONG;
		if (ipItem == NULL)
		{
			return 1;
		}
		*ipItem = iItem;
		TestInfo.pTestThreadEntry = AfxBeginThread(TestEntry,(LPVOID) ipItem,0, CREATE_SUSPENDED, NULL);

		GlobalTestInfo.push_back(TestInfo);
		if (TestInfo.pTestThreadEntry)
		{
			TestInfo.pTestThreadEntry->m_bAutoDelete = FALSE;
			TestInfo.pTestThreadEntry->ResumeThread();
		}
		iItem ++;

		Sleep(500);
	}
	
	return 0;
}
typedef struct TestLog
{
	UINT iTestPosition;
} iTestLog;
UINT TestResultHandle(LPVOID pParam)
{

	UINT i,j,length;
	int iTestEntryNum,iTestEndNum,iTestSencodEntryNum;
	vector<iTestLog> TestLog;
	bool bSomeEnd;
	sTestThreadInfo TestInfoTmp;
	i = 0;
	j = 0;
	
	length = 0;
	while(TRUE)
	{

		for (i = 0; i < GlobalTestInfo.size(); i ++)
		{
			TestInfoTmp = GlobalTestInfo[i];
			iTestEndNum = 0;
			iTestEntryNum = 0;
			bSomeEnd = false;
			iTestSencodEntryNum = 0;
			for (j = 0;j < GlobalTestInfo.size(); j ++)
			{
				if (abs((GlobalTestInfo[j].iTestPosition - TestInfoTmp.iTestPosition)) < 5)
				{
					iTestEntryNum ++;
					if (GlobalTestInfo[j].bTestResult)
					{
						bSomeEnd = true;

					}else
					{
						if (GlobalTestInfo[j].bTestEnd)
						{
							iTestEndNum ++;
						}
					}

				}else
				{
					iTestSencodEntryNum ++;
				}
			}

			if (bSomeEnd || (iTestSencodEntryNum != 0 && iTestEndNum == iTestEntryNum))
			{
				if (bSomeEnd)
				{
					ex->ShowPass();
					AddProcInfo("测试OK");
				} 
				else
				{
					ex->ShowError();
					AddProcInfo("测试NG");
				}
				WaitForSingleObject(hTestResult, INFINITE);//等待互斥量
				vector<sTestThreadInfo>::iterator iter;
				iter = GlobalTestInfo.begin();
				for (j = 0;j < GlobalTestInfo.size(); j ++)
				{
					if (abs(GlobalTestInfo[j].iTestPosition - TestInfoTmp.iTestPosition) < 5 || GlobalTestInfo[j].bInvalid)
					{
						DWORD code;
						bool res = GetExitCodeThread(GlobalTestInfo[j].pTestThreadEntry, &code);
						if (!res && code==STILL_ACTIVE)//线程还活着	如果pWinTh 已经结束了，code的值为线程的退出码或返回值
						{
							TerminateThread(GlobalTestInfo[j].pTestThreadEntry,NULL);
						}
						delete GlobalTestInfo[j].mTestData;
						GlobalTestInfo.erase(iter + j);
						j --;
					}
				}
				ReleaseMutex(hTestResult);//释放互斥量
				break;
			}
			
		}


		Sleep(2000);

	}


	return 0;
}

UINT TestEntry(LPVOID pParam)
{
	ULONG iNum;
	iNum = *(ULONG*)pParam;

	delete (ULONG*)pParam;

	Mat mPicDebug;
	int c;
	int i;
	CString sMessage;
	sTestPicConfig TestConfig;
	sTestPicResult TestResult;
	CString PicName;

	//	return 0;
	c = GlobalTestInfo.size();
	for(i = 0;i < c;i ++)
	{
		Sleep(0);
		if (GlobalTestInfo[i].iTestItem == iNum)
		{
			break;
		}
	}
	if (i >= c)
	{
		return 0;
	}
	if ((GlobalTestInfo[i].mTestData)->empty())
	{
		GlobalTestInfo[i].bInvalid = true;
		return 0;
	}
#ifdef WMDebug
	(GlobalTestInfo[i].mTestData)->copyTo(mPicDebug);
 	PicName.Format("%s\\%s_%d_%d.bmp",strPicLogPath,GlobalTestInfo[i].TestTime.Format("%Y-%m-%d-%H-%M-%S"),GlobalTestInfo[i].iTestItem,GlobalTestInfo.size());
 	imwrite(PicName.GetBuffer(0),mPicDebug);
#endif
	fLoadConfig(TestConfig);
	c = CheckPic(GlobalTestInfo[iNum].mTestData,TestResult,TestConfig,&mPicDebug);
#ifdef WMDebug
	//	CString PicName;
//	if (!GlobalTestInfo[i].bInvalid)
	{
		PicName.Format("%s\\%s_%d_%d_1.bmp",strPicLogPath,GlobalTestInfo[i].TestTime.Format("%Y-%m-%d-%H-%M-%S"),GlobalTestInfo[i].iTestItem,GlobalTestInfo.size());
		imwrite(PicName.GetBuffer(0),mPicDebug);
	}

#endif
	switch (c)
	{
		 WaitForSingleObject(hTestResult, INFINITE);//等待互斥量
	case 0:
		{
			for(i = 0;i < GlobalTestInfo.size();i ++)
			{
				if (GlobalTestInfo[i].iTestItem == iNum)
				{
					GlobalTestInfo[i].bTestResult = true;
					break;
				}
			}
				
			break;
		}
	case 2:
		{
			for(i = 0;i < GlobalTestInfo.size();i ++)
			{
				if (GlobalTestInfo[i].iTestItem == iNum)
				{
					GlobalTestInfo[i].bTestEnd = true;
					GlobalTestInfo[i].bInvalid = true;
					break;
				}
			}
			break;
		}
	default:
		{
			for(i = 0;i < GlobalTestInfo.size();i ++)
			{
				if (GlobalTestInfo[i].iTestItem == iNum)
				{
					GlobalTestInfo[i].bTestEnd = true;
					break;
				}
			}
			break;
		}
	}
	
#ifdef ss
//	CString PicName;
	if (!GlobalTestInfo[i].bInvalid)
	{
		PicName.Format("%s\\%s_1.bmp",strPicLogPath,GlobalTestInfo[i].TestTime.Format("%Y-%m-%d-%H-%M-%S"));
		imwrite(PicName.GetBuffer(0),mPicDebug);
	}
	
#endif
	ReleaseMutex(hTestResult);//释放互斥量
	return 0;
}


#if 0
/**************************
mPic: 
iColor:
iRStep:行步数
iCStep:列步数
iLineWide:线宽

**************************/
bool ImageSegmentation(Mat* mPic,int iColor,int iRStep,int iCStep,int iLineWide)
{
	if (iRStep > 4 || iCStep >4 || iLineWide < 1)
	{
		return false;
	}

	if (mPic->empty())
	{
		return false;
	}

	
	int rows,cols,channels;
	int iTotal,i,j;
	int icstep,irstep;			

	ptrdiff_t mapstep = NULL;
	
	channels = mPic->channels();
	irstep = iRStep;
	icstep = iCStep * channels;
	rows = mPic->rows;
	cols = mPic->cols;

	if (channels < iColor)
	{
		return false;
	}

	Mat dst(rows,cols, CV_8U, Scalar(0));
	iTotal = (cols - 2) * channels;
	mapstep = cols * channels;

	Mat dx(rows, cols, CV_16SC(1));
	Mat dy(rows, cols, CV_16SC(1));


	for ( i = 0;i < rows;)
	{
		uchar * p = mPic->ptr<uchar>(i);
		for (j = channels ;j < iTotal; )
		{


			j += icstep;
		}
		i += irstep;
	}



	return true;
}
#endif
int fLoadConfig(sTestPicConfig& TestConfig)
{
#define MAX_LEN 200
	char Path[200];
	CString Configfile;
	CString ConfigInfo;
//	HMODULE h = NULL;
//	::GetComputerName(tempcomputer.GetBuffer(1024),&nSize);
//	tempcomputer.ReleaseBuffer();
//	computer.Format("%s",tempcomputer);
//	h=LoadLibrary("SajetConnect.dll");

	GetCurrentDirectory(sizeof(Path),Path);
	Configfile.Format("%s\\Config.ini",Path);

//	WritePrivateProfileString("Basic Info","AutoScanControl",strAutoScanControl,strIniFile);
//	GetPrivateProfileString("Basic Info","PassTimes",NULL,ConfigInfo.GetBuffer(MAX_LEN),MAX_LEN,Configfile);
	TestConfig.iTargetPixMinLength  =GetPrivateProfileInt("Basic Info","TargetPixMinLength",0,Configfile);
	TestConfig.iBackgroundMinLength =GetPrivateProfileInt("Basic Info","BlackgroundMinLength",0,Configfile);
	TestConfig.iBackgroundDeviation =GetPrivateProfileInt("Basic Info","BackgroundDeviation",0,Configfile);
	TestConfig.iTargetMinLength     =GetPrivateProfileInt("Basic Info","TargetMinLength",0,Configfile);
	TestConfig.iTargetDeviation     =GetPrivateProfileInt("Basic Info","TargetDeviation",0,Configfile);

	TestConfig.iFont1MinLength       =GetPrivateProfileInt("Basic Info","Font1MinLength",0,Configfile);
	TestConfig.iFont1BandMinWidth    =GetPrivateProfileInt("Basic Info","Font1BandMinWidth",0,Configfile);
	TestConfig.iFont1Deviation       =GetPrivateProfileInt("Basic Info","Font1Deviation",0,Configfile);
	TestConfig.iFont1SkipMaxWidth    =GetPrivateProfileInt("Basic Info","Font1SkipMaxWidth",0,Configfile);
	TestConfig.iFont1Spec            =GetPrivateProfileInt("Basic Info","Font1Spec",0,Configfile);
	TestConfig.iFont1MinWidth        =GetPrivateProfileInt("Basic Info","Font1MinWidth",0,Configfile);

	TestConfig.iFont2MinLength       =GetPrivateProfileInt("Basic Info","Font2MinLength",0,Configfile);
	TestConfig.iFont2BandMinWidth    =GetPrivateProfileInt("Basic Info","Font2BandMinWidth",0,Configfile);
	TestConfig.iFont2Deviation       =GetPrivateProfileInt("Basic Info","Font2Deviation",0,Configfile);
	TestConfig.iFont2SkipMaxWidth    =GetPrivateProfileInt("Basic Info","Font2SkipMaxWidth",0,Configfile);
	TestConfig.iFont2Spec            =GetPrivateProfileInt("Basic Info","Font2Spec",0,Configfile);
	TestConfig.iFont2MinWidth        =GetPrivateProfileInt("Basic Info","Font2MinWidth",0,Configfile);

	TestConfig.iSpotMinLength       =GetPrivateProfileInt("Basic Info","SpotMinLength",0,Configfile);
	TestConfig.iSpotMinWidth        =GetPrivateProfileInt("Basic Info","SpotMinWidth",0,Configfile);
	TestConfig.iSpotDeviation       =GetPrivateProfileInt("Basic Info","SpotDeviation",0,Configfile);

	TestConfig.iBandFirstMinWidth   =GetPrivateProfileInt("Basic Info","BandFirstMinWidth",0,Configfile);
	TestConfig.iBandSecondMinWidth  =GetPrivateProfileInt("Basic Info","BandSecondMinWidth",0,Configfile);
	TestConfig.iBandThirdMinWidth  =GetPrivateProfileInt("Basic Info","BandThirdMinWidth",0,Configfile);


	TestConfig.iSpotSpec            =GetPrivateProfileInt("Basic Info","SpotSpec",0,Configfile);
	TestConfig.iSpotSpec1            =GetPrivateProfileInt("Basic Info","SpotSpec1",0,Configfile);
	TestConfig.iTargetSpec1         =GetPrivateProfileInt("Basic Info","TargetSpec1",0,Configfile);
	TestConfig.iTargetSpec2         =GetPrivateProfileInt("Basic Info","TargetSpec2",0,Configfile);
	
	TestConfig.iBackgroundSpec      =GetPrivateProfileInt("Basic Info","BackgroundSpec",0,Configfile);
	TestConfig.iTargetTopBottomMinPlace=GetPrivateProfileInt("Basic Info","TargetTopBottomMinPlace",0,Configfile);
	TestConfig.iTargetLeftRightMinPlace=GetPrivateProfileInt("Basic Info","TargetLeftRightMinPlace",0,Configfile);
	return 0;
}

int FindConnectDomain(Mat* mPic,int& rows,int& cols,int& channels,int spec,vector<sSpotInfo>& vSpot)
{
	int irows = rows;
	int icols = cols;
	int ichannels = channels;
	int ispec = spec;
	int i,j,k;
	uchar * ptr;
	vector<sSpotInfo> vSpot1;
	vector<sSpotInfo> vSpot2;
	sSpotInfo sSpot1,sSpot2;
	bool a,b,c;
	int irowsend,icolsend;

	irowsend = mPic->rows - 1;
	icolsend = mPic->cols - 1;

	if(mPic == NULL || mPic->empty())
	{
		return 1;
	}
	sSpot1.rows = irows;
	sSpot1.cols = icols;
	vSpot1.push_back(sSpot1);
	vSpot.clear();
	c = false;
	while(vSpot1.size() > 0)
	{
		sSpot1 = vSpot1.back();
// 		for(i = 0;i < vSpot.size();i ++)
// 		{
// 			if(vSpot[i].rows == sSpot1.rows&&vSpot[i].cols == sSpot1.cols)
// 			{
// 				if (vSpot1.empty())
// 				{
// 					return 0;
// 				}
// 				vSpot1.pop_back();
// 				continue;
// 			}
// 		}
		if (vSpot1.empty())
		{
			return 0;
		}
		if (sSpot1.rows > irowsend || sSpot1.rows < 1 || sSpot1.cols > icolsend || sSpot1.cols < 1)
		{
			vSpot1.pop_back();
			continue;
		}
		ptr = mPic->ptr<uchar>(sSpot1.rows);
		
		vSpot1.pop_back();
		if(ptr[sSpot1.cols*ichannels + 2] <= ispec||!c)
		{
			c = true;
//			vSpot2.push_back(sSpot1);
			//右上
			a = true;
			b = true;
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows - 1&&vSpot2[i].cols == sSpot1.cols + 1)
				{
					a = false;
					break;
				}
			}
			for(i = 0;i < vSpot1.size();i ++)
			{
				if(!a || (vSpot1[i].rows == sSpot1.rows - 1&&vSpot1[i].cols == sSpot1.cols + 1))
				{
					b = false;
					break;
				}
			}
			if(a&&b)
			{
				sSpot2.rows = sSpot1.rows - 1;
				sSpot2.cols = sSpot1.cols + 1;
				vSpot1.push_back(sSpot2);
			}
			//上
			a = true;
			b = true;
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows - 1&&vSpot2[i].cols == sSpot1.cols)
				{
					a = false;
					break;
				}
			}
			for(i = 0;i < vSpot1.size();i ++)
			{
				if(!a || (vSpot1[i].rows == sSpot1.rows - 1&&vSpot1[i].cols == sSpot1.cols))
				{
					b = false;
					break;
				}
			}
			if(a&&b)
			{
				sSpot2.rows = sSpot1.rows - 1;
				sSpot2.cols = sSpot1.cols;
				vSpot1.push_back(sSpot2);
			}
			//左上
			a = true;
			b = true;
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows - 1&&vSpot2[i].cols == sSpot1.cols - 1)
				{
					a = false;
					break;
				}
			}
			for(i = 0;i < vSpot1.size();i ++)
			{
				if(!a || (vSpot1[i].rows == sSpot1.rows - 1&&vSpot1[i].cols == sSpot1.cols - 1))
				{
					b = false;
					break;
				}
			}
			if(a&&b)
			{
				sSpot2.rows = sSpot1.rows - 1;
				sSpot2.cols = sSpot1.cols - 1;
				vSpot1.push_back(sSpot2);
			}
			//左
			a = true;
			b = true;
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows &&vSpot2[i].cols == sSpot1.cols - 1)
				{
					a = false;
					break;
				}
			}
			for(i = 0;i < vSpot1.size();i ++)
			{
				if(!a || (vSpot1[i].rows == sSpot1.rows &&vSpot1[i].cols == sSpot1.cols - 1))
				{
					b = false;
					break;
				}
			}
			if(a&&b)
			{
				sSpot2.rows = sSpot1.rows;
				sSpot2.cols = sSpot1.cols - 1;
				vSpot1.push_back(sSpot2);
			}
			//左下
			a = true;
			b = true;
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows + 1&&vSpot2[i].cols == sSpot1.cols - 1)
				{
					a = false;
					break;
				}
			}
			for(i = 0;i < vSpot1.size();i ++)
			{
				if(!a || (vSpot1[i].rows == sSpot1.rows + 1&&vSpot1[i].cols == sSpot1.cols - 1))
				{
					b = false;
					break;
				}
			}
			if(a&&b)
			{
				sSpot2.rows = sSpot1.rows + 1;
				sSpot2.cols = sSpot1.cols - 1;
				vSpot1.push_back(sSpot2);
			}
			//下
			a = true;
			b = true;
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows + 1&&vSpot2[i].cols == sSpot1.cols)
				{
					a = false;
					break;
				}
			}
			for(i = 0;i < vSpot1.size();i ++)
			{
				if(!a || (vSpot1[i].rows == sSpot1.rows + 1&&vSpot1[i].cols == sSpot1.cols))
				{
					b = false;
					break;
				}
			}
			if(a&&b)
			{
				sSpot2.rows = sSpot1.rows + 1;
				sSpot2.cols = sSpot1.cols;
				vSpot1.push_back(sSpot2);
			}
			//右下
			a = true;
			b = true;
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows + 1&&vSpot2[i].cols == sSpot1.cols + 1)
				{
					a = false;
					break;
				}
			}
			for(i = 0;i < vSpot1.size();i ++)
			{
				if(!a || (vSpot1[i].rows == sSpot1.rows + 1&&vSpot1[i].cols == sSpot1.cols + 1))
				{
					b = false;
					break;
				}
			}
			if(a&&b)
			{
				sSpot2.rows = sSpot1.rows + 1;
				sSpot2.cols = sSpot1.cols + 1;
				vSpot1.push_back(sSpot2);
			}
		}else
		{
			vSpot.push_back(sSpot1);
		}
		vSpot2.push_back(sSpot1);
	}

	return 0;
}
int FindRowsBottomSpot(vector<sSpotInfo>& vSpotInfo,sSpotInfo & sTargetOne)
{
	if(vSpotInfo.size() < 1)
		return 1;

	int i;

	sTargetOne = vSpotInfo[0];

	for(i = 1;i < vSpotInfo.size();i ++)
	{
		if(vSpotInfo[i].rows > sTargetOne.rows)
		{
			sTargetOne.cols = vSpotInfo[i].cols;
			sTargetOne.rows = vSpotInfo[i].rows;
		}
	}

	return 0;
}
int FindRowsTopSpot(vector<sSpotInfo>& vSpotInfo,sSpotInfo & sTargetOne)
{
	if(vSpotInfo.size() < 1)
		return 1;

	int i;

	sTargetOne = vSpotInfo[0];

	for(i = 1;i < vSpotInfo.size();i ++)
	{
		if(vSpotInfo[i].rows < sTargetOne.rows)
		{
			sTargetOne.cols = vSpotInfo[i].cols;
			sTargetOne.rows = vSpotInfo[i].rows;
		}
	}

	return 0;
}
int FindColsRightSpot(vector<sSpotInfo>& vSpotInfo,sSpotInfo & sRightOne)
{
	if(vSpotInfo.size() < 1)
		return 1;

	int i;

	sRightOne = vSpotInfo.front();

	for(i = 1;i < vSpotInfo.size();i ++)
	{
		if(vSpotInfo[i].cols > sRightOne.cols)
		{
			sRightOne.cols = vSpotInfo[i].cols;
			sRightOne.rows = vSpotInfo[i].rows;
		}
	}
	return 0;
}
int FindColsLeftSpot(vector<sSpotInfo>& vSpotInfo,sSpotInfo & sLeftOne)
{
	if(vSpotInfo.size() < 1)
		return 1;

	int i;

	sLeftOne = vSpotInfo.front();

	for(i = 1;i < vSpotInfo.size();i ++)
	{
		if(vSpotInfo[i].cols < sLeftOne.cols)
		{
			sLeftOne.cols = vSpotInfo[i].cols;
			sLeftOne.rows = vSpotInfo[i].rows;
		}
	}

	return 0;
}
int FindMidLeftSpot(vector<sSpotInfo>& vSpotInfo,sSpotInfo& MidLeftOne)
{
	if(vSpotInfo.size() < 1)
		return 1;

	int i,j;

	FindRowsBottomSpot(vSpotInfo,MidLeftOne);
	i = MidLeftOne.rows;
	FindRowsTopSpot(vSpotInfo,MidLeftOne);
	j = MidLeftOne.rows;

	i = (i + j)/2;
	FindColsLeftSpot(vSpotInfo,MidLeftOne);
	MidLeftOne.rows  = i;
//	MidLeftOne.cols = 0;
// 	for(i = 0;i < vSpotInfo.size();i ++)
// 	{
// 		if(vSpotInfo[i].rows == MidLeftOne.rows)
// 		{
// 			MidLeftOne.cols = MidLeftOne.cols > vSpotInfo[i].cols?MidLeftOne.cols:vSpotInfo[i].cols;
// 		}
// 	}
	return 0;
}

int FindSpotLocation(Mat * mPic,
				int iSpotSpec,
				int iCols,
				int iSpotMinSpecLength,
				int iRows,
				int& iDirect,
				vector<sSpotInfo>& vSpot1,
				vector<sSpotInfo>& vSpot2)
{
	int irows;
	int icols;
	int ichannels;
	uchar * ptr;
	int i,j,iEnd;
	int iDTarget,iDBack,iCEnd,iTargetNum,iBack;
	int state;
	vector<int> vRowInfo;
	if(mPic == NULL || mPic->empty())
	{
		return 1;
	}

	ichannels = mPic->channels();

	state =  FindConnectDomain(mPic,iRows,iCols,ichannels,iSpotSpec,vSpot1);
	if(state != 0||vSpot1.size() < 10)
	{
		return 2;
	}
	sSpotInfo sTmp1,sTmp2;
	int m,n,x,y;

	//FindRowsBottomSpot(vSpot1,sTmp1); //目标开口向右倾斜 定位到的时第一个点
	FindColsRightSpot(vSpot1,sTmp2);//最右边的那个点
	
	x = sTmp2.rows - 30;
	y = (sTmp2.cols + 2 + 30) * ichannels;
	for(m = sTmp2.rows;m > x;m --)
	{
		ptr = mPic->ptr<uchar>(m);
		iTargetNum = 0;
		for(n = (sTmp2.cols + 2) * ichannels;n < y;)
		{
			if(ptr[n] < iSpotSpec)
			{
				iTargetNum ++;
				if(iTargetNum > 3)
				{
					iDirect = 0;
					int tmp;
					tmp = n/ichannels;
					state =  FindConnectDomain(mPic,m,tmp,ichannels,iSpotSpec,vSpot2);
					if(state != 0||vSpot2.size() < 10)
					{
						return 4;
					}else
					{
						return 0;
					}
				}
			}else
			{
				iTargetNum = 0;
			}
			n = n + ichannels;
		}
	}
	//FindRowsTopSpot(vSpot1,sTmp1); //目标开口向左倾斜 定位到的时第二个点
	FindColsLeftSpot(vSpot1,sTmp1);  //取最左面的点
	x = sTmp1.rows - 30;
	y = (sTmp1.cols - 2 - 30 ) * ichannels;
	for(m = sTmp1.rows;m > x;m --)
	{
		ptr = mPic->ptr<uchar>(m);
		iTargetNum = 0;
		for(n = (sTmp1.cols + 2) * ichannels;n < y;)
		{
			if(ptr[n] < iSpotSpec)
			{
				iTargetNum ++;
				if(iTargetNum > 3)
				{
					iDirect = 1;
					int tmp;
					vector<sSpotInfo> stmp;
					stmp = vSpot1;
					vSpot2 = stmp;
					tmp = n/ichannels;
					state =  FindConnectDomain(mPic,m,tmp,ichannels,iSpotSpec,vSpot1);
					if(state != 0||vSpot1.size() < 10)
					{
						return 4;
					}else
					{
						return 0;
					}
				}
			}else
			{
				iTargetNum = 0;
			}
			n = n - ichannels;
		}
	}
	return 3;

}

int FixAngleAndRowStart(vector<sSpotInfo>& vSpot1,
				vector<sSpotInfo>& vSpot2,
				int& iDirect,
				float& fAngleValue,
				int& rows,
				int& iEdgeRightPosition)
{
	sSpotInfo sTmp1,sTmp2;

	FindRowsTopSpot(vSpot1,sTmp1);
	FindRowsTopSpot(vSpot2,sTmp2);
	if(iDirect == 1) //向左
	{
		fAngleValue = 1.0*(sTmp2.rows - sTmp1.rows)/(sTmp2.cols - sTmp1.cols);

		rows = sTmp1.rows ;
		FindColsLeftSpot(vSpot2,sTmp2);
		iEdgeRightPosition = sTmp2.cols;
		return 0;
	}
	if(iDirect == 0)//向右
	{
		fAngleValue = 1.0*(sTmp1.rows - sTmp2.rows)/(sTmp2.cols = sTmp1.cols);
		
		rows = sTmp2.rows ;

		FindColsLeftSpot(vSpot2,sTmp2);
		iEdgeRightPosition = sTmp2.cols;
		return 0;
	}
	return 0;
}
int CountFont(Mat* mPic,
				int& rows,
				int& cols,
				int& channels,
				int& iFontSpec,
				int& iFontMinLength,
				int& iFontNum,
				int& iEdgeRightPosition,
				float& angle,
				int iDirect,
				sTestPicResult& Result)
{
	vector<sSpotInfo> vSpot;
	sSpotInfo sTmp;
	bool bCol;
	int i,j,k,x,y,m,n,z;
	int iTargetNum;
	int state;
	uchar * ptr;
	bool bA1,bA2;


	bCol = true;
	iTargetNum = 0;
	iFontNum = 0;
	state = 0;

	do
	{
		state = FindConnectDomain(mPic,rows,cols,channels,iFontSpec,vSpot);
		if(state||vSpot.size() < 10)
		{
			return 4;
		}
		iFontNum ++;
		FindMidLeftSpot(vSpot,sTmp);  //取左面中间的点

		x = sTmp.rows ;

		y = sTmp.cols + 3;
		k = y;
		i = iEdgeRightPosition * channels;
		iTargetNum = 0;
		bA1 = false;
		bA2 = false;

		for(n = k * channels;n < i;y ++,n = n + channels)
		{
			
				if(!iDirect)//= 0,目标开口向右倾斜
					m = floor(x - (y - k) * angle);
				else//=1,目标开口向左倾斜
					m = floor(x + (y - k) * angle);
				ptr = mPic->ptr<uchar>(m);
				if(!bA1 && ptr[n] > iFontSpec)
				{
					iTargetNum ++;
					if (iTargetNum > 5)
					{
						iTargetNum = 0;
						bA1 = true;
					}
					
					continue;
				}
				if(ptr[n] < iFontSpec)
				{
				
					iTargetNum ++;
					if(iTargetNum > 2)
					{
						bCol = true;
						rows = m;
						cols = n/channels;
						goto whileFindTarget;//找到目标
					}
				}else
				{
					iTargetNum = 0;
				}

		}
whileFindTarget:
		if(n >= i)
			return 0;
	}while(bCol);

	return 0;
	
}

/*
传入开始点，向左向右遍历Spec个，如果在Spec范围内发现黑点，且满足白点宽度大于Spec 返回OK
否则NG
*/
int SkipTargetBand(Mat* mPic,
			sTestPicInfo& TestInfo,
			uchar** ptr[],
			int iTargetSpec,
			int& iTargetTopBottomRange,
			int& iTargetLeftRightRange,
			float angle,
			int iBandWidthSpec,
			int iTargetLengthSpec)
{
	int irows,ichannels,icols,irowsbegin,icolsbegin;
	int i,j;
	int iDTarget,iCNum,iTargetNum,iTargetLength;
	int state;


	if(mPic == NULL || mPic->empty() || ptr == NULL)
	{
		return 1;
	}

	ichannels = TestInfo.channels;

	iTargetNum = 0;
	
	//首先我根据给出的row col 和角度计算出我需要遍历的row的所有起始包括col
	//后面有了起始点和角度再加上终止点我就可以遍历所有的了
	//如果向右偏如果计算出Row小于0应该从等于2(实际等于0)开始


	for(i = 0;i < iTargetTopBottomRange;i ++)
	{
		//需遍历的行列 以行为角度 确定列
		irows = TestInfo.NextStartSpot.rows - i;     
		icols = TestInfo.NextStartSpot.cols - i * angle;//向左向右一样计算公式
	

		iTargetNum = 0;
		//需遍历的以列为角度,遍历列,行会发生变化
		//列变化有两种可能向左向右  大于0 小于0
		for (j = 0,icolsbegin = icols * ichannels;j < iTargetLeftRightRange;j ++)  //向右
		{
			irowsbegin = irows - j * angle;
			if (((*ptr)[irowsbegin])[icolsbegin + 2] < iTargetSpec)
			{
				iTargetNum ++;
				if (iTargetNum >= iTargetLengthSpec) //找到黑点
				{
					if (abs(irows - TestInfo.NextStartSpot.rows) > iBandWidthSpec)
					{
						TestInfo.NextStartSpot.rows = irowsbegin;
						TestInfo.NextStartSpot.cols = icols  + j;
						return 0;
					}
				}
			}
			icolsbegin += ichannels;
		}
		

		iTargetNum = 0;
		for (j = 0,icolsbegin = icols * ichannels;j > -iTargetLeftRightRange;j --)
		{
			irowsbegin = irows - j * angle;
			if (((*ptr)[irowsbegin])[icolsbegin + 2] < iTargetSpec)
			{
				iTargetNum ++;
				if (iTargetNum >= iTargetLengthSpec) //找到黑点
				{
					if (abs(irows - TestInfo.NextStartSpot.rows) > iBandWidthSpec)
					{
						TestInfo.NextStartSpot.rows = irowsbegin;
						TestInfo.NextStartSpot.cols = icols  + j;
						return 0;
					}
				}
			}
			icolsbegin -= ichannels;
		}

	}
	TestInfo.NextStartSpot.rows = irowsbegin;
	TestInfo.NextStartSpot.cols = icols  + j;
	return 2;
}


int SkipBandFixFont(Mat* mPic,
				sTestPicInfo& TestInfo,
				uchar** ptr[],
				int iTargetSpec,
				int& iTargetTopBottomRange,
				int& iTargetLeftRightRange,
				float angle,
				int iBandWidthSpec,
				int iTargetSpecLength,
				vector<sSpotInfo>& vFontLeftEdge,
				vector<sSpotInfo>& vFontRightEdge)
{
	int irows,ichannels,icols,irowsbegin,icolsbegin,icolsend;
	int i,j,m,n;
	int iDTarget,iTargetNum;
	bool bFind1,bFind2;
	sSpotInfo tmp;

	if(mPic == NULL || mPic->empty())
	{
		return 1;
	}

	ichannels = TestInfo.channels;
	icolsend = TestInfo.cols * ichannels;

	/*
	传进来的为黑点，那么我向左向右找目标白点.当白点数目达到目标时认为ok
	当向左向右Spec 范围内为全白时认为找到白条return
	记录左下点，右上点，白条点。
	*/
	iTargetNum = 0;
	for(i = 0;i < iTargetTopBottomRange;i ++)
	{
		//需遍历的行列 以列为角度
		irows = TestInfo.NextStartSpot.rows - i;     
		icols = TestInfo.NextStartSpot.cols - i * angle;//向左向右一样计算公式

		iTargetNum = 0;
		//需遍历的以行为角度
		//列有两种可能大于0 小于0
		bFind1 = false;
		for (j = 0,icolsbegin = icols * ichannels;icolsbegin < icolsend ;j ++)
		{
			irowsbegin = irows - j * angle;
			if (((*ptr)[irowsbegin])[icolsbegin + 1] > iTargetSpec)
			{
				iTargetNum ++;
				if (iTargetNum >= iTargetLeftRightRange) //找到白点
				{
					if (abs(j - iTargetLeftRightRange) < 5) //纯白点
					{
						TestInfo.NextStartSpot.rows = irowsbegin;
						TestInfo.NextStartSpot.cols = icols  + j;
						bFind1 = true;
						break;
					}else //有黑点部分
					{
						tmp.rows = irowsbegin;
						tmp.cols = icols  + j - iTargetLeftRightRange + 2;
						vFontRightEdge.push_back(tmp);//记录
						break;
					}
				}
			}else
			{
				iTargetNum = 0;
			}
			icolsbegin += ichannels;
		}

		
		iTargetNum = 0;
		for (j = 0,icolsbegin = icols * ichannels;icolsbegin > 12 ;j --)
		{
			irowsbegin = irows - j * angle;
			if (((*ptr)[irowsbegin])[icolsbegin + 1] > iTargetSpec)
			{
				iTargetNum ++;
				if (iTargetNum >= iTargetLeftRightRange) //找到白点
				{
					if ((abs(j) - iTargetLeftRightRange) < 5) //纯白点
					{
						TestInfo.NextStartSpot.rows = irowsbegin;
						TestInfo.NextStartSpot.cols = icols  + j;
						bFind2 = true;
						if (bFind1)
						{
							return 0;
						}
						break;
					}else //有黑点部分
					{
						tmp.rows = irowsbegin;
						tmp.cols = icols  + j + iTargetLeftRightRange - 2;
						vFontLeftEdge.push_back(tmp);//所有点都记录（最后根据angle取最大最小）
						break;
					}
				}
			}else
			{
				iTargetNum = 0;
			}
			icolsbegin -= ichannels;
		}

	}
	return 2;
}

int FindOneSpotLocation(Mat * mPic,
					sTestPicInfo& TestInfo,
					uchar** ptr[],
					int& iTargetSpec,
					int& iSpotSpec,
					int& iTargetTopBottomMinPlace,
					int& iTargetDeviation,
					int& iSpotMinLength,
					int& iSpotDeviation,
					int& iEdgeLeftPosition,
					int& iEdgeRightPosition,
					vector<sSpotInfo>& vSpot1,
					vector<sSpotInfo>& vSpot2,
					float& angle,
					int& iDirect)
{
	if (mPic == NULL || mPic->empty() || ptr == NULL)
	{
		return 1;
	}
	int i,j,k,state;
	int iColsEnd;
	int iTargetPixNum = 0;
	bool bF1,bF2,bF3;
	ULONG uTargetMeasureLength = 0;
	ULONG uTargetMeasureNum = 0;
	ULONG uTargetMeasureDNum = 0;
	int irows,icols;

	iTargetPixNum = 0;
	iColsEnd = (TestInfo.cols - 2)* TestInfo.channels;

	for(i = TestInfo.NextStartSpot.rows;i > iTargetTopBottomMinPlace;i --)
	{
		state = 0;
		iTargetPixNum = 0;
		bF1 = false;
		bF2 = false;
		bF3 = false;
		for (j = 0;j < iColsEnd; )
		{
			switch (state)
			{
				case 0:
				{
					if(((*ptr)[i])[j + 2] > iTargetSpec)//白色出现
					{
						iTargetPixNum ++;
						if(iTargetPixNum > 10)
						{
							iTargetPixNum = 0;
							bF1 = true;
							state = 1;
						}
					}else
					{
						iTargetPixNum = 0;
					}
					break;
				}
				case 1:
				{
					if(((*ptr)[i])[j + 2] < iSpotSpec)//点出现
					{
						iTargetPixNum ++;
						if(iTargetPixNum > iSpotDeviation)
						{
							iTargetPixNum = 0;
							bF2 = true;
							icols = j/TestInfo.channels;
							irows = i;
							state = 2;
						}
					}else
					{
						iTargetPixNum = 0;
					}
					break;
				}
				case 2:
				{
					if(((*ptr)[i])[j + 2] > iTargetSpec)//白色出现
					{
						iTargetPixNum ++;
						if(iTargetPixNum > 4)
						{
							
							bF3 = true;
							state = 3;
							goto ForSwitchEnd;
						}
					}else
					{
						iTargetPixNum = 0;
					}
					break;
				}
				default:
				{
					break;
				}
			}
ForSwitchEnd:	
			if(bF1 && bF2 && bF3)
			{
//				state = FindSpotLocation( mPic,iSpotSpec,irows,iSpotMinLength,icols,iDirect,vSpot1,vSpot2);
//				if(state == 0)
//				{
					TestInfo.NextStartSpot.rows = irows;
					TestInfo.NextStartSpot.cols = icols;
//					FixAngleAndRowStart(vSpot1,vSpot2,iDirect,angle,rows,iEdgeRightPosition);
					return 0;
//				}else
//				{
//					break;
//					state = 0;
//				}

			}
			j = j + TestInfo.channels;
		}
	}
	return 2;
}

int FindAllSpotLocation(Mat * mPic,
	sTestPicInfo& TestInfo,
	uchar** ptr[],
	int& iTargetSpec,
	int& iSpotSpec,
	int& iTargetTopBottomMinPlace,
	int& iTargetDeviation,
	int& iSpotMinLength,
	int& iSpotDeviation,
	int& iEdgeLeftPosition,
	int& iEdgeRightPosition,
	vector<sSpotInfo>& vSpot1,
	vector<sSpotInfo>& vSpot2,
	float& angle)
{
	if (mPic == NULL || mPic->empty())
	{
		return 1;
	}
	int i,j,k,state;
	int iColsEnd;
	int iTargetPixNum = 0;
	bool bF1,bF2,bF3;
	ULONG uTargetMeasureLength = 0;
	ULONG uTargetMeasureNum = 0;
	ULONG uTargetMeasureDNum = 0;
	int irows,icols;

	iTargetPixNum = 0;
	iColsEnd = TestInfo.cols * TestInfo.channels;

	for(i = TestInfo.NextStartSpot.rows;i > iTargetTopBottomMinPlace;i --)
	{
		state = 0;
		iTargetPixNum = 0;
		bF1 = false;
		bF2 = false;
		bF3 = false;
		for (j = 0;j < iColsEnd; )
		{
			switch (state)
			{
			case 0:
				{
					if((*ptr[i])[j+2] > iTargetSpec)//白色出现
					{
						iTargetPixNum ++;
						if(iTargetPixNum > 10)
						{
							iTargetPixNum = 0;
							bF1 = true;
							state = 1;
						}
					}else
					{
						iTargetPixNum = 0;
					}
					break;
				}
			case 1:
				{
					if((*ptr[i])[j+2] < iSpotSpec)//点出现
					{
						iTargetPixNum ++;
						if(iTargetPixNum > iSpotDeviation)
						{
							iTargetPixNum = 0;
							bF2 = true;
							k = j/TestInfo.channels;
							state = 2;
						}
					}else
					{
						iTargetPixNum = 0;
					}
					break;
				}
			case 2:
				{
					if((*ptr[i])[j+2] > iTargetSpec)//白色出现
					{
						iTargetPixNum ++;
						if(iTargetPixNum > 4)
						{

							bF3 = true;
							irows = i;
							icols = j/TestInfo.channels;
							state = 3;
							goto ForSwitchEnd;
						}
					}else
					{
						iTargetPixNum = 0;
					}
					break;
				}
			default:
				{
					break;
				}
			}
ForSwitchEnd:	
			if(bF1 && bF2 && bF3)
			{
				//				state = FindSpotLocation( mPic,iSpotSpec,irows,iSpotMinLength,icols,iDirect,vSpot1,vSpot2);
				//				if(state == 0)
				//				{
				TestInfo.NextStartSpot.rows = irows;
				TestInfo.NextStartSpot.cols = k;
				//					FixAngleAndRowStart(vSpot1,vSpot2,iDirect,angle,rows,iEdgeRightPosition);
				return 0;
				//				}else
				//				{
				//					break;
				//					state = 0;
				//				}

			}
			j =j + TestInfo.channels;
		}
	}
	return 2;
}


#if 1
int FindEdgeLocation(Mat * mPic,
					sTestPicInfo& TestInfo,
					uchar** ptr[],
					int& iTargetSpec,
					int& iTargetTopBottomMinPlace,
					int& iTargetDeviation,
					int& iBackgroundDeviation,
					int& iTargetMinLength,
					int& iBandFirstMinWidth,
					int& iEdgeLeftPosition,
					int& iEdgeRightPosition,
					int& iEdgePosition)
{
	if (mPic == NULL || mPic->empty() || ptr == NULL)
	{
		return 1;
	}
	int i,j,k;
	int iCheckNum;
	int iColsEnd;
	vector<int> vRowInfo;
	vector<int> vRowChange;
	int iTargetPixNum = 0;
	int iBackgroundPixNum = 0;
	bool bTargetFind;
	ULONG uTargetMeasureLength = 0;
	ULONG uTargetMeasureNum = 0;
	ULONG uTargetMeasureDNum = 0;

	bTargetFind = false;
	iColsEnd = TestInfo.cols * TestInfo.channels;

	for(i = TestInfo.rows - 1;i > iTargetTopBottomMinPlace; i--)
	{
		vRowChange.clear();
		vRowInfo.clear();
		iTargetPixNum = 0;
		iBackgroundPixNum = 0;

		for (j = 0;j < iColsEnd; )
		{
			if ( ((*ptr)[i])[j + 2] > iTargetSpec )  //可能目标出现
			{
				iTargetPixNum ++;
				iBackgroundPixNum = 0;
				if(iTargetPixNum > iTargetDeviation)  //连续出现多少的目标点后认为OK
				{
					if (vRowInfo.size() == 0)
					{
						vRowInfo.push_back(1);
						vRowChange.push_back(j/TestInfo.channels);
					}else if (vRowInfo.back() != 1)
					{
						vRowInfo.push_back(1);
						vRowChange.push_back(j/TestInfo.channels - iTargetPixNum - 4); //前一个背景坐标
						vRowChange.push_back(j/TestInfo.channels - iTargetPixNum - 1); //目标点坐标
					}
				}
			}else
			{
				iBackgroundPixNum ++;
				iTargetPixNum = 0;
				if(iBackgroundPixNum > iBackgroundDeviation)
				{
					if (vRowInfo.size() == 0)
					{
						vRowInfo.push_back(0);
						vRowChange.push_back(j/TestInfo.channels);
					}else if (vRowInfo.back() != 0)
					{
						vRowInfo.push_back(0);
						vRowChange.push_back(j/TestInfo.channels - iBackgroundPixNum - 3);
						vRowChange.push_back(j/TestInfo.channels - iBackgroundPixNum - 1);
					}
				}
			}
			j =j + TestInfo.channels;
		}
		vRowChange.push_back(j/TestInfo.channels);
		if (vRowInfo.size() < 2)
		{
			goto ColsEnd;
		}
				
		bTargetFind = false;
		uTargetMeasureLength = 0;
		for (j = 0;j < vRowInfo.size();j ++)
		{
			uTargetMeasureLength = vRowChange[j*2 + 1] - vRowChange[j*2];
			if(vRowInfo[j] == 1 && uTargetMeasureLength > iTargetMinLength)
			{
				bTargetFind = true;
				uTargetMeasureNum ++;
				iEdgeLeftPosition = vRowChange[j*2];
				iEdgeRightPosition = vRowChange[j*2 + 1];
				if (uTargetMeasureNum > iBandFirstMinWidth)  //这个要完善
				{
					//返回的是第一次侦测到的点
					iEdgePosition = i + uTargetMeasureNum - 1;
					uTargetMeasureNum = 0;
					uTargetMeasureDNum = 0;
					TestInfo.NextStartSpot.rows = i;
					TestInfo.NextStartSpot.cols  = vRowChange[j*2] - iBandFirstMinWidth;
					return 0;
				}
				
			}
		}
		if (!bTargetFind) //当出现有一行不是目标点则重新开始
		{
			uTargetMeasureNum = 0;
		}
ColsEnd:
	;
	}

	return 1;
}
#endif
/*
	0 OK
	1 图片错误
	2 没有找到图片底部
	3 超过一定范围未找到目标
	4 没有完整的目标
*/

int CheckPic(Mat * mPic,sTestPicResult& TestResult,sTestPicConfig& TestConfig,Mat* mPicDebug)
{

	if (mPic == NULL || mPic->empty())
	{
		return 1;
	}
	
	
//	CV_Assert( mPic->depth() == CV_8U );
	int rows,cols,channels;
	int rowsold,colsold;
	int state;
	vector<int> vRowInfo;
	vector<int> vRowChange;
	bool bTargetFind = false;
	float angle;
	int iNum,iDirect;
	int iTargetTopBottomMinPlace,iTargetLeftRightMinPlace;
	int iFontNum;
	deque<sSpotInfo> vEdgeInfo;
	vector<RowTestInfo> vPicTestInfo;
	vector<sSpotInfo> vFontLeftEdge;
	vector<sSpotInfo> vFontRightEdge;

	sSpotInfo sSpot,eSpot;
	sSpotInfo EdgeStartSpotLog,EndSpot;
	sSpotInfo Font1StartSpotLog;
	sSpotInfo Font2StartSpotLog;
	sSpotInfo SpotStartSpotLog;
	sSpotInfo FirstBandStartSpotLog;
	sSpotInfo SecendStartSpotLog;
	sSpotInfo ThirdStartSpotLog;
	sSpotInfo NextStartSpot,LastStartSpot;
	sTestPicInfo TestInfo;

	int length,width;
	uchar* ptr;
	uchar** pPicRows;

	state = 0;
	pPicRows = NULL;
	TestInfo.rows = mPic->rows;
	TestInfo.cols = mPic->cols;
	TestInfo.channels = mPic->channels();
	channels = TestInfo.channels;

	state = CheckNewPic(mPic,TestInfo,&pPicRows,5,200,90);
	if(state)
	{
		delete (pPicRows);
		return 2;
	}

	iTargetTopBottomMinPlace = TestInfo.rows - TestConfig.iTargetTopBottomMinPlace;
	 //初始状态寻找边界白条
	state = FindEdgeLocation(mPic,
							TestInfo,
							&pPicRows,
							TestConfig.iTargetSpec1,
							iTargetTopBottomMinPlace,
							TestConfig.iTargetDeviation,
							TestConfig.iBackgroundDeviation,
							TestConfig.iTargetMinLength,
							TestConfig.iBandFirstMinWidth,
							TestResult.iEdgeLeftPosition,
							TestResult.iEdgeRightPosition,
							TestResult.iEdgePosition);
	if(state)
	{
		delete (pPicRows);
		return 3;
	}
// 	if (TestResult.iEdgePosition == TestResult)
// 	{
// 		return 3;
// 	}
	EdgeStartSpotLog.rows = TestInfo.NextStartSpot.rows;
	EdgeStartSpotLog.cols = TestInfo.NextStartSpot.cols;
#ifdef WMDebug
	ptr = (*mPicDebug).ptr<uchar>(EdgeStartSpotLog.rows);
	ptr[EdgeStartSpotLog.cols * channels] = 0;
	ptr[EdgeStartSpotLog.cols * channels + 1] = 255;
	ptr[EdgeStartSpotLog.cols * channels + 2] = 0;
#endif
	

	state = FindEndwiseEdge(mPic,
						TestInfo,
						&pPicRows,
						5,
						TestConfig.iTargetSpec1 - 6,
						vEdgeInfo);

	if(state)
	{
		delete (pPicRows);
		return 4;
	}
#ifdef WMDebug
	for(deque<sSpotInfo>::const_iterator i = vEdgeInfo.begin();i != vEdgeInfo.end();i ++)
	{
		ptr = (*mPicDebug).ptr<uchar>((*i).rows);
		ptr[(*i).cols * channels] = 255;
		ptr[(*i).cols * channels + 1] = 0;
		ptr[(*i).cols * channels + 2] = 0;
	}
#endif

	//查找点
	state = FindOneSpotLocation(mPic,
					TestInfo,
					&pPicRows,
					TestConfig.iTargetSpec1,
					TestConfig.iSpotSpec,
					iTargetTopBottomMinPlace,
					TestConfig.iTargetDeviation,
					TestConfig.iSpotMinLength,
					TestConfig.iSpotDeviation,
					TestResult.iEdgeLeftPosition,
					TestResult.iEdgeRightPosition,
					TestResult.vSpot1,
					TestResult.vSpot2,
					angle,
					iDirect);
	if(state)
	{
		delete (pPicRows);
		return 5;
	}

	angle = 0.0;
	sSpot.rows = TestInfo.NextStartSpot.rows - 10;
	sSpot.cols = TestInfo.NextStartSpot.cols;
	
	state = CalculationAngle(sSpot,
				vEdgeInfo,
				angle,
				eSpot);
	if(state)
	{
		delete (pPicRows);
		return 6;
	}

#ifdef WMDebug
	ptr = (*mPicDebug).ptr<uchar>(sSpot.rows);
	ptr[sSpot.cols * channels] = 0;
	ptr[sSpot.cols * channels + 1] = 255;
	ptr[sSpot.cols * channels + 2] = 0;

	ptr = (*mPicDebug).ptr<uchar>(eSpot.rows);
	ptr[eSpot.cols * channels] = 0;
	ptr[eSpot.cols * channels + 1] = 255;
	ptr[eSpot.cols * channels + 2] = 0;
#endif
//	return 0;
	//计算点下面的白色区域开始点
	//首先我根据给出的row col 和角度计算出我需要遍历的row的所有起始包括col
	//后面有了起始点和角度再加上终止点我就可以遍历所有的了
	//以找到的中点为轴线，向左向右分别遍历不等长度即可。顺便判断长度合法性

	//跳过点
	//此刻rows 和cols还是在点中
	//从左到右只有两种或三种状态认为跳过
	/*
	首先取其边缘点向上几个点，使其开始点位于白条中

	*/

	TestInfo.NextStartSpot.rows = eSpot.rows - 5;
	TestInfo.NextStartSpot.cols = eSpot.cols - 5 * angle;
	iTargetTopBottomMinPlace = 50;
	iTargetLeftRightMinPlace = 20;
	state = SkipTargetBand(mPic,
		TestInfo,
		&pPicRows,
		TestConfig.iTargetSpec1,
		iTargetTopBottomMinPlace,
		iTargetLeftRightMinPlace,
		angle,
		TestConfig.iBandFirstMinWidth,
		3);
	if(state)
	{
		delete (pPicRows);
		return 7;
	}

	SpotStartSpotLog.rows = TestInfo.NextStartSpot.rows;
	SpotStartSpotLog.cols = TestInfo.NextStartSpot.cols;

#ifdef WMDebug
	ptr = (*mPicDebug).ptr<uchar>(SpotStartSpotLog.rows);
	ptr[SpotStartSpotLog.cols * channels] = 0;
	ptr[SpotStartSpotLog.cols * channels + 1] = 255;
	ptr[SpotStartSpotLog.cols * channels + 2] = 255;

#endif
	//确定点的范围
		
	iTargetTopBottomMinPlace = 40;
	iTargetLeftRightMinPlace = 20;
	state = SkipBandFixFont(mPic,
					TestInfo,
					&pPicRows,
					TestConfig.iSpotSpec1,
					iTargetTopBottomMinPlace,
					iTargetLeftRightMinPlace,
					angle,
					TestConfig.iBandFirstMinWidth,
					30,
					vFontLeftEdge,
					vFontRightEdge);
	if(state)
	{
		delete (pPicRows);
		return 8;
	}
#ifdef WMDebug
	iNum = vFontLeftEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontLeftEdge[j].rows);
		ptr[vFontLeftEdge[j].cols * channels] = 255;
		ptr[vFontLeftEdge[j].cols * channels + 1] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 2] = 0;
	}
	iNum = vFontRightEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontRightEdge[j].rows);
		ptr[vFontRightEdge[j].cols * channels] = 0;
		ptr[vFontRightEdge[j].cols * channels + 1] = 255;
		ptr[vFontRightEdge[j].cols * channels + 2] = 0;
	}
#endif
//	return 0;
	//统计点
	state =  CountBandNum( mPic,
					TestInfo,
					&pPicRows,
					angle,
					TestConfig.iSpotSpec1,
					vFontLeftEdge,
					vFontRightEdge,
					3,
					2,
					2);
#ifdef WMDebug
	iNum = vFontLeftEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontLeftEdge[j].rows);
		ptr[vFontLeftEdge[j].cols * channels] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 1] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 2] = 255;
	}
	iNum = vFontRightEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontRightEdge[j].rows);
		ptr[vFontRightEdge[j].cols * channels] = 0;
		ptr[vFontRightEdge[j].cols * channels + 1] = 0;
		ptr[vFontRightEdge[j].cols * channels + 2] = 255;
	}
#endif
 	
// 	if(state != 4 )
// 	{
// 		delete (pPicRows);
// 		return 5;
// 	}

#ifdef WMDebug
	ptr = (*mPicDebug).ptr<uchar>(TestInfo.NextStartSpot.rows);
	ptr[TestInfo.NextStartSpot.cols * channels] = 0;
	ptr[TestInfo.NextStartSpot.cols * channels + 1] = 0;
	ptr[TestInfo.NextStartSpot.cols * channels + 2] = 255;

#endif
//	return 0;
	//跳过Band 1
	iTargetTopBottomMinPlace = 300;
	iTargetLeftRightMinPlace = 200;
	state = SkipTargetBand(mPic,
		TestInfo,
		&pPicRows,
		TestConfig.iFont1Spec,
		iTargetTopBottomMinPlace,
		iTargetLeftRightMinPlace,
		angle,
		TestConfig.iBandSecondMinWidth,
		3);
	if(state)
	{
		delete (pPicRows);
		return 10;
	}

	Font1StartSpotLog.rows = TestInfo.NextStartSpot.rows;
	Font1StartSpotLog.cols = TestInfo.NextStartSpot.cols;

#ifdef WMDebug
	ptr = (*mPicDebug).ptr<uchar>(Font1StartSpotLog.rows);
	ptr[Font1StartSpotLog.cols * channels] = 255;
	ptr[Font1StartSpotLog.cols * channels + 1] = 0;
	ptr[Font1StartSpotLog.cols * channels + 2] = 0;

#endif
//	return 1;
	//确定字1的范围
	vFontLeftEdge.clear();
	vFontRightEdge.clear();
	iTargetTopBottomMinPlace = 40;
	iTargetLeftRightMinPlace = 60;
	state = SkipBandFixFont(mPic,
					TestInfo,
					&pPicRows,
					TestConfig.iFont1Spec,
					iTargetTopBottomMinPlace,
					iTargetLeftRightMinPlace,
					angle,
					TestConfig.iBandFirstMinWidth,
					30,
					vFontLeftEdge,
					vFontRightEdge);
	if(state)
	{
		delete (pPicRows);
		return 11;
	}
#ifdef WMDebug
	iNum = vFontLeftEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontLeftEdge[j].rows);
		ptr[vFontLeftEdge[j].cols * channels] = 255;
		ptr[vFontLeftEdge[j].cols * channels + 1] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 2] = 0;
	}
	iNum = vFontRightEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontRightEdge[j].rows);
		ptr[vFontRightEdge[j].cols * channels] = 0;
		ptr[vFontRightEdge[j].cols * channels + 1] = 255;
		ptr[vFontRightEdge[j].cols * channels + 2] = 0;
	}
#endif
	//统计字1
	state =  CountBandNum( mPic,
					TestInfo,
					&pPicRows,
					angle,
					TestConfig.iFont1Spec,
					vFontLeftEdge,
					vFontRightEdge,
					2,
					4,
					2);
#ifdef WMDebug
	iNum = vFontLeftEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontLeftEdge[j].rows);
		ptr[vFontLeftEdge[j].cols * channels] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 1] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 2] = 255;
	}
	iNum = vFontRightEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontRightEdge[j].rows);
		ptr[vFontRightEdge[j].cols * channels] = 0;
		ptr[vFontRightEdge[j].cols * channels + 1] = 0;
		ptr[vFontRightEdge[j].cols * channels + 2] = 255;
	}
#endif
	if(state != 8 )
	{
		delete (pPicRows);
		return 5;
	}
	//跳过第三个白条
#ifdef WMDebug
	ptr = (*mPicDebug).ptr<uchar>(TestInfo.NextStartSpot.rows);
	ptr[TestInfo.NextStartSpot.cols * channels] = 0;
	ptr[TestInfo.NextStartSpot.cols * channels + 1] = 0;
	ptr[TestInfo.NextStartSpot.cols * channels + 2] = 255;

#endif
	iTargetTopBottomMinPlace = 70;
	iTargetLeftRightMinPlace = 60;
	state = SkipTargetBand(mPic,
		TestInfo,
		&pPicRows,
		TestConfig.iFont2Spec,
		iTargetTopBottomMinPlace,
		iTargetLeftRightMinPlace,
		angle,
		TestConfig.iBandThirdMinWidth,
		3);
	if(state)
	{
		delete (pPicRows);
		return 13;
	}

	Font2StartSpotLog.rows = TestInfo.NextStartSpot.rows;
	Font2StartSpotLog.cols = TestInfo.NextStartSpot.cols;

#ifdef WMDebug
	ptr = (*mPicDebug).ptr<uchar>(Font2StartSpotLog.rows);
	ptr[Font2StartSpotLog.cols * channels] = 255;
	ptr[Font2StartSpotLog.cols * channels + 1] = 0;
	ptr[Font2StartSpotLog.cols * channels + 2] = 0;

#endif
	//确定字2的范围
	vFontLeftEdge.clear();
	vFontRightEdge.clear();

	iTargetTopBottomMinPlace = 70;
	iTargetLeftRightMinPlace = 60;
	state = SkipBandFixFont(mPic,
					TestInfo,
					&pPicRows,
					TestConfig.iFont2Spec,
					iTargetTopBottomMinPlace,
					iTargetLeftRightMinPlace,
					angle,
					TestConfig.iBandFirstMinWidth,
					30,
					vFontLeftEdge,
					vFontRightEdge);
 	if(state)
 	{
 		delete (pPicRows);
 		return 14;
 	}
#ifdef WMDebug
	iNum = vFontLeftEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontLeftEdge[j].rows);
		ptr[vFontLeftEdge[j].cols * channels] = 255;
		ptr[vFontLeftEdge[j].cols * channels + 1] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 2] = 0;
	}
	iNum = vFontRightEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontRightEdge[j].rows);
		ptr[vFontRightEdge[j].cols * channels] = 0;
		ptr[vFontRightEdge[j].cols * channels + 1] = 255;
		ptr[vFontRightEdge[j].cols * channels + 2] = 0;
	}
#endif
	//统计字2
	state =  CountBandNum( mPic,
					TestInfo,
					&pPicRows,
					angle,
					TestConfig.iFont1Spec,
					vFontLeftEdge,
					vFontRightEdge,
					2,
					4,
					3);
#ifdef WMDebug
	iNum = vFontLeftEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontLeftEdge[j].rows);
		ptr[vFontLeftEdge[j].cols * channels] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 1] = 0;
		ptr[vFontLeftEdge[j].cols * channels + 2] = 255;
	}
	iNum = vFontRightEdge.size();
	for (int j = 0;j < iNum;j ++)
	{
		ptr = (*mPicDebug).ptr<uchar>(vFontRightEdge[j].rows);
		ptr[vFontRightEdge[j].cols * channels] = 0;
		ptr[vFontRightEdge[j].cols * channels + 1] = 0;
		ptr[vFontRightEdge[j].cols * channels + 2] = 255;
	}
#endif
	if(state != 5)
	{
		
		CString tmp;
		return 5;
	}

	delete (pPicRows);
//	imwrite("a.bmp",*mPic);
	return 0;
}
int splitcharacter(uchar**pFont,int iFontSkip,int &k,vector<vector<int>>& iFontChange,sTestPicConfig& TestConfig,int & iEdgeRightPosition)
{
	vector<int> vFontCols;
	vector<int> iFontFind;
	int iFontSkipMid;
	int* pFontDe1; //字像素出现次数
	int* pFontDe2; //背景像素出现次数
	int iBackgroundPix,iBandWidthTmp,j;

	if(pFont == NULL)
		return 7;
	pFontDe1 = new int[iFontSkip];
	if(pFontDe1 == NULL)
	{
		return 8;
	}
	pFontDe2 = new int[iFontSkip];
	if(pFontDe2 == NULL)
	{
		return 8;
	}
	for(j = 0;j < iFontSkip;j++)
	{
		iFontFind.push_back(1);//奇数 -- 背景
		vFontCols.clear();
		vFontCols.push_back(k);
		iFontChange.push_back(vFontCols);
	}
	iFontSkipMid = iFontSkip >> 1;
	iBandWidthTmp = 0;
	
	for(;k < iEdgeRightPosition;)   //遍历并找到一个字母
	{
		for(j = 0;j < iFontSkip; j++)
		{
			
			iBackgroundPix = 0;
			if(pFont[j][k] > TestConfig.iFont2Spec)
			{
				iBackgroundPix ++;
				pFontDe2[j] ++;
				if(pFontDe2[j] > 4)
				{
					pFontDe1[j] = 0;
					if(iFontFind[j]%2 == 0)
					{
						iFontFind[j] ++;
						iFontChange[j].push_back(k - pFontDe2[j]);
					}
				}
			}else
			{
				pFontDe1[j] ++;
				if(pFontDe1[j] > 4)
				{
					pFontDe2[j] = 0;
					if(iFontFind[j]%2 != 0)
					{
						iFontFind[j] ++;
						iFontChange[j].push_back(k - pFontDe1[j]);
					}
				}
			}
		}
		if(iBackgroundPix >= iFontSkip && iFontFind[iFontSkipMid] != 1)
		{
			iBandWidthTmp ++;
			if(iBandWidthTmp > TestConfig.iFont2BandMinWidth)
			{
				iBackgroundPix = k - iBandWidthTmp + 2;
				for(j = 0;j < iFontSkip; j++)
					iFontChange[j].push_back(iBackgroundPix);
				break;      //找到一个字母结束
			}
		}
		k = k + 3;
	}

	return 0;
}
int checkA1(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig)
{
	
	int checkstate;
	int checkNum,j;

	checkstate = splitcharacter(pFont,iFontSkip,k,TestResult.iA1FontChange,TestConfig,TestResult.iEdgeRightPosition);
	if(!checkstate)
	{
		return checkstate;
	}
	checkstate = 1;
	checkNum = 0;
	for(j = 0;j< iFontSkip;j++)
	{
		switch (checkstate)
		{
			case 1:
			{
				if(TestResult.iA1FontChange[j].size() == 6 && TestResult.iA1FontChange[j][3] - TestResult.iA1FontChange[j][2] > 4)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 2;
					}
				}
				break;
			}
			case 2:
			{
				if(TestResult.iA1FontChange[j].size() == 10 && TestResult.iA1FontChange[j][3] - TestResult.iA1FontChange[j][2] > 4 && TestResult.iA1FontChange[j][7] - TestResult.iA1FontChange[j][6] > 4)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 3;
					}
				}
				break;
			}
			case 3:
			{
				if(TestResult.iA1FontChange[j].size() == 6 && TestResult.iA1FontChange[j][3] - TestResult.iA1FontChange[j][2] > 6)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 4;
					}
				}
				break;
			}
			case 4:
				{
					break; //check A1 ok
				}
			default:
			{
				return 7;
			}
		}
	}
	return 0;
}
int checkR(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig)
{
	int checkstate;
	int checkNum,j;

	checkstate = splitcharacter(pFont,iFontSkip,k,TestResult.iRFontChange,TestConfig,TestResult.iEdgeRightPosition);
	if(!checkstate)
	{
		return checkstate;
	}

	checkstate = 1;
	checkNum = 0;
	for(j = 0;j< iFontSkip;j++)
	{
		switch (checkstate)
		{
			case 1:
			{
				if(TestResult.iRFontChange[j].size() == 6 && TestResult.iRFontChange[j][3] - TestResult.iRFontChange[j][2] > 4)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 2;
					}
				}
				break;
			}
			case 2:
			{
				if(TestResult.iRFontChange[j].size() == 10 && TestResult.iRFontChange[j][3] - TestResult.iRFontChange[j][2] > 1 && TestResult.iRFontChange[j][7] - TestResult.iRFontChange[j][6] > 10)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 3;
					}
				}
				break;
			}
			case 3:
			{
				if(TestResult.iRFontChange[j].size() == 6 && TestResult.iRFontChange[j][3] - TestResult.iRFontChange[j][2] > 14)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 4;
					}
				}
				break;
			}
			case 4:
			{
				if(TestResult.iRFontChange[j].size() == 6 && TestResult.iRFontChange[j][3] - TestResult.iRFontChange[j][2] > 6)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 5;
					}
				}
				break;
			}
			case 5:
				{
					break; //check R ok
				}
			default:
			{
				return 7;
			}
		}
	}
	return 0;
}
int checkU(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig)
{
	int checkstate;
	int checkNum,j;

	checkstate = splitcharacter(pFont,iFontSkip,k,TestResult.iUFontChange,TestConfig,TestResult.iEdgeRightPosition);
	if(!checkstate)
	{
		return checkstate;
	}
	checkstate = 1;
	checkNum = 0;
	for(j = 0;j< iFontSkip;j++)
	{
		switch (checkstate)
		{
			case 1:
			{
				if(TestResult.iUFontChange[j].size() == 10 && TestResult.iUFontChange[j][3] - TestResult.iUFontChange[j][2] > 4 && TestResult.iUFontChange[j][7] - TestResult.iUFontChange[j][6] > 4)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 2;
					}
				}
				break;
			}
			case 2:
			{
				if(TestResult.iUFontChange[j].size() == 6 && TestResult.iUFontChange[j][3] - TestResult.iUFontChange[j][2] > 6)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 3;
					}
				}
				break;
			}
			case 3:
				{
					break; //check U ok
				}
			default:
			{
				return 7;
			}
		}
	}
	return 0;
}
int checkB(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig)
{
		
	int checkstate;
	int checkNum,j;

	checkstate = splitcharacter(pFont,iFontSkip,k,TestResult.iBFontChange,TestConfig,TestResult.iEdgeRightPosition);
	if(!checkstate)
	{
		return checkstate;
	}
	checkstate = 1;
	checkNum = 0;
	for(j = 0;j< iFontSkip;j++)
	{
		switch (checkstate)
		{
			case 1:
			{
				if(TestResult.iBFontChange[j].size() == 6 && TestResult.iBFontChange[j][3] - TestResult.iBFontChange[j][2] > 4)
				{
					checkNum ++;
					if(checkNum > 8)
					{
						checkNum = 0;
						checkstate = 2;
					}
				}
				break;
			}
			case 2:
			{
				if(TestResult.iBFontChange[j].size() == 10 && TestResult.iBFontChange[j][3] - TestResult.iBFontChange[j][2] > 4 && TestResult.iBFontChange[j][7] - TestResult.iBFontChange[j][6] > 4)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 3;
					}
				}
				break;
			}
			case 3:
			{
				if(TestResult.iBFontChange[j].size() == 6 && TestResult.iBFontChange[j][3] - TestResult.iBFontChange[j][2] > 6)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 4;
					}
				}
				break;
			}
			case 4:
				{
					break; //check B ok
				}
			default:
			{
				return 7;
			}
		}
	}
	return 0;
}
int checkA2(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig)
{
	int checkstate;
	int checkNum,j;

	checkstate = splitcharacter(pFont,iFontSkip,k,TestResult.iA2FontChange,TestConfig,TestResult.iEdgeRightPosition);
	if(!checkstate)
	{
		return checkstate;
	}
	checkstate = 1;
	checkNum = 0;
	for(j = 0;j< iFontSkip;j++)
	{
		switch (checkstate)
		{
			case 1:
			{
				if(TestResult.iA2FontChange[j].size() == 6 && TestResult.iA2FontChange[j][3] - TestResult.iA2FontChange[j][2] > 4)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 2;
					}
				}
				break;
			}
			case 2:
			{
				if(TestResult.iA2FontChange[j].size() == 10 && TestResult.iA2FontChange[j][3] - TestResult.iA2FontChange[j][2] > 4 && TestResult.iA2FontChange[j][7] - TestResult.iA2FontChange[j][6] > 4)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 3;
					}
				}
				break;
			}
			case 3:
			{
				if(TestResult.iA2FontChange[j].size() == 6 && TestResult.iA2FontChange[j][3] - TestResult.iA2FontChange[j][2] > 6)
				{
					checkNum ++;
					if(checkNum > 4)
					{
						checkNum = 0;
						checkstate = 4;
					}
				}
				break;
			}
			case 4:
				{
					break; //check A ok
				}
			default:
			{
				return 7;
			}
		}
	}
	return 0;
}

int FindEndwiseEdge(Mat* mPic,
				sTestPicInfo& TestInfo,
				uchar** ptr[],
				int Skip,
				int Spec,
				deque<sSpotInfo>& EdgeInfo)
{
	if (mPic == NULL || mPic->empty() || ptr == NULL)
	{
		return 1;
	}
	if (Skip < 1)
		return 2;
	
	int irows,icols,ichannels,icolsend,irowsend;
	int i,j,x,y,z,m,n,k;
	int i1,j1,x1,y1,z1,m1,n1;
	int imid;
	int iend;
	int iretry;
	sSpotInfo spottmp;
	int RowStart;
	int ColStart;


	ichannels = TestInfo.channels;
	irows = TestInfo.rows - 5;
	icols = TestInfo.cols - 1;
	
	imid = Skip/2;
	irowsend = irows - imid;
	i1 = 0 - imid;
	ColStart = TestInfo.NextStartSpot.cols;
	RowStart = TestInfo.NextStartSpot.rows;


	icolsend = icols * ichannels;

	iend = 0;
	iretry = 3;

	//向右
	for(k = RowStart,z1 = ColStart,x = ColStart * ichannels;x < icolsend;)
	{
// 		for(j = 0,i = i1;j < Skip;j ++,i ++)
// 		{
// 			if (k + i > irows || k + i < Skip)
// 			{
// 				goto RightEnd;
// 			}
// 			ptr[j] = mPic->ptr<uchar>(k + i);
// 		}

		m = 0;
		n = 0;
		for(i = i1;i < imid;i ++)
		{
			y = k + i;
			if(((*ptr)[y])[x] > Spec)
			{
				m = y;  //记录最后一个分界点行坐标
				n ++;
			}
		}

		if(n == Skip)  //全白
		{
			
			spottmp.rows = k;
			spottmp.cols = z1;
			EdgeInfo.push_back(spottmp);//压入上一个分界点
			k = k + imid;
			if (k > irowsend)
			{
				goto RightEnd;
			}
			iend ++;
			if(iend > iretry)
				break;
		}else if(n == 0) //全黑
		{
			
			spottmp.rows = k;
			spottmp.cols = z1;
			EdgeInfo.push_back(spottmp);//压入上一个分界点
			 k = k - imid;
			if (k > irowsend)
			{
				goto RightEnd;
			}
			iend ++;
			if(iend > iretry)
				break;
			
		}else
		{
			iend = 0;
			spottmp.rows = k;
			spottmp.cols = z1;
			EdgeInfo.push_back(spottmp);//压入上一个分界点
			k = m; 
			if (k > irowsend)
			{
				goto RightEnd;
			}
		}

		
		/*原本是k = k + m - imid 那么行永远比实际目标晚一个像素点因为它是根据上一个点调整到下一个点。
		  如果我判断m与imid的大小可以预判出下一个行是加1还是减1*/
// 		if(m < imid)
// 		{
// 			k = k + m - imid - 1;
// 		}else
// 		{
// 			k = k + m - imid + 1;
// 		}
		//这样的算法貌似效果不是很好，会出现重影
		z1 ++;
		x = x + ichannels; //列向右平移
	}
RightEnd :
	//向左
	for(k = RowStart,z1 = ColStart,x = ColStart * ichannels ;x > ichannels;)
	{

		m = 0;
		n = 0;
		for(i = i1;i < imid;i ++)
		{
			y = k + i;
			if(((*ptr)[y])[x] > Spec)
			{
				m = y;
				n ++;
			}
		}

		if(n == Skip)
		{
			spottmp.rows = k;
			spottmp.cols = z1;
			EdgeInfo.push_front(spottmp);//压入上一个分界点
			k = k +  imid;
			if (k < Skip)
			{
				goto LeftEnd;
			}
			iend ++;
			if(iend > iretry)
				break;
		}else if (n == 0)
		{
			spottmp.rows = k;
			spottmp.cols = z1;
			EdgeInfo.push_front(spottmp);//压入上一个分界点
			k = k -  imid;
			if (k < Skip)
			{
				goto LeftEnd;
			}
			iend ++;
			if(iend > iretry)
				break;
		}else
		{
			iend = 0;
			spottmp.rows = k;
			spottmp.cols = z1;
			EdgeInfo.push_front(spottmp);//压入上一个分界点
			k = m;
			if (k < Skip)
			{
				goto LeftEnd;
			}
		}
		z1 --;
		x = x - ichannels;
	}
LeftEnd:

	return 0;

}
int FindBroadwiseEdge(Mat* mPic)
{
	return 0;
}



bool CompareLength(const sSpotLocationLength &A,const sSpotLocationLength &B)
{ 
	return (A.length < B.length);//升序排列，如果改为return A.length > B.length，则为降序
} 
bool CompareRows(const sSpotLocationLength &A,const sSpotLocationLength &B)
{ 
	return (A.Spot.rows < B.Spot.rows);//升序排列
}
int AnalysisData(vector<sSpotLocationLength>& SpotSet,sSpotInfo& LocationSpot)
{

	if (SpotSet.empty())
	{
		return 1;
	}

	int i,j,x,y,z;
	vector<sSpotLocationLength>::iterator iter;
	z = SpotSet.size() - 3;

	/*
	理论上说最小值就是我要找的目标，因为考虑到我的列的Skip是5，行的左右为1 最大的跨度
	也就是3，而正常跳到下个点，跨度为2,所以理论上不会影响到长度排序

	实际做法是选择最小长度相等的取中间值
	*/

	for (x = 0;x < z;x ++)
	{
		if (SpotSet[x].length > SpotSet[0].length)
		{
			iter = SpotSet.begin() + x;
			break;
		}

	}
	sort(SpotSet.begin(),iter,CompareRows);
	z = x/2;
	LocationSpot.rows = SpotSet[z].Spot.rows;
	LocationSpot.cols = SpotSet[z].Spot.cols;

	return 0;
}
int DetermineRange(vector<RowTestInfo>& RowRange,
				sTestPicInfo& TestInfo,
				float& angle,
				sSpotInfo& spot,
				int iTargetTopBottomMinRange,
				int iMaxLength,
				int iMinLength)
{
	int i,j,x,y,z;
	RowTestInfo tmp;

	
	if (angle > 0.0 ) //开口向右偏
	{
		 j = 4;
	} 
	else //开口向左偏
	{
		j = TestInfo.cols - 4;
	}

	for(i = 0;i < iTargetTopBottomMinRange;i ++)
	{
		tmp.sMidPoint.rows = spot.rows + i;
		tmp.sMidPoint.cols = spot.cols - i * angle;
		if (tmp.sMidPoint.cols > j)
		{
			return 0;
		}
		tmp.iLeftLength = iMaxLength;
		tmp.iRightLength = iMinLength;
		RowRange.push_back(tmp);
	}
	return 0;

}
int CalculationAngle(sSpotInfo& spot,
			deque<sSpotInfo>& edgeinfo,
			float& angle,
			sSpotInfo& LocationSpot)
{
	if (edgeinfo.empty())
	{
		return 1;
	}

	int x,y,z;
	int iend;
	int mid;
	int high,low,section;
	int i,j;
	int length;

	vector<sSpotLocationLength> vPSpotLocationLength;
	vector<sSpotLocationLength> vJSpotLocationLength;
	sSpotLocationLength sSpotTmp;

	high = edgeinfo.size();
	low = 0;
	iend = high;
	section = high - low;



#if 0
	while (section > 2)
	{
		mid = section/2;

		x = abs(edgeinfo[mid].rows - spot.rows) + abs(edgeinfo[mid].cols - spot.cols);
		y = abs(edgeinfo[mid + 1].rows - spot.rows) + abs(edgeinfo[mid + 1].cols - spot.cols);
		z = abs(edgeinfo[mid - 1].rows - spot.rows) + abs(edgeinfo[mid - 1].cols - spot.cols);
		if (z < x&&x < y)
		{
			low = mid + 1;
		}else if(x < z&&x >y)
		{
			high = mid - 1;
		}else
		{
			LocationSpot.rows = edgeinfo[mid].rows;
			LocationSpot.cols = edgeinfo[mid].cols;
			angle = abs(edgeinfo[mid].rows - spot.rows)/abs(edgeinfo[mid].cols - spot.cols);
			return 0;
		}
		section = high - low;
	}
#endif
	z = edgeinfo.size();
	//预排序
	for (x = 0;x < z;x ++)
	{
		length = abs(edgeinfo[x].rows - spot.rows) + abs(edgeinfo[x].cols - spot.cols);
		sSpotTmp.Spot = edgeinfo[x];
		sSpotTmp.length = length;
		vPSpotLocationLength.push_back(sSpotTmp);
	}

	sort(vPSpotLocationLength.begin(),vPSpotLocationLength.end(),CompareLength); 

 //精排序  取前30个
	for (x = 0;x < 30;x ++)
	{
		length = (vPSpotLocationLength[x].Spot.rows - spot.rows)*(vPSpotLocationLength[x].Spot.rows - spot.rows) + (vPSpotLocationLength[x].Spot.cols - spot.cols)*(vPSpotLocationLength[x].Spot.cols - spot.cols);
		sSpotTmp.Spot = vPSpotLocationLength[x].Spot;
		sSpotTmp.length = length;
		vJSpotLocationLength.push_back(sSpotTmp);
	}

	sort(vJSpotLocationLength.begin(),vJSpotLocationLength.end(),CompareLength);

	AnalysisData(vJSpotLocationLength,LocationSpot);

//	angle = float(abs(LocationSpot.cols - spot.cols))/float(abs(LocationSpot.rows - spot.rows));  //因为行不可能相等的，这样可以排除除数为0情况
	angle = float((LocationSpot.cols - spot.cols))/float((LocationSpot.rows - spot.rows));  //因为行不可能相等的，这样可以排除除数为0情况

	return 0;
}

int CheckNewPic(Mat * mPic,sTestPicInfo& TestInfo,uchar** ptr[],int bLength,int wLength,int Spec)
{
	if (mPic == NULL || mPic->empty())
	{
		return 1;
	}
	int rows,cols,channels;
	int i,j;
	int num;

	rows = TestInfo.rows;
	cols = TestInfo.cols;
	channels = TestInfo.channels;


	(*ptr) = new uchar*[rows];

	if (*ptr == NULL)
	{
		return 1;
	}

	//初始化
	for (i = 0;i < rows;i ++)
	{
		(*ptr)[i] = mPic->ptr<uchar>(i);
	}

	//初判
	int TargetLength;
	num = 0;
	TargetLength = bLength * channels;
	for (i = 0;i < TargetLength;)
	{
		for(j = 0;j < rows;j ++)
		{
			if (((*ptr)[j])[i] > Spec)
			{
				num ++;
				if (num > 5)
				{
					return 2;
				}
				
			}else
			{
				num = 0;
			}
			
		}
		i = i + channels;
	}

	num = 0;
	
	for (i = TargetLength,TargetLength = wLength * channels;i < TargetLength;)
	{
		for(j = 0;j < rows;j ++)
		{
			if (((*ptr)[j])[i] > Spec)
			{
				num ++;
				if (num > 5)
				{
					rows --;
					cols = rows;
					cols --;
					TargetLength = TestInfo.channels * TestInfo.cols;
					for (i = 0;i <TargetLength;)
					{
						if (((*ptr)[rows])[i] > Spec || ((*ptr)[cols])[i] > Spec)
						{
							num ++;
							if (num > 5)
							{
								return 2;
							}

						}else
						{
							num = 0;
						}
						i += channels;
					}
					return 0;	
				}

			}else
			{
				num = 0;
			}

		}
		i = i + channels;
	}



	return 2;

}

int CountBandNum(Mat* mPic,
			sTestPicInfo& TestInfo,
			uchar*** pFont,
			float angle,
			int Spec,
			vector<sSpotInfo>& vFontLeftEdge,
			vector<sSpotInfo>& vFontRightEdge,
			int wSpecNum,
			int bSpecNum,
			int bWithSpecNum)
	
{
	if (mPic == NULL || mPic->empty() || vFontLeftEdge.empty() || vFontRightEdge.empty())
	{
		return 1;
	}
	if ((*pFont) == NULL )
	{
		return 1;
	}

	int i,j,num,length,i1;
	int lengthend,widthend;
	int ichannels;
	int irows,icols,irowsbegin,*icolsbegin;
	int irowstop,irowsbottom;
	vector<int> datainfo;
	sSpotInfo sleft;
	sSpotInfo sright;
	sSpotInfo slefttop,srighttop;
	sSpotInfo sleftbottom,srightbottom;

	sSpotInfo sTlefttop,sTrighttop;
	sSpotInfo sTleftbottom,sTrightbottom;

	sSpotInfo stmp;
	double dangle,dsin,dcos;
	double b1,b2,slope,Nslope;

	FindColsLeftSpot(vFontLeftEdge,sleft);
	FindRowsBottomSpot(vFontLeftEdge,sleftbottom);
	FindRowsTopSpot(vFontLeftEdge,slefttop);
	FindColsRightSpot(vFontRightEdge,sright);
	FindRowsBottomSpot(vFontRightEdge,srightbottom);
	FindRowsTopSpot(vFontRightEdge,srighttop);

	ichannels = TestInfo.channels;

	slope = (1.0)/angle;
	Nslope = (-slope);

	b1 = (-sleft.rows) + sleft.cols * Nslope;
	sTlefttop.cols = floor((Nslope * (slefttop.rows + b1) + slefttop.cols)/((Nslope)*(Nslope) + 1));
//	sTlefttop.rows = floor(Nslope * sTlefttop.cols - b1);
	sTlefttop.rows = floor((Nslope * (slefttop.cols) - b1 - slefttop.rows)/((Nslope)*(Nslope)) + slefttop.rows);

	sTleftbottom.cols = floor((Nslope * (sleftbottom.rows + b1) + sleftbottom.cols)/((Nslope)*(Nslope) + 1));
//	sTleftbottom.rows = floor(Nslope * sTleftbottom.cols - b1);
	sTleftbottom.rows = floor((Nslope * (sleftbottom.cols) - b1 - sleftbottom.rows)/((Nslope)*(Nslope)) + sleftbottom.rows);

	b2 = (-sright.rows) + sright.cols * Nslope;
	sTrighttop.cols = floor((Nslope * (srighttop.rows + b2) + srighttop.cols)/((Nslope)*(Nslope) + 1));
//	sTrighttop.rows = floor(Nslope * sTrighttop.cols - b2);
	sTrighttop.rows = floor((Nslope * (srighttop.cols) - b1 - srighttop.rows)/((Nslope)*(Nslope)) + srighttop.rows);



	sTrightbottom.cols = floor((Nslope * (srightbottom.rows + b2) + srightbottom.cols)/((Nslope)*(Nslope) + 1));
//	sTrightbottom.rows = floor(Nslope * sTrightbottom.cols - b2);
	sTrightbottom.rows = floor((Nslope * (srightbottom.cols) - b1 - srightbottom.rows)/((Nslope)*(Nslope)) + srightbottom.rows);


	//以左面线为判断依据

	if (sTrighttop.rows > sTlefttop.rows)
	{
		sTrighttop.rows = sTlefttop.rows;
		sTrighttop.cols = floor((b2 + sTrighttop.rows)/Nslope);
	}else
	{
		sTlefttop.rows = sTrighttop.rows;
		sTlefttop.cols = floor((b1 + sTlefttop.rows)/Nslope);
	}

	if (sTrightbottom.rows < sTleftbottom.rows)
	{
		sTrightbottom.rows = sTleftbottom.rows;
		sTrightbottom.cols = floor((b2 + sTrightbottom.rows)/Nslope);
	}else
	{
		sTleftbottom.rows = sTrightbottom.rows;
		sTleftbottom.cols = floor((b1 + sTleftbottom.rows)/Nslope);
	}

// 	dangle = atan(angle);
// 	dsin = sin(dangle);
// 	dcos = cos(dangle);
// 
// 	length = sright.cols - sleft.cols + 16;
// 
// 	irows = srightbottom.rows + length * angle;
// 
// 	if (sTrightbottom.rows < sTleftbottom.rows)  //最低的底的行
// 	{
// 		irows = sleftbottom.rows;
// 	}else
// 	{
// 
// 	}
// 
// 	icols = srighttop.rows + length * angle;
// 
// 	if (slefttop.rows > icols)  //最高的顶的行
// 	{
// 		icols = slefttop.rows;
// 	}


//	num = vFontLeftEdge.size();
	num = sTleftbottom.rows - sTlefttop.rows;
	length = sTrightbottom.cols - sTleftbottom.cols;
	if (num < 1)
	{
		return 3;
	}
	icolsbegin = new int[num];
	if (icolsbegin == NULL)
	{
		return 4;
	}

	vFontLeftEdge.clear();
	for(i = 0;i < num;i ++)
	{
		stmp.rows = sTleftbottom.rows - i;
		stmp.cols = floor((b1 + sTleftbottom.rows)/Nslope);
		vFontLeftEdge.push_back(stmp);
		icolsbegin[i] = stmp.cols * ichannels;
	}
	vFontRightEdge.clear();
	for(i = 0;i < num;i ++)
	{
		stmp.rows = sTrightbottom.rows - i;
		stmp.cols = floor((b2 + sTrightbottom.rows)/Nslope);
		vFontRightEdge.push_back(stmp);
	}

//  	vFontRightEdge.push_back(srightbottom);
//  	vFontRightEdge.push_back(srighttop);
//  	vFontLeftEdge.push_back(sleftbottom);
//  	vFontLeftEdge.push_back(slefttop);

	datainfo.push_back(0); //0 空白 1 黑底
	int bnum,wnum,bwnum;
	bnum = 0;
	wnum = 0;
	bwnum = 0;
	for (i = 0,i1 = 0;i < length;i ++)
	{
		bwnum = 0;
		for (j = 0;j < num;j ++)
		{
			icols = icolsbegin[j] + i1;
			irows = vFontLeftEdge[j].rows - i * angle;
			if (((*pFont)[irows])[icols + 1] < Spec)  //绿色差别比较大
			{
				bwnum ++;
				if (bwnum > bWithSpecNum)
				{
					bnum ++;
					if(datainfo.back() != 1 && bnum > bSpecNum) 
					{
						wnum = 0;
						datainfo.push_back(1);
					}
					break;
				}
				
			}
		}
		if (j >= num)
		{
			wnum ++;
			if(datainfo.back() != 0 && wnum > wSpecNum) 
			{
				bnum = 0;
				datainfo.push_back(0);
			}
		}
		i1 += ichannels;
	}
	delete icolsbegin;
	return count(datainfo.begin(),datainfo.end(),1);
	
}