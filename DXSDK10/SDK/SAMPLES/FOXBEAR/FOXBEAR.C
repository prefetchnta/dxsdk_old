/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *  Copyright (C) 1994-1995 ATI Technologies Inc. All Rights Reserved.
 *
 *  File:       winfox.c
 *  Content:    Windows fox sample game
 *
 ***************************************************************************/
#include "foxbear.h"

LPDIRECTDRAWSURFACE     lpFrontBuffer;
LPDIRECTDRAWSURFACE     lpBackBuffer;
LPDIRECTDRAW            lpDD;
SHORT                   lastInput = 0;
HWND                    hWndMain;
BOOL                    bShowFrameCount=TRUE;

/*
 * Msg
 */
void __cdecl Msg( LPSTR fmt, ... )
{
    char        buff[256];

    wvsprintf( buff, fmt, (LPVOID)(&fmt+1) );
    OutputDebugString( buff );
    OutputDebugString( "\r\n" );

} /* Msg */

BOOL ProcessFox(SHORT sInput)
{
    ProcessInput(sInput);
    NewGameFrame();
    return TRUE;

} /* ProcessFox */

static HFONT    hFont;
static HDC      hDC;
static HBITMAP  hBitmap;
static LPSTR    pBmpBits;

#define RATE_HEIGHT     20
#define RATE_WIDTH      72
#define WIDTHBYTES(i)   ((i+31)/32*4)

LPDIRECTDRAWSURFACE     lpNumSurface;

/*
 * makeFontStuff
 */
static BOOL makeFontStuff( void )
{
    HDC                 hdc;
    PALETTEENTRY        entries[256];
    LPBITMAPINFO        pbmi;
    DDCOLORKEY          ddck;
    int                 i;

    /*
     * create a surface to copy our bits to
     */
    lpNumSurface = DDCreateSurface( RATE_WIDTH, RATE_HEIGHT );
    if( lpNumSurface == NULL )
    {
        return FALSE;
    }
    ddck.dwColorSpaceLowValue = 0;
    ddck.dwColorSpaceHighValue = 0;
    lpNumSurface->lpVtbl->SetColorKey( lpNumSurface, DDCKEY_SRCBLT, &ddck);

    pbmi = LocalAlloc( LPTR, sizeof( BITMAPINFO ) + 256 * sizeof( RGBQUAD ) );
    if( pbmi == NULL )
    {
        return FALSE;
    }

    hFont = CreateFont(
        24,
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        VARIABLE_PITCH,
        "Arial" );

    /*
     * create a memory DC for rendering our text into
     */
    hdc = GetDC( HWND_DESKTOP );
    hDC = CreateCompatibleDC( hdc );
    ReleaseDC( NULL, hdc );

    /*
     * create a dib section for containing the bits
     */
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = RATE_WIDTH;
    pbmi->bmiHeader.biHeight = -1 * RATE_HEIGHT;
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biBitCount = 8;

    pbmi->bmiHeader.biCompression = BI_RGB;
    pbmi->bmiHeader.biXPelsPerMeter = 0;
    pbmi->bmiHeader.biYPelsPerMeter = 0;
    pbmi->bmiHeader.biClrUsed = 256;
    pbmi->bmiHeader.biClrImportant = 256;

    pbmi->bmiHeader.biSizeImage = WIDTHBYTES(RATE_WIDTH*8) * RATE_HEIGHT;

    /*
     * make a palette based on the current system one...
     */
    GetSystemPaletteEntries( hdc, 0, 256, entries );

    for( i=0;i<256;i++ )
    {
        pbmi->bmiColors[i].rgbRed = entries[i].peRed;
        pbmi->bmiColors[i].rgbGreen = entries[i].peGreen;
        pbmi->bmiColors[i].rgbBlue = entries[i].peBlue;
    }

    hBitmap = CreateDIBSection(
            hdc,
            pbmi,
            DIB_RGB_COLORS,
            (void**) &pBmpBits,
            NULL,
            0
            );
    LocalFree( pbmi );

    /*
     * set up our memory DC
     */
    SelectObject( hDC, hBitmap );
    SelectObject( hDC, hFont );
    SetBkColor( hDC, RGB( 0, 0, 0 ) );
    SetTextColor( hDC, RGB( 255, 255, 255 ) );
    return TRUE;

} /* makeFontStuff */


DWORD   dwFrameCount;
DWORD   dwFrameTime;
DWORD   dwFrames;

/*
 * DisplayFrameRate
 */
