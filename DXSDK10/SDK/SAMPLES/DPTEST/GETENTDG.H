/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   getentdlg.h
 *  Content:    
 *              
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CGetEntityInfoDlg dialog

class CGetEntityInfoDlg : public CDialog
{
// Construction
public:
    CGetEntityInfoDlg(CWnd* pParent = NULL);   // standard constructor
    IDirectPlay *m_pDP;

// Dialog Data
    //{{AFX_DATA(CGetEntityInfoDlg)
    enum { IDD = IDD_GET_ENT_INF };
    DWORD   m_playerID;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGetEntityInfoDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CGetEntityInfoDlg)
    afx_msg void OnGetinfo();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

