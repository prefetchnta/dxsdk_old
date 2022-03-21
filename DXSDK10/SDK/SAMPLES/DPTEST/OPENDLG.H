/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   opendlg.h
 *  Content:    
 *              
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog

class COpenDlg : public CDialog
{
// Construction
public:
    COpenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(COpenDlg)
    enum { IDD = IDD_OPENDLG };
    DWORD   m_numPlayers;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(COpenDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(COpenDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

