
// TrafficSwarmDlg.h : header file
//

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
	CSandboxWnd* m_pSandboxWnd;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnChildClosing(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRunsandbox();
};
