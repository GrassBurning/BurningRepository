#pragma once
#include "tisudshl.h"

// Config �Ի���
#define NOT_AVAILABLE	"n\\a"
using namespace _DSHOWLIB_NAMESPACE;


class Config : public CDialogEx
{
	DECLARE_DYNAMIC(Config)

public:
	Config(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~Config();

// �Ի�������
	enum { IDD = IDD_D_Config };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	virtual BOOL OnInitDialog();

	
	afx_msg void OnClickedButtonconfigcanmera();
};
