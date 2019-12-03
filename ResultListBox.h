#pragma once
#include <afxwin.h>

#include "TrialRun.h"

class CResultListBox :
	public CListBox
{
public:
	void AddResult(UINT iChild, const CTrialRun::RUN_RESULTS& results);
	void ClearAll();
	CTrialRun::RUN_RESULTS* GetCurResults();
	friend std::ostream& operator << (std::ostream& out, const CResultListBox& lb);
	friend std::ostream& operator >> (std::ostream& in, CResultListBox& lb);

	std::vector<CTrialRun::RUN_RESULTS> m_vecResults;
	BOOL m_bAutoSort = FALSE;
	const UINT m_iVersion = 2;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLbnSelchange();
};

std::ostream& operator << (std::ostream& out, const CResultListBox& lb);
std::istream& operator >> (std::istream& in, CResultListBox& lb);
