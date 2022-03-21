/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *  Copyright (C) 1994-1995 ATI Technologies Inc. All Rights Reserved.
 *
 *  File:       ddraw.c
 *  Content:    Misc. Direct Draw access routines
 *
 ***************************************************************************/
#include "foxbear.h"

BOOL    bDoubleBuffer;

/*
 * DDEnable
 */
BOOL DDEnable( void )
{
    LPDIRECTDRAW        lpdd;
    HRESULT             ddrval;

    bDoubleBuffer = GetProfileInt( "FoxBear", "doublebuffer", 0 );
    
    ddrval = DirectDrawCreate( NULL, &lpdd );
    if( ddrval == DD_OK )
    {
        /*
         * grab exclusive mode
         */
        ddrval = lpdd->lpVtbl->SetExclusiveModeOwner( lpdd, DDSEMO_FULLSCREEN,
                                                        TRUE );
        if( ddrval == DD_OK )
        {
            /*
             * set to 640x480x8
             */
            ddrval = lpdd->lpVtbl->SetDisplayMode( lpdd, C_SCREEN_W,
                                                    C_SCREEN_H, 8 );
            if( ddrval == DD_OK )
            {
                /*
                 * make sure we have enough free vidmem to run
                 */
                DDCAPS  ddcaps;
                ddcaps.dwSize = sizeof( ddcaps );
                ddrval = lpdd->lpVtbl->GetCaps( lpdd, &ddcaps );
                if( ddrval == DD_OK )
                {
                    if( ddcaps.dwVidMemFree > 1662000l )
                    {
                        lpDD = lpdd;
                        return TRUE;
                    }
                    MessageBox( NULL, "Requires 1662000l of free VRAM to run", "FOXBEAR", MB_OK );
                }
                else
                {
                    MessageBox( NULL, "GetCaps FAILED", "FOXBEAR", MB_OK );
                }
            }
            else
            {
                MessageBox( NULL, "ModeSet FAILED", "FOXBEAR", MB_OK );
            }
        }
        else
        {
                MessageBox( NULL, "SetExclusiveModeOwner FAILED", "FOXBEAR", MB_OK );
        }
    }
    else
    {
        MessageBox( NULL, "DirectDrawCreate FAILED", "FOXBEAR", MB_OK );
    }
    return FALSE;

} /* DDEnable */

/*
 * DDCreateFlippingSurface
 */
BOOL DDCreateFlippingSurface( void )
{
    DDSURFACEDESC       ddsd;
    HRESULT             ddrval;
    DDSCAPS             ddcaps;
    extern HWND         hWndMain;

    /*
     * fill in surface desc:
     * want a primary surface with 2 back buffers
     */
    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );

    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                        DDSCAPS_FLIP |
                        DDSCAPS_COMPLEX;

    if( bDoubleBuffer )
    {
        ddsd.dwBackBufferCount = 1;
    }
    else
    {
        ddsd.dwBackBufferCount = 2;
    }

    /*
     * create the surface
     */
    ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd );
    if( ddrval != DD_OK )
    {
        Msg( "CreateSurface FAILED! %08lx", ddrval );
        return FALSE;
    }

    lpFrontBuffer = ddsd.lpDDSurface;

    /*
     * attach our window to the front buffer
     */
    ddrval = lpFrontBuffer->lpVtbl->SetHWnd( lpFrontBuffer, hWndMain );
    if( ddrval != DD_OK )
    {
        Msg( "SetHWnd FAILED! %08lx", ddrval );
        return FALSE;
    }

    /*
     * go find the back buffer
     */
    ddcaps.dwCaps = DDSCAPS_BACKBUFFER;
    ddrval = lpFrontBuffer->lpVtbl->GetAttachedSurface(
                lpFrontBuffer,
                &ddcaps,
                &lpBackBuffer );

    if( ddrval != DD_OK )
    {
        Msg( "GetAttachedSurface FAILED! %08lx", ddrval );
        return FALSE;
    }

    return TRUE;

} /* DDCreateFlippingSurface */

/*
 * DDCreateSurface
 */
LPDIRECTDRAWSURFACE DDCreateSurface( DWORD width, DWORD height )
{
    DDSURFACEDESC       ddsd;
    HRESULT             ddrval;
    DDCOLORKEY          ddck;
    LPDIRECTDRAWSURFACE psurf;
    
    
    /*
     * fill in surface desc
     */
    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    
    ddsd.dwHeight = height;
    ddsd.dwWidth = width;

    ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd );
    
    /*
     * set the color key for this bitmap (always black)
     */
    if( ddrval == DD_OK )
    {
        psurf = ddsd.lpDDSurface;
        ddck.dwColorSpaceLowValue = 0xff;
        ddck.dwColorSpaceHighValue = 0xff;
        psurf->lpVtbl->SetColorKey( psurf, DDCKEY_SRCBLT, &ddck);
    }
    else
    {
        Msg( "CreateSurface FAILED, rc = %ld", (DWORD) LOWORD( ddrval ) );
        psurf = NULL;
    }

     return psurf;

} /* DDCreateSurface */

/*
 * ReadPalFile
 *
 * Create a DirectDrawPalette from a palette file
 */
LPDIRECTDRAWPALETTE ReadPalFile( char *fname )
{
    int                 fh;
    WORD                i;
    LPPALETTEENTRY      ppe;
    HRESULT             ddrval;
    unsigned char       pRgb[4];
    LPDIRECTDRAWPALETTE ppal;

    fh = _lopen( fname, OF_READ);
    if( fh == -1 )
    {
        Msg("Can't open palette file.");
        return FALSE;
    }

    ppe = LocalAlloc(LPTR, 256*sizeof(PALETTEENTRY) );
    if( ppe == NULL )
    {
        return NULL;
    }

    _llseek(fh, 24, SEEK_SET);  

     for( i=0; i<255; i++ )
     {
        if( sizeof (pRgb) != _lread (fh, pRgb, sizeof (pRgb)))
        {
            LocalFree( ppe );
            return FALSE;
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
     
    ddrval = lpDD->lpVtbl->CreatePalette(
                            lpDD,
                            DDPCAPS_8BIT,
                            &ppal,
                            ppe );
    LocalFree( ppe );
    if( ddrval != DD_OK )
    {
        return NULL;
    }
    return ppal;

} /* ReadPalFile */


/*
 * Splash
 *
 * Draw a splash screen during startup
 * For the time being, just fill the screen with black
 */
void Splash( void )
{
    DDBLTFX     ddbltfx;

    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = 0;
    lpFrontBuffer->lpVtbl->Blt(
                        lpFrontBuffer,          // dest surface
                        NULL,                   // dest rect
                        NULL,                   // src surface
                        NULL,                   // src rect
                        DDBLT_COLORFILL,
                        &ddbltfx );

} /* Splash */


/*
 * MEMORY ALLOCATION ROUTINES...
 */
DWORD   dwMemTotal;

LPVOID MemAlloc( UINT size )
{
    LPVOID      ptr;

    ptr = LocalAlloc( LPTR, size );
    if( ptr != NULL )
    {
        dwMemTotal += size;
    }
    return ptr;
}

LPVOID CMemAlloc( UINT cnt, UINT isize )
{
    DWORD       size;
    LPVOID      ptr;

    size = cnt * isize;
    ptr = LocalAlloc( LPTR, size );
    if( ptr != NULL  ){
        dwMemTotal += size;
    }
    return ptr;
}

void MemFree( LPVOID ptr )
{
    if( ptr != NULL )
    {
        LocalFree( ptr );
    }
}
