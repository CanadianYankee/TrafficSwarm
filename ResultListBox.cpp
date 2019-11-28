#include "pch.h"
#include "ResultListBox.h"

void CResultListBox::AddResult(UINT iChild, const CTrialRun::RUN_RESULTS& results)
{
	CString str; 
	str.Format(_T("Child #%d: Score = %0.1f"), iChild, results.Score()); 
	m_vecResults.push_back(results);
	int nIndex = AddString(str);
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

//int CResultListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
//{
//	float s1 = m_vecResults[(size_t)(lpCompareItemStruct->itemData1)].Score();
//	float s2 = m_vecResults[(size_t)(lpCompareItemStruct->itemData2)].Score();
//	
//	if (s1 < s2) return -1;
//	if (s2 > s1) return 1;
//
//	return 0;
//}


//void CResultListBox::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
//{
//
//	// TODO:  Add your code to draw the specified item
//}
BEGIN_MESSAGE_MAP(CResultListBox, CListBox)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, &CResultListBox::OnLbnSelchange)
	ON_CONTROL_REFLECT(LBN_SELCANCEL, &CResultListBox::OnLbnSelchange)
END_MESSAGE_MAP()


void CResultListBox::OnLbnSelchange()
{
	int nIndex = GetCurSel();
	auto pResults = nIndex >= 0 ? &(m_vecResults[nIndex]) : nullptr;
	GetParent()->PostMessage(WM_USER_RESULTS_SELECTED, (WPARAM)pResults);
}
