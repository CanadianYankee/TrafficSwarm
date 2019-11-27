#pragma once

#include "resource.h"
#include "TrialRun.h"

class CCourse;
class CAgentGenome;

// CEvolutionDlg dialog

constexpr auto GENERATION_SIZE = 100;

class CEvolutionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEvolutionDlg)

public:
	CEvolutionDlg(CWnd* pParent, CCourse *pCourse);  
	virtual ~CEvolutionDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EVOLVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCourseName;
	CString m_strRunCount;
	CListBox m_listResults;
	CString m_strGeneration;

	void SeedGenomes(const std::vector<CAgentGenome>* pGenomes);

protected:
	void CreateGeneration();

	std::vector<CAgentGenome> m_vecParents;
	std::vector<CAgentGenome> m_vecChildren;
	std::vector<CTrialRun::RUN_RESULTS> m_vecResults;
};
