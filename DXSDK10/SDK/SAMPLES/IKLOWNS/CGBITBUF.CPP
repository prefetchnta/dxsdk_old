/*===========================================================================*\
|
|  File:        cgbitbuf.cpp
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "cgdebug.h"
#include "cgdib.h"
#include "cgbitbuf.h"
#include "cgimage.h"
#include "cgglobl.h"

#define DDRAW_RETRY 4000

/*---------------------------------------------------------------------------*\
|
|       Class CGameBitBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/

CGameBitBuffer::CGameBitBuffer(
    CGameDIB* pDIB,
    int width,
    int height,
    COLORREF transColor
    ) : mhPalette( NULL ),
        mTransColor( transColor )
{
}   

CGameBitBuffer::CGameBitBuffer(
    int width,
    int height,
    HPALETTE hPal,
    COLORREF transColor
    ) : mhPalette( NULL ),
        mTransColor( transColor )
{
}   

CGameBitBuffer::~CGameBitBuffer()
{
    if (mhPalette)
    {
        DeleteObject( mhPalette );
    }
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameGDIBitBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
// construct a buffer from existing DIB, optionally stretching to fit new size
CGameGDIBitBuffer::CGameGDIBitBuffer(
        CGameDIB* pDIB,
        int width,      // 0 (default) means use DIB's width
        int height,     // 0 (default) means use DIB's height
        COLORREF transColor
        ) : CGameBitBuffer(pDIB, width, height, transColor),
            mhdcMem( NULL ),
            mhbm( NULL ),
            mhOldBitmap( NULL ),
            mIsValid( FALSE )
{
    if (width == 0)
        width = pDIB->GetWidth();

    if (height == 0)
        height = pDIB->GetHeight();

    // create a memory DC compatible with the current screen
    HDC hdc = GetDC(NULL);

    if (hdc)
    {
        mhPalette = pDIB->CreatePalette();

        mhdcMem = CreateCompatibleDC( hdc );

        mhbm = CreateCompatibleBitmap( hdc, width, height );

        mhOldBitmap = SelectObject(mhdcMem, mhbm);

        SetPalette( mhPalette );
        SetBkColor( mhdcMem, transColor );

        // use StretchDIBits to put bits into DC
        mIsValid = StretchDIBits(
                mhdcMem,
                0,
                0,
                width,      // stretch to fit
                height,     // stretch to fit
                0,
                0,
                pDIB->GetWidth(),
                pDIB->GetHeight(),
                pDIB->GetBits(),
                pDIB->GetBitmapInfo(),
                DIB_RGB_COLORS,
                SRCCOPY
                );

        DB_CHECK( mIsValid, "DEBUG: StretchDIBits failed in CGameGDIBitBuffer." );

        ReleaseDC(NULL, hdc);
    }
}   

// construct an "empty" buffer with given size
CGameGDIBitBuffer::CGameGDIBitBuffer(
        int width,
        int height,
        HPALETTE hPal,
        COLORREF transColor
        ) : CGameBitBuffer(width, height, hPal, transColor),
            mhdcMem( NULL ),
            mhbm( NULL ),
            mhOldBitmap( NULL ),
            mIsValid( FALSE )
{
    // create a memory DC compatible with the current screen
    HDC hdc = GetDC(NULL);

    if (hdc)
    {
        mhdcMem = CreateCompatibleDC( hdc );

        mhbm = CreateCompatibleBitmap( hdc, width, height );

        mhOldBitmap = SelectObject(mhdcMem, mhbm);

        if (hPal)
        {
            SetPalette( hPal );
        }
        SetBkColor( mhdcMem, transColor );
//      HPALETTE hPalette = GetCurrentObject( hdc, OBJ_PAL );

//      SelectPalette( mhdcMem, hPalette, FALSE );
//      RealizePalette( mhdcMem );

        mIsValid = (mhdcMem && mhbm);

        ReleaseDC(NULL, hdc);
    }
}   

CGameGDIBitBuffer::~CGameGDIBitBuffer()
{
    if (mhdcMem)
    {
        if (mhOldBitmap)
            SelectObject( mhdcMem, mhOldBitmap );

        DeleteDC( mhdcMem );
    }

    if (mhbm)
        DeleteObject( mhbm );
}   

void CGameGDIBitBuffer::Blt(
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
    DB_CHECK( mhdcMem && pSrcBuffer, "DEBUG: invalid object in Blt()" );

    // negative width or height means invert the image
    if ((wDest < 0) || (hDest < 0))
    {
        StretchBlt(
            mhdcMem,
            xDest,
            yDest,
            wDest,
            hDest,
            ((CGameGDIBitBuffer*)pSrcBuffer)->mhdcMem,
            xSrc,
            ySrc,
            (wDest < 0) ? -wDest : wDest,   // undo negatation
            (hDest < 0) ? -hDest : hDest,   // undo negatation
            rop
            );
            
    }
    else
        BitBlt( mhdcMem, xDest, yDest, wDest, hDest, ((CGameGDIBitBuffer*)pSrcBuffer)->mhdcMem, xSrc, ySrc, rop );
}   

void
CGameGDIBitBuffer::SetPalette( HPALETTE hPal )
{
    SelectPalette( mhdcMem, hPal, FALSE );
    RealizePalette( mhdcMem );
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameDDrawBitBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
// static members
LPDIRECTDRAW CGameDDrawBitBuffer::mpDDrawDriver = NULL;
int CGameDDrawBitBuffer::mInstanceCount = 0;    // for allocating/freeing mpDDrawDriver

// construct a buffer from existing DIB, optionally stretching to fit new size
CGameDDrawBitBuffer::CGameDDrawBitBuffer(
        CGameDIB* pDIB,
        int width,      // 0 (default) means use DIB's width
        int height,     // 0 (default) means use DIB's height
        COLORREF transColor
        ) : CGameBitBuffer(pDIB, width, height, transColor),
            mpSurface( NULL ),
            mIsAttached( FALSE ),
            mIsValid( FALSE ),
            mpFileName( NULL )
{
    if (width == 0)
        width = pDIB->GetWidth();

    if (height == 0)
        height = pDIB->GetHeight();

    // we need to copy the dib's filename so we can reload later if necessary
    char* pFileName = (char*) pDIB->GetNamePtr();

    if (pFileName)
    {
        mpFileName = new char[lstrlen(pFileName)+1];
        lstrcpy( mpFileName, pFileName );
    }

    InitDDraw();
    CreateSurface( width, height );

    if (mIsValid)
    {
        SetBits( pDIB );    // copy DIB bits onto surface

        mhPalette = pDIB->CreatePalette();
    }
 }  

// construct an "empty" buffer with given size
CGameDDrawBitBuffer::CGameDDrawBitBuffer(
        int width,
        int height,
        HPALETTE hPal,
        COLORREF transColor
        ) : CGameBitBuffer(width, height, hPal, transColor),
            mpSurface( NULL ),
            mIsAttached( FALSE ),
            mIsValid( FALSE ),
            mpFileName( NULL )
{
    InitDDraw();
    CreateSurface( width, height );
}

void
CGameDDrawBitBuffer::InitDDraw(
    )
{
    HRESULT result;

    // do we need to get the Direct Draw driver?
    if (mpDDrawDriver == NULL)
    {
        result = DirectDrawCreate( "display", &mpDDrawDriver );
    }

    ++mInstanceCount;   // keep count so we know when to release driver
}

void
CGameDDrawBitBuffer::CreateSurface(
    int width,
    int height
    )
{
    if (mpDDrawDriver)
    {
        DDSURFACEDESC surfDesc;

        memset( &surfDesc, 0, sizeof( surfDesc ) );

        surfDesc.dwSize = sizeof( surfDesc );
//      surfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN  | DDSCAPS_COLORKEY;
        surfDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
//      surfDesc.dwFlags |= DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
        surfDesc.dwFlags |= DDSD_HEIGHT | DDSD_WIDTH;
        surfDesc.dwHeight = height;
        surfDesc.dwWidth = width;

        surfDesc.dwFlags |= DDSD_CAPS;
    //  surfDesc.dwBackBufferCount = 1;

//      surfDesc.ddsCaps.dwSize = sizeof( DDSCAPS );

        surfDesc.ddckCKSrcBlt.dwColorSpaceLowValue = 1;
        surfDesc.ddckCKSrcBlt.dwColorSpaceHighValue = 1;

        // ask driver for the surface
        HRESULT result;
        if ((result = mpDDrawDriver->CreateSurface(&surfDesc)) ==
            DD_OK)
        {
            mpSurface = surfDesc.lpDDSurface;

            // have to call SetColorKey
            result = mpSurface->SetColorKey( DDCKEY_SRCBLT, &surfDesc.ddckCKSrcBlt );

            mIsValid = TRUE;
        }
    }
}

void
CGameDDrawBitBuffer::SetBits(
    CGameDIB* pSource
    )
{
    HRESULT     ddrawrval;
    DDSURFACEDESC   dsd;
//  DDSURFACEDESC   surfd;
    LPBYTE      lpdest;
//  DWORD       x;
    DWORD       y;
    DWORD       bytes_scanline;
    LPBYTE      lpdib_bits;
#if 0
    LPBYTE      pbits;
    RGBQUAD     rgb;
    RGBTRIPLE       rgb_t;
    LPBITMAPCOREINFO    pbmci;
    int         index;
    COLORREF        colorref;
    HDC         hdc;
    LPLOGPALETTE    lplp;
    HPALETTE        hpal;
    int         cnt;
    BYTE        val;
#endif

    /*
    * pointer to bitmap bits
    */
