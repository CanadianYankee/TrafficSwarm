// EvolutionDlg.cpp : implementation file
//

#include "pch.h"
#include "TrafficSwarm.h"
#include "EvolutionDlg.h"
#include "afxdialogex.h"
#include "Course.h"
#include "AgentGenome.h"
#include "TrialRun.h"

std::ostream& operator << (std::ostream& out, const CResultListBox& lb);
std::istream& operator >> (std::istream& in, CResultListBox& lb);

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

CEvolutionDlg::CEvolutionDlg(CWnd* pParent, const CCourse& cCourse)
	: CDialogEx(IDD_DIALOG_EVOLVE, pParent)
	, m_strCourseName(cCourse.m_strName)
	, m_cCourse(cCourse)
	, m_strRunCount(_T(""))
	, m_strGeneration(_T(""))
	, m_eStatus(STATUS::NotRunning)
	, m_strStatus(_T(""))
	, m_nAgents(2048)
	, m_nChildren(100)
	, m_strLastRun(_T(""))
	, m_iCurrentChild(0)
	, m_iCurrentGeneration(0)
	, m_strSelGenome(_T(""))
	, m_strSelScores(_T(""))
	, m_nGenerations(10)
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
	DDX_Text(pDX, IDC_EDIT_TRIALS, m_nChildren);
	DDV_MinMaxInt(pDX, m_nChildren, 0, INT_MAX);
	DDX_Text(pDX, IDC_EDIT_LASTRUN, m_strLastRun);
	DDX_Text(pDX, IDC_EDIT_SELGENOME, m_strSelGenome);
	DDX_Text(pDX, IDC_EDIT_SELSCORES, m_strSelScores);
	DDX_Text(pDX, IDC_EDIT_GENERATIONS, m_nGenerations);
}


BEGIN_MESSAGE_MAP(CEvolutionDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CEvolutionDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_EVOLVE, &CEvolutionDlg::OnBnClickedButtonEvolve)
	ON_MESSAGE(WM_USER_TRIAL_ENDED, &CEvolutionDlg::OnTrialEnded)
	ON_BN_CLICKED(IDC_BUTTON_ENDNOW, &CEvolutionDlg::OnBnClickedButtonEndnow)
	ON_MESSAGE(WM_USER_RESULTS_SELECTED, &CEvolutionDlg::OnUserResultsSelected)
	ON_BN_CLICKED(IDC_BUTTON_SAVERESULTS, &CEvolutionDlg::OnBnClickedButtonSaveresults)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CEvolutionDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_LOADTWO, &CEvolutionDlg::OnBnClickedButtonLoadtwo)
	ON_BN_CLICKED(IDC_BUTTON_ENDGEN, &CEvolutionDlg::OnBnClickedButtonEndgen)
END_MESSAGE_MAP()


// CEvolutionDlg message handlers

void CEvolutionDlg::SetStatus(STATUS eStatus)
{
	m_eStatus = eStatus;
	switch (eStatus)
	{
	case STATUS::NotRunning:
		m_strStatus = _T("Stopped");
		GetDlgItem(IDC_BUTTON_EVOLVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ENDGEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDNOW)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SAVERESULTS)->EnableWindow(m_listResults.m_vecResults.size() > 0);
		GetDlgItem(IDC_BUTTON_LOADRESULTS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LOADTWO)->EnableWindow(m_listResults.m_vecResults.size() > 0);
		GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
		break;
	case STATUS::Running:
		m_strStatus.Format(_T("Evolving - generation with %d parents"), (int)m_vecParents.size());
		GetDlgItem(IDC_BUTTON_EVOLVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDGEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ENDNOW)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SAVERESULTS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOADRESULTS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOADTWO)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		break;
	case STATUS::EndGeneration:
		m_strStatus = _T("Stopping after current generation");
		GetDlgItem(IDC_BUTTON_EVOLVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDGEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDNOW)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SAVERESULTS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOADRESULTS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOADTWO)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		break;
	case STATUS::Ending:
		m_strStatus = _T("Stopping after current run");
		GetDlgItem(IDC_BUTTON_EVOLVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDGEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ENDNOW)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SAVERESULTS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOADRESULTS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOADTWO)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		break;
	default:
		ASSERT(FALSE);
	}
	UpdateData(FALSE);
}

// Seed the next generation with a vector of parent genomes
void CEvolutionDlg::SeedGenomes(const std::vector<CAgentGenome>& vecGenomes)
{
	ASSERT(vecGenomes.size() <= m_nChildren);
	m_vecParents.clear();
	m_vecParents = vecGenomes;
}

