/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   senddlg.h
 *  Content:    
 *              
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CSendDlg dialog

class CSendDlg : public CDialog
{
// Construction
public:
    CSendDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CSendDlg)
    enum { IDD = IDD_SEND };
    CString m_messageString;
    DWORD   m_from;
    DWORD   m_to;
    BOOL    m_BLOCKING;
    BOOL    m_GUARANTEE;
    BOOL    m_HIGHPRIORITY;
    BOOL    m_REPLYREQUESTED;
    BOOL    m_TRYONCE;
    BOOL    m_fAllPermutes;
    DWORD   m_numSends;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSendDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CSendDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