//  lpdib_bits = pSource->GetBits();
//  lpdib_bits = pSource->GetPixelAddress(0,0);

//  pbits = lpdib_bits;

    /*
     * get surface description
     */
    mSurfD.dwSize = sizeof( mSurfD );
    ddrawrval = mpSurface->GetSurfaceDesc( &mSurfD );
    if( ddrawrval != DD_OK )
    {
        DB_LOG( DB_PROBLEM, "Could not get surface description!" );
        return;
    }

    if( !(mSurfD.ddpfPixelFormat.dwFlags & DDPF_RGB) )
    {
        DB_LOG( DB_PROBLEM, "Can only copy to RGB surfaces for now" );
        return;
    }

    bytes_scanline = pSource->BytesPerScanline();

    /*
     * access the surface
     */
    dsd.dwSize = sizeof( dsd );
    do {
        ddrawrval = mpSurface->Lock( NULL, &dsd, DDLOCK_SURFACEMEMORYPTR, NULL );
        if( ddrawrval != DD_OK && ddrawrval != DDERR_WASSTILLDRAWING )
        {
            DB_LOG( DB_PROBLEM, "Lock failed in creating DDBitBuffer" );
            return;
        }
    } 
    while( ddrawrval == DDERR_WASSTILLDRAWING );

    lpdest = (LPBYTE) dsd.lpSurface;
    mPitch = mSurfD.lPitch;

