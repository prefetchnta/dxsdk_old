/*===========================================================================*\
|
|  File:        cgscreen.cpp
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
#include "cgdebug.h"
#include "cgbitbuf.h"
#include "cgscreen.h"

#define DDRAW_RETRY 4000

/*---------------------------------------------------------------------------*\
|
|       Class CGameScreen
|
|  DESCRIPTION:
|       Encapsulates access to video display in Manhattan sample game
|
|
\*---------------------------------------------------------------------------*/
CGameScreen::CGameScreen(
        HWND hwnd,          // this window determines "screen" size
        int logWidth,
        int logHeight,
        int orgX,
        int orgY
        ) : mxCurrent( orgX ),
            myCurrent( orgY ),
            mhwnd( hwnd ),
            mOutWidth( 0 ),
            mOutHeight( 0 ),
            mOldPalette( NULL )
{
    DB_CHECK( hwnd, "DEBUG: NULL hwnd in CGameScreen.");

    if (hwnd)
    {
        HDC hdc = GetDC( hwnd );

        // save previous palette
        mOldPalette = GetCurrentObject( hdc, OBJ_PAL );

        //now tell Windows to let us set our palette
        SetSystemPaletteUse( hdc, SYSPAL_NOSTATIC );

        // determine size of output bitmap
        if (GetClientRect( hwnd, &mScreenRect ))
        {
            mOutWidth = mScreenRect.right;
            mOutHeight = mScreenRect.bottom;

            PatBlt(hdc, 0,0,mOutWidth,mOutHeight,BLACKNESS);
        }
        else
        {
            DB_LOG( DB_PROBLEM, "DEBUG: GetClientRect failed in CGameScreen" );
        }

        ReleaseDC( hwnd, hdc );
    }
}   

CGameScreen::~CGameScreen()
{
    HDC hdc = GetDC( HWND_DESKTOP );

    PatBlt(hdc, 0,0,mOutWidth,mOutHeight,BLACKNESS);
    if (mOldPalette)
    {
        SelectPalette( hdc, mOldPalette, FALSE );
    }

    SetSystemPaletteUse( hdc, SYSPAL_STATIC );
    RealizePalette( hdc );

    ReleaseDC( HWND_DESKTOP, hdc );
}   

