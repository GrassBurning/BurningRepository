
// VTestCheckDlg.h : 头文件
//
#include "General.h"
#include "easysize.h"
#pragma once


// CVTestCheckDlg 对话框
class CVTestCheckDlg : public CDialogEx
{
#ifdef EASYSIZEOPEN	
	DECLARE_EASYSIZE
#endif
// 构造
public:
	CVTestCheckDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_VTESTCHECK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	CStatic m_Picture_PF;
	afx_msg void OnBnClickedBSetposition();
	CListBox m_ListBox;
	afx_msg void OnBnClickedConfig();
	CStatic m_cStaticVideoWindow;

	void ShowError();
	void ShowPass();
};
