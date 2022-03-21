/*===========================================================================*\
|
|  File:        cgglobl.h
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

#ifndef CGGLOBL_H
#define CGGLOBL_H

#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern HINSTANCE ghInst;
extern HWND ghMainWnd;
extern BOOL gUse_DDraw;
extern BOOL gDoubleBuffer;

extern int gSoundMode;
extern BOOL gMusicOn;

extern char gDataPath[];

#ifdef __cplusplus
}
#endif

#endif // CGGLOBL_H
