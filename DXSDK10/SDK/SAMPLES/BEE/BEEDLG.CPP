/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:    beedlg.cpp
 *  Content: MFC dlg control for bee example.
 *              
 ***************************************************************************/


// beedlg.cpp : implementation file
//

#include "stdafx.h"
#include "bee.h"
#include "beedlg.h"
#include "animate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

// Implementation
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CenterWindow();
    
    // TODO: Add extra about dlg initialization here
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CBeeDlg dialog

CBeeDlg::CBeeDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CBeeDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CBeeDlg)
    m_csWave = _T("chainsw.wav");
    m_bContinuous = bContinuous;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_bAnimate = FALSE;
}

void CBeeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CBeeDlg)
    DDX_Control(pDX, IDC_BROWSE, m_cBrowse);
    DDX_Control(pDX, IDCANCEL, m_cExit);
    DDX_Control(pDX, IDOK, m_cGo);
    DDX_Control(pDX, ID_STOP, m_cStop);
    DDX_Text(pDX, IDC_WAVE, m_csWave);
    DDX_Check(pDX, IDC_CONT, m_bContinuous);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBeeDlg, CDialog)
    //{{AFX_MSG_MAP(CBeeDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_STOP, OnStop)
    ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeeDlg message handlers

BOOL CBeeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CenterWindow();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty())
    {
        pSysMenu->AppendMenu(MF_SEPARATOR);
        pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
    
    // TODO: Add extra initialization here
    
    m_cStop.EnableWindow(FALSE);
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBeeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBeeDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBeeDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

static BOOL RunTest(CBeeDlg *pcBeeDlg)
{
    HANDLE hThread;
    DWORD  dwID;

    ((CWnd *)pcBeeDlg)->ShowWindow(SW_MINIMIZE);
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) Animate,
                           pcBeeDlg->GetWaveFile(), 0, &dwID);


    WaitForSingleObject(hThread, INFINITE   );
    InterlockedExchange((LPLONG) &bStop, FALSE);
    pcBeeDlg->EnableButtons(TRUE);
    pcBeeDlg->UnLock();
    ((CWnd *)pcBeeDlg)->ShowWindow(SW_RESTORE);
    return(TRUE);
}

void CBeeDlg::OnOK() 
{
    if (InterlockedExchange((LPLONG) &m_bAnimate, TRUE))
        return;

    EnableButtons(FALSE);

    HANDLE hThread;
    DWORD  dwID;

    UpdateData(TRUE);
    bContinuous = m_bContinuous;
    hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) RunTest,
                           (LPVOID) this, 0, &dwID);

    CloseHandle(hThread);
    
}

void CBeeDlg::OnStop() 
{
    // TODO: Add your control notification handler code here
    InterlockedExchange((LPLONG) &bStop, TRUE);
}


BOOL CBeeDlg::UnLock()
{
    return( (BOOL) InterlockedExchange((LPLONG) &m_bAnimate, FALSE));
}

VOID CBeeDlg::EnableButtons(BOOL bb)
{
    m_cBrowse.EnableWindow(bb);
    m_cExit.EnableWindow(bb);
    m_cGo.EnableWindow(bb);
    m_cStop.EnableWindow(!bb);
}



void CBeeDlg::OnBrowse() 
{
    CFileDialog dlg(TRUE, "wav", NULL, NULL, "Wave Files | *.wav", NULL);
    char chBuffer[256];

    GetCurrentDirectory(256, chBuffer);

    int nResponse = dlg.DoModal();

    if (nResponse == IDOK)
    {
    m_csWave = dlg.GetPathName();
    UpdateData(FALSE);
    }
    else if (nResponse == IDCANCEL)
    {
    }
    SetCurrentDirectory(chBuffer);

}


