
// VTestCheck.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CVTestCheckApp:
// �йش����ʵ�֣������ VTestCheck.cpp
//

class CVTestCheckApp : public CWinApp
{
public:
	CVTestCheckApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCancel();
};

extern CVTestCheckApp theApp;