#if 0   // we only work with 8bpp for now

    if( lpBI->bmiHeader.biSize == sizeof(BITMAPCOREHEADER) ) {
    pbmci = (LPBITMAPCOREINFO) lpBI;
    } else {
    pbmci = NULL;
    }

    Msg( "biBitCount = %d, bpp=%d", lpBI->bmiHeader.biBitCount,
        mSurfD.dpfPixelFormat.dwRGBBitCount );
    /*
     * check what if we need to do some sort of color conversion
     * on this DIB
     */
    if( lpBI->bmiHeader.biBitCount != mSurfD.dpfPixelFormat.dwRGBBitCount ) {

    Msg( "Doing color conversion" );
    Msg( "BMP: height = %d, width = %d", lpBI->bmiHeader.biHeight, lpBI->bmiHeader.biWidth );

    /*
     * don't work with > 8bpp bmps right now
     */
    if( lpBI->bmiHeader.biBitCount > 8 ) {
        Msg( "Bitcount = %ld: NOT SUPPORTED", lpBI->bmiHeader.biBitCount );
        psurf->Unlock( NULL );
        return;
    }

    /*
     * build a system palette if we are palettized
     */
    if( mSurfD.dpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED ) {
        hdc = GetDC( NULL );
        cnt = GetSystemPaletteEntries( hdc, 0, 255, NULL );
        lplp = LocalAlloc( LPTR, sizeof( LOGPALETTE ) + sizeof( PALETTEENTRY ) * cnt );
        if( lplp == NULL ) {
        Msg( "out of memory!" );
        psurf->Unlock( NULL );
        return;
        }
        lplp->palVersion = 0x300;
        lplp->palNumEntries = cnt;
        GetSystemPaletteEntries( hdc, 0, cnt, &lplp->palPalEntry[0] );
        hpal = CreatePalette( lplp );
        Msg( "hpal = %x", hpal );
        if( hpal == NULL ) {
        Msg( "Could not create palette" );
        LocalFree( lplp );
        psurf->Unlock( NULL );
        return;
        }
    }

    /*
     * loop through the bitmap, converting one at a time...
     */
    for( y=0;y<mSurfD.dwHeight;y++ ) {
        for( x=0;x<mSurfD.dwWidth;x++ ) {
        /*
         * get an index into the palette
         */
        switch( lpBI->bmiHeader.biBitCount ) {
        case 1:
            index = pbits[x/8];
            index >>= (x % 8);
            index &= 1;
            break;
        case 4:
            index = pbits[x/2];
            if( (x % 2) == 1 ) {
            index /= 16;
            }
            index &= 0xf;
            break;
        case 8:
            index = pbits[x/2];
            break;
        }

        /*
         * get the RGB value
         */
        if( pbmci != NULL ) {
            rgb_t = pbmci->bmciColors[index];
            rgb.rgbRed = rgb_t.rgbtRed;
            rgb.rgbGreen = rgb_t.rgbtGreen;
            rgb.rgbBlue = rgb_t.rgbtBlue;
            rgb.rgbReserved = 0;
        } else {
            rgb = lpBI->bmiColors[index];
        }
        colorref = RGB( rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue );

        /*
         * convert this RGB value to the nearest displayable color
         */
        switch( mSurfD.dpfPixelFormat.dwRGBBitCount ) {
        case 8:
            /*
             * 8bpp - assume palettized!
             */
            val = GetNearestPaletteIndex( hpal, colorref );
            if( !bIsDDrawDrv ) {
            lpdest[x] = val;
            }
            #if 0
            Msg( "(%d,%d): %d:(%d,%d,%d)", x,y, val, rgb.rgbRed,
                    rgb.rgbBlue, rgb.rgbGreen );
            #endif
            break;
        case 16:
            //PUNT
            break;
        case 24:
            if( !bIsDDrawDrv ) {
            lpdest[3*x] = rgb.rgbRed;
            lpdest[3*x+1] = rgb.rgbGreen;
            lpdest[3*x+2] = rgb.rgbBlue;
            }
            break;
        case 32:
            if( !bIsDDrawDrv ) {
            lpdest[4*x] = rgb.rgbRed;
            lpdest[4*x+1] = rgb.rgbGreen;
            lpdest[4*x+2] = rgb.rgbBlue;
            }
            break;
        }
        }
        /*
         * move down throught the bitmap
         */
        pbits += ((bytes_scanline + 3) & ~3);
        lpdest += (DWORD) mSurfD.dpfPixelFormat.lStride;
    }
    if( mSurfD.dpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED ) {
        DeleteObject( hpal );
        LocalFree( lplp );
        ReleaseDC( NULL, hdc );
    }

    /*
    * same color format, just blast it
    */
    } else {
    Msg( "Direct copy" );
#endif
    for( y=0;y<mSurfD.dwHeight;y++ )
    {
        lpdib_bits = pSource->GetPixelAddress(0,y);
        memcpy( lpdest, lpdib_bits, bytes_scanline );
        lpdest += (DWORD) mSurfD.lPitch;
//      lpdib_bits += ((bytes_scanline + 3) & ~3);
    }
//  }

    /*
    * release the surface and go home...
    */
    mpSurface->Unlock( NULL );
}   