void CGameScreen::SetPalette(HPALETTE hPal)
{
    HDC hdc = GetDC( HWND_DESKTOP );

    SelectPalette( hdc, hPal, FALSE );

    RealizePalette( hdc );

    ReleaseDC( HWND_DESKTOP, hdc );
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameDDrawScreen
|
|  DESCRIPTION:
|       provide screen access via DirectDraw
|
|
\*---------------------------------------------------------------------------*/

#define FRONT_BUFFER 0
#define BACK_BUFFER 1

CGameDDrawScreen::CGameDDrawScreen(
        HWND hwnd,          // this window determines "screen" size
        int logWidth,
        int logHeight,
        int orgX,
        int orgY
        ) : CGameScreen( hwnd, logWidth, orgX, orgY ),
            mBackSurface( 1 )
{
    // create the primary surfaces (front & back)
    mpSurfaces[0] = new CGameDDrawScreenBuffer;
    mpSurfaces[1] = new CGameDDrawScreenBuffer(mpSurfaces[0]);

    // move window to fit in our mode
    SetWindowPos(
        hwnd,
        NULL,
        0,
        0,
        0,
        0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW
        );
}

CGameDDrawScreen::~CGameDDrawScreen()
{
    delete mpSurfaces[1];
    delete mpSurfaces[0];
}   

void CGameDDrawScreen::Render(
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
    pSrcBuffer->BltDDraw(
        mpSurfaces[BACK_BUFFER],
        xDest,
        yDest,
        wDest,
        hDest,
        xSrc,
        ySrc,
        rop
        );
}   

void CGameDDrawScreen::TransRender(
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
    pSrcBuffer->TransBltDDraw(
        (CGameDDrawBitBuffer*) mpSurfaces[BACK_BUFFER],
        xDest,
        yDest,
        wDest,
        hDest,
        xSrc,
        ySrc,
        1
        );
}   

void CGameDDrawScreen::PageFlip()
{

    HRESULT result;

    int stopCount = DDRAW_RETRY;

    do
    {
        result = mpSurfaces[FRONT_BUFFER]->mpSurface->Flip(
                    NULL
                    );

    }
    while( (result != DD_OK) && (--stopCount > 0));
    DB_BREAK_IF( stopCount == 0 );

}   

void
CGameDDrawScreen::SetMode(
    int width,
    int height,
    int bits
    )
{
    mpSurfaces[FRONT_BUFFER]->SetMode(
                        width,
                        height,
                        bits
                        );
}

void
CGameDDrawScreen::RestoreMode()
{
    mpSurfaces[FRONT_BUFFER]->RestoreMode();
}

void
CGameDDrawScreen::ColorFill(
    LPRECT pRect,
    int palIndex
    )
{
    HRESULT result;
    DDBLTFX dbf;
    RECT tempR = *pRect;

    // rects are off by one
    tempR.right++;
    tempR.bottom++;

    memset( &dbf, 0, sizeof( dbf ) );
    dbf.dwSize = sizeof(dbf);
    dbf.dwFillColor = palIndex;

    int stopCount = DDRAW_RETRY;

    do
    {
        result = mpSurfaces[BACK_BUFFER]->mpSurface->Blt( 
            &tempR,     // dest rect
            NULL,       // src surf
            NULL,       // src rect
            DDBLT_COLORFILL,// flags
            &dbf );     // bltfx

        // surface may have been lost due to mode switch
        if (result == DDERR_SURFACELOST)
        {
            delete mpSurfaces[1];
            delete mpSurfaces[0];
        
            mpSurfaces[0] = new CGameDDrawScreenBuffer;
            mpSurfaces[1] = new CGameDDrawScreenBuffer(mpSurfaces[0]);
        }
    }
    while( (result != DD_OK) && (--stopCount > 0));
    DB_BREAK_IF( stopCount == 0 );

}

// read in a .pal file & set our palette to it
void CGameDDrawScreen::SetPalette( char* pFile )
{
    int fh;
    WORD i;
    LPPALETTEENTRY  ppe;
    unsigned char   pRgb[4];

    fh = _lopen( pFile, OF_READ);
    if( fh == -1 )
    {
        DB_LOG(DB_PROBLEM, "Can't open palette file.");
        return;
    }

    ppe = new PALETTEENTRY[256];

    _llseek(fh, 24, SEEK_SET);  

    for( i=0; i<255; i++ )
    {
        if( sizeof (pRgb) != _lread (fh, pRgb, sizeof (pRgb)))
        {
            delete[] ppe;
            _lclose( fh );
            return;
        }
        ppe[i].peRed = (BYTE)pRgb[0];
        ppe[i].peGreen = (BYTE)pRgb[1];
        ppe[i].peBlue = (BYTE)pRgb[2];
    }

    ppe[0].peRed = (BYTE)0;
    ppe[0].peGreen = (BYTE)0;
    ppe[0].peBlue = (BYTE)0;

    ppe[255].peRed = (BYTE)255;
    ppe[255].peGreen = (BYTE)255;
    ppe[255].peBlue = (BYTE)255;

    mpSurfaces[FRONT_BUFFER]->SetPalette( ppe );
    delete[] ppe;

    _lclose( fh );
}


/*---------------------------------------------------------------------------*\
|
|       Class CGameDSScreen
|
|  DESCRIPTION:
|       provide screen access via CreateDIBSection
|
|
\*---------------------------------------------------------------------------*/

CGameDSScreen::CGameDSScreen(
        HWND hwnd,  // this window determines "screen" size
        int logWidth,
        int logHeight,
        int orgX,
        int orgY
        ) : CGameScreen( hwnd, logWidth, orgX, orgY )
{
    // !!! just try this to get the correct palette
    CGameDIB palDIB( "pie.bmp" );
    HPALETTE hOurPal = palDIB.CreatePalette();

    // create the primary surfaces (front & back)
    mpScreenDIB = new CGameDIB( logWidth, logHeight, hOurPal );
    mpBufferDIB = new CGameDIB( logWidth, logHeight, hOurPal );

    if (hwnd)
    {
        mhdcOut = GetDC( hwnd );

        mhdcBackBuffer = CreateCompatibleDC( mhdcOut );

        SelectPalette( mhdcOut, hOurPal, FALSE );
        RealizePalette( mhdcOut );

        SelectPalette( mhdcBackBuffer, hOurPal, FALSE );
        RealizePalette( mhdcBackBuffer );

        HBITMAP hbmOld = SelectObject( mhdcBackBuffer, mpBufferDIB->GetHBitmap() );

        if (hbmOld)
            DeleteObject( hbmOld );

//      mhbmOld = SelectObject( mhdcOut, mpScreenDIB->GetHBitmap() );
    }

    DeleteObject( hOurPal );
}

CGameDSScreen::~CGameDSScreen()
{
    if (mhdcOut)
    {
        // restore the old palette
        if (mOldPalette)
            SelectPalette( mhdcOut, mOldPalette, FALSE );

//      if (mhbmOld)
//          SelectObject( mhdcOut, mhbmOld );

        ReleaseDC( mhwnd, mhdcOut );
    }

    // free up the backbuffer DC
    if (mhdcBackBuffer)
        DeleteDC( mhdcBackBuffer );

    delete mpBufferDIB;
    delete mpScreenDIB;
}   

void CGameDSScreen::Render(
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
    int scanDest = mpBufferDIB->BytesPerScanline();
    int scanSrc = ((CGameDSBitBuffer*)pSrcBuffer)->mpDIB->BytesPerScanline();

    CopyDIBBits(
        mpBufferDIB->GetPixelAddress( xDest, yDest ),
        ((CGameDSBitBuffer*)pSrcBuffer)->mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) -scanDest,
        (DWORD) -scanSrc
        );
}   

