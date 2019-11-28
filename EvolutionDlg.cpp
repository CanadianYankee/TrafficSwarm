// EvolutionDlg.cpp : implementation file
//

#include "pch.h"
#include "TrafficSwarm.h"
#include "EvolutionDlg.h"
#include "afxdialogex.h"
#include "Course.h"
#include "AgentGenome.h"
#include "TrialRun.h"

// CEvolutionDlg dialog

UINT RunThreadedTrial(LPVOID pParams)
{
	CEvolutionDlg* pOwner  = ((CEvolutionDlg*)pParams);
	return pOwner->RunThreadedTrial();
}

bool CompareResults(CTrialRun::RUN_RESULTS& r1, CTrialRun::RUN_RESULTS& r2)
{
	return r1.Score() < r2.Score();
}

IMPLEMENT_DYNAMIC(CEvolutionDlg, CDialogEx)

CEvolutionDlg::CEvolutionDlg(CWnd* pParent, std::shared_ptr<CCourse> pCourse)
	: CDialogEx(IDD_DIALOG_EVOLVE, pParent)
	, m_strCourseName(pCourse->m_strName)
	, m_pCourse(pCourse)
	, m_strRunCount(_T(""))
	, m_strGeneration(_T(""))
	, m_eStatus(STATUS::NotRunning)
	, m_strStatus(_T(""))
	, m_nAgents(2048)
	, m_nTrials(100)
	, m_strLastRun(_T(""))
	, m_iCurrentChild(0)
	, m_iCurrentGeneration(0)
{
	
}

CEvolutionDlg::~CEvolutionDlg()
{
}

void CEvolutionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_COURSE, m_strCourseName);
	DDX_Text(pDX, IDC_STATIC_RUNCOUNT, m_strRunCount);
	DDX_Control(pDX, IDC_LIST_RESULTS, m_listResults);
	DDX_Text(pDX, IDC_STATIC_GENERATION, m_strGeneration);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	DDX_Text(pDX, IDC_EDIT_AGENTS, m_nAgents);
	DDV_MinMaxInt(pDX, m_nAgents, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDIT_TRIALS, m_nTrials);
	DDV_MinMaxInt(pDX, m_nTrials, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDIT_LASTRUN, m_strLastRun);
}


BEGIN_MESSAGE_MAP(CEvolutionDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CEvolutionDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_EVOLVE, &CEvolutionDlg::OnBnClickedButtonEvolve)
	ON_MESSAGE(WM_TRIAL_ENDED, &CEvolutionDlg::OnTrialEnded)
	ON_BN_CLICKED(IDC_BUTTON_ENDNOW, &CEvolutionDlg::OnBnClickedButtonEndnow)
END_MESSAGE_MAP()


// CEvolutionDlg message handlers

