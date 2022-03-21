/*===========================================================================*\
|
|  File:        cgaction.h
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

#ifndef CGACTION_H
#define CGACTION_H

#include "cgsprite.h"
#include "cgsound.h"

class CGameScreen;

class CGameAction
{
public:
    CGameAction(char* pFileName, char* pActionName);
    virtual ~CGameAction();

    virtual BOOL Activate();
    virtual BOOL DeActivate();
    virtual BOOL Update(int x); // change to next sprite in sequence
    virtual void Render(CGameScreen* pScreen, int x, int y, BOOL revX, BOOL revY, LPRECT pDirty);

    virtual int GetCurWidth()
    {
        return(mpSequence->GetCurWidth());
    }
    virtual int GetCurHeight()
    {
        return(mpSequence->GetCurHeight());
    }
    virtual int NextSprite(int time, BOOL wrap = TRUE)
    {   
        return(mpSequence->NextSprite(time, wrap));
    }

protected:
    CGameSpriteSequence* mpSequence;
    CSoundEffect    *pSoundEffect;
    CSoundEffect    *pEndSoundEffect;
    BOOL        fSoundOnlyWhenVisable;
    int     defaultVolume;
};

#endif // CGACTION_H
