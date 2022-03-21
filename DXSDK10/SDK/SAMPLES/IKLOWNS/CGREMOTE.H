/*===========================================================================*\
|
|  File:        cgremote.h
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/
#ifndef _CGREMOTE_H
#define _CGREMOTE_H
#include "linklist.h"
#include "dplay.h"

#define MAX_OBJ_NAME    9

typedef int ACTION;
typedef char OBJ_ID[MAX_OBJ_NAME];

typedef struct 
{
    OBJ_ID  ObjectID;
    DWORD   InstanceID;
    DPID    OwnerID;
} REMOTE_OBJECT;

#include "cgremque.h"

#define CREATE_OBJECT   0x8001
#define DESTROY_OBJECT  0x8002

REMOTE_OBJECT *CreateRemotePeers(
    char    *name, 
    DWORD   InstanceID
);

inline ACTION PollRemoteAction(
    REMOTE_OBJECT *pObj,
    void *&Data,
    DWORD &nDataSize
)
{
    return(GetNextRemoteAction(GetRemoteObjectQueue(pObj), Data, nDataSize));
}

BOOL SendRemoteAction(
    REMOTE_OBJECT *pObj,
    ACTION Action, 
    void *Data,
    DWORD nDataSize
);

void ReleaseRemoteData(void *);

BOOL DestroyRemotePeer(
    REMOTE_OBJECT *pObj
);

BOOL RemoteConnect(REFGUID pGuid, LPSTR FullName, LPSTR NickName);

void SetCurrentLevel(void   *);

#endif

