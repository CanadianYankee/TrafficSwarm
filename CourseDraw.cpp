#include "pch.h"
#include "CourseDraw.h"

IMPLEMENT_DYNAMIC(CCourseDraw, CStatic)

CCourseDraw::CCourseDraw()
{
	RegisterWindowClass();
}

CCourseDraw::~CCourseDraw()
{

}

BOOL CCourseDraw::RegisterWindowClass(void)
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, _T("CCourseDraw"), &wndcls)))
	{
		// otherwise we need to register a new class

		wndcls.style = CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;
		wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground = NULL; // (HBRUSH)(COLOR_3DFACE + 1);
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = L"CCourseDraw";

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

void CCourseDraw::SetCourse(std::shared_ptr<CCourse> &pCourse)
{
	m_pCourse = pCourse;
	Invalidate(FALSE);
}

BEGIN_MESSAGE_MAP(CCourseDraw, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CCourseDraw::DrawPolyline(CPaintDC &dc, const CCourse::XMPOLYLINE& polyline, float fScale, POINT offset)
{
	std::vector<POINT> pts(polyline.size());
	for (size_t i = 0; i < polyline.size(); i++)
	{
		pts[i].x = (LONG)(polyline[i].x * fScale + 0.5f) + offset.x;
		pts[i].y = (LONG)(polyline[i].y * fScale + 0.5f) + offset.y;
	}
	dc.Polyline(&pts.front(), (int)pts.size());
}

void CCourseDraw::OnPaint()
{
	CPaintDC dc(this); 

	CRect rect;
	CBrush brBlack(RGB(0,0,0));
	dc.GetWindow()->GetClientRect(&rect);
	dc.FillRect(rect, &brBlack);
	brBlack.DeleteObject();

	if (m_pCourse)
	{
		float fScale = rect.Width() / (m_pCourse->m_fCourseLength + 2);
		int penWidth = (int)(fScale) + 1;
		CPoint offset(penWidth-1, rect.Height() / 2);
		CPen penWall(PS_SOLID, penWidth, RGB(0xbf, 0xbf, 0xbf));
		CPen *oldPen = dc.SelectObject(&penWall);
		for (size_t i = 0; i < m_pCourse->m_vecWalls.size(); i++)
		{
			DrawPolyline(dc, m_pCourse->m_vecWalls[i], fScale, offset);
		}
		for (size_t i = 0; i < m_pCourse->m_vecAgentSS.size(); i++)
		{
			XMFLOAT3 fclr = m_pCourse->m_vecAgentSS[i].vColor;
			CPen penAgent(PS_SOLID, penWidth, RGB((int)(255 * fclr.x), (int)(255 * fclr.y), (int)(255 * fclr.z)));
			dc.SelectObject(penAgent);
			DrawPolyline(dc, m_pCourse->m_vecAgentSS[i].lineSource, fScale, offset);
			DrawPolyline(dc, m_pCourse->m_vecAgentSS[i].lineSink, fScale, offset);
		}
		dc.SelectObject(oldPen);
	}
}
