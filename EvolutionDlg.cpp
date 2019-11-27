// EvolutionDlg.cpp : implementation file
//

#include "pch.h"
#include "TrafficSwarm.h"
#include "EvolutionDlg.h"
#include "afxdialogex.h"
#include "Course.h"
#include "AgentGenome.h"

// CEvolutionDlg dialog

IMPLEMENT_DYNAMIC(CEvolutionDlg, CDialogEx)

CEvolutionDlg::CEvolutionDlg(CWnd* pParent, CCourse *pCourse)
	: CDialogEx(IDD_DIALOG_EVOLVE, pParent)
	, m_strCourseName(pCourse->m_strName)
	, m_strRunCount(_T(""))
	, m_strGeneration(_T(""))
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
}


BEGIN_MESSAGE_MAP(CEvolutionDlg, CDialogEx)
END_MESSAGE_MAP()


// CEvolutionDlg message handlers

void CEvolutionDlg::SeedGenomes(const std::vector<CAgentGenome>* pGenomes)
{
	ASSERT(pGenomes->size() <= GENERATION_SIZE);
	m_vecParents.clear();
	m_vecParents = *pGenomes;
}

void CEvolutionDlg::CreateGeneration()
{
	size_t iRandStart = 0;
	size_t nParents = m_vecParents.size();
	size_t iChild = 0;
	m_vecChildren.clear();
	m_vecChildren.resize(GENERATION_SIZE);

	if (nParents == 1)
	{
		m_vecChildren[0] = m_vecParents[0];
		for (iChild = 1; iChild < GENERATION_SIZE / 5; iChild++)
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
		for (; iChild < (GENERATION_SIZE * 9) / 10; iChild)
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
	for (; iChild < GENERATION_SIZE; iChild++)
	{
		m_vecChildren[iChild].RandomizeAll();
	}
}