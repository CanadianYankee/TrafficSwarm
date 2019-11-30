#pragma once
#include <afxwin.h>
#include "Course.h"


class CCourseDraw : public CStatic
{
	DECLARE_DYNAMIC(CCourseDraw);

public:
	CCourseDraw();
	~CCourseDraw();
	void SetCourse(CCourse *pCourse);

protected:
	BOOL RegisterWindowClass(void);
	DECLARE_MESSAGE_MAP()

	CCourse *m_pCourse;

	static void DrawPolyline(CPaintDC &dc, const CCourse::XMPOLYLINE& polyline, float fScale, POINT offset);

public:
	afx_msg void OnPaint();
};

