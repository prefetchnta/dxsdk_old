/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   opendlg.cpp
 *  Content:    
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"
#include "opendlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog


COpenDlg::COpenDlg(CWnd* pParent /*=NULL*/)
    : CDialog(COpenDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(COpenDlg)
    m_numPlayers = 1;
    //}}AFX_DATA_INIT
}


void COpenDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(COpenDlg)
    DDX_Text(pDX, IDC_NUMPLAYERS, m_numPlayers);
    DDV_MinMaxDWord(pDX, m_numPlayers, 1, 99999999);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenDlg, CDialog)
    //{{AFX_MSG_MAP(COpenDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COpenDlg message handlers