CGameDDrawBitBuffer::~CGameDDrawBitBuffer()
{
    if (mpSurface && !mIsAttached)
    {
        mpSurface->Release(  );
    }

    if (--mInstanceCount == 0)
    {
        if (mpDDrawDriver)
        {
            mpDDrawDriver->Release( );
            mpDDrawDriver = NULL;
        }
    }

    delete mpFileName;
}

// call this to re-create the surface if it was lost
// this can happen if we've switched modes & switch back
void
CGameDDrawBitBuffer::ReCreate(
    )
{
    int width = mSurfD.dwWidth;
    int height = mSurfD.dwHeight;

    // !!! what do we do if we're attached?
    // delete old surface object
    if (mpSurface && !mIsAttached)
    {
        mpSurface->Release(  );
        mpSurface = NULL;
    }

    CreateSurface( width, height );

    if (mIsValid && mpFileName)
    {
        // reload the file
        CGameDIB dib( mpFileName );
        SetBits( &dib );    // copy DIB bits onto surface
    }
}


// generic blt call to this buffer from another DDraw surface
void
CGameDDrawBitBuffer::Blt(
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
    HRESULT result;
    DDBLTFX dbf;
    BOOL useFX = FALSE;

    // negative width or height means mirror the blt
    RECT destRect = 
    {
        xDest,
        yDest,
        xDest+((wDest<0) ? -wDest : wDest),
        yDest+((hDest<0) ? -hDest : hDest)
    };

    RECT srcRect = 
    {
        xSrc,
        ySrc,
        xSrc+((wDest<0) ? -wDest : wDest),
        ySrc+((hDest<0) ? -hDest : hDest)
    };


    if ((wDest < 0) || (hDest < 0))
    {
        useFX = TRUE;
        memset( &dbf, 0, sizeof( dbf ) );

        dbf.dwSize = sizeof(dbf);
        dbf.dwDDFX = (wDest<0) ? DDBLTFX_MIRRORLEFTRIGHT : 0;
        dbf.dwDDFX |= (hDest<0) ? DDBLTFX_MIRRORUPDOWN : 0;
    }

    result = mpSurface->Blt( 
            &destRect,  // dest rect
            ((CGameDDrawBitBuffer*)pSrcBuffer)->mpSurface,      // src surf
            &srcRect,   // src rect
            useFX ? DDBLT_DDFX : 0,// flags
            useFX ? &dbf : NULL );  // bltfx

    // !!! any reason to check return code here?
}   

