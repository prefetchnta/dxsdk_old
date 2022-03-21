/*===========================================================================*\
|
|  File:        cgload.h
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
#ifndef _CGLOAD_H
#define _CGLOAD_H

#include "cgsound.h"
#include "cgtext.h"

typedef struct _TXTCOLOR {
    COLORREF    main;
    COLORREF    shadow;
} TXTCOLOR;

class CLoadingScreen {
private:
    POINT       Origin;
    int     curTotal;
    HDC     hdcScreen;
    HDC     hdcLoading;
    CGameText   *pText;
    CSoundEffect    *pSoundStart;
    CSoundEffect    *pSoundUpdate;
    CSoundEffect    *pSoundEnd;

public:

    CLoadingScreen(LPSTR pBitmapName, 
        int     StringId, 
        POINT   pt,
        TXTCOLOR color,
        RECT    rect,
        CSoundEffect    *pSoundStart=NULL,
        CSoundEffect    *pSoundUpdate=NULL,
        CSoundEffect    *pSoundEnd=NULL,
        LPSTR       MidiFile=NULL
        ); 
    ~CLoadingScreen();
    void Update(int Increment=5);
    void Paint();
};

#endif
