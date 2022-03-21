/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   destroyp.cpp
 *  Content:    
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"
#include "destroyp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDestroyPlayerDlg dialog


CDestroyPlayerDlg::CDestroyPlayerDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CDestroyPlayerDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDestroyPlayerDlg)
    m_playerID = 0;
    //}}AFX_DATA_INIT
}


void CDestroyPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDestroyPlayerDlg)
    DDX_Text(pDX, IDC_EDIT2, m_playerID);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDestroyPlayerDlg, CDialog)
    //{{AFX_MSG_MAP(CDestroyPlayerDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDestroyPlayerDlg message handlers

