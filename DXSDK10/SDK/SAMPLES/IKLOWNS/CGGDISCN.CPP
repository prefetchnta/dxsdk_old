/*===========================================================================*\
|
|  File:        cggdiscn.cpp
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

#include "cgdebug.h"
#include "cggdiscn.h"

/*---------------------------------------------------------------------------*\
|
|       Class CGameGDIScreen
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameGDIScreen::CGameGDIScreen(
        HWND hwnd,
        int logWidth,
        int logHeight,
        int orgX,
        int orgY
        ) : CGameScreen( hwnd, logWidth, logHeight, orgX, orgY ),
            mhdcOut( NULL ),
            mhdcBackBuffer( NULL ),
            mhbmBack( NULL )
{
    if (hwnd)
    {
        mhdcOut = GetDC( hwnd );

        mhdcBackBuffer = CreateCompatibleDC( mhdcOut );

        // create the backbuffer bitmap -- logical width & height
        mhbmBack = CreateCompatibleBitmap( mhdcOut, logWidth, logHeight );

        DB_CHECK( mhdcBackBuffer && mhbmBack, "DEBUG: unable to create screen buffers." );

        SelectObject( mhdcBackBuffer, mhbmBack );

        // set up for transparency blts
        SetBkColor( mhdcBackBuffer,RGB(255, 255, 255));
        SetTextColor( mhdcBackBuffer, RGB(0,0,0) );
    }
}   

CGameGDIScreen::~CGameGDIScreen()
{
    if (mhdcOut)
    {
        // restore the old palette
        if (mOldPalette)
            SelectPalette( mhdcOut, mOldPalette, FALSE );

        ReleaseDC( mhwnd, mhdcOut );
    }

    // free up the backbuffer DC
    if (mhdcBackBuffer)
        DeleteDC( mhdcBackBuffer );

    // & blow away our bitmap
    if (mhbmBack)
        DeleteObject( mhbmBack );
}   

void CGameGDIScreen::Render(
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    CGameBitBuffer* pSrcBuffer,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    DB_CHECK( mhdcBackBuffer && pSrcBuffer, "DEBUG: invalid screen object in Render()" );

    BitBlt( mhdcBackBuffer, xDest, yDest, wDest, hDest, ((CGameGDIBitBuffer*)pSrcBuffer)->mhdcMem, xSrc, ySrc, rop );
}   

void CGameGDIScreen::TransRender(
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    CGameBitBuffer* pSrcBuffer,
    int xSrc,
    int ySrc,
    CGameMonoMask* pTransMask,
    int xMask,
    int yMask
    )
{
    DB_CHECK( mhdcBackBuffer && pSrcBuffer, "DEBUG: invalid screen object in Render()" );

    BOOL revX = (wDest < 0);
    BOOL revY = (hDest < 0);

    // use BitBlt if you can
    if (revX || revY)
    {
        StretchBlt( mhdcBackBuffer,
            revX ? xDest - wDest : xDest,
            revY ? yDest - hDest : yDest,
            wDest, hDest,
            ((CGameGDIBitBuffer*)pSrcBuffer)->mhdcMem, xSrc, ySrc,
            revX ? -wDest : wDest,
            revY ? -hDest : hDest,
            SRCINVERT );

        StretchBlt( mhdcBackBuffer,
            revX ? xDest - wDest : xDest,
            revY ? yDest - hDest : yDest,
            wDest, hDest,
            pTransMask->mhdcMem, xMask, yMask,
            revX ? -wDest : wDest,
            revY ? -hDest : hDest,
            SRCAND );

        StretchBlt( mhdcBackBuffer,
            revX ? xDest - wDest : xDest,
            revY ? yDest - hDest : yDest,
            wDest, hDest,
            ((CGameGDIBitBuffer*)pSrcBuffer)->mhdcMem, xSrc, ySrc,
            revX ? -wDest : wDest,
            revY ? -hDest : hDest,
            SRCINVERT );
    }
    else
    {
        BitBlt( mhdcBackBuffer, xDest, yDest, wDest, hDest, ((CGameGDIBitBuffer*)pSrcBuffer)->mhdcMem, xSrc, ySrc, SRCINVERT );
        BitBlt( mhdcBackBuffer, xDest, yDest, wDest, hDest, pTransMask->mhdcMem, xMask, yMask, SRCAND );
        BitBlt( mhdcBackBuffer, xDest, yDest, wDest, hDest, ((CGameGDIBitBuffer*)pSrcBuffer)->mhdcMem, xSrc, ySrc, SRCINVERT );
    }
}   

void CGameGDIScreen::PageFlip()
{
    BitBlt( mhdcOut, 0, 0, mOutWidth, mOutHeight, mhdcBackBuffer, mxCurrent, myCurrent, SRCCOPY );
}   

void CGameGDIScreen::SetPalette(HPALETTE hPal)
{
    CGameScreen::SetPalette( hPal );

    SelectPalette( mhdcBackBuffer, hPal, FALSE );
    RealizePalette( mhdcBackBuffer );


//  SelectPalette( mhdcOut, hPal, FALSE );
//  RealizePalette( mhdcOut );
}

void
CGameGDIScreen::ColorFill(
    LPRECT pRect,
    int palIndex
    )
{
    RECT fill;

    if (pRect == NULL)
        fill = mScreenRect;
    else
        fill = *pRect;

    // GDI wants the index to be 1 lower
    HBRUSH hb = CreateSolidBrush( PALETTEINDEX(palIndex-1) );

    // FillRect is non-inclusive; so increment by one
    ++fill.right;
    ++fill.bottom;

    FillRect( mhdcBackBuffer, &fill, hb );

    DeleteObject( hb );
}
