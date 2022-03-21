/*===========================================================================*\
|
|  File:        cgremote.cpp
|
|  Description:
|   Routines to send and receive remote actions for controlling objects
|   on remote machines.  Uses DirectPlay for transferring data.
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

//** include files **
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "cgglobl.h" // For ghInst for dialog box
#include "cgrsrce.h"
#include "dplay.h"
#include "cglevel.h"
#include "cgremote.h"
#include "strrec.h"

#include <stdarg.h>
#include <stdio.h>

static void InitReceivePoll( void );

//** local definitions **
// structure to be used to pass remote actions across the link
typedef struct _GAMEMESSAGE {
    ACTION          Action;     // action code
    REMOTE_OBJECT       RemObj;     // unique object id
    DWORD           LevelId;
    DWORD           NumBytes;   // size of data 
    char            Data[1];    // action specific data
} GAMEMESSAGE, *PGAMEMESSAGE;


//** external functions **

//** external data **
// KLUDGE:
CGameLevel  *gCurLevel=NULL;

//** public data **
//** private data **
static  IDirectPlay *lpIDC=NULL;    // DirectPlay Object
static  DPID    dcoID=0;            // our DirectPlay ID

static BOOL fAbort = FALSE; // Abort flag for RemoteConnect()

//** public functions **
//** private functions **
static DWORD RequestThreadProc(void *Dummy);
static BOOL CALLBACK AbortDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


// ----------------------------------------------------------
// CreateRemotePeer - create a new character across all peers
// ----------------------------------------------------------
REMOTE_OBJECT *CreateRemotePeers(
    char    *name,      // name of object as per level->Add() 
    DWORD   InstanceID  // unique instance ID of object type
)
{
    // Gotta have a DCO object to do this!
    if (lpIDC == NULL)
    {
        return(NULL);
    }

    // Setup a message and an unique obect id
    char *pBuffer = NULL;
    DWORD lenBuff;
    PGAMEMESSAGE    pGameMsg;
    REMOTE_OBJECT *pObj = new REMOTE_OBJECT;
    memset(pObj, 0, sizeof(REMOTE_OBJECT));

    // Make buffer big enough to hold game message plus
    // leading non-system-char byte
    lenBuff = sizeof(GAMEMESSAGE)+1;
    pBuffer = new char[lenBuff];
    pBuffer[0] = SYSTEM_MESSAGE-1;
    pGameMsg = (PGAMEMESSAGE)&pBuffer[1];

    // Fill in object id info so that this object is unique across
    // all machines
    pObj->OwnerID = dcoID;
    pObj->InstanceID = InstanceID;
    lstrcpyn(pObj->ObjectID, name, MAX_OBJ_NAME);
    memcpy(&pGameMsg->RemObj, pObj, sizeof(REMOTE_OBJECT));

    pGameMsg->Action = CREATE_OBJECT;

    // Broadcast it to everyone in the group.
    lpIDC->Send( dcoID,  // From
                 DP_BROADCAST_ID,
                 DPSEND_TRYONCE,
                 lenBuff,
                 (LPSTR)pBuffer,
                 NULL,
                 NULL
               );   

    // Delete buffer, once sent
    delete []pBuffer;

    return(pObj);
}   

// ----------------------------------------------------------
// SendRemoteAction - broadcast an action to remote peers
// ----------------------------------------------------------
BOOL SendRemoteAction(
    REMOTE_OBJECT   *pObj,      // unique object id
    ACTION      Action,     // action code 
    void        *Data,      // action data
    DWORD       nDataSize   // size of action data
)
{
    PGAMEMESSAGE    gameMsg;
    char *pBuffer;
    DWORD lenBuff;

    // Gotta have a valid DirectPlay object
    if (lpIDC == NULL)
    {
        return(NULL);
    }

    // Allocate the GAME buffer with room for data,
    // plus room at the beginning for a non-sytem-message char
    lenBuff = sizeof(GAMEMESSAGE) + nDataSize + 1; 
    pBuffer = new char [lenBuff]; 
    pBuffer[0] = SYSTEM_MESSAGE-1;
    // Now point gameMsg at the rest of the buffer
    gameMsg = (PGAMEMESSAGE)&pBuffer[1];

    // Copy the action code and data to message buffer
    gameMsg->Action = Action;
    memcpy(&gameMsg->RemObj, pObj, sizeof(REMOTE_OBJECT));
    gameMsg->NumBytes = nDataSize;
    memcpy(gameMsg->Data, Data, nDataSize);

    // Broadcast the action to all peers
    lpIDC->Send(dcoID, // from
                DP_BROADCAST_ID, // to
                DPSEND_TRYONCE,
                lenBuff,
                pBuffer,
                NULL, // length of reply
                NULL  // reply message
               );   
    delete []pBuffer;
    return(TRUE);
}   

// ----------------------------------------------------------
// DestroyRemotePeer - Tell remote peers to kill an object
// ----------------------------------------------------------
BOOL DestroyRemotePeer(
    REMOTE_OBJECT *pObj     // unique object id
)
{
    PGAMEMESSAGE    gameMsg;
    char *pBuffer = NULL;
    DWORD lenBuff;

    if (pObj == NULL)
        return(TRUE);

    // Allocate a buffer for the GAME buffer with roon for data,
    // plus an extra character at the beginning to be the non-system
    // message char.
    lenBuff = sizeof(GAMEMESSAGE)+1; 
    pBuffer = new char [lenBuff];
    pBuffer[0] = SYSTEM_MESSAGE-1;

    // Point the game message pointer at the rest of the buffer
    gameMsg = (PGAMEMESSAGE)&pBuffer[1];

    // Format a destroy message
    memcpy(&gameMsg->RemObj, pObj, sizeof(REMOTE_OBJECT));
    gameMsg->RemObj.OwnerID = dcoID;
    gameMsg->NumBytes = 0;
    gameMsg->Action = DESTROY_OBJECT;

    // Broadcast the destroy message to all peers
    lpIDC->Send( dcoID, 
                 DP_BROADCAST_ID,
                 DPSEND_TRYONCE,
                 lenBuff,
                 pBuffer,
                 NULL,
                 NULL
               );   
    // Delete buffer
    delete []pBuffer;

    // Don't need the object id anymore
    delete pObj;
    return(TRUE);
}   

// -----------------------------------------------------------------
// RemoteConnect - establish an active connection with remotes
// -----------------------------------------------------------------
BOOL RemoteConnect(REFGUID pGuid, LPSTR FullName, LPSTR NickName)
{
    GUID    guid;
    DWORD maxPlayers;
    DWORD dwOpenStatus;
    BOOL fNewPlayerMade;
    HRESULT status;
    BOOL retVal = FALSE;
    HWND hAbortDlg = NULL;

    // Be sure we aren't already initialized.
    if (lpIDC != NULL)
    {
        return( retVal );
    }


    // Get pointer to module
    memset( &guid, 0, sizeof(guid) );
    if ( DP_OK == DirectPlayCreate( &guid, &lpIDC ) ) {
        // Now we do an Open on the object.
        // We pass a maximum of 10 players
        maxPlayers = 10;

        status = lpIDC->Open( (LPGUID)&pGuid,
                        "Immortal Klowns",
                        0, // session - not currently supported
                        &maxPlayers, // max num players
                        0, // flags - doesn't include "open previous sesssion"
                        &dwOpenStatus
                  );
        switch( status )
        {
            case DP_OK:
                break;
            case DPERR_NOCONNECTION:
                // User cancelled
                retVal = FALSE;
                break;
            default:
                // Other error condition to be handled
                retVal = FALSE;
                break;
        }
        if ( status != DP_OK ) {
            // Open failed - delete object and return
            lpIDC->Release();
            lpIDC = NULL;
            return( retVal );            
        }
        
        // Put up a modeless dialog with a cancel button, so that the
        // user can cancel us if they get bored of waiting.
        fAbort = FALSE;
        hAbortDlg = CreateDialog( ghInst, MAKEINTRESOURCE(IDD_REMCONNABORTDLG), NULL, (DLGPROC)AbortDlgProc );
        if ( ! hAbortDlg ) {
            MessageBox( NULL, "Problem creating dialog", "RemoteConnect", MB_OK );
            lpIDC->Release();
            lpIDC = NULL;
            return( retVal );
        }
        ShowWindow( hAbortDlg, SW_SHOW );

        fNewPlayerMade = FALSE;
        do {
            MSG msg;

            if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
                if ( ! IsDialogMessage( hAbortDlg, &msg) ) {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
            }
            if ( ! fAbort ) {
                status = lpIDC->CreatePlayer( &dcoID,
                                 NickName,
                                 FullName
                                );
                switch( status ) {
                    case DP_OK:
                        fNewPlayerMade = TRUE;
                        break;
                    case DPERR_CANTCREATEPLAYER:
                        MessageBox( NULL, "Creation of our player failed", "Remote play error", MB_OK );
                        lpIDC->Release();
                        lpIDC = NULL;
                        return( FALSE );
                        break;
                    default:
                        break;
                }
            }
        } while ( ! fNewPlayerMade && ! fAbort );
        DestroyWindow( hAbortDlg );
        if ( fAbort ) {
            // Destroy things and return false
            lpIDC->Release();
            lpIDC = NULL;
            return( FALSE );
        }
        
        // Instead, call init function for polled routines, below at end 
        // of file
        InitReceivePoll();
        retVal = TRUE;
    } else {
        // Create failed
        lpIDC = NULL;
        retVal = FALSE;
    }
    return( retVal );

}

// -----------------------------------------------------------------
// SetCurrentLevel - set pointer to current level object
// -----------------------------------------------------------------
void SetCurrentLevel(
    void    *newLevel
)
{
    gCurLevel = (CGameLevel *)newLevel;
}

// ----------------------------------------------------------
// CreateNewCharacter - create a new game object!
// ----------------------------------------------------------
void CreateNewCharacter(
    REMOTE_OBJECT *pObj // unique object id
)
{
    char dataBuf[256];
    char graphicsBuf[256];

    // Get specific data on object from profile
    // NOTE: This means we hit the disk.  For performance,
    // it would be better if objects could be created based
    // on an in memory object so that we don't ever have to
    // hit the disk!


    GetPrivateProfileString(
            gCurLevel->GetLevelName(),
            "Graphics",
            "",
            graphicsBuf,
            sizeof( graphicsBuf ),
            gCurLevel->GetProfileName()
            );

    GetPrivateProfileString(
            graphicsBuf,
            pObj->ObjectID,
            "",
            dataBuf,
            sizeof( dataBuf ),
            gCurLevel->GetProfileName()
            );

    // parse the data string into fields
    CStringRecord fields( dataBuf, "," );

    // Add the object to the game list!
    gCurLevel->Add(pObj->ObjectID,
        atoi(fields[1]), 
        atoi(fields[3]),
        atoi(fields[4]),
        (void *)pObj);
}

// ----------------------------------------------------------
// ProcessIncomingActions - Parse received messages & queue them
// ----------------------------------------------------------
BOOL ProcessIncomingActions(
    REMOTE_OBJECT *pObj,    // unique object id
    ACTION action,      // action code
    void *Data,     // action data
    DWORD nDataSize     // sizeof action data
)
{
    CLinkedList *ActionList;
    BOOL        fQueuedUp = FALSE;

    // Guard against activity occurring before we are ready!
    if (gCurLevel == NULL)
        return(FALSE);

    // Don't do anything about our own requests
    if (pObj->OwnerID == dcoID)
        return(FALSE);
        
    // Figure out what to do with this message
    switch (action) {

    // Need to create a brand new object locally
    case CREATE_OBJECT: {

        // Create a game character
        if (!FindRemoteObjectEntry(pObj))
        {
            CreateNewCharacter(pObj);
        }
        break;
    }

    // Time to destroy the object!
    case DESTROY_OBJECT:    {
        REMOTE_DATA *pRemoteEntry = FindRemoteObjectEntry(pObj);

        // Need to tell level object to remove the character!
        if (pRemoteEntry != NULL)
            gCurLevel->Remove((CGameGraphic *)pRemoteEntry->Data);

        break;
    }

    // Must be some character specific action
    default:{

        // Get action list for this object
        ActionList = GetRemoteObjectQueue(pObj);

        // No action list -> a new object, so try to create it.
        if (ActionList == NULL)
        {
            CreateNewCharacter(pObj);
            ActionList = GetRemoteObjectQueue(pObj);
            if (ActionList == NULL)
                return(FALSE);
        }

        // Place this action onto the queue
        QueueRemoteAction(ActionList, action, Data, nDataSize);
        fQueuedUp = TRUE;
        break;
    }
    }
    return(fQueuedUp);
}

// ----------------------------------------------------------
// ReleaseRemoteData - free remote buffer
// ----------------------------------------------------------
void ReleaseRemoteData(
    LPVOID  pData       // ptr to app data portion
)
{
    PGAMEMESSAGE    pMsg;   // desired pointer
    int     iDiff;  // difference between the two

    // Pretend we got passed in the proper ptr
    pMsg = (PGAMEMESSAGE)pData;

    // Calculate the difference between what we got
    // and what we need.
    iDiff = (LPBYTE)(&pMsg->Data) - (LPBYTE)(pMsg);

    // Adjust accordingly and free the message!
    pMsg = (PGAMEMESSAGE)((LPBYTE)pMsg - iDiff);
    delete pMsg;
}

#define MAX_BUFFER_SIZE (sizeof(GAMEMESSAGE)+512)




static char *pBuffer = NULL;
static DWORD lenBuff = max( MAX_BUFFER_SIZE, sizeof(SysMsg) ); 

static void InitReceivePoll( void )
{
    // We allocate a buffer to receive into first,
    // which may have to cope with system messages,
    // before copying the real message to the buffer to pass
    // to the game
    pBuffer = new char[ lenBuff ];

}

void PollForRemoteReceive( void )
{
    DPID        fromID, dcoReceiveID;
    DPMID       dcoMID;
    DWORD       nBytes;
    DWORD       dwStatus;
    PGAMEMESSAGE    pMsg;
    int         i;
    const       int MAX_MESSAGES = 16;
    BOOL        fCheckForMore = TRUE;

    // Paranoia check
    if ( lpIDC ) {
        // We try to receive MAX_MESSAGES at a time so that
        // a backlog doesn't build up.  If we run out of messages,
        // we stop looking for them.
        for( i = 0; i < MAX_MESSAGES && fCheckForMore ; i++ ) {
            HRESULT status;
            nBytes = lenBuff;
            status = lpIDC->Receive(
                            &fromID,
                            &dcoReceiveID,
                            DPRECEIVE_TRYONCEFIRST,
                            &nBytes,
                            pBuffer,
                            &dcoMID,
                            &dwStatus
                          );
            switch( status )
            {
                case DP_OK:
                    if ( SYSTEM_MESSAGE == pBuffer[0] ) {
                        // We do not in fact utilise the system
                        // messages during the game.
                    } else {
                        // User message - we copy the buffer minus the
                        // user message byte to the game buffer, then 
                        // pass it on.

                        // Allocate buffer for game message
                        pMsg = (PGAMEMESSAGE)new char [MAX_BUFFER_SIZE];
                        memcpy( pMsg, &pBuffer[1], nBytes-1 );

                        if (!ProcessIncomingActions(&pMsg->RemObj, pMsg->Action
                        , pMsg->Data, nBytes))
                            delete pMsg;
                    }
                    break;
                case DPERR_NOMESSAGES:
                    fCheckForMore = FALSE;
                    break;
                default:
                    // Error condition of some kind - we just stop
                    // checking for now
                    fCheckForMore = FALSE;
                    break;
            }
        }
    }
}


BOOL CALLBACK AbortDlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    WORD wID, wNotifyCode;
    BOOL retVal = 0;

    switch( uMsg )
    {
        case WM_INITDIALOG:
            // We didn't set focus
            retVal = 1;
            break;
        case WM_COMMAND:
            wID = LOWORD( wParam );
            wNotifyCode = HIWORD( wParam );
            switch ( wID )
            {
                case IDCANCEL:
                    if ( BN_CLICKED == wNotifyCode ) {
                        fAbort = TRUE;
                        retVal = 1;
                    }
                    break;
                default:
                    break;
            }   
            break;
    }
    return( retVal );
}
