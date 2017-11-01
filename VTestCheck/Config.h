#pragma once
#include "tisudshl.h"

// Config 对话框
#define NOT_AVAILABLE	"n\\a"
using namespace _DSHOWLIB_NAMESPACE;


class Config : public CDialogEx
{
	DECLARE_DYNAMIC(Config)

public:
	Config(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Config();

// 对话框数据
	enum { IDD = IDD_D_Config };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	virtual BOOL OnInitDialog();

	
	afx_msg void OnClickedButtonconfigcanmera();
};
