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

private:
	bool m_bSizing;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	afx_msg void OnEnterSizeMove();
	afx_msg void OnExitSizeMove();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

