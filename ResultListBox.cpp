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

std::ostream& operator << (std::ostream& out, const CAgentGenome& g)
{
	for (size_t i = 0; i < (size_t)CAgentGenome::GENE::NUM_GENES; i++)
	{
		out << g.Gene((CAgentGenome::GENE)i) << " ";
	}
	return out;
}

std::istream& operator >> (std::istream& in, CAgentGenome& g)
{
	for (size_t i = 0; i < (size_t)CAgentGenome::GENE::NUM_GENES; i++)
	{
		float f;
		in >> f;
		g.SetGene((CAgentGenome::GENE)i, f);
	}
	return in;
}

std::ostream& operator << (std::ostream& out, const CTrialRun::RUN_RESULTS& rr)
{
	out << rr.genome;
	out << " " << rr.nAgents << " " << rr.nComplete << " " << rr.nSpawnFails << " " << rr.nLeftEscapes << " " << rr.nRightEscapes << " " <<
		rr.fSimulatedTime << " " << rr.fRealTime << " " << rr.fFPS << " " << rr.fAvgLifetime << " " << rr.fAvgAACollisions << " " << rr.fAvgAWCollisions;
	return out;
}

std::istream& operator >> (std::istream& in, CTrialRun::RUN_RESULTS& rr)
{
	in >> rr.genome;
	in >> rr.nAgents >> rr.nComplete >> rr.nSpawnFails >> rr.nLeftEscapes >> rr.nRightEscapes >>
		rr.fSimulatedTime >> rr.fRealTime >> rr.fFPS >> rr.fAvgLifetime >> rr.fAvgAACollisions >> rr.fAvgAWCollisions;

	return in;
}

std::ostream& operator << (std::ostream& out, const CResultListBox& lb)
{
	out << lb.m_vecResults.size() << "\n";
	for (size_t i = 0; i < lb.m_vecResults.size(); i++)
	{
		out << lb.m_vecResults[i];
		out << "\n";
	}
	return out;
}

std::istream& operator >> (std::istream& in, CResultListBox& lb)
{
	lb.m_vecResults.clear();
	size_t n;
	in >> n;
	lb.m_vecResults.reserve(n);
	for (size_t i = 0; i < n; i++)
	{
		in >> lb.m_vecResults[i];
	}

	return in;
}

