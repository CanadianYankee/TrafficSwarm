#pragma once
#include <afxwin.h>
#include "DXSandbox.h"
#include "AgentGenome.h"

class CCourse;

class CSandboxWnd :
	public CWnd
{
public:
	CSandboxWnd(CWnd* pOwner, std::shared_ptr<CCourse> pCourse, const CAgentGenome& cGenome);
	BOOL Create();

protected:
	CWnd* m_pOwner;
	std::shared_ptr<CDXSandbox> m_pSandbox;
	std::shared_ptr<CCourse>  m_pCourse;
	CAgentGenome m_cGenome;

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

