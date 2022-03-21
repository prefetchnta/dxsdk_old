/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   recvdlg.cpp
 *  Content:    
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"
#include "recvdlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecvDlg dialog


CRecvDlg::CRecvDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CRecvDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CRecvDlg)
    m_HIGHPRIORITY = FALSE;
    m_NORMAL = FALSE;
    m_PEEK = FALSE;
    m_TOPLAYER = FALSE;
    m_TRYONCEFIRST = FALSE;
    m_fAllPermutes = FALSE;
    m_fShortBuffer = FALSE;
    //}}AFX_DATA_INIT
}


void CRecvDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRecvDlg)
    DDX_Check(pDX, IDC_DPRECEIVE_HIGHPRIORITY, m_HIGHPRIORITY);
    DDX_Check(pDX, IDC_DPRECEIVE_NORMAL, m_NORMAL);
    DDX_Check(pDX, IDC_DPRECEIVE_PEEK, m_PEEK);
    DDX_Check(pDX, IDC_DPRECEIVE_TOPLAYER, m_TOPLAYER);
    DDX_Check(pDX, IDC_DPRECEIVE_TRYONCEFIRST, m_TRYONCEFIRST);
    DDX_Check(pDX, IDC_ALLPERMUTES, m_fAllPermutes);
    DDX_Check(pDX, IDC_SHORTBUFF, m_fShortBuffer);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecvDlg, CDialog)
    //{{AFX_MSG_MAP(CRecvDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRecvDlg message handlers

