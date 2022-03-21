/*===========================================================================*\
|
|  File:        cgoption.cpp
|
|  Description: 
|   Routines to display lines of text (from resource strings) overlayed on 
|   a bitmap.  The text will be sized appropriately to fit within the 
|   rectangle specified in the profile.  The user may hilight a particular 
|   line and the index of the line will be returned to the caller.
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#include <windows.h>
#include "cgglobl.h"
#include "strrec.h"
#include "cgdib.h"
#include "cgmidi.h"
#include "cgsound.h"
#include "cgload.h"
#include "cgimage.h"

#define BASE_HWND   ghMainWnd

extern HDC LoadBitmapFile (LPSTR pBitmapFile);
extern LPSTR NewStringResource( HINSTANCE hInst, int idString);

// ----------------------------------------------------------
// CLoadingScreen - 
// ----------------------------------------------------------
CLoadingScreen::CLoadingScreen(
    LPSTR       pBitmapName,    // bkgrnd bitmap
    int     iTextId,    // resource text to overlay
    POINT       pt,     // location of circle
    TXTCOLOR    color,      // color of overlay texxt
    RECT        rect,
    CSoundEffect    *pSoundStart,   // sound effect to play
    CSoundEffect    *pSoundUpdate,  // sound effect to play
    CSoundEffect    *pSoundEnd, // sound effect to play
    LPSTR       MidiFile
) : pText( NULL )
{
    HBRUSH      hBrush;

    hdcScreen = GetDC(BASE_HWND);
    ShowCursor(FALSE);
    PatBlt(hdcScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACKNESS);

    CLoadingScreen::pSoundStart = pSoundStart;
    CLoadingScreen::pSoundUpdate = pSoundUpdate;
    CLoadingScreen::pSoundEnd = pSoundEnd;

    if ((MidiFile != NULL) && (gMusicOn))
    {
        playMusic(MidiFile, TRUE);
    }

    // Load the bitmap
    if ((hdcLoading = LoadBitmapFile(pBitmapName)) != NULL)
    {
        hBrush = CreateSolidBrush(COLOR_YELLOW);
        SelectObject(hdcLoading, hBrush);
        SetBkMode(hdcLoading, TRANSPARENT);

        if (pSoundStart != NULL) 
        {
            pSoundStart->Play();
        }

        Origin = pt;

        // Create text to be overlayed 
        pText = new CGameText(hdcLoading, &rect, 1, 1);
        pText->AddLine(NewStringResource(ghInst, iTextId)
        , color.main, color.shadow);

        pText->TextBlt();
        BitBlt(hdcScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdcLoading, 0, 0, SRCCOPY);
    }

    curTotal = 0;
}

#define MIN_RADIUS  5
// ----------------------------------------------------------
// Update - 
// ----------------------------------------------------------
void CLoadingScreen::Update(
    int Increment
)
{
    int radius;
    RECT    rect;

    curTotal += Increment;
//  radius = MIN_RADIUS + curTotal*2;
    radius = curTotal;

    rect.left = Origin.x - radius;
    rect.top = Origin.y - radius;
    rect.right = Origin.x + radius;
    rect.bottom = Origin.y + radius;

    Ellipse(hdcLoading, rect.left, rect.top, rect.right, rect.bottom);
    pText->TextBlt();
#if 0
    BitBlt(hdcScreen, rect.left, rect.top, rect.right, rect.bottom
    , hdcLoading, rect.left, rect.top, SRCCOPY);
#else
    BitBlt(hdcScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT
    , hdcLoading, 0, 0, SRCCOPY);
#endif

#if 0
    if (pSoundUpdate != NULL)
    {
        pSoundUpdate->Play();
    }
#endif

}

// ----------------------------------------------------------
// Paint
// ----------------------------------------------------------
void CLoadingScreen::Paint()
{
    BitBlt(hdcScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT
    , hdcLoading, 0, 0, SRCCOPY);
}

// ----------------------------------------------------------
// ~CLoadingScreen - 
// ----------------------------------------------------------
CLoadingScreen::~CLoadingScreen()
{
    closeMusic();

    delete pText;

    if (pSoundStart != NULL)
    {
        delete pSoundStart;
    }
    if (pSoundUpdate != NULL)
    {
        delete pSoundUpdate;
    }

    if (pSoundEnd != NULL)
    {
        pSoundEnd->Play();
    }

    HPALETTE hPal = GetCurrentObject( hdcLoading, OBJ_PAL );
    DeleteObject( hPal );

    HBRUSH hb = GetCurrentObject( hdcLoading, OBJ_BRUSH );
    DeleteObject( hb );

    PatBlt(hdcScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACKNESS);
    DeleteDC(hdcLoading);
    ReleaseDC(BASE_HWND, hdcScreen);
    ShowCursor(TRUE);
}
