// Password.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VTestCheck.h"
#include "Password.h"
#include "afxdialogex.h"
#include "Config.h"

// CPassword �Ի���
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


// CPassword ��Ϣ�������


void CPassword::OnBnClickedOk()
{
	CString strPassWord;

	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CDialogEx::OnCancel();
}
