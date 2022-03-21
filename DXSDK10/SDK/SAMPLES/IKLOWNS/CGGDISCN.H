/*===========================================================================*\
|
|  File:        cggdiscn.h
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

#ifndef CGGDISCN_H
#define CGGDISCN_H

#include <windows.h>
#include "cgscreen.h"
#include "cgbitbuf.h"

class CGameGDIScreen : public CGameScreen
{
public:
    CGameGDIScreen(HWND hwnd, int logWidth, int logHeight, int orgX=0, int orgY=0);
    virtual ~CGameGDIScreen();

    virtual void Render(
        int xDest,
        int yDest,
        int wDest,
        int hDest,
        CGameBitBuffer* pSrcBuffer,
        int xSrc,
        int ySrc,
        DWORD rop
        );

    // render with transparency mask
    virtual void TransRender(
        int xDest,
        int yDest,
        int wDest,
        int hDest,
        CGameBitBuffer* pSrcBuffer,
        int xSrc,
        int ySrc,
        CGameMonoMask* pTransMask,
        int xMask=0,
        int yMask=0
        );

    virtual void PageFlip();
    virtual void SetPalette(HPALETTE);
    virtual void ColorFill( LPRECT pRect, int palIndex );

protected:
    HDC mhdcOut;            // keep output hdc around (assumes window is CS_OWNDC!)
    HDC mhdcBackBuffer;     // for holding backbuffer

    HBITMAP mhbmBack;       // bitmap for buffered display

//  HPALETTE mOldPalette;   // saved to restore when object goes away
};

#endif // CGGDISCN_H

