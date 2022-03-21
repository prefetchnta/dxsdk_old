/*===========================================================================*\
|
|  File:        cgglobl.cpp
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

#include "cgglobl.h"

HINSTANCE ghInst = NULL;
HWND ghMainWnd = NULL;
BOOL gDoubleBuffer = 1;
BOOL gUse_DDraw = 1;
int gSoundMode=1;
BOOL gMusicOn=1;

char gDataPath[MAX_PATH];

