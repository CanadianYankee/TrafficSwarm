
// TrafficSwarmDlg.h : header file
//

#include "ResultListBox.h"

#pragma once
class CSandboxWnd;

// CTrafficSwarmDlg dialog
class CTrafficSwarmDlg : public CDialogEx
{
// Construction
public:
	CTrafficSwarmDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAFFICSWARM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	std::unordered_set<CWnd *> m_setChildren;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnChildClosing(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRunsandbox();
	afx_msg void OnBnClickedButtonRunTrials();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonDoevolution();
	afx_msg void OnBnClickedButtonLoadresults();
	CResultListBox m_listResults;
	CString m_strSelScores;
	CString m_strSelGenome;
protected:
	afx_msg LRESULT OnUserResultsSelected(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonClearresults();
};
