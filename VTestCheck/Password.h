#pragma once


// CPassword �Ի���

class CPassword : public CDialogEx
{
	DECLARE_DYNAMIC(CPassword)

public:
	CPassword(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPassword();

// �Ի�������
	enum { IDD = IDD_D_Password };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
