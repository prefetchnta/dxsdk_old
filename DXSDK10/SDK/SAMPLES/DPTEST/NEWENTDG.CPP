/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   newentdg.cpp
 *  Content:    
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"
#include "newentdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewEntityDlg dialog


CNewEntityDlg::CNewEntityDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CNewEntityDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNewEntityDlg)
    m_fullName = _T("");
    m_nickName = _T("");
    //}}AFX_DATA_INIT
}


void CNewEntityDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNewEntityDlg)
    DDX_Text(pDX, IDC_FULLNAME, m_fullName);
    DDV_MaxChars(pDX, m_fullName, 80);
    DDX_Text(pDX, IDC_NICKNAME, m_nickName);
    DDV_MaxChars(pDX, m_nickName, 80);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewEntityDlg, CDialog)
    //{{AFX_MSG_MAP(CNewEntityDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNewEntityDlg message handlers

