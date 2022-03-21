/*===========================================================================*\
|
|  File:        cgmisc.cpp
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
#include "cgchdll.h"
#include "cgchar.h"
#include "cgtimer.h"
#include "cginput.h"
#include "cgimage.h"

char inifile [260];


HINSTANCE hInst = NULL;            // our library instance
// This is returned by the 'Ident' function, but isn't used internally at all
CGameVersionIdent version =
{
    RELEASE1_0,
    GAMEID
};

// prototypes so we can fill in the action arrays
int PlaneCreate( CGameCharacter *, CGameLevel * );
int PlaneAction( CGameCharacter *, CGameLevel * );
int PlaneDestroy( CGameCharacter *, CGameLevel * );
int PlaneCollide( CGameCharacter *, CGameCharacter *, CGameLevel * );

int CloudCreate( CGameCharacter *, CGameLevel * );
int CloudAction( CGameCharacter *, CGameLevel * );
int CloudDestroy( CGameCharacter *, CGameLevel * );
int CloudCollide( CGameCharacter *, CGameCharacter *, CGameLevel * );

// returned by 'Info' function, and isn't used either (internally)
CGameCharInfo character1 =
{
    "Plane",
    PlaneCreate,
    PlaneAction,
    PlaneDestroy,
    NULL,
    PlaneCollide
};

CGameCharInfo character2 =
{
    "Cloud1",
    CloudCreate,
    CloudAction,
    CloudDestroy,
    NULL,
    CloudCollide
};

CGameCharInfo character3 =
{
    "Cloud2",
    CloudCreate,
    CloudAction,
    CloudDestroy,
    NULL,
    CloudCollide
};

CGameCharInfo character4 =
{
    "Cloud3",
    CloudCreate,
    CloudAction,
    CloudDestroy,
    NULL,
    CloudCollide
};

// This array allows the caller to get our information directly
CGameCharInfo *characters[] =
{
    &character1,
    &character2,
    &character3,
    &character4
};

CGameInfo dllinfo =
{
    4,                 // number of characters implemented in
    // this DLL
    characters             // array of CGameCharInfo pointers
};

// EXPORTED as ordinal #1:
#ifdef __BORLANDC__
extern "C" void CALLBACK Ident( CGameVersionIdent * id )
#else
void CALLBACK Ident( CGameVersionIdent * id )
#endif
{    
    GetModuleFileName(NULL, inifile, 259);
    char *p = strrchr(inifile, '.');
    if (p)
        lstrcpy(p+1, "GAM");

    /*
    gFastKlown = GetPrivateProfileInt("KRUSTY.DLL", "fastklown", 0, inifile);
    gFastVel = GetPrivateProfileInt("KRUSTY.DLL", "fastvel", 20, inifile);
    gDebugOut = GetPrivateProfileInt("KRUSTY.DLL", "debugout", 0, inifile);
    gPlaneSpeed = GetPrivateProfileInt("KRUSTY.DLL", "Planespeed", 5, inifile);
    gPlaneRange = GetPrivateProfileInt("KRUSTY.DLL", "Planerange", 500, inifile);
    gQDepth = GetPrivateProfileInt("KRUSTY.DLL", "qdepth", 10, inifile);
    */
    memcpy( id, &version, sizeof( version ) );
}

// EXPORTED as ordinal #2:
#ifdef __BORLANDC__
extern "C" void CALLBACK Info( CGameInfo * info )
#else
void CALLBACK Info( CGameInfo * info )
#endif
{    
    memcpy( info, &dllinfo, sizeof( dllinfo ) );
}

int     PlaneCreate( CGameCharacter *me, CGameLevel *level )
{
    int posx,posy;
    me->GetXY(&posx, &posy);
    me->SetVelocity(-32,0);
    me->MoveTo(level->GetMaxX(), posy);
    return ( ACTION_COMPLETED );
}

int     PlaneAction( CGameCharacter *me, CGameLevel *level )
{
    int posx, posy, velx, vely;
//  int originalx;
    int time = level->GetFrameTime();
    int slices = (me->mLastTime == -1) ? 1 : (time - me->mLastTime);
    me->mLastTime = time;

    me->GetVelocity(&velx, &vely);
    // remember to use sub-pixels!
    me->GetSubXY(&posx, &posy);


    posx += SUBPIXEL_DELTA(velx, slices);
    posy += SUBPIXEL_DELTA(vely, slices);

    // did we move off the screen?  If so, start over at maxx...
    if (SUB2WORLD(posx) < -level->GetMaxX())
        posx = WORLD2SUB(level->GetMaxX());

    me->SetAndMove(posx, posy);
    me->NextSprite(level->GetTimer()->Time, FALSE);

    return ( ACTION_COMPLETED );
}

int     PlaneDestroy( CGameCharacter *me, CGameLevel *level )
{
    return ( ACTION_COMPLETED );
}

int     PlaneCollide( CGameCharacter *me, CGameCharacter *other, CGameLevel *level )
{   
    return(ACTION_COMPLETED);   
}


int     CloudCreate( CGameCharacter *me, CGameLevel *level )
{
//  int posx,posy;
//  me->GetXY(&posx, &posy);
    me->SetVelocity(64 / (me->GetXParallax()+1),0);
//  me->MoveTo(posx, posy);
    return ( ACTION_COMPLETED );
}

int     CloudAction( CGameCharacter *me, CGameLevel *level )
{
    int posx, posy, velx, vely;
//  int originalx;
    int time = level->GetFrameTime();
    int slices = (me->mLastTime == -1) ? 1 : (time - me->mLastTime);
    me->mLastTime = time;

    me->GetVelocity(&velx, &vely);
    // remember to use sub-pixels!
    me->GetSubXY(&posx, &posy);

    posx += SUBPIXEL_DELTA(velx, slices);
//  posy += vely * slices;

    // did we move off the screen?  If so, start over at maxx...
#if 0
    if (posx < -level->GetMaxX())
        posx = level->GetMaxX();
#endif
    if (posx > WORLD2SUB(SCREEN_WIDTH))
        posx = WORLD2SUB(-SCREEN_WIDTH);

    me->SetAndMove(posx, posy);
//  me->NextSprite(level->GetTimer()->Time, FALSE);

    return ( ACTION_COMPLETED );
}

int     CloudDestroy( CGameCharacter *me, CGameLevel *level )
{
    return ( ACTION_COMPLETED );
}

int     CloudCollide( CGameCharacter *me, CGameCharacter *other, CGameLevel *level )
{   
    return(ACTION_COMPLETED);   
}