// After a completed generation, sort by ascending score and take the 10% 
// lowest-scoring children as the parents of the next generation. The remaining
// 90% will be discarded.
void CEvolutionDlg::PullParents(const std::vector<CTrialRun::RUN_RESULTS> &vecResults)
{
	std::vector<CTrialRun::RUN_RESULTS> vec(vecResults);
	std::sort(vec.begin(), vec.end(), CompareResults);
	int nParents = min(max(1, m_nChildren / 10), (int)vec.size());
	std::vector<CAgentGenome> vecParents(nParents);
	for (int i = 0; i < nParents; i++)
	{
		vecParents[i] = vec[i].genome;
	}
	SeedGenomes(vecParents);
}

// Given two trial runs, pull 10% of the lowest-scoring from each to be 
// parents of a new generation (that is, cross-breed two runs). 
void CEvolutionDlg::PullParents(const std::vector<CTrialRun::RUN_RESULTS>& vecRuns1, const std::vector<CTrialRun::RUN_RESULTS>& vecRuns2)
{
	std::vector<CTrialRun::RUN_RESULTS> vec1(vecRuns1), vec2(vecRuns2);
	std::sort(vec1.begin(), vec1.end(), CompareResults);
	std::sort(vec2.begin(), vec2.end(), CompareResults);
	int nParents1 = min(max(1, m_nChildren / 10), (int)vec1.size());
	int nParents2 = min(max(1, m_nChildren / 10), (int)vec2.size());
	int nParents = min(nParents1, nParents2);
	std::vector<CAgentGenome> vecParents(nParents);
	for (int i = 0; i < nParents; i++)
	{
		vecParents[i] = CAgentGenome::CrossBreed(vec1[rand() % nParents1].genome, vec2[rand() % nParents2].genome);
	}
	SeedGenomes(vecParents);
}

// For each generation, the chosen parents survive intact. Then children 
// are bred to fill up 90% of the population and the final 10% are completely 
// random. 
//
// Breeding process:
// 1. Choose two parents at random, indexed as i,j.
// 2. If i == j, then copy parent(i) and mutate one gene. A second gene gets mutated to a 
//    random value with probability 0.2, a third with probability (0.2)(0.2) = 0.04, etc.
// 3. If i != j, then randomly select each gene from either parent. One gene gets mutated 
//    to a random value with probability 0.2, a second with probability (0.2)(0.2) = 0.04, etc.
void CEvolutionDlg::SetupGeneration()
{
	size_t iRandStart = 0;
	size_t nParents = m_vecParents.size();
	size_t iChild = 0;
	m_vecChildren.clear();
	m_vecChildren.resize(m_nChildren);

	if (nParents == 1)
	{
		m_vecChildren[0] = m_vecParents[0];
		for (iChild = 1; iChild < m_nChildren / 5; iChild++)
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
		for (; iChild < (m_nChildren * 9) / 10; iChild++)
		{
			bool bRepeat = false;
			do
			{
				bRepeat = false;
				size_t i = rand() % nParents;
				size_t j = rand() % nParents;
				if (i == j)
				{
					m_vecChildren[iChild] = m_vecParents[i];
					do
					{
						m_vecChildren[iChild].RandomizeOne();
					} while (frand() < 0.2);
				}
				else
				{
					m_vecChildren[iChild] = CAgentGenome::CrossBreed(m_vecParents[i], m_vecParents[j]);
				}
				for (size_t n = 0; n < iChild; n++)
				{
					// Don't create identical children - preserve genetic diversity
					if (m_vecChildren[n] == m_vecChildren[iChild])
					{
						bRepeat = true;
						break;
					}
				}
			} while (bRepeat);
		}
	}
	for (; iChild < m_nChildren; iChild++)
	{
		m_vecChildren[iChild].RandomizeAll();
	}

	m_iCurrentChild = 0;
	m_iCurrentGeneration++;
	m_vecResults.clear();
	m_vecResults.reserve(m_nChildren);
	m_listResults.ClearAll();
	m_strLastRun.Empty();
	m_strSelGenome.Empty();
	m_strSelScores.Empty();
	m_strRunCount.Format(_T("%d of %d"), 1, m_nChildren);
	m_strGeneration.Format(_T("%d of %d"), m_iCurrentGeneration, m_nGenerations);
}

BOOL CEvolutionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetStatus(STATUS::NotRunning);
	m_listResults.m_bAutoSort = TRUE;

	return TRUE;  
}


void CEvolutionDlg::OnBnClickedCancel()
{
	GetParent()->PostMessage(WM_USER_CHILD_CLOSING);

	CDialogEx::OnCancel();
}


