/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   destroyp.h
 *  Content:    
 *              
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CDestroyPlayerDlg dialog

class CDestroyPlayerDlg : public CDialog
{
// Construction
public:
    CDestroyPlayerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CDestroyPlayerDlg)
    enum { IDD = IDD_DESTROYPLAYER };
    DWORD   m_playerID;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDestroyPlayerDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CDestroyPlayerDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