void CEvolutionDlg::SetStatus(STATUS eStatus)
{
	m_eStatus = eStatus;
	switch (eStatus)
	{
	case STATUS::NotRunning:
		m_strStatus = _T("");
		GetDlgItem(IDC_BUTTON_EVOLVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ENDGEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDNOW)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		break;
	case STATUS::Running:
		m_strStatus = _T("Evolving!");
		GetDlgItem(IDC_BUTTON_EVOLVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDGEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ENDNOW)->EnableWindow(TRUE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		break;
	case STATUS::EndGeneration:
		m_strStatus = _T("Ending after current generation");
		GetDlgItem(IDC_BUTTON_EVOLVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDGEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDNOW)->EnableWindow(TRUE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		break;
	case STATUS::Ending:
		m_strStatus = _T("Ending after current run");
		GetDlgItem(IDC_BUTTON_EVOLVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDGEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDNOW)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		break;
	default:
		ASSERT(FALSE);
	}
	UpdateData(FALSE);
}

void CEvolutionDlg::SeedGenomes(const std::vector<CAgentGenome>& pGenomes)
{
	ASSERT(pGenomes.size() <= m_nTrials);
	m_vecParents.clear();
	m_vecParents = pGenomes;
}

void CEvolutionDlg::SetupGeneration()
{
	size_t iRandStart = 0;
	size_t nParents = m_vecParents.size();
	size_t iChild = 0;
	m_vecChildren.clear();
	m_vecChildren.resize(m_nTrials);

	if (nParents == 1)
	{
		m_vecChildren[0] = m_vecParents[0];
		for (iChild = 1; iChild < m_nTrials / 5; iChild++)
		{
			m_vecChildren[iChild] = m_vecParents[0];
			m_vecChildren[iChild].RandomizeOne();
		}
	} else if (nParents > 1)
	{
		for (iChild = 0; iChild < nParents; iChild++)
		{
			m_vecChildren[iChild] = m_vecParents[iChild];
		}
		for (; iChild < (m_nTrials * 9) / 10; iChild++)
		{
			size_t i = rand() % nParents;
			size_t j = rand() % nParents;
			if (i == j)
			{
				m_vecChildren[iChild] = m_vecParents[i];
				m_vecChildren[iChild].RandomizeOne();
			}
			else
			{
				m_vecChildren[iChild] = CAgentGenome::CrossBreed(m_vecParents[i], m_vecParents[j]);
			}
		}
	}
	for (; iChild < m_nTrials; iChild++)
	{
		m_vecChildren[iChild].RandomizeAll();
	}

	m_iCurrentChild = 0;
	m_iCurrentGeneration++;
	m_vecResults.clear();
	m_vecResults.reserve(m_nTrials);
	m_strRunCount.Format(_T("%d of %d"), 1, m_nTrials);
	m_strGeneration.Format(_T("%d"), m_iCurrentGeneration);
}

BOOL CEvolutionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetStatus(STATUS::NotRunning);

	return TRUE;  
}


void CEvolutionDlg::OnBnClickedCancel()
{
	GetParent()->PostMessage(WM_CHILD_CLOSING);

	CDialogEx::OnCancel();
}


void CEvolutionDlg::OnBnClickedButtonEvolve()
{
	if (UpdateData(TRUE))
	{
		SetupGeneration();
		SetStatus(STATUS::Running);
		AfxBeginThread(::RunThreadedTrial, (LPVOID)(this));
	}
}

// Only called from within the worker run-trial thread
UINT CEvolutionDlg::RunThreadedTrial()
{
	HRESULT hr = S_OK;
	BOOL bResult = TRUE;
	CTrialRun cTrial;

	hr = cTrial.Intialize(m_nAgents, m_pCourse, m_vecChildren[m_iCurrentChild]);
	bResult = SUCCEEDED(hr);
	if (bResult)
	{
		m_vecResults.resize(m_iCurrentChild + 1);

		bResult = cTrial.Run(m_vecResults[m_iCurrentChild]);
	}
	::PostMessage(this->GetSafeHwnd(), WM_TRIAL_ENDED, (WPARAM)bResult, 0);

	return 0;
}

// Post results from last trial and launch next one if we're not done or 
// waiting for a stop. 
afx_msg LRESULT CEvolutionDlg::OnTrialEnded(WPARAM wParam, LPARAM lParam)
{
	if (!wParam)
	{
		ASSERT(FALSE);
		SetStatus(STATUS::NotRunning);
		return 0;
	}

	// Report results
	CTrialRun::RUN_RESULTS& results = m_vecResults[m_iCurrentChild];
	m_strLastRun.Format(_T("Child #%d: %d/%d complete; Score = %.1f\r\nAvg Life = %.1f  Avg AA = %.2f  Avg AW = %.2f\r\nSimulated %.0f seconds (%.0f FPS) in %.1f real seconds."),
		m_iCurrentChild+1, results.nComplete, results.nAgents, results.Score(), results.fAvgLifetime,
		results.fAvgAACollisions, results.fAvgAWCollisions, results.fSimulatedTime, results.fFPS, results.fRealTime);

	// Save full results in list box for breeding/saving/etc.
	m_listResults.AddResult(m_iCurrentChild + 1, results);

	// If we are still going, then launch the next child/trial
	m_iCurrentChild++;
	if (m_eStatus == STATUS::Ending)
	{
		SetStatus(STATUS::NotRunning);
	}
	else if (m_iCurrentChild < (UINT)m_nTrials)
	{
		m_strRunCount.Format(_T("%d of %d"), m_iCurrentChild + 1, m_nTrials);
		SetStatus(STATUS::Running);
		AfxBeginThread(::RunThreadedTrial, (LPVOID)(this));
	}
	else {
		std::vector<CTrialRun::RUN_RESULTS> vecResults = m_listResults.m_vecResults;
		m_listResults.ClearAll();
		std::sort(vecResults.begin(), vecResults.end(), CompareResults);
		int nParents = max(1, vecResults.size() / 10);
		std::vector<CAgentGenome> vecParents(nParents);
		for (int i = 0; i < nParents; i++)
		{
			vecParents[i] = vecResults[i].genome;
		}
		SeedGenomes(vecParents);
		SetupGeneration();
		SetStatus(STATUS::Running);
		AfxBeginThread(::RunThreadedTrial, (LPVOID)(this));
	}

	return 0;
}


void CEvolutionDlg::OnBnClickedButtonEndnow()
{
	SetStatus(STATUS::Ending);
}
