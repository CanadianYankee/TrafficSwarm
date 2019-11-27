
// TrafficSwarmDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TrafficSwarm.h"
#include "TrafficSwarmDlg.h"
#include "afxdialogex.h"
#include "SandboxWnd.h"
#include "Course.h"
#include "TrialRun.h"
#include "RunStatistics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTrafficSwarmDlg dialog



CTrafficSwarmDlg::CTrafficSwarmDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TRAFFICSWARM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrafficSwarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTrafficSwarmDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RUNSANDBOX, &CTrafficSwarmDlg::OnBnClickedButtonRunsandbox)
	ON_BN_CLICKED(IDC_BUTTON_RUNTRIALS, &CTrafficSwarmDlg::OnBnClickedButtonRunTrials)
	ON_MESSAGE(WM_CHILD_CLOSING, OnChildClosing)
	ON_BN_CLICKED(IDCANCEL, &CTrafficSwarmDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CTrafficSwarmDlg message handlers

BOOL CTrafficSwarmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrafficSwarmDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrafficSwarmDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrafficSwarmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTrafficSwarmDlg::OnBnClickedButtonRunTrials()
{
	CCourse* pCourse = new CCourse();
	pCourse->LoadHourglass();
	
	CTrialRun trial;
	CTrialRun::RUN_RESULTS results;
	trial.Intialize(2048, pCourse);
	trial.Run(results);
	CString str;
	str.Format(_T("Run of \"%s\": %d/%d complete;\nAvg Life = %f  Avg AA = %f  Avg AW = %f\nSimulated %f seconds in %f real seconds.\n"), 
		results.strCourseName.GetBuffer(), results.nComplete, results.nAgents, results.fAvgLifetime, 
		results.fAvgAACollisions, results.fAvgAWCollisions, results.fSimulatedTime, results.fRealTime);
	OutputDebugString(str);
	MessageBox(str, _T("Trial Results"));
}

void CTrafficSwarmDlg::OnBnClickedButtonRunsandbox()
{
	CCourse *pCourse = new CCourse();
	pCourse->LoadHourglass();
	CSandboxWnd *pSandboxWnd = new CSandboxWnd(this, pCourse);

	BOOL bSuccess = pSandboxWnd->Create();
	ASSERT(bSuccess);
	pSandboxWnd->ShowWindow(SW_SHOW);

	m_setChildren.insert((CWnd*)pSandboxWnd);
}

LRESULT CTrafficSwarmDlg::OnChildClosing(WPARAM wParam, LPARAM lParam)
{
	auto iter = m_setChildren.find(reinterpret_cast<CWnd*>(wParam));
	if (iter != m_setChildren.end())
	{
		delete *iter;
		m_setChildren.erase(iter);
	}
	return TRUE;
}

void CTrafficSwarmDlg::OnBnClickedCancel()
{
	for (auto iter = m_setChildren.begin(); iter != m_setChildren.end(); iter++)
	{
		delete* iter;
	}
	m_setChildren.clear();

	CDialogEx::OnCancel();
}
