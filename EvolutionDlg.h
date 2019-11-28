#pragma once

#include "resource.h"
#include "TrialRun.h"
#include "ResultListBox.h"

class CCourse;
class CAgentGenome;

// CEvolutionDlg dialog

class CEvolutionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEvolutionDlg)

public:
	CEvolutionDlg(CWnd* pParent, std::shared_ptr<CCourse> pCourse);  
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
	CResultListBox m_listResults;
	CString m_strGeneration;

	void SeedGenomes(const std::vector<CAgentGenome>& pGenomes);
	UINT RunThreadedTrial();
	BOOL Complete() { return m_iCurrentGeneration >= (UINT)m_nGenerations && m_iCurrentChild >= (UINT)m_nTrials; }

protected:
	enum class STATUS 
	{
		NotRunning,
		Running,
		Ending,
		EndGeneration
	};
	void SetupGeneration();
	void SetStatus(STATUS eStatus);

	STATUS m_eStatus;
	std::shared_ptr<CCourse> m_pCourse;
	std::vector<CAgentGenome> m_vecParents;
	std::vector<CAgentGenome> m_vecChildren;
	std::vector<CTrialRun::RUN_RESULTS> m_vecResults;

	UINT m_iCurrentChild;
	UINT m_iCurrentGeneration;

public:
	CString m_strStatus;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonEvolve();
	int m_nAgents;
	int m_nTrials;
	CString m_strLastRun;
protected:
	afx_msg LRESULT OnTrialEnded(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonEndnow();
protected:
	afx_msg LRESULT OnUserResultsSelected(WPARAM wParam, LPARAM lParam);
public:
	CString m_strSelGenome;
	CString m_strSelScores;
	int m_nGenerations;
};
