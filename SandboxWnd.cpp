#include "pch.h"
#include "SandboxWnd.h"

CString g_szWndClass;

CSandboxWnd::CSandboxWnd(CWnd* pOwner) :
	m_pOwner(pOwner), 
	m_pSandbox(nullptr)
{
	ASSERT(m_pOwner);
}

BOOL CSandboxWnd::Create()
{
	if (g_szWndClass.GetLength() == 0)
	{
		g_szWndClass = AfxRegisterWndClass(NULL);
	}

	BOOL bSuccess = CWnd::CreateEx(WS_EX_OVERLAPPEDWINDOW, g_szWndClass, _T("Traffic Sandbox"), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		0, 0);

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CSandboxWnd, CWnd)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


void CSandboxWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	BOOL bSuccess = TRUE;

	if (bShow)
	{
		if (!m_pSandbox)
		{
			m_pSandbox = new CDXSandbox();
			bSuccess = m_pSandbox->Initialize(GetSafeHwnd());
			if (!bSuccess)
			{
				ASSERT(FALSE);
				PostMessage(WM_CLOSE);
			}
		}
		else
		{
			m_pSandbox->Resume();
		}
	}
	else
	{
		if (m_pSandbox) m_pSandbox->Pause();
	}
}

void CSandboxWnd::OnPaint()
{
	CPaintDC dc(this); 

	// Defer drawing to the DirectX controller; then invalidate so that paint is called as 
	// often as possible.
	m_pSandbox->Tick();
	this->Invalidate(FALSE);
}


void CSandboxWnd::OnClose()
{
	CWnd::OnClose();
	if (m_pOwner)
		m_pOwner->PostMessage(WM_CHILD_CLOSING, (WPARAM)this);
}
