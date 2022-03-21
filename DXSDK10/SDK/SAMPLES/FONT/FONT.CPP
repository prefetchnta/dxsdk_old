/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       font.cpp
 *  Content:    Draw a GDI font on a DirectDraw surface
 *
 ***************************************************************************/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "resource.h"

#define TIMER_ID        1
#define WIDTHBYTES(i)   ((i+31)/32*4)
#define PALETTE_SIZE    2
#define FULL_STRING     "Text (0000,0000)"

char                    szClass[]   = "DDFontClass";
char                    szCaption[] = "DirectDraw Font Test";

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface

LPSTR                   lpBmpBits;      // pointer to DIB bits
HDC                     hMemDC;         // memory DC for rendering text
HFONT                   hFont;          // font we render text in
HBITMAP                 hBitmap;        // bitmap for holding text

int                     iBmpHeight;     // height of DIB
int                     iBmpWidth;      // width of DIB


int                     iWinPosX;       // X position of client area of window
int                     iWinPosY;       // Y position of client area of window
int                     iWinWidth;      // width of client area of window
int                     iWinHeight;     // height of client area of window

int                     iScreenWidth;   // width of display
int                     iScreenHeight;  // height of display

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
    if( hMemDC != NULL )
    {
        DeleteDC( hMemDC );
        hMemDC = NULL;
    }
    if( hBitmap != NULL )
    {
        DeleteObject( hBitmap );
        hBitmap = NULL;
    }
    if( hFont != NULL )
    {
        DeleteObject( hFont );
        hFont = NULL;
    }

} /* finiObjects */

/*
 * initDC
 *
 * Creates a DC with an Arial font and a DIB big enough to hold
 * some text selected into it.
 */
static BOOL initDC( void )
{
    HDC                 hdc;
    LPBITMAPINFO        pbmi;
    SIZE                size;

    hFont = CreateFont(
        24,
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        VARIABLE_PITCH,
        "Arial" );

    if( hFont == NULL )
    {
        return FALSE;
    }

    /*
     * create a memory DC for rendering our text into
     */
    hdc = GetDC( HWND_DESKTOP );
    hMemDC = CreateCompatibleDC( hdc );
    ReleaseDC( NULL, hdc );
    if( hMemDC == NULL )
    {
        DeleteObject( hFont );
        return FALSE;
    }

    /*
     * select font, and get text dimensions
     */
    SelectObject( hMemDC, hFont );
    GetTextExtentPoint32( hMemDC, FULL_STRING, sizeof( FULL_STRING ) -1, &size );
    iBmpWidth = size.cx+2;
    iBmpHeight = size.cy+2;

    /*
     * create a dib section for containing the bits
     */
    pbmi = (LPBITMAPINFO) LocalAlloc( LPTR, sizeof( BITMAPINFO ) + PALETTE_SIZE * sizeof( RGBQUAD ) );
    if( pbmi == NULL )
    {
        DeleteObject( hFont );
        DeleteDC( hMemDC );
        return FALSE;
    }
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = iBmpWidth;
    pbmi->bmiHeader.biHeight = -1 * iBmpHeight; // negative height = top-down
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biBitCount = 8;             // 8bpp makes it easy to get data

    pbmi->bmiHeader.biCompression = BI_RGB;
    pbmi->bmiHeader.biXPelsPerMeter = 0;
    pbmi->bmiHeader.biYPelsPerMeter = 0;
    pbmi->bmiHeader.biClrUsed = PALETTE_SIZE;
    pbmi->bmiHeader.biClrImportant = PALETTE_SIZE;

    pbmi->bmiHeader.biSizeImage = WIDTHBYTES(iBmpWidth*8) * iBmpHeight;

    /*
     * just a plain monochrome palette
     */
    pbmi->bmiColors[0].rgbRed = 0;
    pbmi->bmiColors[0].rgbGreen = 0;
    pbmi->bmiColors[0].rgbBlue = 0;
    pbmi->bmiColors[1].rgbRed = 255;
    pbmi->bmiColors[1].rgbGreen = 255;
    pbmi->bmiColors[1].rgbBlue = 255;

    /*
     * create a DIB section that we can use to read the font bits out of
     */
    hBitmap = CreateDIBSection(
            hdc,
            pbmi,
            DIB_RGB_COLORS,
            (void**) &lpBmpBits,
            NULL,
            0
            );
    LocalFree( pbmi );
    if( hBitmap == NULL )
    {
        DeleteObject( hFont );
        DeleteDC( hMemDC );
        return FALSE;
    }

    /*
     * set up our memory DC with the font and bitmap
     */
    SelectObject( hMemDC, hBitmap );
    SetBkColor( hMemDC, RGB( 0, 0, 0 ) );
    SetTextColor( hMemDC, RGB( 255, 255, 255 ) );
    return TRUE;

} /* initDC */

