/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   infplydg.h
 *  Content:    
 *              
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CInformPlayerDlg dialog

class CInformPlayerDlg : public CDialog
{
// Construction
public:
    CInformPlayerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CInformPlayerDlg)
    enum { IDD = IDD_INFORMPLAYERCHANGE };
    DWORD   m_playerID;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CInformPlayerDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CInformPlayerDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

