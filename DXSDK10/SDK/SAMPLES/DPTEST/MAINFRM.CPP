/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   mainfrm.cpp
 *  Content:    implementation of the CMainFrame class
 *              
 ***************************************************************************/

// Define BSTR to be something else, as the MFC definition clashes
// with one in the ole2.h header file included by dplay.h
#define BSTR FOOBAR
#include "stdafx.h"
#undef BSTR
#include <stdio.h>
#define INITGUID  // Allows usage of DEFINE_GUID below to really define a guid
#include "dplay.h"

#include "dptest.h"


#include "newentdg.h"
#include "getentdg.h"
#include "openchan.h"
#include "senddlg.h"
#include "recvdlg.h"
#include "destroyp.h"
#include "opendlg.h"
#include "mainfrm.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DEFINE_GUID(DPTEST_GUID,0xc84e0e40,0x4990,0x11ce,0x85,0x72,0x00,0xaa,0x00,0x49,0x41,0x4e); // ends in IAN

DEFINE_GUID(DPTEST_GUIDB,0xc84e0e40,0x4990,0x11ce,0x85,0x72,0x00,0xaa,0x00,0x49,0x41,0x4d); // ends in IAM
  // For ensuring that the two games don't talk to each other

static const GUID *lpGuid = &DPTEST_GUID;   // Default GUID to use

#define DPRINTF( x )

void Dprintf(PCSTR pcsz, ...)
{
    va_list argp;
    char    szBuf[1024];
    
    va_start(argp, pcsz);

    vsprintf(szBuf, pcsz, argp);

    OutputDebugString(szBuf);
    va_end(argp);
}

void MsgPrintf(PCSTR pcsz, ...)
{
    va_list argp;
    char    szBuf[1024];
    
    va_start(argp, pcsz);

    vsprintf(szBuf, pcsz, argp);

    MessageBox(NULL, szBuf, "DirCon Test: Message", MB_OK);
    va_end(argp);
}


#define ERROR_ENTRY( x ) { x, #x }

typedef struct {
    HRESULT errVal;
    char *errString;
} ErrorEntry;


static ErrorEntry errorTable[] = {
    ERROR_ENTRY( DP_OK                           ),
    ERROR_ENTRY( DPERR_ACTIVEPLAYERS             ),
    ERROR_ENTRY( DPERR_BOTHBUFFERSTOOSMALL       ),
    ERROR_ENTRY( DPERR_CANTCREATEPLAYER          ),
    ERROR_ENTRY( DPERR_CAPSNOTAVAILABLEYET       ),
    ERROR_ENTRY( DPERR_FORMALBUFFERTOOSMALL      ),
    ERROR_ENTRY( DPERR_FRIENDLYBUFFERTOOSMALL    ),
    ERROR_ENTRY( DPERR_INVALIDDPID               ),
    ERROR_ENTRY( DPERR_INVALIDDPMID              ),
    ERROR_ENTRY( DPERR_INVALIDFLAGS              ),
    ERROR_ENTRY( DPERR_INVALIDOBJECTS            ),
    ERROR_ENTRY( DPERR_INVALIDPARAM              ),
    ERROR_ENTRY( DPERR_MESSAGEBUFFERTOOSMALL     ),
    ERROR_ENTRY( DPERR_NOCAPS                    ),
    ERROR_ENTRY( DPERR_NOCONNECTION              ),
    ERROR_ENTRY( DPERR_NOMESSAGES                ),
    ERROR_ENTRY( DPERR_NONAMESERVERFOUND         ),
    ERROR_ENTRY( DPERR_NOPLAYERS                 ),
    ERROR_ENTRY( DPERR_NOSESSIONS                ),
    ERROR_ENTRY( DPERR_NOTSUPPORTED              ),
    ERROR_ENTRY( DPERR_REPLYNOTSUPPORTED         ),
    ERROR_ENTRY( DPERR_TIMEOUT                   ),
    ERROR_ENTRY( DPERR_NOMEMORY                  ),
    ERROR_ENTRY( DPERR_FAIL_GENERIC              )
} ;                                
                                             
