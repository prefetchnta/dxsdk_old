/*===========================================================================*\
|
|  File:        cgdlist.h
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

#ifndef CGDLIST_H
#define CGDLIST_H

#include "cgupdate.h"

class CGameLevel;
class CGameGraphic;
class CGameScreen;

class CGameDisplayList
{
public:
    CGameDisplayList(char* pFileName, char* pLevelName, CGameLevel* pLevel);
    virtual ~CGameDisplayList();

    virtual void Update(CGameLevel* pLevel, CGameUpdateList* pUpdate);
    virtual void Render(CGameLevel* pLevel, CGameScreen* pScreen, CGameUpdateList* pUpdate);

    virtual void Insert( CGameGraphic* pGraphic );
    virtual void Remove( CGameGraphic* pGraphic );
    virtual void ReSort();

    virtual CGameGraphic * First()
    {
        return mpHead;
    }

protected:
    CGameGraphic* mpHead;   // ptr to head of our linked list
};

#endif // CGDLIST_H

