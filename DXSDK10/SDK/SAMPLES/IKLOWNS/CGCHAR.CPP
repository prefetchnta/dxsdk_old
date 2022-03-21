/*===========================================================================*\
|
|  File:        cgchar.cpp
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

#include <windows.h>
#include <memory.h>
#include "cgdebug.h"
#include "cgglobl.h"
#include "cgtimer.h"
#include "cginput.h"
#include "cgchar.h"
#include "cgchdll.h"
#include "cgchrint.h"
#include "cgload.h"
#include "cgremote.h"
#include "strrec.h"

extern void dbgprintf(char *, ...);
extern CLoadingScreen *gLoadingScreen;

static int DefaultCharacterAction (CGameCharacter *me, CGameLevel *level)
{
    // move to next sprite sequence...
    me->NextSprite(level->GetFrameTime());
    // always stays in state 0 ...
    return(0);
}

static int DefaultCharacterCollide (CGameCharacter *me, CGameCharacter *other, CGameLevel *level)
{
    // what to do if we collided????
    return(ACTION_COMPLETED);
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameCharacter
|
|  DESCRIPTION:
|
|
|
\*---------------------------------------------------------------------------*/
CGameCharacter::CGameCharacter(
    char* pFileName,
    char* pCharName,
    char* pGraphicKey,
    CGameLevel *pLevel,
    int minz,
    int maxz,
    int startx,
    int starty,
    void *pNewObjID,
    char *pRemoteName
    ) : CGameGraphic( minz ),
        mpActions( NULL ),
        mpName( NULL ),
        mNumActions( 0 ),
        mpCurAction( NULL ),
        mCurSubX( 0 ),
        mCurSubY( 0 ),
        mVelocityX( 0 ),
        mVelocityY( 0 ),
        mLastTime( -1 )
{
    static DWORD gObjInstance=0;
    char tempBuf[256];


    GetPrivateProfileString(
            pGraphicKey,
            pCharName,
            "",
            tempBuf,
            sizeof( tempBuf ),
            pFileName
            );

        // parse the data string into fields
    CStringRecord fields( tempBuf, "," );
    mNumActions = atoi(fields[5]);
    if (mNumActions <= 0)
        return; // quietly... illegal number of actions, nothing we can do about it!

    // now that we know how many there are, allocate our array & copy the
    mpActions = new CGameAction *[mNumActions];

    mpLevel = pLevel;
    mpName = new char[lstrlen( pCharName ) + 1];

    lstrcpy( mpName, pCharName );

    // get the action list
    char    *actionBuf = new char [32 * mNumActions];
    char    defAction[] = "";       // no default action names

    GetPrivateProfileString( 

        pCharName,
        NULL,       // grab all the key names
        defAction,
        actionBuf,
        32 * mNumActions,
        pFileName
         );

    int ix = 0;
    for ( char *pAction = actionBuf; *pAction && ( ix < mNumActions ); pAction++, ix++ ) {
        if (gLoadingScreen != NULL)
            gLoadingScreen->Update();
        mpActions[ix] = new CGameAction( pFileName, pAction );
        pAction += lstrlen( pAction );  // move beyond terminator
    }

    delete [] actionBuf;

    // Initialize remote stuff
    pObjID = (REMOTE_OBJECT *)pNewObjID;
    pActionQueue = NULL;
    fRemote = FALSE;

    // ron: hook into the character DLL (or built-in) functionality, if present
    // note: if nothing is found, then we'll use *default* actions!
    mpCharInfo = FindCharInfo(pCharName);
    if (mpCharInfo)
    {
        mpCharInfo->Create(this, pLevel);
        pCollideFunc = mpCharInfo->Collide;
        if (pObjID != NULL)
        {
            fRemote = TRUE;
            pActFunc = mpCharInfo->RemoteAction;
            
            // Create an action queue
            pActionQueue = CreateRemoteObjectQueue(pObjID, (void *)this);

        } else {
            if (mpCharInfo->RemoteAction != NULL)
            {
                if (pRemoteName == NULL)
                    pRemoteName = pCharName;

                pObjID = CreateRemotePeers(pRemoteName, gObjInstance++);
            }
            pActFunc = mpCharInfo->Action;
        }

    }
    else
    {
        pActFunc = DefaultCharacterAction;
        pCollideFunc = DefaultCharacterCollide;
    }

    mMaxZ = maxz;
    mMinZ = minz;
    mCurZ = (mMaxZ+mMinZ)/2;

    mpCurAction = mpActions[0];
    curState = -1;
    MoveTo(startx, starty);
//  mLastTime = pLevel->GetFrameTime();
}

CGameCharacter::~CGameCharacter(  )
{
    if (fRemote)
    {
        // Get rid of the action queue
        DestroyRemoteObjectQueue(pObjID);
    } else {
            DestroyRemotePeer(pObjID);
    }

    // delete all the action objects
    for ( ; mNumActions > 0; mNumActions-- ) {
        delete  mpActions[mNumActions - 1];
    }
    delete[] mpActions;

    delete[] mpName;
}

