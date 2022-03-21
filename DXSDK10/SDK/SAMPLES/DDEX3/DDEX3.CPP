/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddex3.cpp
 *  Content:    Direct Draw example program 3.  Adds functionality to 
 *              example program 2.  Creates two offscreen surfaces in 
 *              addition to the primary surface and back buffer.  Loads
 *              a bitmap file into each offscreen surface.  Uses BltFast
 *              to copy the contents of an offscreen surface to the back
 *              buffer and then flips the buffers and copies the next 
 *              offscreen surface to the back buffer.  Press F12 to exit
 *              the program.  This program requires at least 1.2 Megs of 
 *              video ram.
 *
 ***************************************************************************/

#define NAME "DDExample3"
#define TITLE "Direct Draw Example 3"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"

BOOL readBMPIntoSurfaces( void );

#define TIMER_ID        1

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
LPDIRECTDRAWSURFACE     lpDDSOne;       // Offscreen surface 1
LPDIRECTDRAWSURFACE     lpDDSTwo;       // Offscreen surface 2
HRESULT                 ddrval;         // Holds result of DD calls

/*
 * finiObjects
 *
 * finished with all objects we use; release them
 */
static void finiObjects( void )
{
    if( lpDD != NULL )
    {
        if( lpDDSPrimary != NULL )
        {
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        lpDD->Release();
        lpDD = NULL;
    }
} /* finiObjects */

long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    static  int phase = 0;
    RECT        rcRect;

    switch( message )
    {
    case WM_CREATE:
        break;

    case WM_TIMER:
        rcRect.left = 0;
        rcRect.top = 0;
        rcRect.right = 640;
        rcRect.bottom = 480;
        if(phase)
        {
            while( (ddrval = lpDDSBack->BltFast( 0, 0, lpDDSTwo, &rcRect,
                        FALSE ) ) == DDERR_WASSTILLDRAWING );
            phase = 0;
        }
        else
        {
            while( (ddrval = lpDDSBack->BltFast( 0, 0, lpDDSOne, &rcRect,
                        FALSE ) ) == DDERR_WASSTILLDRAWING );
            phase = 1;
        }

        // Flip surfaces
        while(lpDDSPrimary->Flip( NULL ) == DDERR_WASSTILLDRAWING);
        break;
 
    case WM_KEYDOWN:
        switch( wParam )
        {
        case VK_ESCAPE:
        case VK_F12:
            DestroyWindow( hWnd );
            break;
        }
        break;

    case WM_DESTROY:
        ShowCursor( TRUE );
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0L;
} /* WindowProc */

/*
 * This function is called if the initialization function fails
 */
BOOL initFail( HWND hwnd )
{
    MessageBox( NULL, "DirectDraw Init FAILED", "ERROR", MB_OK );
    finiObjects();
    DestroyWindow( hwnd );
    return FALSE;
} /* initFail */

/*
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL doInit( HANDLE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
    DDSURFACEDESC       ddsd;
    DDSCAPS             ddscaps;

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass( &wc );
    
    /*
     * create a window
     */
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        NAME,
        TITLE,
        WS_POPUP,
        0,
        0,
        640,
        480,
        NULL,
        NULL,
        hInstance,
        NULL );

    if( !hwnd )
        return FALSE;

    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );
    SetFocus( hwnd );

    ShowCursor( FALSE );

    /*
     * create the main DirectDraw object
     */
    ddrval = DirectDrawCreate( NULL, &lpDD );
    if( ddrval != DD_OK )
        return initFail(hwnd);

    // Get exclusive mode
    ddrval = lpDD->SetExclusiveModeOwner( DDSEMO_FULLSCREEN, TRUE );
    if(ddrval != DD_OK )
        return initFail(hwnd);

    // Set the video mode to 640x480x8
    ddrval = lpDD->SetDisplayMode( 640, 480, 8);
    if(ddrval != DD_OK)
        return initFail(hwnd);

    // Create the primary surface with 1 back buffer
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                          DDSCAPS_FLIP |
                          DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;
    ddrval = lpDD->CreateSurface( &ddsd );
    if( ddrval != DD_OK )
        return initFail(hwnd);

    lpDDSPrimary = ddsd.lpDDSurface;
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
    ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, &lpDDSBack);
    if( ddrval != DD_OK )
        return initFail(hwnd);

    // Create the primary surface with 1 back buffer
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwHeight = 480;
    ddsd.dwWidth = 640;
    ddrval = lpDD->CreateSurface( &ddsd );
    if( ddrval != DD_OK )
        return initFail(hwnd);

    lpDDSOne = ddsd.lpDDSurface;
    ddrval = lpDD->CreateSurface( &ddsd );
    if( ddrval != DD_OK )
        return initFail(hwnd);
    lpDDSTwo = ddsd.lpDDSurface;

    if(!readBMPIntoSurfaces())
        return initFail(hwnd);

    // Create a timer to flip the pages
    if( !SetTimer( hwnd, TIMER_ID, 500, NULL ) )
        return initFail(hwnd);

    return TRUE;
} /* doInit */

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

    if( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    while( 1 )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( !GetMessage( &msg, NULL, 0, 0 ) )
                return msg.wParam;
            TranslateMessage(&msg); 
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;

} /* WinMain */

