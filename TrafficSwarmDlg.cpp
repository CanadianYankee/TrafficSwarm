
// TrafficSwarmDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TrafficSwarm.h"
#include "TrafficSwarmDlg.h"
#include "afxdialogex.h"
#include "SandboxWnd.h"
#include "TrialRun.h"
#include "RunStatistics.h"
#include "AgentGenome.h"
#include "EvolutionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::istream& operator >> (std::istream& in, CResultListBox& lb);


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
	, m_strSelScores(_T(""))
	, m_strSelGenome(_T(""))
	, m_strCourseName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrafficSwarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULTS, m_listResults);
	DDX_Text(pDX, IDC_EDIT_SELSCORES, m_strSelScores);
	DDX_Text(pDX, IDC_EDIT_SELGENOME, m_strSelGenome);
	DDX_Text(pDX, IDC_STATIC_COURSENAME, m_strCourseName);
	DDX_Control(pDX, IDC_STATIC_COURSEDRAW, m_staticCourseDraw);
}

BEGIN_MESSAGE_MAP(CTrafficSwarmDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RUNSANDBOX, &CTrafficSwarmDlg::OnBnClickedButtonRunsandbox)
	ON_BN_CLICKED(IDC_BUTTON_RUNTRIALS, &CTrafficSwarmDlg::OnBnClickedButtonRunTrials)
	ON_MESSAGE(WM_USER_CHILD_CLOSING, OnChildClosing)
	ON_BN_CLICKED(IDCANCEL, &CTrafficSwarmDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DOEVOLUTION, &CTrafficSwarmDlg::OnBnClickedButtonDoevolution)
	ON_BN_CLICKED(IDC_BUTTON_LOADRESULTS, &CTrafficSwarmDlg::OnBnClickedButtonLoadresults)
	ON_MESSAGE(WM_USER_RESULTS_SELECTED, &CTrafficSwarmDlg::OnUserResultsSelected)
	ON_BN_CLICKED(IDC_BUTTON_CLEARRESULTS, &CTrafficSwarmDlg::OnBnClickedButtonClearresults)
	ON_BN_CLICKED(IDC_BUTTON_LOADCOURSE, &CTrafficSwarmDlg::OnBnClickedButtonLoadcourse)
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
	m_listResults.m_bAutoSort = TRUE;
	m_cCourse.LoadHourglass();
	m_strCourseName = m_cCourse.m_strName;
	GetDlgItem(IDC_STATIC_COURSENAME)->SetWindowTextW(m_strCourseName);
	m_staticCourseDraw.SetCourse(&m_cCourse);

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
	CWaitCursor();

	CAgentGenome genome;
	genome.RandomizeAll(); //.MakeDefault();

	CTrialRun trial;
	CTrialRun::RUN_RESULTS results;
	trial.Intialize(4096, m_cCourse, genome);
	trial.Run(results);
	CString str;
	str.Format(_T("Run of \"%s\": %d/%d complete;\nAvg Life = %f  Avg AA = %f  Avg AW = %f\nSimulated %f seconds (%f FPS) in %f real seconds.\n"), 
		results.strCourseName.GetBuffer(), results.nComplete, results.nAgents, results.fAvgLifetime, 
		results.fAvgAACollisions, results.fAvgAWCollisions, results.fSimulatedTime, results.fFPS, results.fRealTime);
	OutputDebugString(str);
	MessageBox(str, _T("Trial Results"));
}

void CTrafficSwarmDlg::OnBnClickedButtonRunsandbox()
{
	auto pResults = m_listResults.GetCurResults();
	CAgentGenome genome;
	if (pResults)
		genome = pResults->genome;
	else
		genome.RandomizeAll(); // MakeDefault();

	CSandboxWnd *pSandboxWnd = new CSandboxWnd(this, m_cCourse, genome);

	BOOL bSuccess = pSandboxWnd->Create();
	ASSERT(bSuccess);
	pSandboxWnd->ShowWindow(SW_SHOW);

	m_setChildren.insert((CWnd*)pSandboxWnd);
}

void CTrafficSwarmDlg::OnBnClickedButtonDoevolution()
{
	CEvolutionDlg* pDialog = new CEvolutionDlg(this, m_cCourse);

	BOOL bSuccess = pDialog->Create(IDD_DIALOG_EVOLVE);
	ASSERT(bSuccess);
	pDialog->ShowWindow(SW_SHOW);

	m_setChildren.insert((CWnd*)pDialog);
}


LRESULT CTrafficSwarmDlg::OnChildClosing(WPARAM wParam, LPARAM lParam)
{
	auto iter = m_setChildren.find(reinterpret_cast<CWnd*>(wParam));
	if (iter != m_setChildren.end())
	{
		(*iter)->DestroyWindow();
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

void CTrafficSwarmDlg::OnBnClickedButtonLoadresults()
{ 
	CFileDialog dlgFile(TRUE, _T(".txt"), NULL, 0, _T("Text files (*.txt)|*.txt|All Files (*.*)|*.*||"));
	if (dlgFile.DoModal() == IDOK)
	{
		CString strFile = dlgFile.GetPathName();
		std::ifstream file;
		file.open(strFile.GetBuffer(), std::ios_base::in);
		file >> m_listResults;
		file.close();
	}
}


afx_msg LRESULT CTrafficSwarmDlg::OnUserResultsSelected(WPARAM wParam, LPARAM lParam)
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

void CTrafficSwarmDlg::OnBnClickedButtonClearresults()
{
	m_listResults.ClearAll();
	m_strSelGenome.Empty();
	m_strSelScores.Empty();
	UpdateData(FALSE);
}


void CTrafficSwarmDlg::OnBnClickedButtonLoadcourse()
{
	CFileDialog dlgFile(TRUE, _T(".json"), NULL, 0, _T("Json files (*.json)|*.json|All Files (*.*)|*.*||"));
	if (dlgFile.DoModal() == IDOK)
	{
		m_cCourse.LoadFromFile(dlgFile.GetPathName());
		m_strCourseName = m_cCourse.m_strName;
		GetDlgItem(IDC_STATIC_COURSENAME)->SetWindowText(m_strCourseName);
		Invalidate();
	}
}