void CEvolutionDlg::OnBnClickedButtonEvolve()
{
	if (UpdateData(TRUE))
	{
		bool bSetup = true;
		if (m_iCurrentGeneration > 0 && !Complete())
		{
			switch (MessageBox(_T("Would you like to continue the current evolution?\nChoosing 'No' will restart with a new random population."), _T("Evolve"), MB_YESNOCANCEL))
			{
			case IDYES:
				bSetup = false;
				break;
			case IDNO:
				m_vecParents.clear();
				m_iCurrentGeneration = 0;
				break;
			case IDCANCEL:
				return;
			}
		}
		if (bSetup) SetupGeneration();
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

	hr = cTrial.Intialize(m_nAgents, m_cCourse, m_vecChildren[m_iCurrentChild]);
	bResult = SUCCEEDED(hr);
	if (bResult)
	{
		m_vecResults.resize((size_t)m_iCurrentChild + 1);

		bResult = cTrial.Run(m_vecResults[m_iCurrentChild]);
	}
	::PostMessage(this->GetSafeHwnd(), WM_USER_TRIAL_ENDED, (WPARAM)bResult, 0);

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
	m_strLastRun.Format(_T("Child #%d: "), m_iCurrentChild + 1);
	m_strLastRun += results.ToParagraphString(_T("\r\n"));

	// Save full results in list box for breeding/saving/etc.
	m_listResults.AddResult(m_iCurrentChild + 1, results);

	// If we are still going, then launch the next child/trial
	m_iCurrentChild++;
	if (m_eStatus == STATUS::Ending)
	{
		SetStatus(STATUS::NotRunning);
	}
	else if (m_iCurrentChild < (UINT)m_nChildren)
	{
		m_strRunCount.Format(_T("%d of %d"), m_iCurrentChild + 1, m_nChildren);
		SetStatus(m_eStatus == STATUS::EndGeneration ? STATUS::EndGeneration : STATUS::Running);
		AfxBeginThread(::RunThreadedTrial, (LPVOID)(this));
	}
	else if (Complete())
	{
		SetStatus(STATUS::NotRunning);
		// TODO: save results
	}
	else
	{
		PullParents(m_listResults.m_vecResults);
		m_listResults.ClearAll();
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


afx_msg LRESULT CEvolutionDlg::OnUserResultsSelected(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		auto pResults = (CTrialRun::RUN_RESULTS*)wParam;
		m_strSelGenome = pResults->genome.ToString(_T("\r\n"));
		m_strSelScores = pResults->ToListString(_T("\r\n"));
	}
	else
	{
		m_strSelGenome.Empty();
		m_strSelScores.Empty();
	}
	UpdateData(FALSE);
	return 0;
}


void CEvolutionDlg::OnBnClickedButtonSaveresults()
{
	CTime time = CTime::GetCurrentTime();
	CString strName = m_strCourseName + time.Format(_T("_%F_%H-%M-%S.txt"));

	CFileDialog dlgSaveAs(FALSE, _T(".txt"), strName.GetBuffer());

	if (dlgSaveAs.DoModal() == IDOK)
	{
		strName = dlgSaveAs.GetPathName();

		std::ofstream file;
		file.open(strName.GetBuffer(), std::ios_base::out | std::ios_base::trunc);
		file << m_listResults;
		file.close();
	}
}


void CEvolutionDlg::OnBnClickedButtonLoad()
{
	if (UpdateData(TRUE))
	{
		CFileDialog dlgFile(TRUE, _T(".txt"), 0, 0, _T("Text files (*.txt)|*.txt|All Files (*.*)|*.*||"));
		if (dlgFile.DoModal() == IDOK)
		{
			CString strFile = dlgFile.GetPathName();
			std::ifstream file;
			file.open(strFile.GetBuffer(), std::ios_base::in);
			file >> m_listResults;
			file.close();

			PullParents(m_listResults.m_vecResults);
			m_iCurrentGeneration = 0;
			m_iCurrentChild = 0;
		}
	}
}


void CEvolutionDlg::OnBnClickedButtonLoadtwo()
{
	if (UpdateData(TRUE))
	{
		if (m_listResults.m_vecResults.empty())
		{
			MessageBox(_T("There must be results to cross-breed with.  Either do an evolution run or load some previous results to cross-breed."),
				_T("Cross-breed error."), MB_OK);
			return;
		}
		CFileDialog dlgFile(TRUE, _T(".txt"), 0, 0, _T("Text files (*.txt)|*.txt|All Files (*.*)|*.*||"));
		if (dlgFile.DoModal() == IDOK)
		{
			std::vector<CTrialRun::RUN_RESULTS> vecOld = m_listResults.m_vecResults;
			m_listResults.ClearAll();
			CString strFile = dlgFile.GetPathName();
			std::ifstream file;
			file.open(strFile.GetBuffer(), std::ios_base::in);
			file >> m_listResults;
			file.close();

			PullParents(vecOld, m_listResults.m_vecResults);
			m_listResults.ClearAll();
			m_iCurrentGeneration = 0;
			m_iCurrentChild = 0;
			UpdateData();
			OnBnClickedButtonEvolve();
		}
	}
}


void CEvolutionDlg::OnBnClickedButtonEndgen()
{
	SetStatus(STATUS::EndGeneration);
}
