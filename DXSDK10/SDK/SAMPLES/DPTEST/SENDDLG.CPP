/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   senddlg.cpp
 *  Content:    
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"
#include "senddlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendDlg dialog


CSendDlg::CSendDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSendDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSendDlg)
    m_messageString = _T("");
    m_from = 0;
    m_to = 0;
    m_BLOCKING = FALSE;
    m_GUARANTEE = FALSE;
    m_HIGHPRIORITY = FALSE;
    m_REPLYREQUESTED = FALSE;
    m_TRYONCE = TRUE;
    m_fAllPermutes = FALSE;
    m_numSends = 1;
    //}}AFX_DATA_INIT
}


void CSendDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSendDlg)
    DDX_Text(pDX, IDC_MESSAGE, m_messageString);
    DDV_MaxChars(pDX, m_messageString, 1024);
    DDX_Text(pDX, IDC_PLAYERID_FROM, m_from);
    DDX_Text(pDX, IDC_PLAYERID_TO, m_to);
    DDX_Check(pDX, IDC_DPSEND_BLOCKING, m_BLOCKING);
    DDX_Check(pDX, IDC_DPSEND_GUARANTEE, m_GUARANTEE);
    DDX_Check(pDX, IDC_DPSEND_HIGHPRIORITY, m_HIGHPRIORITY);
    DDX_Check(pDX, IDC_DPSEND_REPLYREQUESTED, m_REPLYREQUESTED);
    DDX_Check(pDX, IDC_DPSEND_TRYONCE, m_TRYONCE);
    DDX_Check(pDX, IDC_ALLPERMUTES, m_fAllPermutes);
    DDX_Text(pDX, IDC_NUMSENDS, m_numSends);
    DDV_MinMaxDWord(pDX, m_numSends, 1, 999999999);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendDlg, CDialog)
    //{{AFX_MSG_MAP(CSendDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSendDlg message handlers