/*
 * displayText
 *
 * Displays a given string at a specified location on the primary surface.
 */
static void displayText( int x, int y, LPSTR text, ... )
{
    char                buff[256];
    DDSURFACEDESC       ddsd;
    HRESULT             ddrval;
    va_list             vlist;
    int                 height;
    int                 width;

    if( lpDD == NULL )
    {
        return;
    }

    /*
     * get message to display
     */
    va_start( vlist, text );
    vsprintf( buff, text, vlist );
    va_end( vlist );

    /*
     * output text to our memory DC (the bits end up in our DIB section)
     */
    PatBlt( hMemDC, 0,0,iBmpWidth,iBmpHeight,BLACKNESS );
    TextOut( hMemDC, 1,1, buff, lstrlen( buff ) );

    /*
     * get access to the primary surface
     */
    ddsd.dwSize = sizeof(ddsd);
    ddrval = lpDDSPrimary->Lock( NULL, &ddsd, 0, NULL );
    if( ddrval == DD_OK )
    {
        LPSTR   lpbits;
        LPSTR   lpsrc;
        int     bytes_pixel;

        switch( ddsd.ddpfPixelFormat.dwRGBBitCount )
        {
        case 8:
            bytes_pixel = 1;
            break;
        case 16:
            bytes_pixel = 2;
            break;
        case 24:
            bytes_pixel = 3;
            break;
        case 32:
            bytes_pixel = 4;
            break;
        }
    
        /*
         * correct width/height of new text
         */
        height = iBmpHeight;
        if( height+y > iWinHeight )
        {
            height = iWinHeight-y;
        }
        if( height + (iWinPosY+y) > iScreenHeight )
        {
            height = iScreenHeight - (iWinPosY+y);
        }
    
        width = iBmpWidth;
        if( x+width > iWinWidth )
        {
            width = iWinWidth-x;
        }
        if( width + (iWinPosX+x) > iScreenWidth )
        {
            width = iScreenWidth - (iWinPosX+x);
        }

        /*
         * get pointer to place on screen we want to copy the text to
         */
        lpbits = &(((LPSTR) ddsd.lpSurface)[ (iWinPosY+y) * ddsd.lPitch +
                        (iWinPosX+x)*bytes_pixel ]);

        /*
         * copy the bits.  Fastested implementation would be in assembly of
         * course, but for simplicity we show it in C.
         *
         * We always want red text.   The 8bpp works fastest - we generated
         * a monochrome DIB section, so the bits in the DIB section are either
         * 0 or 1.  0 is black, and 1 is red in the standard 8bpp palettized
         * mode, so we can just copy the values.   For larger bpp, we need
         * to copy the data pixel by pixel and do a conversion.
         */
        if( width > 0  )
        {
            int i;
            int j;

            lpsrc = lpBmpBits;
            switch( bytes_pixel )
            {
            /*
             * 8bpp 
             */
            case 1:
                for( i=0;i<height;i++ )
                {
                    memcpy( lpbits, lpsrc, width );
                    lpbits += ddsd.lPitch;
                    lpsrc += WIDTHBYTES( iBmpWidth*8 );
                }
                break;
            /*
             * 16bpp
             */
            case 2:
                for( i=0;i<height;i++ )
                {
                    for( j=0;j<width;j++ )
                    {
                        if( lpsrc[j] )
                        {
                            ((WORD *)lpbits)[j] = 0x7c00;
                        }
                        else
                        {
                            ((WORD *)lpbits)[j] = 0x0000;
                        }
                    }
                    lpbits += ddsd.lPitch;
                    lpsrc += WIDTHBYTES( iBmpWidth*8 );
                }
                break;
            /*
             * 24bpp
             */
            case 3:
                for( i=0;i<height;i++ )
                {
                    for( j=0;j<width;j++ )
                    {
                        if( lpsrc[j] )
                        {
                            lpbits[j*3] = (char) 0x00;
                            lpbits[j*3+1] = (char) 0x00;
                            lpbits[j*3+2] = (char) 0xff;
                        }
                        else
                        {
                            lpbits[j*3] = (char) 0x00;
                            lpbits[j*3+1] = (char) 0x00;
                            lpbits[j*3+2] = (char) 0x00;
                        }
                    }
                    lpbits += ddsd.lPitch;
                    lpsrc += WIDTHBYTES( iBmpWidth*8 );
                }
                break;
            /*
             * 32bpp
             */
            case 4:
                for( i=0;i<height;i++ )
                {
                    for( j=0;j<width;j++ )
                    {
                        if( lpsrc[j] )
                        {
                            ((DWORD *)lpbits)[j] = 0x00ff0000l;
                        }
                        else
                        {
                            ((DWORD *)lpbits)[j] = 0x00000000l;
                        }
                    }
                    lpbits += ddsd.lPitch;
                    lpsrc += WIDTHBYTES( iBmpWidth*8 );
                }
                break;
            }
        }

        /*
         * done with the primary surface
         */
        lpDDSPrimary->Unlock( NULL );

    }

} /* displayText */

