/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddex1.cpp
 *  Content:    Direct Draw example program 1.  Creates a Direct Draw 
 *              object and then a primary surface with a back buffer.
 *              Slowly flips between the primary surface and the back
 *              buffer.  Press F12 to terminate the program.
 *
 ***************************************************************************/

#define NAME "DDExample1"
#define TITLE "Direct Draw Example 1"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"

#define TIMER_ID        1

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
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

#define MSGSTR "Page Flipping Test: Press F12 to exit"

long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    switch( message )
    {
    case WM_CREATE:
        break;

    case WM_TIMER:
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
    case WM_PAINT:
        BeginPaint( hWnd, &ps );
        SetBkColor( ps.hdc, RGB( 0, 0, 255 ) );
        SetTextColor( ps.hdc, RGB( 255, 255, 0 ) );
        TextOut( ps.hdc, 0, 0, MSGSTR, sizeof( MSGSTR )-1 );
        EndPaint( hWnd, &ps );
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
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL doInit( HANDLE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
    DDSURFACEDESC       ddsd;
    DDSCAPS             ddscaps;
    HDC                 hdc;
    SIZE                size;

    hdc = GetDC( HWND_DESKTOP );
    GetTextExtentPoint32( hdc, MSGSTR, sizeof( MSGSTR )-1, &size );
    ReleaseDC( HWND_DESKTOP, hdc );

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
        (GetSystemMetrics( SM_CXSCREEN )-size.cx)/2,
        (GetSystemMetrics( SM_CYSCREEN )-size.cy)/2,
        size.cx,
        size.cy,
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
    if( ddrval == DD_OK )
    {
        // Get exclusive mode
        ddrval = lpDD->SetExclusiveModeOwner( DDSEMO_FULLSCREEN, TRUE );
        if(ddrval == DD_OK )
        {
            // Create the primary surface with 1 back buffer
            ddsd.dwSize = sizeof( ddsd );
            ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
            ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                                  DDSCAPS_FLIP |
                                  DDSCAPS_COMPLEX;
            ddsd.dwBackBufferCount = 1;
            ddrval = lpDD->CreateSurface( &ddsd );
            if( ddrval == DD_OK )
            {
                // Get a pointer to the back buffer
                lpDDSPrimary = ddsd.lpDDSurface;
                ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, 
                                                          &lpDDSBack);
                if( ddrval == DD_OK )
                {
                     // Create a timer to flip the pages
                     if( SetTimer( hwnd, TIMER_ID, 500, NULL ) )
                         return TRUE;
                }
            }
        }
    }

    MessageBox( NULL, "DirectDraw Init FAILED", "ERROR", MB_OK );
    finiObjects();
    DestroyWindow( hwnd );
    return FALSE;
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

