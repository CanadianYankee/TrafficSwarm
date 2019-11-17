#pragma once
#include <afxwin.h>
#include "DXSandbox.h"

class CSandboxWnd :
	public CWnd
{
public:
	CSandboxWnd(CWnd* pOwner);
	BOOL Create();

protected:
	CWnd* m_pOwner;
	CDXSandbox* m_pSandbox;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg void OnClose();
};

