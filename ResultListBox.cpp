#include "pch.h"
#include "ResultListBox.h"
#include "RunResultsOld.h"

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
	GetParent()->PostMessage(WM_USER_RESULTS_SELECTED, (WPARAM)GetCurResults());
}

CTrialRun::RUN_RESULTS* CResultListBox::GetCurResults()
{
	int nIndex = GetCurSel();
	return nIndex >= 0 ? &(m_vecResults[GetItemData(nIndex)]) : nullptr;
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
	out << lb.m_vecResults.size() << " " << lb.m_iVersion << "\n";
	for (size_t i = 0; i < lb.m_vecResults.size(); i++)
	{
		out << lb.m_vecResults[i];
		out << "\n";
	}
	return out;
}

std::istream& operator >> (std::istream& in, CResultListBox& lb)
{
	lb.ClearAll();
	size_t n;
	in >> n;
	UINT version; 
	in >> version;

	switch (version)
	{
	case 2:
		for (size_t i = 0; i < n; i++)
		{
			CTrialRun::RUN_RESULTS result;
			in >> result;
			lb.AddResult((UINT)i + 1, result);
		}
		break;

	case 1:
		for (size_t i = 0; i < n; i++)
		{
			RUN_RESULTS_1 result1;
			in >> result1;
			CTrialRun::RUN_RESULTS result;
			ConvertRunResult(result, result1);
			lb.AddResult((UINT)i + 1, result);
		}
		break;

	default:
		break;
	}

	return in;
}

