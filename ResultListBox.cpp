#include "pch.h"
#include "ResultListBox.h"

void CResultListBox::AddResult(UINT iChild, const CTrialRun::RUN_RESULTS& results)
{
	CString str; 
	float fScore = results.Score();
	str.Format(_T("Child #%d: Score = %0.1f"), iChild, fScore); 
	m_vecResults.push_back(results);
	int nIndex = -1;
	if (m_bAutoSort)
	{
		for (nIndex = 0; nIndex < GetCount(); nIndex++)
		{
			float fOther = m_vecResults[GetItemData(nIndex)].Score();
			if (fScore <= fOther)
				break;
		}
		InsertString(nIndex, str);
	}
	else
	{
		nIndex = AddString(str);
	}
	SetItemData(nIndex, (DWORD_PTR)(m_vecResults.size() - 1));
}

void CResultListBox::ClearAll()
{
	m_vecResults.clear();
	while (GetCount())
	{
		DeleteString(0);
	}
}

BEGIN_MESSAGE_MAP(CResultListBox, CListBox)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, &CResultListBox::OnLbnSelchange)
	ON_CONTROL_REFLECT(LBN_SELCANCEL, &CResultListBox::OnLbnSelchange)
END_MESSAGE_MAP()


void CResultListBox::OnLbnSelchange()
{
	int nIndex = GetCurSel();
	auto pResults = nIndex >= 0 ? &(m_vecResults[GetItemData(nIndex)]) : nullptr;
	GetParent()->PostMessage(WM_USER_RESULTS_SELECTED, (WPARAM)pResults);
}