void CGameDSScreen::TransRender(
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
    int scanDest = mpBufferDIB->BytesPerScanline();
    int scanSrc = ((CGameDSBitBuffer*)pSrcBuffer)->mpDIB->BytesPerScanline();

    TransCopyDIBBits(
        mpBufferDIB->GetPixelAddress( xDest, yDest ),
        ((CGameDSBitBuffer*)pSrcBuffer)->mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) -scanDest,
        (DWORD) -scanSrc,
        1
        );
}   

void CGameDSScreen::PageFlip()
{
    BitBlt( mhdcOut, 0, 0, mOutWidth, mOutHeight, mhdcBackBuffer, 0, 0, SRCCOPY );
#if 0
    LPBYTE      lpdest;
    DWORD       bytes_scanline;
    LPBYTE      lpdib_bits;

    bytes_scanline = mpScreenDIB->BytesPerScanline();

    lpdest = mpScreenDIB->GetBits();

    for( int y=0;y<mOutHeight;y++ )
    {
        lpdib_bits = mpBufferDIB->GetPixelAddress(0,y);
        memcpy( lpdest, lpdib_bits, bytes_scanline );
        lpdib_bits += ((bytes_scanline + 3) & ~3);
    }
#endif
}   

void
CGameDSScreen::ColorFill(
    LPRECT pRect,
    int palIndex
    )
{
//  int bytes_scanline = mpBufferDIB->BytesPerScanline();

//  LPBYTE lpdest = mpBufferDIB->GetPixelAddress() + (pRect->top * bytes_scanline) + pRect->left;
    LPBYTE lpdest;
    int bytes = pRect->right - pRect->left + 1;

    for (int line=pRect->top; line<=pRect->bottom; line++)
    {
        lpdest = mpBufferDIB->GetPixelAddress( pRect->left, line );
        memset( lpdest, palIndex, bytes );
//      memset( lpdest, 1 + (line / 50 ), bytes );
//      lpdest += bytes_scanline;
    }
}

void CGameDSScreen::SetPalette(HPALETTE hPal)
{
    CGameScreen::SetPalette( hPal );

    SelectPalette( mhdcBackBuffer, hPal, FALSE );
    RealizePalette( mhdcBackBuffer );
}

extern "C"
{
// to avoid stupid link error
void __stdcall _assert4d(LPTSTR, LPTSTR, unsigned)
{
    DB_BREAK();
}
}   
