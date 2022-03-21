/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   openchan.cpp
 *  Content:    
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"
#include "openchan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenChannDlg dialog


COpenChannDlg::COpenChannDlg(CWnd* pParent /*=NULL*/)
    : CDialog(COpenChannDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(COpenChannDlg)
    m_sourceEntityID = 0;
    m_targetPlayerID = 0;
    //}}AFX_DATA_INIT
}


void COpenChannDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(COpenChannDlg)
    DDX_Text(pDX, IDC_SOURCE, m_sourceEntityID);
    DDX_Text(pDX, IDC_TARGET, m_targetPlayerID);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenChannDlg, CDialog)
    //{{AFX_MSG_MAP(COpenChannDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COpenChannDlg message handlers

