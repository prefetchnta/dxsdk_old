/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *  Copyright (C) 1994-1995 ATI Technologies Inc. All Rights Reserved.
 *
 *  File:       foxbear.h
 *  Content:    main include file
 *
 ***************************************************************************/
#ifndef __FOXBEAR_INCLUDED__
#define __FOXBEAR_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h> 
#include <mmsystem.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "gfx.h"
#include "gameproc.h"
#include "fastfile.h"

/*
 * keyboard commands
 */
enum
{
    KEY_STOP = 1,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_JUMP,
    KEY_THROW
};

/*
 * global data
 */
extern LPDIRECTDRAWSURFACE      lpFrontBuffer;
extern LPDIRECTDRAWSURFACE      lpBackBuffer;
extern LPDIRECTDRAW             lpDD;
extern DWORD                    lastKey;

/*
 * fn prototypes
 */
/* ddraw.c */
extern LPDIRECTDRAWSURFACE DDCreateSurface( DWORD width, DWORD height );
extern BOOL DDCreateFlippingSurface( void );
extern BOOL DDEnable( void );
extern void MemFree( LPVOID ptr );
extern void Splash( void );
extern LPVOID CMemAlloc( UINT cnt, UINT isize );
extern LPVOID MemAlloc( UINT size );

extern void __cdecl Msg( LPSTR fmt, ... );

LPDIRECTDRAWPALETTE ReadPalFile( char *fname );
#endif