void CGameCharacter::Update(
    CGameLevel* pLevel,
    CGameUpdateList* pList      // we add to this if we move
    )
{
    if (mpNext)
        mpNext->Update(pLevel, pList);

    CGameAction* mpPrevAction = mpCurAction;

    int newState = (pActFunc)(this, pLevel);

        // set new state
    if (newState != -1)
    {
        // don't change state if out of bounds!
        if (newState >= mNumActions)
        {
            newState = curState;
        }

        if (newState >= 0)
            mpCurAction = mpActions[newState];
    }

    // See if this is a new action we've entered into.
    if (newState != curState)
    {
        mpPrevAction->DeActivate();
        curState = newState;
        if (curState != -1 && curState != -2)
        {
            mpCurAction->Activate();
        }
    }

    //if (mpCurAction != NULL)
    mpCurAction->Update(pLevel->World2ScreenX(SUB2WORLD(mCurSubX), mXParallax));
#if 0
    if (mpNext)
    {
        // see if we've collided...
        int testZ;
        if ((testZ = GetCurrentZ()) == mpNext->GetCurrentZ())
        {
            // might conflict; let's check it out
            CGameCharacter * pTest = (CGameCharacter *) mpNext;
            LPRECT pMyRect = GetRect();
            while (pTest && (testZ == pTest->GetCurrentZ()))
            {
                RECT temprect;
                if (IntersectRect(&temprect, pMyRect, pTest->GetRect()))
                {
                    int removeme = Collided((CGameCharacter *)pTest);
                    int removeother = ((CGameCharacter *)pTest)->Collided((CGameCharacter *) this);
                    if (removeother)
                    {
                        ((CGameCharacter *)pTest)->Destroy(this, pLevel);
                        pLevel->Remove(pTest);
                    }
                    if (removeme)
                        newState == -1;
                }
                pTest = (CGameCharacter *)(pTest->GetNext());
            }
        }
    }
#endif

    if (newState == -1)
    {
        // kill me
        Destroy(this, pLevel);
        pLevel->Remove(this);
    }
}

// display the character in its position on given screen with given screen offset
void
CGameCharacter::Render(
    CGameLevel* pLevel,
    CGameScreen* pScreen,
    CGameUpdateList* pList
    )
{
    if (mpNext)
    {
        // recurse down the list to get background filled in
        mpNext->Render(pLevel, pScreen, pList);
    }

    if (!mpCurAction)   // !!! just in case it isn't initialized
    {
        mpCurAction = mpActions[0];
    }

    RECT update;

    // only draw if we're in the invalid list:
//  if (pList->Intersect(&mRect)
    if (IntersectRect( &update, pList->GetDirtyRect(), &mRect))
    {
        mpCurAction->Render(
                pScreen,
                pLevel->World2ScreenX(SUB2WORLD(mCurSubX), mXParallax),
                pLevel->World2ScreenY(SUB2WORLD(mCurSubY), mYParallax),
                FALSE,  // don't ever reverse the image
                FALSE,
                &update
                );
    }
}   

void
CGameCharacter::MoveTo( int worldX, int worldY )
{
    mpLevel->AddInvalidRect(&mRect);

    mCurSubX = WORLD2SUB(worldX);
    mCurSubY = WORLD2SUB(worldY);

    mRect.left = mpLevel->World2ScreenX(worldX, mXParallax);
    mRect.top = mpLevel->World2ScreenY(worldY, mYParallax);
    mRect.right = mRect.left + GetCurWidth() - 1;
    mRect.bottom = mRect.top + GetCurHeight() - 1;

    mpLevel->AddInvalidRect(&mRect);
}

int
CGameCharacter::NextSprite(
    int time,   // current game time
    BOOL wrap   // should sequence wrap around?
    )
{
    // need to recalculate screen position in case screen moved
    mRect.left = mpLevel->World2ScreenX(SUB2WORLD(mCurSubX), mXParallax);
    mRect.top = mpLevel->World2ScreenY(SUB2WORLD(mCurSubY), mYParallax);
    mRect.right = mRect.left + GetCurWidth() - 1;
    mRect.bottom = mRect.top + GetCurHeight() - 1;

    mpLevel->AddInvalidRect(&mRect);

    return mpCurAction->NextSprite(time, wrap);
}

int CGameCharacter::GetRemoteAction(
    void *&Data,
    DWORD &DataSize
)
{
    return(GetNextRemoteAction(pActionQueue, Data, DataSize));
}   

int CGameCharacter::TransmitRemoteAction(
    int action, 
    void *Data, 
    DWORD DataSize
)
{
    SendRemoteAction(pObjID, action, Data, DataSize);
    return(0);
}   

void CGameCharacter::ReleaseRemoteAction(
    void    *Data
)
{
    ReleaseRemoteData(Data);
}