char *GetDPErrorString( HRESULT error )      
{                                            
    char *retVal = "unknown error";
    DWORD i;

    for ( i = 0; i < (sizeof(errorTable)/sizeof(ErrorEntry)); i++ ) {
        if ( errorTable[i].errVal == error ) {
            retVal = errorTable[i].errString;
            break;
        }
    }
    return( retVal );
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_COMMAND(ID_TEST_INITIALISE, OnTestInitialise)
    ON_COMMAND(ID_TEST_NEWENTITY, OnTestNewentity)
    ON_COMMAND(ID_TEST_SEND, OnTestSend)
    ON_COMMAND(ID_TEST_SHUTDOWN, OnTestShutdown)
    ON_COMMAND(ID_TEST_GETNAMES, OnTestGetnames)
    ON_COMMAND(ID_TEST_BEGINRECEIVE, OnTestBeginreceive)
    ON_COMMAND(ID_TEST_ABORT, OnTestAbort)
    ON_COMMAND(ID_TEST_CREATEOBJECT, OnTestCreateobject)
    ON_COMMAND(ID_TEST_DESTROYPLAYER, OnTestDestroyplayer)
    ON_COMMAND(ID_TEST_DIRECTPLAYENUMERATE, OnTestDirectPlayEnumerate)
    ON_COMMAND(ID_TEST_RELEASE, OnTestRelease)
    ON_COMMAND(ID_GAMEID_ALTERNATEGUID, OnGameidAlternateguid)
    ON_UPDATE_COMMAND_UI(ID_GAMEID_ALTERNATEGUID, OnUpdateGameidAlternateguid)
    ON_COMMAND(ID_GAMEID_DEFAULTGUID, OnGameidDefaultguid)
    ON_UPDATE_COMMAND_UI(ID_GAMEID_DEFAULTGUID, OnUpdateGameidDefaultguid)
    ON_UPDATE_COMMAND_UI(ID_TEST_BEGINRECEIVE, OnUpdateTestBeginreceive)
    ON_UPDATE_COMMAND_UI(ID_TEST_DESTROYPLAYER, OnUpdateTestDestroyplayer)
    ON_UPDATE_COMMAND_UI(ID_TEST_GETNAMES, OnUpdateTestGetnames)
    ON_UPDATE_COMMAND_UI(ID_TEST_INITIALISE, OnUpdateTestInitialise)
    ON_UPDATE_COMMAND_UI(ID_TEST_NEWENTITY, OnUpdateTestNewentity)
    ON_UPDATE_COMMAND_UI(ID_TEST_RELEASE, OnUpdateTestRelease)
    ON_UPDATE_COMMAND_UI(ID_TEST_SEND, OnUpdateTestSend)
    ON_UPDATE_COMMAND_UI(ID_TEST_SHUTDOWN, OnUpdateTestShutdown)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

static UINT BASED_CODE indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here
    
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
          sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

static BOOL fAbort = FALSE;

static IDirectPlay *pIDC;
// static GUID gameGUID;


void CMainFrame::OnTestInitialise() 
{
    COpenDlg dlg;
    HRESULT status;
    DWORD dwOpenStatus;
    DWORD numPlayers = 4;
    char *pDoom = "DOOM"; // just to see what chars are...

    if ( IDOK == dlg.DoModal() ) {
        numPlayers = dlg.m_numPlayers;

        status = pIDC->Open( (LPGUID)lpGuid,
                        "DirectPlayTest",
                        0, // session - don't care here
                        &numPlayers, // max num players
                        0, // flags - doesn't include "open previous sesssion"
                        &dwOpenStatus
                  );


        if ( status != DP_OK ) {
            MsgPrintf( "Open() returned %s\n", GetDPErrorString(status) );
        } else {
            if ( dwOpenStatus & DPOPENSTATUS_INVITED ) {
                MsgPrintf( "We are awaiting invitation by others, status = %lu\n", dwOpenStatus );
            } else {
                MsgPrintf( "Now open for %lu players total, status = %lu\n", numPlayers, dwOpenStatus );
            }
        }
    }
}

#define FUNC "OnTestNewentity:"
void CMainFrame::OnTestNewentity() 
{
    // TODO: Add your command handler code here
    BOOL fExists = 5;
    DPID playerID;
    BOOL fNewPlayerMade = FALSE;
    HRESULT status;

    CNewEntityDlg dlg;

    // Put up a dialog to get entity requirements
    if ( IDOK == dlg.DoModal() ) {
        // Actually try registering a new entity
        DPRINTF( ( FUNC"about to try NewPlayer\n" ) );

        do {  // Loop waiting for open to succeed, trying NewPlayer() until
              // it also has succeeded
            MSG msg;

            status = pIDC->CreatePlayer( &playerID,
                             (char *)(const char *)dlg.m_nickName,
                             (char *)(const char *)dlg.m_fullName
                            );
            if ( DP_OK == status ) {
                fNewPlayerMade = TRUE;
            }
            if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        } while ( ! fNewPlayerMade && ! fAbort );

        if ( fNewPlayerMade ) {
            MsgPrintf( "Player successfully created, id %lu\n", playerID );
        } else {
            MsgPrintf( "CreatePlayer returned %s on last call\n", GetDPErrorString(status) );
        }
    }  
}


static char sendBuffer[2048]; // Limited size of send buffer

void CMainFrame::OnTestSend() 
{
    CSendDlg dlg;
    HRESULT status;
    DWORD dwFlags;
    BOOL fStatusReported;
    int i;

    if ( IDOK == dlg.DoModal() ) {
        if ( ! dlg.m_fAllPermutes ) {
            dwFlags = 0;

            if ( dlg.m_BLOCKING ) {
                dwFlags |= DPSEND_BLOCKING;
            }
            if ( dlg.m_GUARANTEE ) {
                dwFlags |= DPSEND_GUARANTEE;
            }
            if ( dlg.m_HIGHPRIORITY ) {
                dwFlags |= DPSEND_HIGHPRIORITY;
            }
            if ( dlg.m_REPLYREQUESTED ) {
                dwFlags |= DPSEND_REPLYREQUESTED;
            }
            if ( dlg.m_TRYONCE ) {
                dwFlags |= DPSEND_TRYONCE;
            }

            for ( i = 1; i <= dlg.m_numSends; i++ ) {
                fStatusReported = FALSE;
                sprintf( sendBuffer, (const char *)dlg.m_messageString, i );
                status = pIDC->Send( dlg.m_from,
                                     dlg.m_to,
                                     dwFlags,
                                     strlen(sendBuffer),
                                     (LPSTR)sendBuffer,
                                     NULL,  // pointer to receive length
                                     NULL   // pointer to receive message
                                   );
                if ( DP_OK != status ) {
                    fStatusReported = TRUE;
                    MsgPrintf( "Message number %d : Send() returned %s\n", i, GetDPErrorString(status) );
                }
            }
            if ( ! fStatusReported ) {
                MsgPrintf( "Final send returned %s\n", GetDPErrorString(status) );
            }
        } else {
            MsgPrintf( "Number of sends parameter will be ignored for all combinations test\n" );
            // We go through all 2^5=32 flag combinations, outputting
            // results to the debug monitor
            DWORD i;

            for ( i = 0; i < 32; i++ ) {
                dwFlags = 0;
                if ( i & 0x01 ) {
                    dwFlags |= DPSEND_BLOCKING;
                }
                if ( i & 0x02 ) {
                    dwFlags |= DPSEND_GUARANTEE;
                }
                if ( i & 0x04 ) {
                    dwFlags |= DPSEND_HIGHPRIORITY;
                }
                if ( i & 0x08 ) {
                    dwFlags |= DPSEND_REPLYREQUESTED;
                }
                if ( i & 0x10 ) {
                    dwFlags |= DPSEND_TRYONCE;
                }

                Dprintf( "Send: flags:\n" );
                if ( dwFlags & DPSEND_BLOCKING) {
                    Dprintf( "  DPSEND_BLOCKING\n" );
                }
                if ( dwFlags & DPSEND_GUARANTEE) {
                    Dprintf( "  DPSEND_GUARANTEE\n" );
                }
                if ( dwFlags & DPSEND_HIGHPRIORITY) {
                    Dprintf( "  DPSEND_HIGHPRIORITY\n" );
                }
                if ( dwFlags & DPSEND_REPLYREQUESTED) {
                    Dprintf( "  DPSEND_REPLYREQUESTED\n" );
                }
                if ( dwFlags & DPSEND_TRYONCE) {
                    Dprintf( "  DPSEND_TRYONCE\n" );
                }
                status = pIDC->Send( dlg.m_from,
                                     dlg.m_to,
                                     dwFlags,
                                     dlg.m_messageString.GetLength(),
                                     (LPSTR)(const char *)dlg.m_messageString,
                                     NULL,  // pointer to receive length
                                     NULL   // pointer to receive message
                                   );
                Dprintf( "  returned: %s\n", GetDPErrorString(status) );
            }
        }
    }
}

void CMainFrame::OnTestShutdown() 
{
    HRESULT retVal;

    retVal = pIDC->Close();
    MsgPrintf( "Close() returned %s\n", GetDPErrorString(retVal) );
}

void CMainFrame::OnTestGetnames() 
{
    // TODO: Add your command handler code here
    CGetEntityInfoDlg dlg;
    
    dlg.m_pDP = pIDC;
    dlg.DoModal();  
}


// We call this function to directly receive a message
void CMainFrame::OnTestBeginreceive() 
{
    CRecvDlg dlg;

    DPID from, to;
    DPMID dpMID;
    DWORD dwLength;
    DWORD dwFlags;
    const DWORD LEN = 1024;
    char buffer[LEN];
    SysMsg *pSM;
    HRESULT status;
    DWORD dwStatus; // status flags returned by Receive()


    if ( IDOK == dlg.DoModal() ) {
        if ( ! dlg.m_fAllPermutes ) {
            // Form flags dword
            dwFlags = 0;
            if ( dlg.m_HIGHPRIORITY ) {
                dwFlags |= DPRECEIVE_HIGHPRIORITY;
            }
            if ( dlg.m_NORMAL ) {
                dwFlags |= DPRECEIVE_NORMAL;
            }
            if ( dlg.m_PEEK ) {
                dwFlags |= DPRECEIVE_PEEK;
            }
            if ( dlg.m_TOPLAYER ) {
                dwFlags |= DPRECEIVE_TOPLAYER;
            }
            if ( dlg.m_TRYONCEFIRST ) {
                dwFlags |= DPRECEIVE_TRYONCEFIRST;
            }


            if ( dlg.m_fShortBuffer ) {
                // Pretend we only have a 2-character buffer
                dwLength = 2;
            } else {
                dwLength = LEN;
            }
            status = pIDC->Receive( &from, &to,
                           dwFlags, // flags
                           &dwLength,
                           (LPSTR)buffer,
                           &dpMID,
                           &dwStatus
                         );
            switch ( status )
            {
                case DPERR_NOMESSAGES:
                    MsgPrintf( "No messages\n" );
                    break;
                case DP_OK:
                    if ( SYSTEM_MESSAGE == buffer[0] ) {
                        pSM = (SysMsg *)&buffer[0];
                        switch( pSM->hdr.id ) {
                            case DPSYSMSG_CREATEPLAYER:
                                MsgPrintf( "SysMsg: player %lu joined the game, name %s, nickname %s\n",
                                  pSM->msg.CreatePlayer.playerID,
                                  pSM->msg.CreatePlayer.fullName,
                                  (pSM->msg.CreatePlayer.fullName + strlen(pSM->msg.CreatePlayer.fullName)+1)
                                  );
                                break;
                            case DPSYSMSG_DESTROYPLAYER:
                                MsgPrintf( "SysMsg: player %lu left the game, name %s, nickname %s\n",
                                  pSM->msg.CreatePlayer.playerID,
                                  pSM->msg.CreatePlayer.fullName,
                                  (pSM->msg.CreatePlayer.fullName + strlen(pSM->msg.CreatePlayer.fullName)+1)
                                  );
                                break;
                            default:
                                MsgPrintf( "Unknown system message arrived, %d\n", (int)pSM->hdr.id );
                                break;
                        }
                    } else {
                        MsgPrintf( "Message received, len %lu, from player %lu, to player %lu:\n\"%.*s\"\nstatus %lu\n",
                                    dwLength, from, to, (int)dwLength, buffer, dwStatus );
                    }
                    break;
                case DPERR_MESSAGEBUFFERTOOSMALL:
                    MsgPrintf( "Buffer is too small, need buffer size %lu\n", dwLength );
                    break;
                default:
                    MsgPrintf( "Receive returned %s\n", GetDPErrorString(status) );
                    break;
            }
        } else {
            // We do a receive for all 2^5 = 32 permutations, and print the results out to
            // the debug port
            DWORD i;
            for ( i = 0; i < 32; i++ ) {
                dwFlags = 0;
                if ( i & 0x01 ) {
                    dwFlags |= DPRECEIVE_HIGHPRIORITY;
                }
                if ( i & 0x02 ) {
                    dwFlags |= DPRECEIVE_NORMAL;
                }
                if ( i & 0x04 ) {
                    dwFlags |= DPRECEIVE_PEEK;
                }
                if ( i & 0x08 ) {
                    dwFlags |= DPRECEIVE_TOPLAYER;
                }
                if ( i & 0x10 ) {
                    dwFlags |= DPRECEIVE_TRYONCEFIRST;
                }

                Dprintf( "Receive: flags:\n" );
                if ( dwFlags & DPRECEIVE_HIGHPRIORITY ) {
                    Dprintf( "  DPRECEIVE_HIGHPRIORITY\n" );
                }
                if ( dwFlags & DPRECEIVE_NORMAL ) {
                    Dprintf( "  DPRECEIVE_NORMAL\n" );
                }
                if ( dwFlags & DPRECEIVE_PEEK ) {
                    Dprintf( "  DPRECEIVE_PEEK\n" );
                }
                if ( dwFlags & DPRECEIVE_TOPLAYER ) {
                    Dprintf( "  DPRECEIVE_TOPLAYER\n" );
                }
                if ( dwFlags & DPRECEIVE_TRYONCEFIRST ) {
                    Dprintf( "  DPRECEIVE_TRYONCEFIRST\n" );
                }
                
                if ( dlg.m_fShortBuffer ) {
                    // Pretend we only have a 2-character buffer
                    dwLength = 2;
                } else {
                    dwLength = LEN;
                }
                status = pIDC->Receive( &from, &to,
                               dwFlags, // flags
                               &dwLength,
                               (LPSTR)buffer,
                               &dpMID,
                               &dwStatus
                             );
                Dprintf( "  returned: %s, status %lu\n", GetDPErrorString(status), dwStatus );
                switch ( status )
                {
                    case DP_OK:
                        if ( SYSTEM_MESSAGE == buffer[0] ) {
                            pSM = (SysMsg *)&buffer[0];
                            switch( pSM->hdr.id ) {
                                case DPSYSMSG_CREATEPLAYER:
                                    Dprintf( "  SysMsg: player %lu joined the game, name %s, nickname %s\n",
                                      pSM->msg.CreatePlayer.playerID,
                                      pSM->msg.CreatePlayer.fullName,
                                      (pSM->msg.CreatePlayer.fullName + strlen(pSM->msg.CreatePlayer.fullName)+1)
                                      );
                                    break;
                                case DPSYSMSG_DESTROYPLAYER:
                                    Dprintf( "  SysMsg: player %lu left the game, name %s, nickname %s\n",
                                      pSM->msg.CreatePlayer.playerID,
                                      pSM->msg.CreatePlayer.fullName,
                                      (pSM->msg.CreatePlayer.fullName + strlen(pSM->msg.CreatePlayer.fullName)+1)
                                      );
                                    break;
                                default:
                                    MsgPrintf( "  Unknown system message arrived, %d\n", (int)pSM->hdr.id );
                                    break;
                            }
                        } else {
                            Dprintf( "  User message received, len %lu, from player %lu, to player %lu:\n  \"%.*s\"\n",
                                        dwLength, from, to, (int)dwLength, buffer );
                        }
                        break;
                    case DPERR_MESSAGEBUFFERTOOSMALL:
                        Dprintf( "  Buffer is too small, need buffer size %lu\n", dwLength );
                        break;
                }
            } // for
        }  // else receive
    }
}

void CMainFrame::OnTestAbort() 
{
    // TODO: Add your command handler code here
    fAbort = TRUE;  
}


void CMainFrame::OnTestCreateobject() 
{
    HRESULT status;
    GUID guid;  // Doesn't matter what this is, it won't get used currently

    memset( &guid, 0, sizeof(guid) );

    // First we get an IUnknown pointer for the direct play object
    // we want.  Currently DirectPlay only returns one implementation of
    // a DirectConnect class.
    status = DirectPlayCreate( &guid, &pIDC );
    MsgPrintf( "DirectPlayCreate returned %s\n", GetDPErrorString(status) );
}

void CMainFrame::OnTestDestroyplayer() 
{
    // Need a dialog to ask for which player to destroy
    HRESULT status;
    CDestroyPlayerDlg dlg;

    if ( IDOK == dlg.DoModal() ) {
        status = pIDC->DestroyPlayer( dlg.m_playerID );
        MsgPrintf( "DestroyPlayer( %lu ) returned %s\n", dlg.m_playerID, GetDPErrorString(status) );
    }
}

static HRESULT FAR PASCAL EnumDirectPlayCallback(LPGUID pGuid, LPSTR pName, LPDPCAPS pCaps, LPVOID pContext)
{
    if ( (DWORD)5 != (DWORD)pContext ) {
        MsgPrintf( "Callback context pointer not what it should be\n" );
    }
    MsgPrintf( "EnumDirectPlay: returned name %s\n", pName );
    return( DP_OK );
}

void CMainFrame::OnTestDirectPlayEnumerate() 
{
    HRESULT status;
    status = DirectPlayEnumerate( EnumDirectPlayCallback, (void *)(DWORD)5 );
    MsgPrintf( "DirectPlayEnumerate returned %s\n", GetDPErrorString(status) );
}

void CMainFrame::OnTestRelease() 
{
    ULONG refCount;
    // TODO: Add your command handler code here

    // We do a release through the object interface
    // here.  This should release the object as we
    // should only have a reference count of 1 on it.
    refCount = pIDC->Release(); 
    if ( 0 == refCount ) {
        MsgPrintf( "Object freed\n" );
    } else {
        MsgPrintf( "Ref count is %lu\n", refCount );
    }
}

void CMainFrame::OnGameidAlternateguid() 
{
    // TODO: Add your command handler code here
    lpGuid = &DPTEST_GUIDB; 
}

void CMainFrame::OnUpdateGameidAlternateguid(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    if ( &DPTEST_GUIDB == lpGuid ) {
        pCmdUI->SetCheck( 1 );
    } else {
        pCmdUI->SetCheck( 0 );
    }
}

void CMainFrame::OnGameidDefaultguid() 
{
    // TODO: Add your command handler code here
    lpGuid = &DPTEST_GUID;  
}

void CMainFrame::OnUpdateGameidDefaultguid(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    if ( &DPTEST_GUID == lpGuid ) {
        pCmdUI->SetCheck( 1 );
    } else {
        pCmdUI->SetCheck( 0 );
    }
}

void CMainFrame::OnUpdateTestBeginreceive(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable( ( NULL != pIDC ? TRUE : FALSE) );
}

void CMainFrame::OnUpdateTestDestroyplayer(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable( ( NULL != pIDC ? TRUE : FALSE) );
    
}

void CMainFrame::OnUpdateTestGetnames(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable( ( NULL != pIDC ? TRUE : FALSE) );
    
}

void CMainFrame::OnUpdateTestInitialise(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable( ( NULL != pIDC ? TRUE : FALSE) );
    
}

void CMainFrame::OnUpdateTestNewentity(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable( ( NULL != pIDC ? TRUE : FALSE) );
    
}

void CMainFrame::OnUpdateTestRelease(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable( ( NULL != pIDC ? TRUE : FALSE) );
    
}

void CMainFrame::OnUpdateTestSend(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable( ( NULL != pIDC ? TRUE : FALSE) );
    
}

void CMainFrame::OnUpdateTestShutdown(CCmdUI* pCmdUI) 
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable( ( NULL != pIDC ? TRUE : FALSE) );
    
}

