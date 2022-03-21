/*===========================================================================*\
|
|  File:        cgchdll.h
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
#ifndef CGCHDLL_H
#define CGCHDLL_H
// version number in CGameVersionIdent structure returned by Ident()
#define RELEASE1_0  0x00010000
#define GAMEID      0x6b455749  // 'Game'

struct CGameVersionIdent {
    long    version;
    long    id; // must be == GAMEID
};

// ordinal numbers in DLL:
#define EXPORTED_IDENT  1   
#define EXPORTED_INFO   2

#define ACTION_COMPLETED    0
#define ACTION_KILLME       -1

class CGameCharacter;
class CGameLevel;
typedef int (*pActionFunction)(CGameCharacter *mythis, CGameLevel *mylevel);
typedef int (*pCollideFunction)(CGameCharacter *mythis, CGameCharacter * otherthis, CGameLevel *mylevel);

struct CGameCharInfo {
    char    *name ;
    pActionFunction Create;
    pActionFunction Action;
    pActionFunction Destroy;
    pActionFunction RemoteAction;
    pCollideFunction Collide;
};

struct CGameInfo {
    int numcharacters;
    CGameCharInfo **characters;
};

// functions exported by each character DLL
// exported as EXPORTED_IDENT:
void CALLBACK Ident (CGameVersionIdent *);

// exported as EXPORTED_INFO:
void CALLBACK Info (CGameInfo *);
#endif