void DisplayFrameRate( void )
{
    char                buff[256];
    DDSURFACEDESC       ddsd;
    HRESULT             ddrval;
    DWORD               time2;

    if( !bShowFrameCount )
    {
        return;
    }

    dwFrameCount++;
    time2 = timeGetTime() - dwFrameTime;
    if( time2 > 1000 )
    {
        dwFrames = (dwFrameCount*1000)/time2;
        dwFrameTime = timeGetTime();
        dwFrameCount = 0;
    }
    if( dwFrames == 0 )
    {
        return;
    }

    wsprintf( buff, "FPS %ld", dwFrames );

    PatBlt( hDC, 0,0,RATE_WIDTH,RATE_HEIGHT,BLACKNESS );
    TextOut( hDC, 0,0, buff, lstrlen( buff ) );
    ddsd.dwSize = sizeof(ddsd);
    ddrval = lpNumSurface->lpVtbl->Lock(
                lpNumSurface,
                NULL,
                &ddsd,
                0,
                NULL );
    if( ddrval == DD_OK )
    {
        LPSTR   pbits;
        LPSTR   psrc;
        int     i;

        pbits = ddsd.lpSurface;
        psrc = pBmpBits;
        for( i=0;i<RATE_HEIGHT;i++ )
        {
            memcpy( pbits, psrc, RATE_WIDTH );
            pbits += ddsd.lPitch;
            psrc += WIDTHBYTES( RATE_WIDTH*8 );
        }
        lpNumSurface->lpVtbl->Unlock( lpNumSurface, NULL );
        lpBackBuffer->lpVtbl->BltFast( lpBackBuffer, (C_SCREEN_W-RATE_WIDTH)/2, 20,
                                       lpNumSurface, NULL, TRUE );
    }

} /* DisplayFrameRate */

/*
 * MainWndProc
 *
 * Callback for all Windows messages
 */
long FAR PASCAL MainWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
    case WM_CREATE:
        break;
 
    case WM_KEYDOWN:
        switch( wParam )
        {
        case VK_NUMPAD5:
            lastInput=KEY_STOP;
            break;
        case VK_DOWN:
        case VK_NUMPAD2:
            lastInput=KEY_DOWN;
            break;
        case VK_LEFT:
        case VK_NUMPAD4:
            lastInput=KEY_LEFT;
            break;
        case VK_RIGHT:
        case VK_NUMPAD6:
            lastInput=KEY_RIGHT;
            break;
        case VK_UP:
        case VK_NUMPAD8:
            lastInput=KEY_UP;
            break;
        case VK_NUMPAD7:
            lastInput=KEY_JUMP;
            break;
        case VK_NUMPAD3:
            lastInput=KEY_THROW;
            break;
        case VK_F5:
            bShowFrameCount = !bShowFrameCount;
            if( bShowFrameCount )
            {
                dwFrameCount = 0;
                dwFrameTime = timeGetTime();
            }
            break;
        case VK_ESCAPE:
        case VK_F12:
            DestroyWindow( hWnd );
            break;
        }
        break;

    case WM_DESTROY:
        lastInput=0;
        DestroyGame();
        ShowCursor( TRUE );
        PostQuitMessage( 0 );
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0L;

} /* MainWndProc */

/*
 * initApplication
 *
 * Do that Windows initialization stuff...
 */
static BOOL initApplication( HANDLE hInstance, int nCmdShow )
{
    WNDCLASS    wc;
    BOOL        rc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "WinFoxClass";
    rc = RegisterClass( &wc );
    if( !rc )
    {
        return FALSE;
    }

    hWndMain = CreateWindowEx(
        WS_EX_TOPMOST,
        "WinFoxClass",
        "Win Fox Application",
        WS_VISIBLE | WS_POPUP,
        0,
        0,
        C_SCREEN_W,
        C_SCREEN_H,
        NULL,
        NULL,
        hInstance,
        NULL );

    if( !hWndMain )
    {
        return FALSE;
    }

    ShowWindow( hWndMain, nCmdShow );
    UpdateWindow( hWndMain );
    SetFocus( hWndMain );

    ShowCursor( FALSE );

    return TRUE;

} /* initApplication */

/*
 * WinMain
 */
int PASCAL WinMain( HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine,
                        int nCmdShow )
{
    MSG                 msg;
    LPDIRECTDRAWPALETTE ppal;

    if( !initApplication(hInstance, nCmdShow) )
    {
        return FALSE;
    }

    if( !PreInitializeGame() )
    {
        DestroyWindow( hWndMain );
        return FALSE;
    }

    if( !InitializeGame() )
    {
        DestroyWindow( hWndMain );
        return FALSE;
    }

    makeFontStuff();

    ppal = ReadPalFile( "rgb332.pal" );
    if( ppal == NULL )
    {
        Msg( "Palette load failed" );
        DestroyWindow( hWndMain );
        return FALSE;
    }
    lpFrontBuffer->lpVtbl->SetPalette( lpFrontBuffer, ppal );
    {
        DDCAPS  ddcaps;
        ddcaps.dwSize = sizeof( ddcaps );
        lpDD->lpVtbl->GetCaps( lpDD, &ddcaps );
        Msg( "Total=%ld, Free VRAM=%ld", ddcaps.dwVidMemTotal, ddcaps.dwVidMemFree );
        Msg( "Used = %ld", ddcaps.dwVidMemTotal- ddcaps.dwVidMemFree );
    }
    dwFrameTime = timeGetTime();
   
    while( 1 )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( !GetMessage( &msg, NULL, 0, 0 ) )
            {
                return msg.wParam;
            }
            TranslateMessage(&msg); 
            DispatchMessage(&msg);
        }
        else
        {
            ProcessFox(lastInput);
            lastInput=0;
        }
    }
    return msg.wParam;
}
