/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddex5.cpp
 *  Content:    Direct Draw example program 5.  Adds functionality to
 *              example program 4.  Uses GetEntries() to read a palette,
 *              modifies the entries, and then uses SetEntries() to update
 *              the palette.  This program requires 1.2 Meg of video ram.
 *
 ***************************************************************************/

#define NAME "DDExample5"
#define TITLE "Direct Draw Example 5"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"

// Xoffset and Yoffset contain the offsets for each frame in the torus
// animation.
int Xoffset[60] =
{
    0,
    64,
    128,
    192,
    256,
    320,
    384,
    448,
    512,
    576,
    0,
    64,
    128,
    192,
    256,
    320,
    384,
    448,
    512,
    576,
    0,
    64,
    128,
    192,
    256,
    320,
    384,
    448,
    512,
    576,
    0,
    64,
    128,
    192,
    256,
    320,
    384,
    448,
    512,
    576,
    0,
    64,
    128,
    192,
    256,
    320,
    384,
    448,
    512,
    576,
    0,
    64,
    128,
    192,
    256,
    320,
    384,
    448,
    512,
    576
};

int Yoffset[60] =
{
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    64,
    64,
    64,
    64,
    64,
    64,
    64,
    64,
    64,
    64,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    128,
    192,
    192,
    192,
    192,
    192,
    192,
    192,
    192,
    192,
    192,
    256,
    256,
    256,
    256,
    256,
    256,
    256,
    256,
    256,
    256,
    320,
    320,
    320,
    320,
    320,
    320,
    320,
    320,
    320,
    320,
};

BOOL readBMPIntoSurfaces( void );

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
LPDIRECTDRAWSURFACE     lpDDSOne;       // Offscreen surface 1
LPDIRECTDRAWPALETTE     lpDDPal;        // The primary surface palette
HRESULT                 ddrval;         // Holds result of DD calls

BYTE torusColors[256];  // Marks the colors used in the torus

/*
 * updateFrame
 * 
 * Decide what needs to be blitted next, wait for flip to complete,
 * then flip the buffers.
 */
void updateFrame( void )
{
    static  DWORD   lastTickCount[4] = {0,0,0,0};
    DWORD           thisTickCount;
    static  int     currentFrame[3] = {0,0,0};
    RECT            rcRect;
    DWORD           delay[4] = {50, 78, 13, 93};
    int             i;
    int             xpos[3] = {288, 190, 416};
    int             ypos[3] = {128, 300, 256};
    PALETTEENTRY    pe[256];

    // Decide which frame will be blitted next
    thisTickCount = GetTickCount();
    for(i=0; i<3; i++)
    {
        if((thisTickCount - lastTickCount[i]) > delay[i])
        {
            // Move to next frame;
            lastTickCount[i] = thisTickCount;
            currentFrame[i]++;
            if(currentFrame[i] > 59)
                currentFrame[i] = 0;
        }
    }

    // Blit the stuff for the next frame
    rcRect.left = 0;
    rcRect.top = 0;
    rcRect.right = 640;
    rcRect.bottom = 480;
    while( (ddrval = lpDDSBack->BltFast( 0, 0, lpDDSOne, &rcRect, FALSE ))
                                            == DDERR_WASSTILLDRAWING );
    if(ddrval != DD_OK)
        return;

    for(i=0; i<3; i++)
    {
        rcRect.left = Xoffset[currentFrame[i]];
        rcRect.top = Yoffset[currentFrame[i]] + 480;
        rcRect.right = Xoffset[currentFrame[i]]+64;
        rcRect.bottom = Yoffset[currentFrame[i]] + 64 + 480;
        while( (ddrval = lpDDSBack->BltFast( xpos[i], ypos[i], lpDDSOne,
                                 &rcRect, TRUE )) == DDERR_WASSTILLDRAWING );
        if(ddrval != DD_OK)
            return;
    }

    if((thisTickCount - lastTickCount[3]) > delay[3])
    {
        // Change the palette
        if(lpDDPal->GetEntries( 0, 0, 256, pe ) != DD_OK)
            return;
        
        for(i=0; i<256; i++)
        {
            if(!torusColors[i])
                continue;
            pe[i].peRed = (pe[i].peRed+2) % 256;
            pe[i].peGreen = (pe[i].peGreen+1) % 256;
            pe[i].peBlue = (pe[i].peBlue+3) % 256;
        }
            
        if(lpDDPal->SetEntries( 0, 0, 256, pe) != DD_OK)
            return;
    
        lastTickCount[3] = thisTickCount;
    }
    
    // Flip the surfaces
    while(lpDDSPrimary->Flip( NULL ) == DDERR_WASSTILLDRAWING);
}


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
        if( lpDDSOne != NULL )
        {
            lpDDSOne->Release();
            lpDDSOne = NULL;
        }
        if( lpDDPal != NULL )
        {
            lpDDPal->Release();
            lpDDPal = NULL;
        }
        lpDD->Release();
        lpDD = NULL;
    }
} /* finiObjects */

