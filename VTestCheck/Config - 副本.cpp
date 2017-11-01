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
	DDX_Control(pDX, IDC_COMBO_Device, m_cboDevice);
	DDX_Control(pDX, IDC_COMBO_FrameRate, m_cboFrameRate);
	DDX_Control(pDX, IDC_COMBO_InputChannel, m_cboInputChannel);
	DDX_Control(pDX, IDC_COMBO_VideoFormat, m_cboVideoFormat);
	DDX_Control(pDX, IDC_COMBO_VideoNorm, m_cboVideoNorm);
	DDX_Control(pDX, IDC_CHECK_FlipH, OnCheckFliph);
	DDX_Control(pDX, IDC_CHECK_FlipV, OnCheckFlipv);
}


BEGIN_MESSAGE_MAP(Config, CDialogEx)
	ON_BN_CLICKED(IDOK, &Config::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Config::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_Device, &Config::OnSelchangeComboDevice)
	ON_CBN_SELCHANGE(IDC_COMBO_FrameRate, &Config::OnSelchangeComboFramerate)
	ON_CBN_SELCHANGE(IDC_COMBO_InputChannel, &Config::OnSelchangeComboInputchannel)
	ON_CBN_SELCHANGE(IDC_COMBO_VideoFormat, &Config::OnSelchangeComboVideoformat)
	ON_CBN_SELCHANGE(IDC_COMBO_VideoNorm, &Config::OnSelchangeComboVideonorm)
	ON_BN_CLICKED(IDC_CHECK_FlipH, &Config::OnClickedCheckFliph)
	ON_BN_CLICKED(IDC_CHECK_FlipV, &Config::OnClickedCheckFlipv)
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

	// Store the state of the currently valid device.
	saveDeviceSettings();

	// Fill the device combo box with appropriate values.
	updateDevices();

	return TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void Config::OnBnClickedOk()
{
	m_pGrabber->showDevicePage(  );
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void Config::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void Config::OnSelchangeComboDevice()
{
	// Open the device
	if( m_cboDevice.IsWindowEnabled() )
	{
		CString device;

		m_cboDevice.GetWindowText( device );

		if( m_pGrabber->openDev( (LPCTSTR)device ) )
		{
			__int64 iSerial;
			CString serial;

#if 0
			// Try to read out the serial number
			if( m_pGrabber->getDev().getSerialNumber( iSerial ) )
			{
				// Display serial
				serial.Format( TEXT("%I64X"), iSerial );
				m_staticSerialOut.SetWindowText( serial );
			}
			else
			{
				// Display "n\a"
				m_staticSerialOut.SetWindowText( NOT_AVAILABLE );
			}
#endif

			// Update the other controls, since they depend on the current device.
			updateVideoNorms();
			updateInputChannels();
			updateFlip();
		}
	}
}


void Config::OnSelchangeComboFramerate()
{
	if( m_cboFrameRate.IsWindowEnabled() )
	{
		CString framerate;

		m_cboFrameRate.GetWindowText( framerate );

		// Since we retrieve a string from the combo box, but setFPS
		// needs a double value to be passed to, we use atof to typecast
		// the string.
		if( !m_pGrabber->setFPS( atof( framerate.GetBuffer( framerate.GetLength() ) ) ) )
		{
			AfxMessageBox("Framerate Error");
		}
	}
}


void Config::OnSelchangeComboInputchannel()
{
	if( m_cboInputChannel.IsWindowEnabled() )
	{
		CString inputchannel;

		m_cboInputChannel.GetWindowText( inputchannel );

		if( !m_pGrabber->setInputChannel( (LPCTSTR)inputchannel ) )
		{
			AfxMessageBox( TEXT("Input Channel Error") );
		}
	}
}


void Config::OnSelchangeComboVideoformat()
{
	if( m_cboVideoFormat.IsWindowEnabled() )
	{
		CString videoformat;
		m_cboVideoFormat.GetWindowText( videoformat );

		if( !m_pGrabber->setVideoFormat( (LPCTSTR)videoformat ) )
		{
			AfxMessageBox( TEXT("Video Format Error") );
		}
	}

	// Update the frame rate combo box, since the frame rates depend on the 
	// video format
	updateFrameRates();
}


void Config::OnSelchangeComboVideonorm()
{
	if( m_cboVideoNorm.IsWindowEnabled() )
	{
		CString videonorm;

		m_cboVideoNorm.GetWindowText( videonorm );

		if( !m_pGrabber->setVideoNorm( (LPCTSTR)videonorm ) )
		{
			AfxMessageBox( TEXT("Video Norm Error") );
		}
	}

	// Update the video format combo box, since the video formats depend on the 
	// video norm.
	updateVideoFormats();
}





void Config::OnClickedCheckFliph()
{
	if( m_pGrabber->isFlipHAvailable () )
	{
		m_pGrabber->setFlipH( OnCheckFliph.GetCheck() == 1 );
	}
}


void Config::OnClickedCheckFlipv()
{
	if( m_pGrabber->isFlipVAvailable () )
	{
		m_pGrabber->setFlipV( OnCheckFlipv.GetCheck() == 1 );
	}	
}

/////////////////////////////////////////////////////////////////////////////
// Update Procedures


//
// updateDevices
//
// Fills the device combo box with appropriate values and selects 
// the currently valid device.
//
void Config::updateDevices()
{
	// Clear combo box
	m_cboDevice.ResetContent();

	// Retrieve the list of available video capture devices.
	Grabber::tVidCapDevListPtr pVidCapDevList = m_pGrabber->getAvailableVideoCaptureDevices();
	if( pVidCapDevList == 0 || pVidCapDevList->empty() )
	{
		m_cboDevice.AddString( NOT_AVAILABLE );
		m_cboDevice.EnableWindow( false );
		m_cboDevice.SetCurSel( 0 );
		return;
	}

	m_cboDevice.EnableWindow( true );

	// Fill the combo box
	for ( Grabber::tVidCapDevList::iterator it = 
		pVidCapDevList->begin(); 
		it != pVidCapDevList->end(); 
	++it )
	{
		m_cboDevice.AddString( CString( it->c_str() ) );
	}

	// Select the currently valid device
	if( m_pGrabber->isDevValid() )
	{
		m_cboDevice.SelectString( -1, CString( m_pGrabber->getDev().c_str() ) );
	}

	// Update the other controls since they depend on this one.
	updateVideoNorms();
	updateInputChannels();
	updateFlip();
}


//
// updateVideoNorms
//
// Fills the video norm combo box with appropriate values and selects 
// the currently selected video norm.
//
void Config::updateVideoNorms()
{
	// Clear combo box
	m_cboVideoNorm.ResetContent();

	if( m_pGrabber->isVideoNormAvailableWithCurDev() )
	{
		// Retrieve the list of available video norms.
		Grabber::tVidNrmListPtr pVidNrmList = m_pGrabber->getAvailableVideoNorms();
		if ( pVidNrmList == 0 || pVidNrmList->empty() )
		{
			m_cboVideoNorm.AddString( NOT_AVAILABLE );
			m_cboVideoNorm.EnableWindow( false );
			m_cboVideoNorm.SetCurSel( 0 );
			return;
		}

		m_cboVideoNorm.EnableWindow( true );

		// Fill the combo box
		for ( Grabber::tVidNrmList::iterator it = 
			pVidNrmList->begin();
			it != pVidNrmList->end();
		++it )
		{
			m_cboVideoNorm.AddString( CString( it->c_str() ) );
		}

		// Select the currently valid video norm.
		m_cboVideoNorm.SelectString( -1, CString( m_pGrabber->getVideoNorm().c_str() ) );
	}
	else
	{
		m_cboVideoNorm.AddString( NOT_AVAILABLE );
		m_cboVideoNorm.EnableWindow( false );
		m_cboVideoNorm.SetCurSel( 0 );
	}

	// Update the video formats since they depend on the video norm.
	updateVideoFormats();
}


//
// updateVideoFormats
//
// Fills the video format combo box with appropriate values and selects 
// the currently selected video format.
//
void Config::updateVideoFormats()
{
	// Clear combo box
	m_cboVideoFormat.ResetContent();

	if( m_pGrabber->isDevValid() )
	{
		// Retrieve the list of available video formats.
		Grabber::tVidFmtListPtr pVidFmtList = m_pGrabber->getAvailableVideoFormats();
		if ( pVidFmtList == 0 || pVidFmtList->empty() )
		{
			m_cboVideoFormat.AddString( NOT_AVAILABLE );
			m_cboVideoFormat.EnableWindow( false );
			m_cboVideoFormat.SetCurSel( 0 );
		}

		m_cboVideoFormat.EnableWindow( true );

		// Fill the combo box
		for ( Grabber::tVidFmtList::iterator it = 
			pVidFmtList->begin(); 
			it != pVidFmtList->end(); 
		++it )
		{
			m_cboVideoFormat.AddString( CString( it->c_str() ) );
		}


		// Check if there is already a valid video format
		if( !m_pGrabber->getVideoFormat().isValid() )
		{
			m_pGrabber->setVideoFormat( pVidFmtList->at( 0 ) );
		}

		// Select the currently valid video format.
		m_cboVideoFormat.SelectString( -1, CString( m_pGrabber->getVideoFormat().c_str() ) );
	}
	else
	{
		m_cboVideoFormat.AddString( NOT_AVAILABLE );
		m_cboVideoFormat.EnableWindow( false );
		m_cboVideoFormat.SetCurSel( 0 );
	}

	// Update the frame rates since they depend on the video format.
	updateFrameRates();
}


//
// updateInputChannels
//
// Fills the input channel combo box with appropriate values and selects 
// the currently selected input channel.
//
void Config::updateInputChannels()
{
	// Clear combo box
	m_cboInputChannel.ResetContent();

	if( m_pGrabber->isInputChannelAvailableWithCurDev() )
	{
		// Retrieve the list of available input channels.
		Grabber::tInChnListPtr pInChnList = m_pGrabber->getAvailableInputChannels();
		if ( pInChnList == 0 || pInChnList->empty() )
		{
			m_cboInputChannel.AddString( NOT_AVAILABLE );
			m_cboInputChannel.EnableWindow( false );
			m_cboInputChannel.SetCurSel( 0 );
		}

		m_cboInputChannel.EnableWindow( true );

		// Fill the combo box
		for ( Grabber::tInChnList::iterator it = 
			pInChnList->begin(); 
			it != pInChnList->end(); 
		++it )
		{
			m_cboInputChannel.AddString( CString( it->c_str() ) );
		}

		// Select the currently valid input channel.
		m_cboInputChannel.SelectString( -1, CString( m_pGrabber->getInputChannel().c_str() ) );
	}
	else
	{
		m_cboInputChannel.AddString( NOT_AVAILABLE );
		m_cboInputChannel.EnableWindow( false );
		m_cboInputChannel.SetCurSel( 0 );
	}
}


//
// updateFrameRates
//
// Fills the frame rate combo box with appropriate values and selects 
// the currently selected frame rate.
//
void Config::updateFrameRates()
{
	// Clear combo box
	m_cboFrameRate.ResetContent();

	if( m_pGrabber->isFrameRateListAvailable() )
	{
		CString frameRate;

		// Retrieve the list of available frame rates.
		Grabber::tFPSListPtr pFrameRateList = m_pGrabber->getAvailableFPS();
		if ( pFrameRateList == 0 || pFrameRateList->empty() )
		{
			m_cboFrameRate.AddString( NOT_AVAILABLE );
			m_cboFrameRate.EnableWindow( false );
			m_cboFrameRate.SetCurSel( 0 );
		}

		m_cboFrameRate.EnableWindow( true );

		// Fill the combo box
		for ( Grabber::tFPSList::iterator it = 
			pFrameRateList->begin(); 
			it != pFrameRateList->end(); 
		++it )
		{
			frameRate.Format( TEXT("%lf"), *it );			
			m_cboFrameRate.AddString( frameRate );
		}		

		// Select the currently valid frame rate.
		frameRate.Format( TEXT("%lf"), m_pGrabber->getFPS() );
		m_cboFrameRate.SelectString( -1, frameRate );
	}
	else
	{
		m_cboFrameRate.AddString( NOT_AVAILABLE );
		m_cboFrameRate.EnableWindow( false );
		m_cboFrameRate.SetCurSel( 0 );
	}
}


//
// updateFlip
//
// Updates the check boxes for the horizontal/vertical flip.
//
void Config::updateFlip()
{
	// Check horizontal flip.
	if( m_pGrabber->isFlipHAvailable() )
	{
		OnCheckFliph.EnableWindow( true );
		if( m_pGrabber->getFlipH() )
		{
			OnCheckFliph.SetCheck( 1 );
		}
		else
		{
			OnCheckFliph.SetCheck( 0 );
		}
	}
	else
	{
		OnCheckFliph.EnableWindow( false );
		OnCheckFliph.SetCheck( 0 );
	}


	// Check vertical flip.
	if( m_pGrabber->isFlipVAvailable() )
	{
		OnCheckFlipv.EnableWindow( true );
		if( m_pGrabber->getFlipV() )
		{
			OnCheckFlipv.SetCheck( 1 );
		}
		else
		{
			OnCheckFlipv.SetCheck( 0 );
		}
	}
	else
	{
		OnCheckFlipv.EnableWindow( false );
		OnCheckFlipv.SetCheck( 0 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// Helper Procedures

//
// saveDeviceSettings
//
// saves the state of the currently valid device to the 'Settings' data 
// structure.
//
void Config::saveDeviceSettings()
{
	if( m_pGrabber->isDevValid() )
	{
		m_tDeviceState.bDeviceAvail = true;
		m_tDeviceState.szDevice = m_pGrabber->getDev().c_str();

		if( m_pGrabber->isVideoNormAvailableWithCurDev() )
		{
			m_tDeviceState.bVideoNormAvail = true;
			m_tDeviceState.szVideoNorm = m_pGrabber->getVideoNorm().c_str();
		}
		else
		{
			m_tDeviceState.bVideoNormAvail = false;
		}

		m_tDeviceState.szVideoFormat = m_pGrabber->getVideoFormat().c_str();

		if( m_pGrabber->isFrameRateListAvailable() )
		{
			m_tDeviceState.bFrameRateAvail = true;
			m_tDeviceState.lfFrameRate = m_pGrabber->getFPS();
		}
		else
		{
			m_tDeviceState.bFrameRateAvail = false;
		}

		if( m_pGrabber->isInputChannelAvailableWithCurDev() )
		{
			m_tDeviceState.bInputChannelAvail = true;
			m_tDeviceState.szInputChannel = m_pGrabber->getInputChannel().c_str();
		}
		else
		{
			m_tDeviceState.bInputChannelAvail = false;
		}

		if( m_pGrabber->isFlipVAvailable() )
		{
			m_tDeviceState.bFlipVAvail = true;
			m_tDeviceState.bFlipV = m_pGrabber->getFlipV();
		}
		else
		{
			m_tDeviceState.bFlipVAvail = false;
		}

		if( m_pGrabber->isFlipHAvailable() )
		{
			m_tDeviceState.bFlipHAvail = true;
			m_tDeviceState.bFlipH = m_pGrabber->getFlipH();
		}
		else
		{
			m_tDeviceState.bFlipHAvail = false;
		}
	}
	else
	{
		m_tDeviceState.bDeviceAvail = false;
	}
}

//
// restoreDeviceSettings
//
// restores the state of the a device that was previously
// saved to the 'Settings' data structure.
//
void Config::restoreDeviceSettings()
{
	if( m_tDeviceState.bDeviceAvail )
	{
		m_pGrabber->openDev( m_tDeviceState.szDevice );

		if( m_tDeviceState.bVideoNormAvail )
		{
			m_pGrabber->setVideoNorm( m_tDeviceState.szVideoNorm );
		}

		m_pGrabber->setVideoFormat( m_tDeviceState.szVideoFormat );

		if( m_tDeviceState.bFrameRateAvail )
		{
			m_pGrabber->setFPS( m_tDeviceState.lfFrameRate );
		}

		if( m_tDeviceState.bInputChannelAvail )
		{
			m_pGrabber->setInputChannel( m_tDeviceState.szInputChannel );
		}

		if( m_tDeviceState.bFlipVAvail )
		{
			m_pGrabber->setFlipV( m_tDeviceState.bFlipV );
		}

		if( m_tDeviceState.bFlipHAvail )
		{
			m_pGrabber->setFlipH( m_tDeviceState.bFlipH );
		}
	}
	else
	{
		m_pGrabber->closeDev();
	}
}