/*
 * updateDisplay
 */
void updateDisplay( void )
{
    static BOOL bUpdating;
    static int  iXPos = 100;
    static int  iYPos = 100;

    if( bUpdating )
    {
        return;
    }
    bUpdating = TRUE;

    iXPos += (rand() % 3) - 1;
    if( iXPos < 0 )
    {
        iXPos = 0;
    }
    else if( iXPos > iWinWidth )
    {
        iXPos = iWinWidth;
    }

    iYPos += (rand() % 3) - 1;
    if( iYPos < 0 )
    {
        iYPos = 0;
    }
    else if( iYPos > iWinHeight )
    {
        iYPos = iWinHeight;
    }
    displayText( iXPos, iYPos, "Text (%ld,%ld)", iXPos, iYPos );

    bUpdating = FALSE;

} /* updateDisplay */

/*
 * WindowProc
 */
LRESULT CALLBACK WindowProc( HWND hwnd, unsigned uMsg, WPARAM wParam, LPARAM lParam )
{

    switch( uMsg )
    {
    case WM_COMMAND:
        switch( LOWORD( wParam ) )
        {
        case IDM_EXIT:
            DestroyWindow( hwnd );
            break;
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT     ps;

        BeginPaint( hwnd, &ps );
        EndPaint( hwnd, &ps );
        break;
    }

    case WM_TIMER:
        updateDisplay();
        break;

    case WM_MOVE:
        iWinPosX = (int) (short) LOWORD( lParam );
        iWinPosY = (int) (short) HIWORD( lParam );
        return DefWindowProc( hwnd, uMsg, wParam, lParam );

    case WM_SIZE:
        iWinWidth = (int) (short) LOWORD( lParam );
        iWinHeight = (int) (short) HIWORD( lParam );
        return DefWindowProc( hwnd, uMsg, wParam, lParam );

    case WM_DESTROY:
        KillTimer( hwnd, TIMER_ID );
        finiObjects();
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc( hwnd, uMsg, wParam, lParam );
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
    HRESULT             ddrval;
    DDSURFACEDESC       ddsd;

    /*
     * initialize our random number generator
     */
    srand( GetTickCount() );

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName = MAKEINTRESOURCE( IDR_MENU );
    wc.lpszClassName = szClass;
    if( !RegisterClass( &wc ) )
    {
        return FALSE;
    }
    
    /*
     * create a window
     */
    hwnd = CreateWindow(
        szClass,                // class
        szCaption,              // caption
        WS_OVERLAPPEDWINDOW,    // style 
        CW_USEDEFAULT,          // x pos
        CW_USEDEFAULT,          // y pos
        CW_USEDEFAULT,          // width
        CW_USEDEFAULT,          // height
        NULL,                   // parent window
        NULL,                   // menu 
        hInstance,              // instance
        NULL                    // parms
        );
                    
    if( !hwnd )
    {
        return FALSE;
    }

    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );

    /*
     * create a timer for moving text around
     */
    if( !SetTimer( hwnd, TIMER_ID, 30, NULL ) )
    {
        DestroyWindow( hwnd );
        return FALSE;
    }

    /*
     * create the main DirectDraw object
     */
    ddrval = DirectDrawCreate( NULL, &lpDD );
    if( ddrval != DD_OK )
    {
        DestroyWindow( hwnd );
        return FALSE;
    }

    /*
     * make DC, font & DIB section we need
     */
    if( !initDC() )
    {
        lpDD->Release();
        DestroyWindow( hwnd );
        return FALSE;
    }

    /*
     * create the primary surface
     */
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    ddrval = lpDD->CreateSurface( &ddsd );
    if( ddrval != DD_OK )
    {
        finiObjects();
        DestroyWindow( hwnd );
        return FALSE;
    }
    lpDDSPrimary = ddsd.lpDDSurface;

    /*
     * get dimensions of display
     */
    iScreenWidth = GetSystemMetrics( SM_CXSCREEN );
    iScreenHeight = GetSystemMetrics( SM_CYSCREEN );

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

    while( GetMessage( &msg, NULL, NULL, NULL ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return( msg.wParam );

} /* WinMain */
