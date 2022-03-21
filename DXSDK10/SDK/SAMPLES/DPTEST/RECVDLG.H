/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   recvdlg.h
 *  Content:    
 *              
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CRecvDlg dialog

class CRecvDlg : public CDialog
{
// Construction
public:
    CRecvDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CRecvDlg)
    enum { IDD = IDD_RECVDLG };
    BOOL    m_HIGHPRIORITY;
    BOOL    m_NORMAL;
    BOOL    m_PEEK;
    BOOL    m_TOPLAYER;
    BOOL    m_TRYONCEFIRST;
    BOOL    m_fAllPermutes;
    BOOL    m_fShortBuffer;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRecvDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CRecvDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

