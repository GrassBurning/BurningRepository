
// VTestCheckDlg.cpp : ʵ���ļ�
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
#include<vector>
//#include "CmdHelper.h"//������ͷ�ļ�����Ҫ�����ģ��������ܽ����ļ�������
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
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

// CVTestCheckDlg �Ի���
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

typedef struct PicChangeInfo
{
	int iPosition;
	char cDataR;
	char cDataG;
	char cDataB;
	int state;  /*
				1:���� 2:Ŀ����� 3��Ŀ����� 4��������� 5���������

				*/
}sPicChangeInfo,*pPicChangeInfo;
typedef struct TestPicConfig
{
	int iTargetPixMinLength;    //
	int iBackgroundMinLength;     //������С����
	int iBackgroundDeviation;  //���Χ��һֱΪ����ֵ������Ϊ������ʼ
	int iTargetMinLength;          //Ŀ����С����
	int iTargetDeviation;       //���Χ��һֱΪĿ��ֵ������ΪĿ�꿪ʼ


	int iFont1Spec;
	int iFont1MinLength;           //������С����
	int iFont1BandMinWidth;      //������С���
	int iFont1SkipMaxWidth;      //�����������
	int iFont1Deviation;         //���Χ��һֱΪ����ֵ������Ϊ���忪ʼ
	int iFont1MinWidth;

	int iFont2Spec;
	int iFont2MinLength;           //������С����
	int iFont2BandMinWidth;      //������С���
	int iFont2SkipMaxWidth;      //�����������
	int iFont2Deviation;         //���Χ��һֱΪ����ֵ������Ϊ���忪ʼ
	int iFont2MinWidth;

	int iSpotMinLength;            //����С����
	int iSpotMinWidth;             //����С���
	int iSpotDeviation;         //���Χ��һֱΪ��ֵ������Ϊ�㿪ʼ

	int iBandFirstMinWidth;        //�׶˵�һ����ɫ�������
	int iBandSecondMinWidth ;       //�׶˵ڶ�����ɫ�������
	int iBandThirdMinWidth;        //�׶˵�������ɫ�������



	int iSpotSpec;
	int iTargetSpec1;
	int iTargetSpec2;

	int iBackgroundSpec;

	int iTargetTopBottomMinPlace;         //��Ŀ�����³�����ֵ������ΪĿ�겻����
	int iTargetLeftRightMinPlace;         //��Ŀ�����ҳ�����ֵ������ΪĿ�겻����
	
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
int fLoadConfig(sTestPicConfig& TestConfig);
int fTargetLocation(Mat * mPic,int iBottom,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int splitcharacter(uchar**pFont,int iFontSkip,int &k,vector<vector<int>>& iFontChange,sTestPicConfig& TestConfig,int & iEdgeRightPosition);
int checkA1(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int checkR(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int checkU(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int checkB(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);
int checkA2(uchar**pFont,int iFontSkip,int &k,sTestPicResult& TestResult,sTestPicConfig& TestConfig);



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
// CVTestCheckDlg ��Ϣ�������

BOOL CVTestCheckDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
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
	//if( !m_pGrabber->loadDeviceStateFromFile( "lastSelectedDeviceState.xml" ) )
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
	m_pGrabber->setDefaultWindowPosition(false);   //ȡ������Window�Դ�ͼ��ӳ����ʾ��ʽ
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
	m_pGrabber->showDevicePage();  //���öԻ���
	if( ! m_pGrabber->isDevValid() )
	{
		// Set the window that should display the live video.
		//pGrabber->setHWND( m_hWnd );
		AfxMessageBox( TEXT("No device was selected."));
		return FALSE;
	}
	m_pGrabber->closeDev();
#endif
	UpdateWindow();
	ex = this;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CVTestCheckDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		CRect rect1;
		m_pGrabber->setDefaultWindowPosition(true);
		m_pGrabber->stopLive();
//		m_pGrabber->closeDev();
//		GetDlgItem(IDC_STATICVIDEO)->GetWindowRect(&rect1);
		m_cStaticVideoWindow.GetWindowRect(&rect1);
//		m_pGrabber->openDev();
		m_pGrabber->setDefaultWindowPosition(false);
		m_pGrabber->setWindowSize(rect1.Width(),rect1.Height());
		m_pGrabber->setHWND( GetDlgItem( IDC_STATICVIDEO )->m_hWnd );
		m_pGrabber->startLive();
	
		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);

// 		//���岢����һ���ڴ��豸����
// 		CDC  m_dcMemory;
// 		GetDlgItem(IDC_Picture)->GetWindowRect(&rect);  
// 		if( !m_dcMemory.CreateCompatibleDC(&dc) )          // ���������Ե�DC
// 			return;
// 		SetStretchBltMode(dc.m_hDC,STRETCH_HALFTONE);
		
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CVTestCheckDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int ii = 0;

void CVTestCheckDlg::OnBnClickedOk()
{
#if 1
//	Mat srcImage1 = imread("D:\\Work\\opencv\\Desktop\\5.bmp",1);
	Mat srcImage1 = imread("D:\\tt\\12345.bmp",1);
	sTestPicConfig TestConfig;
	sTestPicResult TestResult;
	fLoadConfig(TestConfig);
	fTargetLocation(&srcImage1,2,TestResult,TestConfig);
	//	imshow("Example1",srcImage1);
	//	cvWaitKey();
	return ;
#endif

#if 0
	long height,width;
	Mat *a;
//	width = m_pGrabber->getAcqSizeMaxX();
//	height = m_pGrabber->getAcqSizeMaxY();
//	pSink->setSnapMode( false );
//	m_pGrabber->startLive(false);
//	pSink->snapImages( 1 ); 
	smart_ptr<MemBuffer> pBuffer = pSink->getLastAcqMemBuffer();
	width = pBuffer->getSize().cx;
	height = pBuffer->getSize().cy;
//	
//	m_pGrabber->stopLive();
	a = new Mat(height,width,CV_8UC3,(uchar*)pBuffer->getPtr());
//	pSink->setSnapMode( false );
//	m_pGrabber->startLive(true);
	Mat b;
	a->copyTo(b);
	CString ab;
	ii ++;
	ab.Format("%d.bmp",ii);
	imwrite(ab.GetBuffer(ab.GetLength()),b);
//	m_pGrabber->startLive(true);
//	namedWindow("img");
//	imshow("img", b);
//	waitKey(0);
#endif

}


UINT Test(LPVOID pParam)
{
	long height,width;
	Mat *a;
	Mat b;
	smart_ptr<MemBuffer> pBuffer = pSink->getLastAcqMemBuffer();
	width = pBuffer->getSize().cx;
	height = pBuffer->getSize().cy;
	a = new Mat(height,width,CV_8UC3,(uchar*)pBuffer->getPtr());

	a->copyTo(b);
	imwrite("a.bmp",b);

	namedWindow("img");
	imshow("img", b);
	waitKey(0);
	return 0;
}




void CVTestCheckDlg::OnBnClickedCancel()
{

	
	// Stop the live video.
	m_pGrabber->stopLive();
	
	// Close the device.
	m_pGrabber->closeDev();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void CVTestCheckDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
#ifdef EASYSIZEOPEN	
	UPDATE_EASYSIZE;
#endif

	UpdateWindow();
	// TODO: �ڴ˴������Ϣ����������
}


void CVTestCheckDlg::OnBnClickedBSetposition()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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
		putText(pShowImgTmp, temp, PrePosEnd, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));//ֻ��ʵʱ��ʾ����ƶ������� 
		rectangle(pShowImgTmp,PrePosBegin,PrePosEnd,Scalar(0,255,0),2);
		imshow("PostionImg",pShowImgTmp);
	}
#endif
	else if(SelectFlag && Event == CV_EVENT_LBUTTONUP)
	{
		//��ʾ�����ͼ
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
	return;
}




void CVTestCheckDlg::OnBnClickedConfig()
{
	// Stop the live video.
	m_pGrabber->stopLive();

	// Close the device.
	m_pGrabber->closeDev();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(password.DoModal() == IDOK)
	{

	}
}

typedef struct TestThreadInfo
{
	CWinThread *pTestThreadEntry;
	bool bTestResult;
	bool bTestEnd;
	CTime TestTime;
	Mat * mTestData;
	UINT iTestPosition;
	ULONG iTestItem;

} *pTestThreadInfo,sTestThreadInfo;

vector<sTestThreadInfo> GlobalTestInfo;
UINT TestBegin(LPVOID pParam)
{
	sTestThreadInfo TestInfo;
	long height,width;
	Mat *pData;
	ULONG iItem;

	iItem = 0;
	while (true)
	{
		TestInfo.bTestResult = false;
		TestInfo.bTestEnd = false;
		TestInfo.iTestPosition = -1;
		TestInfo.mTestData = NULL;
		TestInfo.pTestThreadEntry = NULL;
		 
		width = 0;
		height = 0;
		pData = NULL;
		TestInfo.TestTime = CTime::GetCurrentTime();
		TestInfo.iTestItem = iItem;
		smart_ptr<MemBuffer> pBuffer = pSink->getLastAcqMemBuffer();
		width = pBuffer->getSize().cx;
		height = pBuffer->getSize().cy;
		pData = new Mat(height,width,CV_8UC3,(uchar*)pBuffer->getPtr());

		TestInfo.mTestData = pData;
		TestInfo.pTestThreadEntry = AfxBeginThread(TestEntry,(LPVOID) &iItem,0, CREATE_SUSPENDED, NULL);

		
		if (TestInfo.pTestThreadEntry)
		{
			TestInfo.pTestThreadEntry->m_bAutoDelete = FALSE;
			TestInfo.pTestThreadEntry->ResumeThread();
		}
		iItem ++;
		GlobalTestInfo.push_back(TestInfo);
		Sleep(1000);
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
	vector<iTestLog> TestLog;
	
	i = 0;
	j = 0;
	length = 0;
	while(TRUE)
	{
		if (GlobalTestInfo.empty())
		{
			Sleep(2000);
			continue;
		}
		length = GlobalTestInfo.size();
		for (i = 0; i < GlobalTestInfo.size(); i ++)
		{
			if (TestLog.empty())
			{
				break;
			}
			for (j = 0;j < TestLog.size(); j ++)
			{
				vector<sTestThreadInfo>::iterator iter;
				iter = GlobalTestInfo.begin();
				if (TestLog[j].iTestPosition == GlobalTestInfo[i].iTestPosition)
				{

					DWORD code;
					bool res = GetExitCodeThread(GlobalTestInfo[i].pTestThreadEntry, &code);
					if (!res && code==STILL_ACTIVE)//�̻߳�����	���pWinTh �Ѿ������ˣ�code��ֵΪ�̵߳��˳���򷵻�ֵ
					{
						TerminateThread(GlobalTestInfo[i].pTestThreadEntry,NULL);
					}
				
					GlobalTestInfo.erase(iter);
					j --;
					
				}
			}
			
		}

		for (i = 0; i < length; i ++)
		{
			if (GlobalTestInfo[i].bTestResult)
			{

			}
		}
	}


	return 0;
}

UINT TestEntry(LPVOID pParam)
{

	return 0;
}


#if 0
/**************************
mPic: 
iColor:
iRStep:�в���
iCStep:�в���
iLineWide:�߿�

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
	TestConfig.iTargetSpec1         =GetPrivateProfileInt("Basic Info","TargetSpec1",0,Configfile);
	TestConfig.iTargetSpec2         =GetPrivateProfileInt("Basic Info","TargetSpec2",0,Configfile);
	
	TestConfig.iBackgroundSpec      =GetPrivateProfileInt("Basic Info","BackgroundSpec",0,Configfile);
	TestConfig.iTargetTopBottomMinPlace=GetPrivateProfileInt("Basic Info","TargetTopBottomMinPlace",0,Configfile);
	TestConfig.iTargetLeftRightMinPlace=GetPrivateProfileInt("Basic Info","TargetLeftRightMinPlace",0,Configfile);
	return 0;
}
typedef struct SpotInfo
{
	int rows;
	int cols;
}sSpotInfo,*pSpotInfo;
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


	if(mPic == NULL || mPic->empty())
	{
		return 1;
	}
	sSpot1.rows = irows;
	sSpot1.cols = icols;
	vSpot1.push_back(sSpot1);

	while(vSpot1.size() > 0)
	{
		sSpot1 = vSpot1.front();
		ptr = mPic->ptr<uchar>(sSpot.rows);
		vSpot1.pop_front();
		vSpot2.push_back(sSpot1);
		if(ptr[sSpot1.cols*3 + 2] < ispec)
		{
			vSpot2.push_back(sSpot1);
			//����
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows - 1&&vSpot2.cols == sSpot1.cols + 1)
				{
					break;
				}
			}
			if(i >= vSpot2.size())
			{
				sSpot2.rows = sSpot1.rows - 1;
				sSpot2.cols = sSpot1.cols + 1;
				vSpot1.push_back(sSpot2);
			}
			//��
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows - 1&&vSpot2.cols == sSpot1.cols)
				{
					break;
				}
			}
			if(i >= vSpot2.size())
			{
				sSpot2.rows = sSpot1.rows - 1;
				sSpot2.cols = sSpot1.cols;
				vSpot1.push_back(sSpot2);
			}
			//����
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows - 1&&vSpot2.cols == sSpot1.cols - 1)
				{
					break;
				}
			}
			if(i >= vSpot2.size())
			{
				sSpot2.rows = sSpot1.rows - 1;
				sSpot2.cols = sSpot1.cols - 1;
				vSpot1.push_back(sSpot2);
			}
			//��
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows &&vSpot2.cols == sSpot1.cols - 1)
				{
					break;
				}
			}
			if(i >= vSpot2.size())
			{
				sSpot2.rows = sSpot1.rows;
				sSpot2.cols = sSpot1.cols - 1;
				vSpot1.push_back(sSpot2);
			}
			//����
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows + 1&&vSpot2.cols == sSpot1.cols - 1)
				{
					break;
				}
			}
			if(i >= vSpot2.size())
			{
				sSpot2.rows = sSpot1.rows + 1;
				sSpot2.cols = sSpot1.cols - 1;
				vSpot1.push_back(sSpot2);
			}
			//��
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows + 1&&vSpot2.cols == sSpot1.cols)
				{
					break;
				}
			}
			if(i >= vSpot2.size())
			{
				sSpot2.rows = sSpot1.rows + 1;
				sSpot2.cols = sSpot1.cols;
				vSpot1.push_back(sSpot2);
			}
			//����
			for(i = 0;i < vSpot2.size();i ++)
			{
				if(vSpot2[i].rows == sSpot1.rows + 1&&vSpot2.cols == sSpot1.cols + 1)
				{
					break;
				}
			}
			if(i >= vSpot2.size())
			{
				sSpot2.rows = sSpot1.rows + 1;
				sSpot2.cols = sSpot1.cols + 1;
				vSpot1.push_back(sSpot2);
			}
		}else
		{
			vSpot.push_back(sSpot1);
		}
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
		if(vSpotInfo[i].rows < sTargetOne.rows)
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
		if(vSpotInfo[i].rows > sTargetOne.rows)
		{
			sTargetOne.cols = vSpotInfo[i].cols;
			sTargetOne.rows = vSpotInfo[i].rows;
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


	MidLeftOne.rows  = (i + j)/2;
	MidLeftOne.cols = 0;
	for(i = 0;i < vSpotInfo.size();i ++)
	{
		if(vSpotInfo[i].iRows == MidLeftOne.rows)
		{
			MidLeftOne.cols = MidLeftOne.cols > vSpotInfo[i].cols?MidLeftOne.cols:vSpotInfo[i].cols;
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
		if(vSpotInfo[i].cols > sLeftOne.cols)
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
		if(vSpotInfo[i].cols > sLeftOne.cols)
		{
			sLeftOne.cols = vSpotInfo[i].cols;
			sLeftOne.rows = vSpotInfo[i].rows;
		}
	}

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

	ichannels = mPic->ichannels();

	state =  FindConnectDomain(mPic,iRows,iCols,ichannels,iSpotSpec,vSpot1);
	if(state != 0||vSpot1.size() < 10)
	{
		return 2;
	}
	sSpotInfo sTmp1,sTmp2;
	int m,n,x,y;

	//FindRowsBottomSpot(vSpot1,sTmp1); //Ŀ�꿪��������б ��λ����ʱ��һ����
	FindColsRightSpot(vSpot1,sTmp2);//���ұߵ��Ǹ���
	
	x = sTmp2.rows - 60;
	y = (sTmp1.cols + 2 + 70) * ichannels;
	for(m = sTmp2.rows;m > x;m --)
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
					iDirect = 0;
					goto NextSpotTarget;//�ҵ�Ŀ��2
				}
			}else
			{
				iTargetNum = 0;
			}
			n = n + ichannels;
		}
	}
	//FindRowsTopSpot(vSpot1,sTmp1); //Ŀ�꿪��������б ��λ����ʱ�ڶ�����
	FindColsLeftSpot(vSpot1,sTmp1);  //ȡ������ĵ�
	x = sTmp1.rows - 60;
	y = (sTmp1.cols - 2 - 70 ) * ichannels;
	for(m = sTmp1.rows;m > x;m --)
	{
		ptr = mPic->ptr<uchar>(m);
		iTargetNum = 0;
		for(n = (sTmp1.cols + 2) * ichannels;n < y;)
		{
			if(ptr[n] < iSpec2)
			{
				iTargetNum ++;
				if(iTargetNum > 3)
				{
					iDirect = 1;
					vector<sSpotInfo> vtmp;
					vtmp = vSpot2;
					vSpot2 = vSpot1;
					vSpot1 = vtmp;
					goto NextSpotTarget;//�ҵ�Ŀ��2
				}
			}else
			{
				iTargetNum = 0;
			}
			n = n - ichannels;
		}
	}
	return 3;
NextSpotTarget:
	state =  FindConnectDomain(mPic,m,n/ichannels,ichannels,iSpec2,vSpot1);
	if(state != 0||vSpot1.size() < 10)
	{
		return 4;
	}else
	{
		return 0;
	}

}

int FixAngleAndRowStart(vector<sSpotInfo>& vSpot1,
				vector<sSpotInfo>& vSpot2,
				int& iDirect,
				float& fAngleValue,
				int & rows)
{
	sSpotInfo sTmp1,sTmp2;

	FindRowsTopSpot(vSpot1,sTmp1);
	FindRowsTopSpot(vSpot2,sTmp2);
	if(iDirect == 1) //����
	{
		fAngleValue = 1.0*(sTmp2.rows - sTmp1.rows)/(sTmp2.cols - sTmp1.cols);

		rows = sTmp1.rows ;
		return 0;
	}
	if(iDirect == 0)//����
	{
		fAngleValue = 1.0*(sTmp1.rows - sTmp2.rows)/(sTmp2.cols = sTmp1.cols);
		FindColsLeftSpot(vSpot1,sTmp2);
		rows = sTmp2.rows ;
	
		return 0;
	}
	return 0;
}
int CountFont(Mat* mPic,
				int& rows,
				int& cols,
				int& channels;
				int& iFontSpec,
				int& iFontMinLength,
				int& iFontNum;
				int& iEdgeRightPosition,
				int iDirect,
				vector<sTestPicResult>& Result)
{
	vector<sSpotInfo> vSpot;
	sSpotInfo sTmp;
	bool bCol;
	int i,j,k,x,y,m,n;
	int iTargetNum;
	bCol = true;
	iTargetNum = 0;
	iFontNum = 0;
	do
	{
		state = FindConnectDomain(mPic,rows,cols,channels,spec,vSpot);
		if(!state||vSpot.size() < 10)
		{
			return 4;
		}
		iFontNum ++;
		FindMidLeftSpot(vSpot,sTmp);  //ȡ�����м�ĵ�
		x = sTmp.rows;
		y = sTmp.cols + 2;
		k = y;

		iTargetNum = 0;
		for(n = k * channels;n < iEdgeRightPosition;y ++)
		{
			if(!iDirect)//= 0,Ŀ�꿪��������б
				m = cvFlood(x - (y - k) * angle);
			else//=1,Ŀ�꿪��������б
				m = cvFlood(x + (y - k) * angle);
			ptr = mPic->ptr<uchar>(m);
			if(ptr[n] < iFontSpec)
			{
				iTargetNum ++;
				if(iTargetNum > 3)
				{
					bCol = true;
					rows = m;
					cols = n/channels - 1;
					break;//�ҵ�Ŀ��
				}
			}else
			{
				iTargetNum = 0;
			}
			n = n + ichannels;
			
		}
		if(n >= iEdgeRightPosition)
			return 0;
	}while(bCol)

	return 0;
	
}
int FindConnectDomainEdge(Mat* mPic,
						int iDirect,
						int Spec,
						int rows,
						float angle,
						int cols,
						int BandLengthSpec,
						int iTargetLengthSpec)
{
	int irows;
	int icols;
	int ichannels;
	int iskipnum;
	int ispec = Spec;
	uchar * ptr;
	int i,j,irEnd;
	int iDTarget,iDBack,iCEnd,iTarget,iBack;
	int state;

	if(mPic == NULL || mPic->empty() || pEdgeInfo == NULL)
	{
		return 1;
	}
	irows = rows;

	irEnd = 70;
	icEnd = cols * 3;
	if(iDirect == 1)
	{
		for(i = irows;i > iEnd;i --)
		{
			for(j = 0;j < iCEnd)
			{
				irows = cvFlood(rows + j * angle);
				ptr = mPic->ptr<uchar>(irows);
				if(ptr[j] > Spec)
				{
					break;
				}
			}
		}
	}
	if(iDirect == 0)
	{
		for(i = irows;i > iEnd;i --)
		{
			for(j = 0;j < iCEnd)
			{
				irows = cvFlood(rows - j * angle);
				ptr = mPic->ptr<uchar>(irows);
				if(ptr[j] > Spec)
				{
					break;
				}
			}
		}
	}

}
int SkipBandFixFont(Mat* mPic,
			int iDirect,
			int iTargetSpec,
			int iFontSpec,
			int& iTargetTopBottomMinPlace;
			int& rows,
			int& cols;
			float angle,
			int& ColsEnd,
			int& iEdgeLeftPosition,
			int iBandWidthSpec,
			int iSpotSpec,
			int iTargetLengthSpec)
{
	int irows,ichannels,icols;
	uchar * ptr;
	int i,j,irEnd;
	int iDTarget,iDBack,iCEnd,iTargetNUm,iTargetLength;
	int state;

	if(mPic == NULL || mPic->empty())
	{
		return 1;
	}
	irows = rows;

	irEnd = 70;
	ichannels = mPic->channels();
	icEnd = ColsEnd * ichannels;
	iTargetNum = 0;
	for(i = irows;i > iTargetTopBottomMinPlace;i --)
	{
		switch (state)
		{
			case 0:
			{
				
				for(icols = 0,j = cols * ichannels;j < iCEnd;icols ++)
				{
					if(iDirect == 1)
					{
						irows = cvFlood(rows - (ColsEnd - icols)*angle);
					}else
					{
						irows = cvFlood(rows + (ColsEnd - icols)*angle);
					}
					ptr = mPic->ptr<uchar>(irows);
					if(ptr[j + 2] > iTargetSpec)
					{
						iTargetLength ++;
						if(iTargetLength > iTargetLengthSpec)
						{
							iTargetNum ++;
							if(iTargetNum > iBandWidthSpec)
							{
								cols = j/ichannels;
								iTargetNum = 0;
								state = 1;
								break;
							}
						}
					}else
					{
						iTargetLength = 0;
					}
					j = j + ichannels;
				}
			}
			case 1:
			{
				for(j = cols * ichannels;j < iCEnd;)
				{
					if(iDirect == 1)
					{
						irows = cvFlood(rows - (ColsEnd - icols)*angle);
					}else
					{
						irows = cvFlood(rows + (ColsEnd - icols)*angle);
					}
					ptr = mPic->ptr<uchar>(irows);
					if(ptr[j + 2] < iFontSpec)
					{
						iTargetLength ++;
						if(iTargetLength > 2)
						{
							iTargetNum ++;
							if(iTargetNum > 2)
							{
								int tmp;
								tmp = j/ichannels;
								iEdgeLeftPosition = (cols + tmp)/2;
								cols = tmp;
								rows = i;
								return 0;
							}
						}
					}else
					{
						iTargetLength = 0;
					}
					j = j + ichannels;
				}
			}
		}
	}
	return 1;
}

int FindAllSpotLocation(Mat * mPic,
					int& rows,
					int& cols,
					int& channels,
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
	if (mPic == NULL || mPic->empty())
	{
		return 1;
	}
	
	uchar* ptr = NULL;
	int i,j,k,state;
	int iColsEnd;
	int iTargetPixNum = 0;
	bool bF1,bF2,bF3;
	ULONG uTargetMeasureLength = 0;
	ULONG uTargetMeasureNum = 0;
	ULONG uTargetMeasureDNum = 0;


	iTargetPixNum = 0;
	i = rows;
	iColsEnd = mPic->cols * channels;
	for(i = rows;i < iTargetTopBottomMinPlace;i --)
	{
		ptr = mPic->ptr<uchar>(i);
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
					if(ptr[j + 2] > iTargetSpec)//��ɫ����
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
				}
				case 1:
				{
					if(ptr[j + 2] < iSpotSpec)//�����
					{
						iTargetPixNum ++;
						if(iTargetPixNum > 2)
						{
							iTargetPixNum = 0;
							bF2 = true;
							state = 2;
						}
					}else
					{
						iTargetPixNum = 0;
					}
				}
				case 2:
				{
					if(ptr[j + 2] > iTargetSpec)//��ɫ����
					{
						iTargetPixNum ++;
						if(iTargetPixNum > 8)
						{
							iTargetPixNum = 0;
							bF3 = true;
							state = 3;
						}
					}else
					{
						iTargetPixNum = 0;
					}
				}
				default:
				{
				}
			}
				
			if(bF1 && bF2 && bF3)
			{
				state = FindSpotLocation( mPic,iSpotSpec,j/3,iSpotMinLength,i,iDirect,vSpot1,vSpot2);
				if(state == 0)
				{
					FixAngleAndRowStart(vSpot1,vSpot2,iDirect,fAngleValue,rows);
					return 0;
				}
			}
			j =j + 3;
		}
	}
	return 1;
}

#if 1
int FindEdgeLocation(Mat * mPic,
					int& rows,
					int& cols,
					int& channels,
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
	if (mPic == NULL || mPic->empty())
	{
		return 1;
	}
	
	uchar* ptr = NULL;
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
	iColsEnd = cols * channels;
	for(i = rows - 1;i > iTargetTopBottomMinPlace; i--)
	{
		vRowChange.clear();
		vRowInfo.clear();

		iTargetPixNum = 0;
		iBackgroundPixNum = 0;
			
		for (j = 0;j < iColsEnd; )
		{
			if ( ptr[j + 2] > iTargetSpec )  //����Ŀ�����
			{
				iTargetPixNum ++;
				iBackgroundPixNum = 0;
				if(iTargetPixNum > iTargetDeviation)  //�������ֶ��ٵ�Ŀ������ΪOK
				{
					if (vRowInfo.size() == 0)
					{
						vRowInfo.push_back(1);
						vRowChange.push_back(j/3);
					}else if (vRowInfo.back() != 1)
					{
						vRowInfo.push_back(1);
						vRowChange.push_back(j/3 - iTargetPixNum - 4); //ǰһ����������
						vRowChange.push_back(j/3 - iTargetPixNum - 1); //Ŀ�������
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
						vRowChange.push_back(j/3);
					}else if (vRowInfo.back() != 0)
					{
						vRowInfo.push_back(0);
						vRowChange.push_back(j/3 - iBackgroundPixNum - 3);
						vRowChange.push_back(j/3 - iBackgroundPixNum - 1);
					}
				}
			}
			j =j + channels;
		}
		vRowChange.push_back(j/channels);
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
				if (uTargetMeasureNum > iBandFirstMinWidth)
				{
					iEdgePosition = i + uTargetMeasureNum - 1;
					uTargetMeasureNum = 0;
					uTargetMeasureDNum = 0;
					rows = i;
					cols = j;
					return 0;
				}
				
			}
		}
		if (!bTargetFind) //��������һ�в���Ŀ��������¿�ʼ
		{
			uTargetMeasureNum = 0;
		}
ColsEnd:
	}

	
	return 1;
}
#endif
/*
	0 OK
	1 ͼƬ����
	2 û���ҵ�ͼƬ�ײ�
	3 ����һ����Χδ�ҵ�Ŀ��
	4 û��������Ŀ��
*/

int CheckPic(Mat * mPic,sTestPicResult& TestResult,sTestPicConfig& TestConfig)
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
	int iDirect;
	int iTargetTopBottomMinPlace;
	int iFontNum;
	state = 0;
	rows = mPic->rows;
	cols = mPic->cols;
	channels = mPic->channels();
	iTargetTopBottomMinPlace = rows - TestConfig.iTargetTopBottomMinPlace;
	   //��ʼ״̬Ѱ�ұ߽����
	rowsold = rows;
	colsold = cols;
	state = FindEdgeLocation(mPic,
								rows,
								cols,
								channels,
								TestConfig.iTargetSpec1,
								iTargetTopBottomMinPlace,
								TestConfig.iTargetDeviation,
								TestConfig.iBackgroundDeviation,
								TestConfig.iTargetMinLength,
								TestConfig.iBandFirstMinWidth,
								TestResult.iEdgeLeftPosition,
								TestResult.iEdgeRightPosition,
								TestResult.iEdgePosition);
	if(!state)
		return 2;

	//���ҵ�
	angle = 0.0;
	iDirect = 3;
	rowsold = rows;
	colsold = cols;
	state = FindAllSpotLocation(mPic,
					rows,
					cols,
					channels,
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
	if(!state)
		return 3;
	
	//������1�±�Ե�ҵ���1
	
	rowsold = rows;
	colsold = cols;
	state = SkipBandFixFont(mPic,
			iDirect,
			iTargetSpec,
			iFontSpec,
			iTargetTopBottomMinPlace;
			rows,
			cols;
			angle,
			ColsEnd,
			iEdgeLeftPosition,
			iBandWidthSpec,
			iSpotSpec,
			iTargetLengthSpec)
	if(!state)
		return 4;
	
	//�����1
	rowsold = rows;
	colsold = cols;
	iFontNum = 0;

	state =  CountFont(mPic,
				rows,
				cols,
				channels;
				iFontSpec,
				iFontMinLength,
				iFontNum;
				iEdgeRightPosition,
				iDirect,
				TestResult);
	if(!state || iFontNum != 8)
		return 5;
  //������2�±�Ե�ҵ���2
	rowsold = rows;
	colsold = cols;
	state = SkipBandFixFont(mPic,
			iDirect,
			iTargetSpec,
			iFontSpec,
			iTargetTopBottomMinPlace;
			rows,
			cols;
			angle,
			ColsEnd,
			iEdgeLeftPosition,
			iBandWidthSpec,
			iSpotSpec,
			iTargetLengthSpec)
	if(!state)
		return 4;
	  //�����2����
	rowsold = rows;
	colsold = cols;
	iFontNum = 0;

	state =  CountFont(mPic,
				rows,
				cols,
				channels;
				iFontSpec,
				iFontMinLength,
				iFontNum;
				iEdgeRightPosition,
				iDirect,
				TestResult);
	if(!state || iFontNum != 5)
		return 7;

//	imwrite("a.bmp",*mPic);
	return 0;
}
int splitcharacter(uchar**pFont,int iFontSkip,int &k,vector<vector<int>>& iFontChange,sTestPicConfig& TestConfig,int & iEdgeRightPosition)
{
	vector<int> vFontCols;
	vector<int> iFontFind;
	int iFontSkipMid;
	int* pFontDe1; //�����س��ִ���
	int* pFontDe2; //�������س��ִ���
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
		iFontFind.push_back(1);//���� -- ����
		vFontCols.clear();
		vFontCols.push_back(k);
		iFontChange.push_back(vFontCols);
	}
	iFontSkipMid = iFontSkip >> 1;
	iBandWidthTmp = 0;
	
	for(;k < iEdgeRightPosition;)   //�������ҵ�һ����ĸ
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
				break;      //�ҵ�һ����ĸ����
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



