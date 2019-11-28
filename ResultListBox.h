#pragma once
#include <afxwin.h>

#include "TrialRun.h"

class CResultListBox :
	public CListBox
{
public:
	void AddResult(UINT iChild, const CTrialRun::RUN_RESULTS& results);
	void ClearAll();

	std::vector<CTrialRun::RUN_RESULTS> m_vecResults;
public:
//	virtual int CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/);
//	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};