// generic blt call -- determine type of destination buffer
void
CGameDDrawBitBuffer::Blt(
    CGameBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    switch (pDestBuffer->TypeID())
    {
        case BB_DDraw:
            Blt(
                (CGameDDrawBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                rop
                );
            break;
        case BB_DS:
            Blt(
                (CGameDSBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                rop
                );
            break;
        default:
            DB_BREAK();
            break;
    }
}

// generic transblt call -- determine type of source buffer
void
CGameDDrawBitBuffer::TransBlt(
    CGameBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    switch (pDestBuffer->TypeID())
    {
        case BB_DDraw:
            TransBlt(
                (CGameDDrawBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                transColor
                );
            break;
        case BB_DS:
            TransBlt(
                (CGameDSBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                transColor
                );
            break;
        default:
            DB_BREAK();
            break;
    }
}

// specific blt call for another DDraw surface
void
CGameDDrawBitBuffer::BltDDraw(
    CGameDDrawBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    HRESULT result;
    DDBLTFX dbf;
    BOOL useFX = FALSE;

    // negative width or height means mirror the blt
    RECT destRect =
    {
        xDest,
        yDest,
        xDest+((wDest<0) ? -wDest : wDest),
        yDest+((hDest<0) ? -hDest : hDest)
    };

    RECT srcRect =
    {
        xSrc,
        ySrc,
        xSrc+((wDest<0) ? -wDest : wDest),
        ySrc+((hDest<0) ? -hDest : hDest)
    };

    if ((wDest < 0) || (hDest < 0))
    {
        useFX = TRUE;
        memset( &dbf, 0, sizeof( dbf ) );

        dbf.dwSize = sizeof(dbf);
        dbf.dwDDFX = (wDest<0) ? DDBLTFX_MIRRORLEFTRIGHT : 0;
        dbf.dwDDFX |= (hDest<0) ? DDBLTFX_MIRRORUPDOWN : 0;
    }

    // force a stop if we never succeed
    int stopCount = DDRAW_RETRY;

    do
    {
        result = pDestBuffer->mpSurface->BltFast( 
                destRect.left,
                destRect.top,
                mpSurface,      // src surf
                &srcRect,   // src rect
                FALSE           // transparent
                );

        // surface may have been lost due to mode switch
        if (result == DDERR_SURFACELOST)
        {
            mIsValid = FALSE;
            // re-create our surface
            ReCreate();
        }
    }
    while( (result != DD_OK) && (--stopCount > 0));

    DB_BREAK_IF( stopCount == 0 );
}   

// specific transblt call for another DDraw buffer
void
CGameDDrawBitBuffer::TransBltDDraw(
    CGameDDrawBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    HRESULT result;

    RECT destRect = 
    {
        xDest, yDest, xDest+wDest, yDest+hDest
    };

    RECT srcRect =
    {
        xSrc, ySrc, xSrc+wDest, ySrc+hDest
    };

//  memset( &dbf, 0, sizeof( dbf ) );

//  dbf.dwSize = sizeof(dbf);

    // force a stop if we never succeed
    int stopCount = DDRAW_RETRY;

    do
    {
        result = pDestBuffer->mpSurface->BltFast( 
                destRect.left,
                destRect.top,
                mpSurface,      // src surf
                &srcRect,   // src rect
                TRUE            // transparent
                );

        // surface may have been lost due to mode switch
        if (result == DDERR_SURFACELOST)
        {
            mIsValid = FALSE;
            // re-create our surface
            ReCreate();
        }
    }
    while( (result != DD_OK) && (--stopCount > 0));
    DB_BREAK_IF( stopCount == 0 );
}

// specific blt call for a dibsection destination buffer
void
CGameDDrawBitBuffer::BltDS(
    CGameDSBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    LPBYTE pSrc = GetLockedAddress( xSrc, ySrc );
    int scanDest = -(pDestBuffer->mpDIB->BytesPerScanline());
    int scanSrc = mPitch;

    CopyDIBBits(
        pDestBuffer->mpDIB->GetPixelAddress( xDest, yDest ),
        pSrc,
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc
        );

    Unlock();
}

// specific transblt call for a ds buffer
void
CGameDDrawBitBuffer::TransBltDS(
    CGameDSBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    LPBYTE pSrc = GetLockedAddress( xSrc, ySrc );
    int scanDest = -(pDestBuffer->mpDIB->BytesPerScanline());
    int scanSrc = mPitch;

    TransCopyDIBBits(
        pDestBuffer->mpDIB->GetPixelAddress( xDest, yDest ),
        pSrc,
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc,
        transColor
        );

    Unlock();
}

void
CGameDDrawBitBuffer::SetPalette( HPALETTE hPal )
{
//  SelectPalette( mhdcMem, hPal, FALSE );
//  RealizePalette( mhdcMem );
}

LPBYTE  // return address to given pixel in locked surface
CGameDDrawBitBuffer::GetLockedAddress(
    int x,
    int y
    )
{
    HRESULT     ddrawrval;
    DDSURFACEDESC   dsd;
    /*
    * access the surface
    */
    dsd.dwSize = sizeof( dsd );

    int stopCount = DDRAW_RETRY;

    do
    {
        ddrawrval = mpSurface->Lock( NULL, &dsd, DDLOCK_SURFACEMEMORYPTR, NULL );

        // surface may have been lost due to mode switch
        if (ddrawrval == DDERR_SURFACELOST)
        {
            mIsValid = FALSE;
            // re-create our surface
            ReCreate();
        }
    }
    while( (ddrawrval != DD_OK) && (--stopCount > 0) );
    DB_BREAK_IF( stopCount == 0 );

#if 0
    if( ddrawrval != DD_OK )
    {
        DB_LOG( DB_PROBLEM, "Lock failed in creating DDBitBuffer" );
        return NULL;
    }
#endif

    return (LPBYTE) dsd.lpSurface + x + (y * mPitch);
}

void
CGameDDrawBitBuffer::Unlock()
{
    mpSurface->Unlock( NULL );
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameDDrawScreenBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
// construct the screen's primary surface
CGameDDrawScreenBuffer::CGameDDrawScreenBuffer(
        ) : CGameDDrawBitBuffer(),
        mpPalette( NULL ),
        mpOldPalette( NULL )
{
    InitDDraw();

    // call our create surface to create the primary surface
    CreateSurface( 0,0 );
}   

#if 0
DWORD CALLBACK EnumCallback(LPVOID lpContext, LPDDSURFACEDESC lpDDrawSurfaceInfo)
{
    if (lpDDrawSurfaceInfo->ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)
    {
        ((CGameDDrawScreenBuffer*) lpContext)->mpSurface = lpDDrawSurfaceInfo->lpDDSurface;
        ((CGameDDrawScreenBuffer*) lpContext)->mSurfD = *lpDDrawSurfaceInfo;
        ((CGameDDrawScreenBuffer*) lpContext)->mPitch = lpDDrawSurfaceInfo->lPitch;
        return 0;   // stop enum'ing (?)
    }
    return 1;
} /* EnumCallback */
#endif

// create an attached-buffer object from given primary surface
CGameDDrawScreenBuffer::CGameDDrawScreenBuffer(
    CGameDDrawScreenBuffer* pFront
    ) : CGameDDrawBitBuffer(),
        mpPalette( NULL ),
        mpOldPalette( NULL )
{
    HRESULT result;
#if 1 // GetAttachedSurface isn't working
    DDSCAPS caps;

    memset(&caps, 0, sizeof(caps));
//  caps.dwSize = sizeof(caps);
    caps.dwCaps = DDSCAPS_BACKBUFFER;

    result = pFront->mpSurface->GetAttachedSurface(
                &caps,
                &mpSurface);

    pFront->mpSurface->GetSurfaceDesc(
                &mSurfD
                );

    mPitch = mSurfD.lPitch;
#else

    FARPROC pproc;
    pproc = MakeProcInstance( (FARPROC) EnumCallback, ghInst );
    
    result = mpDDrawDriver->EnumSurfaces(
        DDENUMSURFACES_DOESEXIST | DDENUMSURFACES_ALL,
        0,
        NULL,
//      (LPVOID) &mpSurface,
        (LPVOID) this,
        (LPDDENUMSURFACESCALLBACK) pproc );
    FreeProcInstance( pproc );
#endif

    if (mpSurface)
        mIsValid = TRUE;

    // flag this as an attached surface so we won't release it
    mIsAttached = TRUE;
    ++mInstanceCount;   // keep count so we know when to release driver
}

void
CGameDDrawScreenBuffer::CreateSurface(
    int width,
    int height
    )
{
    HRESULT result;

    mpDDrawDriver->SetExclusiveModeOwner(DDSEMO_FULLSCREEN, TRUE);
#if 1
    // first, force display into our 640x480, 8bpp mode
    result = mpDDrawDriver->SetDisplayMode(
                        SCREEN_WIDTH,
                        SCREEN_HEIGHT,
                        8
                        );
#endif


    memset( &mSurfD, 0, sizeof( mSurfD ) );

    mSurfD.dwSize = sizeof( mSurfD );
    mSurfD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

    mSurfD.dwFlags |= DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
    if( gDoubleBuffer )
    {
        mSurfD.dwBackBufferCount = 1;
    }
    else
    {
        mSurfD.dwBackBufferCount = 2;
    }


    // ask DirectDraw for the surface
    if ((result = mpDDrawDriver->CreateSurface(&mSurfD)) ==
        DD_OK)
    {
        mpSurface = mSurfD.lpDDSurface;
        mPitch = mSurfD.lPitch;
        mIsValid = TRUE;
    }

    DB_BREAK_IF( !mIsValid );
}

CGameDDrawScreenBuffer::~CGameDDrawScreenBuffer()
{
    // free up palette
    DeleteDDPalette();

    // restore display mode
//  mpDDrawDriver->RestoreDisplayMode();
    mpDDrawDriver->SetExclusiveModeOwner(DDSEMO_FULLSCREEN, FALSE);
}   

void
CGameDDrawScreenBuffer::SetMode(
    int width,
    int height,
    int bits
    )
{
    mpDDrawDriver->SetDisplayMode(
                        width,
                        height,
                        bits
                        );
}

void
CGameDDrawScreenBuffer::RestoreMode()
{
    mpDDrawDriver->FlipToGDISurface();
    mpDDrawDriver->RestoreDisplayMode();
}

void
CGameDDrawScreenBuffer::SetPalette(
    LPPALETTEENTRY pPal
    )
{
    // grab current palette if we don't already have it
    if (mpOldPalette == NULL)
    {
        mpSurface->GetPalette( &mpOldPalette );
    }

    // free up current palette
    if (mpPalette)
    {
        DeleteDDPalette();
    }

    if ((mpDDrawDriver->CreatePalette(
                DDPCAPS_8BIT,
                &mpPalette,
                pPal
                ) == DD_OK) && mpSurface)
    {
        mpSurface->SetPalette( mpPalette );
    }
    
}

void
CGameDDrawScreenBuffer::DeleteDDPalette()
{
    if (mpPalette)
    {
        // reset to old palette if we have it
        if (mpOldPalette && mpSurface)
        {
            mpSurface->SetPalette( mpOldPalette );
        }

        if (mpPalette->Release() == DD_OK)
            mpPalette = NULL;
    }
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameMonoMask
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
// construct an "empty" buffer with given size
CGameMonoMask::CGameMonoMask(
        int width,
        int height
        ) : CGameGDIBitBuffer()
{
    // create a memory DC compatible with the current screen
    HDC hdc = GetDC(NULL);

    if (hdc)
    {
        mhdcMem = CreateCompatibleDC( hdc );

        mhbm = CreateBitmap( width, height, 1, 1, NULL );
//      mhbm = CreateCompatibleBitmap( hdc, width, height );

        mhOldBitmap = SelectObject(mhdcMem, mhbm);

        ReleaseDC(NULL, hdc);
    }
}

CGameMonoMask::~CGameMonoMask()
{
    
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameDSBitBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
// construct a buffer from existing DIB, optionally stretching to fit new size
CGameDSBitBuffer::CGameDSBitBuffer(
        CGameDIB* pDIB,
        int width,      // 0 (default) means use DIB's width
        int height,     // 0 (default) means use DIB's height
        COLORREF transColor
        ) : CGameBitBuffer(pDIB, width, height, transColor),
            mpBitmapInfo( NULL ),
            mpDIB( NULL ),
            mpBits( NULL ),
            mIsValid( FALSE )
{
    if (width == 0)
        width = pDIB->GetWidth();

    if (height == 0)
        height = pDIB->GetHeight();

    mhPalette = pDIB->CreatePalette();

    // create our empty DIBSection
    mpDIB = new CGameDIB( width, height, mhPalette );
    SetBits( pDIB );    // copy DIB bits onto surface

    // cache important ptrs for blting
    mpBits = mpDIB->GetBits();
    mpBitmapInfo = mpDIB->GetBitmapInfo();
    mIsValid = TRUE;
}   

// construct an "empty" buffer with given size
CGameDSBitBuffer::CGameDSBitBuffer(
        int width,
        int height,
        HPALETTE hPal,
        COLORREF transColor
        ) : CGameBitBuffer(width, height, hPal, transColor),
            mpBitmapInfo( NULL ),
            mpDIB( NULL ),
            mpBits( NULL ),
            mIsValid( FALSE )
{
    mpDIB = new CGameDIB( width, height, hPal );

    // cache important ptrs for blting
    mpBits = mpDIB->GetBits();
    mpBitmapInfo = mpDIB->GetBitmapInfo();
    mIsValid = TRUE;
}

void
CGameDSBitBuffer::SetBits(
    CGameDIB* pSource
    )
{
    LPBYTE      lpdest;
    DWORD       bytes_scanline;
    LPBYTE      lpdib_bits;

    bytes_scanline = pSource->BytesPerScanline();

    for( int y=0;y<pSource->GetHeight();y++ )
    {
        lpdib_bits = pSource->GetPixelAddress(0,y);
        lpdest = mpDIB->GetPixelAddress(0,y);
        CopyMemory( lpdest, lpdib_bits, bytes_scanline );
    }
}   

CGameDSBitBuffer::~CGameDSBitBuffer()
{
    delete mpDIB;
}   

// specific blt call to this buffer from another DS buffer
void CGameDSBitBuffer::Blt(
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
    int scanDest = -(mpDIB->BytesPerScanline());
    int scanSrc = -(((CGameDSBitBuffer*)pSrcBuffer)->mpDIB->BytesPerScanline());

    CopyDIBBits(
        mpDIB->GetPixelAddress( xDest, yDest ),
        ((CGameDSBitBuffer*)pSrcBuffer)->mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc
        );
}   

// generic dest blt call -- determine type of source buffer
void
CGameDSBitBuffer::Blt(
    CGameBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    switch (pDestBuffer->TypeID())
    {
        case BB_DDraw:
            Blt(
                (CGameDDrawBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                rop
                );
            break;
        case BB_DS:
            Blt(
                (CGameDSBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                rop
                );
            break;
        default:
            DB_BREAK();
            break;
    }
}

// generic transblt call -- determine type of dest buffer
void
CGameDSBitBuffer::TransBlt(
    CGameBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    switch (pDestBuffer->TypeID())
    {
        case BB_DDraw:
            TransBlt(
                (CGameDDrawBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                transColor
                );
            break;
        case BB_DS:
            TransBlt(
                (CGameDSBitBuffer*)pDestBuffer,
                xDest,
                yDest,
                wDest,
                hDest,
                xSrc,
                ySrc,
                transColor
                );
            break;
        default:
            DB_BREAK();
            break;
    }
}

// specific blt call for a DS buffer
void CGameDSBitBuffer::BltDS(
    CGameDSBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    int scanDest = -(pDestBuffer->mpDIB->BytesPerScanline());
    int scanSrc = -(mpDIB->BytesPerScanline());

    CopyDIBBits(
        pDestBuffer->mpDIB->GetPixelAddress( xDest, yDest ),
        mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc
        );
}   

// specific transblt call for a DDraw buffer
void
CGameDSBitBuffer::TransBltDS(
    CGameDSBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    int scanDest = -(pDestBuffer->mpDIB->BytesPerScanline());
    int scanSrc = -(mpDIB->BytesPerScanline());

    TransCopyDIBBits(
        pDestBuffer->mpDIB->GetPixelAddress( xDest, yDest ),
        mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc,
        transColor
        );
}

// specific blt call for a DDraw buffer
void CGameDSBitBuffer::BltDDraw(
    CGameDDrawBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    DWORD rop
    )
{
    LPBYTE lpdest;

    lpdest = pDestBuffer->GetLockedAddress( xDest, yDest );

    int scanDest = pDestBuffer->mPitch;
    int scanSrc = -(mpDIB->BytesPerScanline());

    CopyDIBBits(
        lpdest,
        mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc
        );

    pDestBuffer->Unlock();
}

// specific transblt call for a DDraw buffer
void
CGameDSBitBuffer::TransBltDDraw(
    CGameDDrawBitBuffer* pDestBuffer,
    int xDest,
    int yDest,
    int wDest,
    int hDest,
    int xSrc,
    int ySrc,
    int transColor
    )
{
    LPBYTE lpdest = pDestBuffer->GetLockedAddress( xDest, yDest );

    int scanDest = pDestBuffer->mPitch;
    int scanSrc = -(mpDIB->BytesPerScanline());

    TransCopyDIBBits(
        lpdest,
        mpDIB->GetPixelAddress( xSrc, ySrc ),
        wDest,  // width pixels
        hDest,
        (DWORD) scanDest,
        (DWORD) scanSrc,
        transColor
        );

    pDestBuffer->Unlock();
}

void
CGameDSBitBuffer::SetPalette( HPALETTE hPal )
{
//  SelectPalette( mhdcMem, hPal, FALSE );
//  RealizePalette( mhdcMem );
}

void
TransCopyDIBBits(
        LPBYTE pDest,       // destination
        LPBYTE pSource,     //         ; source pointer
        DWORD dwWidth,      //         ; width pixels
        DWORD dwHeight,     //        ; height pixels
        DWORD dwScanD,      //         ; width bytes dest
        DWORD dwScanS,      //         ; width bytes source
        BYTE bTranClr       //        ; transparent color
    )
{
    _asm
    {
        push ds
        push esi
        push edi

        mov ecx, dwWidth
        or ecx,ecx
        jz tcdb_nomore     ; test for silly case

        mov edx, dwHeight       ; EDX is line counter
        mov ah, bTranClr        ; AL has transparency color

//        xor esi, esi
//        lds si, pSource         ; DS:[ESI] point to source
        mov esi, pSource         ; DS:[ESI] point to source


//        mov es, DestSelector    ; es:[edi] point to dest

//        mov edi, DestOffset
        mov edi, pDest

        sub dwScanD,ecx         ; bias these
        sub dwScanS,ecx

        mov ebx,ecx             ; save this for later

        align 4

tcdb_morelines:
        mov ecx, ebx            ; ECX is pixel counter
        shr ecx,2
        jz  short tcdb_nextscan

// ;
// ; The idea here is to not branch very often so we unroll the loop by four
// ; and try to not branch when a whole run of pixels is either transparent
// ; or not transparent.
// ;
// ; There are two loops. One loop is for a run of pixels equal to the
// ; transparent color, the other is for runs of pixels we need to store.
// ;
// ; When we detect a "bad" pixel we jump to the same position in the
// ; other loop.
// ;
// ; Here is the loop we will stay in as long as we encounter a "transparent"
// ; pixel in the source.
// ;

        align 4

tcdb_same:
        mov al, ds:[esi]
        cmp al, ah
        jne short tcdb_diff0

tcdb_same0:
        mov al, ds:[esi+1]
        cmp al, ah
        jne short tcdb_diff1

tcdb_same1:
        mov al, ds:[esi+2]
        cmp al, ah
        jne short tcdb_diff2

tcdb_same2:
        mov al, ds:[esi+3]
        cmp al, ah
        jne short tcdb_diff3

tcdb_same3:
        add edi,4
        add esi,4
        dec ecx
        jnz short tcdb_same
        jz  short tcdb_nextscan

// ;
// ; Here is the loop we will stay in as long as 
// ; we encounter a "non transparent" pixel in the source.
// ;

        align 4

tcdb_diff:
        mov al, ds:[esi]
        cmp al, ah
        je short tcdb_same0

tcdb_diff0:
        mov es:[edi],al
        mov al, ds:[esi+1]
        cmp al, ah
        je short tcdb_same1

tcdb_diff1:
        mov es:[edi+1],al
        mov al, ds:[esi+2]
        cmp al, ah
        je short tcdb_same2

tcdb_diff2:
        mov es:[edi+2],al
        mov al, ds:[esi+3]
        cmp al, ah
        je short tcdb_same3

tcdb_diff3:
        mov es:[edi+3],al

        add edi,4
        add esi,4
        dec ecx
        jnz short tcdb_diff
        jz  short tcdb_nextscan

// ;
// ; We are at the end of a scan, check for odd leftover pixels to do
// ; and go to the next scan.
// ;

        align 4

tcdb_nextscan:
        mov ecx,ebx
        and ecx,11b
        jnz short tcdb_oddstuff
        // ; move on to the start of the next line

tcdb_nextscan1:
        add esi, dwScanS
        add edi, dwScanD

        dec edx                 // ; line counter
        jnz short tcdb_morelines
        jz  short tcdb_nomore

// ;
// ; If the width is not a multiple of 4 we will come here to clean up
// ; the last few pixels
// ;

tcdb_oddstuff:
        inc ecx
tcdb_oddloop:
        dec ecx
        jz  short tcdb_nextscan1
        mov al, ds:[esi]
        inc esi
        inc edi
        cmp al, ah
        je  short tcdb_oddloop
        mov es:[edi-1],al
        jmp short tcdb_oddloop

tcdb_nomore:
        pop edi
        pop esi
        pop ds
    }
}

void
CopyDIBBits(
        LPBYTE pDest,   //           ; dest pointer
        LPBYTE pSource, //         ; source pointer
        DWORD dwWidth,  //         ; width pixels
        DWORD dwHeight, //        ; height pixels
        DWORD dwScanD,  //         ; width bytes dest
        DWORD dwScanS   //         ; width bytes source
    )
{
    _asm
    {
        push ds
        push esi
        push edi

        mov ecx, dwWidth
        or ecx,ecx
        jz cdb_nomore     ; test for silly case

        mov edx, dwHeight       ; EDX is line counter
        or edx,edx
        jz cdb_nomore     ; test for silly case

//        xor esi, esi
//        lds si, pSource         ; DS:[ESI] point to source
        mov esi, pSource         ; DS:[ESI] point to source

//        xor edi, edi
//        les di, pDest           ; ES:[EDI] point to dest
        mov edi, pDest           ; ES:[EDI] point to dest

        sub dwScanD,ecx         ; bias these
        sub dwScanS,ecx

        mov ebx,ecx
        shr ebx,2

        mov eax,ecx
        and eax,11b

        align 4

cdb_loop:
        mov ecx, ebx
        rep movs dword ptr es:[edi], dword ptr ds:[esi]
        mov ecx,eax
        rep movs byte ptr es:[edi], byte ptr ds:[esi]

        add esi, dwScanS
        add edi, dwScanD
        dec edx                 ; line counter
        jnz short cdb_loop

cdb_nomore:
        pop edi
        pop esi
        pop ds
    }
}

