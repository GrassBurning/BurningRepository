// Config.cpp : 实现文件
//

#include "stdafx.h"
#include "VTestCheck.h"
#include "Config.h"
#include "afxdialogex.h"
#include "tisudshl.h"
using namespace _DSHOWLIB_NAMESPACE;

using namespace DShowLib;
// Config 对话框
extern DShowLib::Grabber* m_pGrabber;
IMPLEMENT_DYNAMIC(Config, CDialogEx)
bool setupDeviceFromFile( Grabber& gr, const std::string& devStateFilename= "lastSelectedDeviceState.xml");
Config::Config(CWnd* pParent /*=NULL*/)
	: CDialogEx(Config::IDD, pParent)
{

}

Config::~Config()
{
}

void Config::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Config, CDialogEx)
	ON_BN_CLICKED(IDOK, &Config::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Config::OnBnClickedCancel)

	ON_BN_CLICKED(IDC_BUTTONConfigCanmera, &Config::OnClickedButtonconfigcanmera)
END_MESSAGE_MAP()


// Config 消息处理程序



BOOL Config::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if( m_pGrabber->isDevValid() )
	{
		if( m_pGrabber->isLive() )
		{
			// Display a message that the dialog cannot be displayed while the
			// grabber is in live mode.
			AfxMessageBox( TEXT("The device settings dialog is not available while the live video is running.\n\nStop the live video first.") );

			// call the click event of OK button of the super class
			CDialog::OnOK();

			return TRUE;

		}
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void Config::OnBnClickedOk()
{
	
	
	CDialogEx::OnOK();
}


void Config::OnBnClickedCancel()
{
	
	CDialogEx::OnCancel();
}






void Config::OnClickedButtonconfigcanmera()
{
	setupDeviceFromFile(*m_pGrabber,"lastSelectedDeviceState.xml");
}

bool	setupDeviceFromFile(Grabber& gr, 
	const std::string& devStateFilename )
{
	if( gr.loadDeviceStateFromFile( devStateFilename ) )
	{
	
	}
	else
	{

	}
	if( !gr.showDevicePage() || !gr.isDevValid() )
	{
		return false;
	}
	gr.saveDeviceStateToFile( devStateFilename, true, true, false );
	return true;
}
