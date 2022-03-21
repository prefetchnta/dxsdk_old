/*===========================================================================*\
|
|  File:        cgscreen.h
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

#ifndef CGSCREEN_H
#define CGSCREEN_H

#include <windows.h>
#include <ddraw.h>
#include "cgbitbuf.h"

class CGameScreen
{
public:
    CGameScreen(HWND hwnd, int logWidth, int logHeight, int orgX=0, int orgY=0);
    virtual ~CGameScreen();

    virtual void SetPalette(HPALETTE);
    virtual void SetPalette( char* ) = 0;
    
    virtual void Render(
            int xDest,
            int yDest,
            int wDest,
            int hDest,
            CGameBitBuffer* pSrcBuffer,
            int xSrc,
            int ySrc,
            DWORD rop
            ){};

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
        ) = 0;

    virtual void PageFlip()=0;
    virtual void SetMode( int width, int height, int bits )=0;
    virtual void RestoreMode()=0;

    virtual void ColorFill( LPRECT pRect, int palIndex )=0;

// !!! move this to images -- screens don't scroll
//  virtual void Scroll(int dx, int dy);
//  virtual void ScrollTo( int x, int y );

protected:
    HWND mhwnd;             // parent window
    int mOutWidth;          // width of output screen
    int mOutHeight;         // height of output screen
    RECT mScreenRect;       // describes entire screen

    int mxCurrent;          // current x scroll position
    int myCurrent;          // current y scroll position

    HPALETTE mOldPalette;   // saved to restore when object goes away
};

class CGameDDrawScreen : public CGameScreen
{
public:
    CGameDDrawScreen(HWND hwnd, int logWidth, int logHeight, int orgX=0, int orgY=0);
    virtual ~CGameDDrawScreen();

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
    virtual void SetMode( int width, int height, int bits );
    virtual void RestoreMode();
    virtual void ColorFill( LPRECT pRect, int palIndex );

    virtual void SetPalette( char* );
//  virtual void SetPalette(CGameDIB* pDIB);

protected:
    CGameDDrawScreenBuffer* mpSurfaces[2];  // ptrs to front & back buffer
    int mBackSurface;           // index to current back buffer
};

// use CreateDIBSection for accessing bits
class CGameDSScreen : public CGameScreen
{
public:
    CGameDSScreen(HWND hwnd, int logWidth, int logHeight, int orgX=0, int orgY=0);
    virtual ~CGameDSScreen();

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
    virtual void SetMode( int width, int height, int bits ){};
    virtual void RestoreMode(){};
    virtual void SetPalette(HPALETTE);
    virtual void SetPalette( char* ){};
    virtual void ColorFill( LPRECT pRect, int palIndex );

protected:
    HDC mhdcOut;            // keep output hdc around (assumes window is CS_OWNDC!)
    HDC mhdcBackBuffer;     // for holding backbuffer

    CGameDIB* mpBufferDIB;      // ptr to back buffer
    CGameDIB* mpScreenDIB;      // ptr to screen

    HBITMAP mhbmOld;        // for restoring screen's bitmap
};

#endif // CGSCREEN_H