long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
    case WM_CREATE:
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
    DDCOLORKEY          ddck;

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
    ddsd.dwHeight = 864;
    ddsd.dwWidth = 640;
    ddrval = lpDD->CreateSurface( &ddsd );
    if( ddrval != DD_OK )
        return initFail(hwnd);

    lpDDSOne = ddsd.lpDDSurface;

    // Set the color key for this bitmap (black)
    ddck.dwColorSpaceLowValue = 0xff;
    ddck.dwColorSpaceHighValue = 0xff;
    lpDDSOne->SetColorKey( DDCKEY_SRCBLT, &ddck );    


    if(!readBMPIntoSurfaces())
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
        else
        {
            updateFrame();
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
    char        *paths[] = { "", "..\\", "..\\samples\\ddex5\\" };

    for( int i=0;i<sizeof( paths )/sizeof( paths[0] );i++ )
    {
        char    szNewPath[MAX_PATH];
        strcpy( szNewPath, paths[i] );
        strcat( szNewPath, "ALL.BMP" );
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
    MessageBox( NULL, "Open ALL.BMP Failed", "ERROR", MB_OK );
    return NULL;

} /* openBMP */

/*
 * readBmpIntoSurfaces - This function reads the bitmap file ALL.BMP
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
    DDSURFACEDESC       DDSDesc;
    LPSTR               lpBits;
    LPSTR               lpSrc;
    int                 i, x, y;

    Image = (LPBYTE) LocalAlloc( LPTR, 864*640 );
    if( Image == NULL )
        return NULL;

    hFile = openBMP();
    if( hFile == NULL )
        return FALSE;

    // Read the header and info structures
    if(!ReadFile(hFile, &BMPFileHead, sizeof(BMPFileHead), &actualRead, NULL))
        return FALSE;

    if(!ReadFile(hFile, &BMPFileInfo, sizeof(BMPFileInfo), &actualRead, NULL))
        return FALSE;

    // Check to make sure this is a compatible file
    if((BMPFileInfo.biWidth != 640) ||
        (BMPFileInfo.biHeight != 864) ||
        (BMPFileInfo.biBitCount != 8))
        return FALSE;

    // Read the palette information
    if(!ReadFile(hFile, Palette, sizeof(Palette), &actualRead, NULL))
        return FALSE;

    // Read the image bits
    if(!ReadFile(hFile, Image, 864*640, &actualRead, NULL))
        return FALSE;

    CloseHandle(hFile);

    // Create a Direct Draw Palette and associate it with the front buffer
    for(i=0; i<256; i++)
    {
        pe[i].peRed = Palette[i].rgbRed;
        pe[i].peGreen = Palette[i].rgbGreen;
        pe[i].peBlue = Palette[i].rgbBlue;
    }

    ddrval = lpDD->CreatePalette( DDPCAPS_8BIT, &lpDDPal, pe );
    if(ddrval != DD_OK)
        return FALSE;

    lpDDSPrimary->SetPalette( lpDDPal );

    DDSDesc.dwSize = sizeof(DDSDesc);
    ddrval = lpDDSOne->Lock(NULL, &DDSDesc, 0, NULL);
    if(ddrval != DD_OK)
        return FALSE;

    // Copy bitmap bits from system memory to video memory
    lpBits = (LPSTR)DDSDesc.lpSurface;
    lpSrc = (LPSTR)(&Image[863*640]);
    for(i=0; i<864; i++)
    {
        memcpy( lpBits, lpSrc, 640 );
        lpBits += DDSDesc.lPitch;
        lpSrc -= 640;
    }
    lpDDSOne->Unlock( NULL );

    // Mark the colors used in the torus frames
    // First, set all colors as unused
    for(i=0; i<256; i++)
        torusColors[i] = 0;

    // Now search through the torus frames and mark used colors
    for(y=0; y<384; y++)
        for(x=0; x<640; x++)
            torusColors[Image[y*640+x]] = 1;
    
    return TRUE;
}
