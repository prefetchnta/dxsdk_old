/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   getentdg.cpp
 *  Content:    
 *              
 ***************************************************************************/

#define BSTR FOOBAR  // Redefine to get around clash with OLE2 stuff
#include "stdafx.h"
#undef BSTR
#include "dptest.h"
#include "dplay.h"

#include "util.h"
#include "getentdg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetEntityInfoDlg dialog


CGetEntityInfoDlg::CGetEntityInfoDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CGetEntityInfoDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CGetEntityInfoDlg)
    m_playerID = 0;
    //}}AFX_DATA_INIT
}


void CGetEntityInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CGetEntityInfoDlg)
    DDX_Text(pDX, IDC_PLAYERID, m_playerID);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetEntityInfoDlg, CDialog)
    //{{AFX_MSG_MAP(CGetEntityInfoDlg)
    ON_BN_CLICKED(IDC_GETINFO, OnGetinfo)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGetEntityInfoDlg message handlers

void CGetEntityInfoDlg::OnGetinfo() 
{
    char formalName[LEN_FORMALNAME];
    char friendlyName[LEN_FRIENDLYNAME];
    DWORD lenFormal, lenFriendly;
    HRESULT status;
    DWORD i;

    // TODO: Add your control notification handler code here
    if ( UpdateData( TRUE ) ) {
        // Use GetName to get the names of the entity from the server
        lenFormal = LEN_FORMALNAME;
        lenFriendly = LEN_FRIENDLYNAME;
        status = m_pDP->GetPlayerName( m_playerID,
                         &lenFriendly, friendlyName,
                         &lenFormal, formalName
                       );
        if ( DP_OK == status )
        {
            SetDlgItemText( IDC_FULLNAME, formalName );
            SetDlgItemText( IDC_NICKNAME, friendlyName );
        } else {
            char *pText = "not found";
            SetDlgItemText( IDC_FULLNAME, pText );
            SetDlgItemText( IDC_NICKNAME, pText );
            MsgPrintf( "GetName() returned %s\n", GetDPErrorString(status) );
        }
        // Now do tests for return codes on buffer-too-small situations
        // 15 possibilities (tested one above), we test them all..

        for ( i = 0; i <=0x0E; i++ )
        {
            DWORD *d1, *d2;
            char *p1, *p2;

            if ( i & 0x01 ) {
                d1 = &lenFriendly;                
            } else {
                d1 = NULL;
            }
            if ( i & 0x02 ) {
                p1 = friendlyName;
                lenFriendly = LEN_FRIENDLYNAME;
            } else {
                p1 = NULL;
                lenFriendly = 0;
            }
            if ( i & 0x04 ) {
                d2 = &lenFormal;
            } else {
                d2 = NULL;
            }
            if ( i & 0x08 ) {
                p2 = formalName;
                lenFormal = LEN_FORMALNAME;
            } else {
                p2 = NULL;
                lenFormal = 0;
            }

            memset( formalName, 0, sizeof(formalName ) );
            memset( friendlyName, 0, sizeof(friendlyName ) );
            Dprintf( "Before GetPlayerName( %lu, %0x, %0x, %0x, %0x ):\n",
                     m_playerID, d1, p1, d2, p2 );
            Dprintf( "  lenFriendly (at %0x) = %lu, lenFormal (at %0x) = %lu\n",
                        &lenFriendly, lenFriendly, &lenFormal, lenFormal );
            status = m_pDP->GetPlayerName( m_playerID,
                             d1, p1, d2, p2
                           );
            Dprintf( "  GetPlayerName( %lu, %0x, %0x, %0x, %0x ) returned %s\n",
                m_playerID, d1, p1, d2, p2, GetDPErrorString( status )
             );
            Dprintf( "After call:\n" );
            Dprintf( "  lenFriendly (at %0x) = %lu, lenFormal (at %0x) = %lu\n",
                        &lenFriendly, lenFriendly, &lenFormal, lenFormal );
            Dprintf( "  friendlyName (at %0x) = \"%s\", formalName (at %0x) = \"%s\"\n",
                        friendlyName, friendlyName, formalName, formalName );

        }

    }
}

