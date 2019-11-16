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

	// Default rect will allow a margin of 1/8 of the screen on all four sides.
//	int w = ::GetSystemMetrics(SM_CXFULLSCREEN);
//	int h = ::GetSystemMetrics(SM_CYFULLSCREEN);
//	int cx = w >> 1;
//	int cy = h >> 1;
//	w = (w >> 2) * 3; h = (h >> 2) * 3;
//	RECT rcCreate;
//	rcCreate.left = cx - (w >> 1);
//	rcCreate.right = cx + (w >> 1);
//	rcCreate.top = cy - (h >> 1);
//	rcCreate.bottom = cy + (h >> 1);

	BOOL bSuccess = CWnd::CreateEx(WS_EX_OVERLAPPEDWINDOW, g_szWndClass, _T("Traffic Sandbox"), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		0, 0);

	return bSuccess;
}


BEGIN_MESSAGE_MAP(CSandboxWnd, CWnd)
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CSandboxWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	BOOL bSuccess = TRUE;

/*	if (bShow)
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
	}*/
}

/*
void CSandboxWnd::OnPaint()
{
	CPaintDC dc(this); 

	// Defer drawing to the DirectX controller; then invalidate so that paint is called as 
	// often as possible.
	m_pSandbox->Tick();
	this->Invalidate(FALSE);
}
*/