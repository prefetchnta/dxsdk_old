/*===========================================================================*\
|
|  File:        cgchrint.cpp
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
#include "memory.h"
#include "cgchdll.h"
#include "cglevel.h"
#include "cgchar.h"
#include "cgchrint.h"
#include "cginput.h"

// for now, only allow up to 9 DLLs (plus the internal list)
#define RON_GAME_INFO_MAX 10
static int ronGameInfoCount = 0;       // none loaded yet
static CGameInfo ronGameInfo[RON_GAME_INFO_MAX];

static CGameInfo gInternalCharInfo = 
{
    0, 
    NULL
};

void    LoadCharInfo( CGameInfo * info )
{
    if ( ronGameInfoCount == RON_GAME_INFO_MAX )
        return;

    // scans 'info' for chars; adds them to our internal list of love
    if ( info == NULL ) {
        // NULL means load internal characters
        info = &gInternalCharInfo;
    }
    // add the info ptr to our internal table
    ronGameInfo[ronGameInfoCount] = *info;
    ++ronGameInfoCount;
}

CGameCharInfo *FindCharInfo( char *name )
{
    static CGameCharInfo *lastFound = NULL;

    CGameCharInfo *result = NULL;
    int     x,
            y;
    int     found = 0;

    // first off - is this the same as the last one?
    if (lastFound && (stricmp(name, lastFound->name)==0))
    {
        result = lastFound;
        ++found;
    }
    else
    {
        // scan to find a character in the list(s)
        for ( x = 0; x < ronGameInfoCount; x++ ) {
            for ( y = 0; y < ronGameInfo[x].numcharacters; y++ ) {
                result = ronGameInfo[x].characters[y];
                if ( stricmp( result->name, name ) == 0)
                {
                    ++ found;
                    lastFound = result;
                    break;
                }
            }
            if (found) break;
        }
    }

    if ( !found )
        result = NULL;

    return ( result );
}

void    LoadMyDLL( char *path, char *name )
{
    char   *p;
    char    thename[260];
    CGameVersionIdent ident;
    CGameInfo info;
    int     (CALLBACK *fpIdent ) ( CGameVersionIdent * );
    int     (CALLBACK *fpInfo ) ( CGameInfo * );

    lstrcpy( thename, path );
    p = strrchr( thename, '/' );
    if ( p == NULL )
        p = strrchr( thename, '\\' );
    if ( p == NULL )
        lstrcat( thename, "\\" );
    else
        *( p + 1 ) = 0;

    lstrcat( thename, name );

    HANDLE  hlib = LoadLibrary( thename );

    if ( hlib != NULL ) {
        fpIdent = ( int (CALLBACK * ) ( CGameVersionIdent * ) ) GetProcAddress( hlib, MAKEINTRESOURCE( EXPORTED_IDENT ) );
        if ( fpIdent != NULL ) {
            ( *fpIdent ) ( &ident );
            if ( ( ident.version == RELEASE1_0 ) && ( ident.id == GAMEID ) ) {
                fpInfo = ( int ( CALLBACK * ) ( CGameInfo * ) ) GetProcAddress( hlib, MAKEINTRESOURCE( EXPORTED_INFO ) );
                if ( fpInfo != NULL ) {
                    ( *fpInfo ) ( &info );
                    LoadCharInfo( &info );
                }
            }
        }
    }
}

