
// VTestCheckDlg.h : ͷ�ļ�
//
#include "General.h"
#include "easysize.h"
#pragma once


// CVTestCheckDlg �Ի���
class CVTestCheckDlg : public CDialogEx
{
#ifdef EASYSIZEOPEN	
	DECLARE_EASYSIZE
#endif
// ����
public:
	CVTestCheckDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_VTESTCHECK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
