/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   newentdg.h
 *  Content:    
 *              
 ***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CNewEntityDlg dialog

class CNewEntityDlg : public CDialog
{
// Construction
public:
    CNewEntityDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CNewEntityDlg)
    enum { IDD = IDD_NEW_ENT };
    CString m_fullName;
    CString m_nickName;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNewEntityDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNewEntityDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

