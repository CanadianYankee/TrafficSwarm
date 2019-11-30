#include "pch.h"
#include "SandboxWnd.h"
#include "Course.h"

CString g_szWndClass;

CSandboxWnd::CSandboxWnd(CWnd* pOwner, const CCourse& cCourse, const CAgentGenome& cGenome) :
	m_pOwner(pOwner),
	m_bSizing(false),
	m_cCourse(cCourse)
{
	m_cGenome = cGenome;
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
ON_WM_ENTERSIZEMOVE()
ON_WM_EXITSIZEMOVE()
ON_WM_ERASEBKGND()
ON_WM_SIZE()
END_MESSAGE_MAP()


void CSandboxWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	BOOL bSuccess = TRUE;

	if (bShow)
	{
		if (!m_pSandbox)
		{
			m_pSandbox = std::make_shared<CDXSandbox>();
			bSuccess = m_pSandbox->Initialize((CWnd *)this, m_cCourse, m_cGenome);
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

BOOL CSandboxWnd::OnEraseBkgnd(CDC* pDC)
{
	// Swallow this event since we're letting DirectX do all the drawing. 
	return TRUE;
}

void CSandboxWnd::OnClose()
{
	CWnd::OnClose();
	if (m_pOwner)
		m_pOwner->PostMessage(WM_USER_CHILD_CLOSING, (WPARAM)this);
}

void CSandboxWnd::OnEnterSizeMove()
{
	// Pause the action while the user is adjusting the window
	m_pSandbox->Pause();
	m_bSizing = true;

	CWnd::OnEnterSizeMove();
}

void CSandboxWnd::OnExitSizeMove()
{
	// Resume the action and pass the new size on to the sandbox
	m_bSizing = false;
	RECT rcClient;
	GetClientRect(&rcClient);
	m_pSandbox->Resume(&rcClient);

	CWnd::OnExitSizeMove();
}

void CSandboxWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	switch (nType)
	{
	case SIZE_MINIMIZED:
		m_pSandbox->Pause();
		break;

	default:
		if (!m_bSizing)
		{
			RECT rcClient;
			GetClientRect(&rcClient);
			m_pSandbox->Resume(&rcClient);
		}
		break;
	}
}
