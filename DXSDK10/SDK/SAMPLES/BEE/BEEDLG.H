// beedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBeeDlg dialog

class CBeeDlg : public CDialog
{
// Construction
public:
    CBeeDlg(CWnd* pParent = NULL);  // standard constructor
    BOOL m_bAnimate;
    BOOL UnLock();
    VOID EnableButtons(BOOL);
    char *GetWaveFile() {return(m_csWave.GetBuffer(m_csWave.GetLength()));}

// Dialog Data
    //{{AFX_DATA(CBeeDlg)
    enum { IDD = IDD_BEE_DIALOG };
    CButton m_cBrowse;
    CButton m_cExit;
    CButton m_cGo;
    CButton m_cStop;
    CString m_csWave;
    BOOL    m_bContinuous;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CBeeDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CBeeDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnOK();
    afx_msg void OnStop();
    afx_msg void OnBrowse();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()


};


