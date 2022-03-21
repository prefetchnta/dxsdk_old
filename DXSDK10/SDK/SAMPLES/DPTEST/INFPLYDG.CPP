/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   infplydg.cpp
 *  Content:    
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"
#include "infplydg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInformPlayerDlg dialog


CInformPlayerDlg::CInformPlayerDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CInformPlayerDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CInformPlayerDlg)
    m_playerID = 0;
    //}}AFX_DATA_INIT
}


void CInformPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CInformPlayerDlg)
    DDX_Text(pDX, IDC_PLAYERID, m_playerID);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInformPlayerDlg, CDialog)
    //{{AFX_MSG_MAP(CInformPlayerDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInformPlayerDlg message handlers

