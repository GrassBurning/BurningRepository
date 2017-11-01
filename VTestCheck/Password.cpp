// Password.cpp : 实现文件
//

#include "stdafx.h"
#include "VTestCheck.h"
#include "Password.h"
#include "afxdialogex.h"
#include "Config.h"

// CPassword 对话框
Config config;
IMPLEMENT_DYNAMIC(CPassword, CDialogEx)

CPassword::CPassword(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPassword::IDD, pParent)
{

}

CPassword::~CPassword()
{
}

void CPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPassword, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPassword::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPassword::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPassword 消息处理程序


void CPassword::OnBnClickedOk()
{
	CString strPassWord;

	// TODO: 在此添加控件通知处理程序代码
	GetDlgItemText(IDC_E_Password,strPassWord);
	if (strPassWord.CompareNoCase("sadf") == 0)
	{
		config.DoModal();
		CDialogEx::OnOK();
	}else
	{
		if (strPassWord.CompareNoCase("cd") == 0)
		{
			config.DoModal();
			CDialogEx::OnOK();
		}
		else
		{
			AfxMessageBox("Password Error!");
			SetDlgItemText(IDC_E_Password,"");
			GetDlgItem(IDC_E_Password)->SetFocus();
		}
	}
	
}


void CPassword::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialogEx::OnCancel();
}