/*
 * openBMP
 */
static HANDLE openBMP( void )
{
    HANDLE      hFile;
    char        *paths[] = { "", "..\\", "..\\samples\\ddex3\\" };

    for( int i=0;i<sizeof( paths )/sizeof( paths[0] );i++ )
    {
        char    szNewPath[MAX_PATH];
        strcpy( szNewPath, paths[i] );
        strcat( szNewPath, "FRNTBACK.BMP" );
        hFile = CreateFile(szNewPath,
                           GENERIC_READ,
                           FILE_SHARE_READ,
                           (LPSECURITY_ATTRIBUTES) NULL,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           (HANDLE) NULL);
        if( hFile != INVALID_HANDLE_VALUE)
        {
            return hFile;
        }
    }
    MessageBox( NULL, "Open FRNTBACK.BMP Failed", "ERROR", MB_OK );
    return INVALID_HANDLE_VALUE;

} /* openBMP */

/*
 * readBmpIntoSurfaces - This function reads the bitmap file FRNTBACK.BMP
 * and stores half of it in offscreen surface 1 and the other half in 
 * offscreen surface 2.  It also sets the palette associated with the
 * front buffer to the palette of the bitmap file.
 */
BOOL readBMPIntoSurfaces( void )
{
    LPBYTE              Image;          // Holds bitmap data
    HANDLE              hFile;
    BITMAPFILEHEADER    BMPFileHead;
    BITMAPINFOHEADER    BMPFileInfo;
    RGBQUAD             Palette[256];
    DWORD               actualRead;
    PALETTEENTRY        pe[256];
    LPDIRECTDRAWPALETTE ppal;
    DDSURFACEDESC       DDSDesc;
    LPSTR               lpBits;
    LPSTR               lpSrc;
    int                 i;

    Image = (LPBYTE) LocalAlloc( LPTR, 480*640*2 );
    if( Image == NULL )
        return FALSE;

    hFile = openBMP();
    if( hFile == INVALID_HANDLE_VALUE )
        return FALSE;

    // Read the header and info structures
    if(!ReadFile(hFile, &BMPFileHead, sizeof(BMPFileHead), &actualRead, NULL))
        return FALSE;

    if(!ReadFile(hFile, &BMPFileInfo, sizeof(BMPFileInfo), &actualRead, NULL))
        return FALSE;

    // Check to make sure this is a compatible file
    if((BMPFileInfo.biWidth != 640) ||
        (BMPFileInfo.biHeight != 480*2) ||
        (BMPFileInfo.biBitCount != 8))
        return FALSE;

    // Read the palette information
    if(!ReadFile(hFile, Palette, sizeof(Palette), &actualRead, NULL))
        return FALSE;

    // Read the image bits
    if(!ReadFile(hFile, Image, 640*480*2, &actualRead, NULL))
        return FALSE;

    CloseHandle(hFile);

    // Create a Direct Draw Palette and associate it with the front buffer
    for(i=0; i<256; i++)
    {
        pe[i].peRed = Palette[i].rgbRed;
        pe[i].peGreen = Palette[i].rgbGreen;
        pe[i].peBlue = Palette[i].rgbBlue;
    }

    ddrval = lpDD->CreatePalette( DDPCAPS_8BIT, &ppal, pe );
    if(ddrval != DD_OK)
        return FALSE;

    lpDDSPrimary->SetPalette( ppal );

    DDSDesc.dwSize = sizeof(DDSDesc);
    ddrval = lpDDSOne->Lock(NULL, &DDSDesc, 0, NULL);
    if(ddrval != DD_OK)
        return FALSE;

    // Copy bitmap bits from system memory to video memory
    lpBits = (LPSTR)DDSDesc.lpSurface;
    lpSrc = (LPSTR)(&Image[479*640]);
    for(i=0; i<480; i++)
    {
        memcpy( lpBits, lpSrc, 640 );
        lpBits += DDSDesc.lPitch;
        lpSrc -= 640;
    }
    lpDDSOne->Unlock( NULL );

    ddrval = lpDDSTwo->Lock(NULL, &DDSDesc, 0, NULL);
    if(ddrval != DD_OK)
        return FALSE;

    // Copy bitmap bits from system memory to video memory
    lpBits = (LPSTR)DDSDesc.lpSurface;
    lpSrc = (LPSTR)(&Image[(480*2-1)*640]);
    for(i=0; i<480; i++)
    {
        memcpy( lpBits, lpSrc, 640 );
        lpBits += DDSDesc.lPitch;
        lpSrc -= 640;
    }
    lpDDSTwo->Unlock( NULL );
    LocalFree( Image );

    return TRUE;
}
