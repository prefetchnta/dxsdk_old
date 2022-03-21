/*===========================================================================*\
|
|  File:        cgremque.h
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
#ifndef _CGREMQUE_H
#define _CGREMQUE_H

// Entry for object in master object list
typedef struct 
{
    REMOTE_OBJECT   RemObj;
    void        *Data;
    CLinkedList *ActionList;
} REMOTE_DATA;

CLinkedList *CreateRemoteObjectQueue(REMOTE_OBJECT *, void *);
void DestroyRemoteObjectQueue(REMOTE_OBJECT *);
CLinkedList *GetRemoteObjectQueue(REMOTE_OBJECT *);

ACTION GetNextRemoteAction(CLinkedList *, void *&, DWORD &);
BOOL QueueRemoteAction(CLinkedList *, ACTION, void *, DWORD);

REMOTE_DATA *FindRemoteObjectEntry(REMOTE_OBJECT *);
#endif
