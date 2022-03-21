/*===========================================================================*\
|
|  File:        iklowns.cpp
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

#define INITGUID
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdarg.h>
#include <eh.h>
#include "strrec.h"
#include "cgglobl.h"
#include "cgrsrce.h"  // Windows resource IDs
#include "cgexcpt.h"
#include "cggdiscn.h"
#include "cgimage.h"
#include "cgdib.h"
#include "cgchar.h"
#include "cglevel.h"
#include "cgtimer.h"
#include "cginput.h"
#include "cgmidi.h"
#include "cgoption.h"
#include "cgload.h"
#include "iklowns.h"   // specific to this program

#define SCROLL_UNITS 8
#define WPM_ANIMATE (WM_USER+1)

#define CHOICE_SOLO 0
#define CHOICE_TWO  1
#define CHOICE_NET  2
#define CHOICE_QUIT 3

#define FIVE_SECONDS    5000

//** local definitions **
// ImmortalKlowns 279afa8b-4981-11ce-a521-0020af0be560
DEFINE_GUID(IMMORTALKLOWNS_GUID,0x279AFA8B,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);

char szAppName[] = "IKlowns";   // The name of this application
char szTitle[]   = "Immortal Klowns for Windows"; // The title bar text

CGameScreen* pGameScreen = NULL;
CGameLevel* pRumbleLevel = NULL;

CGameTimer* Timer = NULL;
CGameInput* Input = NULL;
CLoadingScreen* gLoadingScreen = NULL;
static COptionScreen* pOptionScreen = NULL;
int gGameMode = 0;

void ShutDownApp(void);

// unhandled exception handler
void UnHandler(void);

#if 1
void dbgprintf(char *fmt,...)
{
   char out [ 256 ];
   va_list vlist;
   va_start(vlist, fmt);
   wvsprintf(out, fmt, vlist);
   OutputDebugString(out);      
}
#endif

void MessageLoop(void)
{
#if 0
    MSG msg;

    while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD ) ) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }   
    // prime pump for next time...
    InvalidateRect(ghMainWnd, NULL, TRUE);
#endif
}

/****************************************************************************

        FUNCTION: WinMain(HINSTANCE, HINSTANCE, LPSTR, int)

        PURPOSE: calls initialization function, processes message loop

        COMMENTS:

                Windows recognizes this function by name as the initial entry point
                for the program.  This function calls the application initialization
                routine, if no other instance of the program is running, and always
                calls the instance initialization routine.  It then executes a message
                retrieval and dispatch loop that is the top-level control structure
                for the remainder of execution.  The loop is terminated when a WM_QUIT
                message is received, at which time this function exits the application
                instance by returning the value passed by PostQuitMessage().

                If this function must abort before entering the message loop, it
                returns the conventional value NULL.

****************************************************************************/
int CALLBACK WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    MSG msg;
    HANDLE hAccelTable;
    int result = NULL;

    // only allow 1 instance
    if (hPrevInstance)
      return NULL;

    // install default exception handler
    set_terminate( UnHandler );

    ghInst = hInstance;

    if (InitInstance(hInstance, nCmdShow))
    {
        hAccelTable = LoadAccelerators (hInstance,
        MAKEINTRESOURCE(IDR_ACCELERATOR1));

        /* Acquire and dispatch messages until a WM_QUIT message is
        received.
        */

        while (GetMessage(&msg, // message structure
          NULL,   // handle of window receiving the message
          0,      // lowest message to examine
          0))     // highest message to examine
        {
            if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);// Translates virtual key codes
                DispatchMessage(&msg); // Dispatches message to window
            }
        }

        ShutDownApp();  // call de-init code
        result = msg.wParam;
    }
    return (result);
}

// ----------------------------------------------------------
// GetSoundFromProfile - description
// ----------------------------------------------------------
CSoundEffect *GetSoundFromProfile(
    LPSTR   SectionName,
    LPSTR   TopicName,
    LPSTR   ProfileName
)
{
    char    dataBuf[256];

    // Load any sound effects to be played
    GetPrivateProfileString(SectionName, TopicName, ""
    , dataBuf, sizeof(dataBuf), ProfileName);

    CStringRecord fields( dataBuf, "," );
    BOOL fLoop=FALSE;

    // See if the sound is to be looped when it is played
    if ((fields.GetNumFields() > 2) && (toupper(*fields[2]) == 'L'))
    {
        fLoop = TRUE;
    }

    // Create sound effect object based on WAV file
    CSoundEffect *pSound = new CSoundEffect(fields[0], 0, fLoop, gSoundMode);

    // If a volume was specified and sound got created ok,
    // set the defaul volume
    if (pSound != NULL) 
    {
        if (fields.GetNumFields() > 1)
            pSound->SetVolume(atoi(fields[1]));
    }

    return(pSound);
        
}

// ----------------------------------------------------------
// GetRectFromProfile - description
// ----------------------------------------------------------
void GetRectFromProfile(
    RECT    &rect,
    LPSTR   SectionName,
    LPSTR   EntryName,
    LPSTR   ProfileName
)
{
    char    dataBuf[256];

    GetPrivateProfileString(SectionName, EntryName, ""
    , dataBuf, sizeof(dataBuf), ProfileName);
    {
        CStringRecord fields( dataBuf, "," );
        if (fields.GetNumFields() == 4)
        {
            rect.left = atoi(fields[0]);
            rect.top = atoi(fields[1]);
            rect.right = atoi(fields[2]);
            rect.bottom = atoi(fields[3]);
        }
    }
    
}

// ----------------------------------------------------------
// GetPointFromProfile - description
// ----------------------------------------------------------
void GetPointFromProfile(
    POINT   &pt,
    LPSTR   SectionName,
    LPSTR   EntryName,
    LPSTR   ProfileName
)
{
    char    dataBuf[256];

    GetPrivateProfileString(SectionName, EntryName, ""
    , dataBuf, sizeof(dataBuf), ProfileName);
    {
        CStringRecord fields( dataBuf, "," );
        if (fields.GetNumFields() == 2)
        {
            pt.x = atoi(fields[0]);
            pt.y = atoi(fields[1]);
        }
    }
    
}

// ----------------------------------------------------------
// GetColorFromProfile - description
// ----------------------------------------------------------
COLORREF GetColorFromProfile(
    LPSTR   SectionName,
    LPSTR   EntryName,
    LPSTR   ProfileName
)
{
    char    dataBuf[256];
    COLORREF    color=0;

    // Initialize text color
    GetPrivateProfileString(SectionName, EntryName, ""
    , dataBuf, sizeof(dataBuf), ProfileName);
    {
        CStringRecord fields( dataBuf, "," );
        if (fields.GetNumFields() == 3)
        {
            color = PALETTERGB(atoi(fields[0])
            , atoi(fields[1]), atoi(fields[2]));
        }
    }

    return(color);

}

/****************************************************************************

        FUNCTION:  InitInstance(HINSTANCE, int)

        PURPOSE:  Saves instance handle and creates main window

        COMMENTS:

                This function is called at initialization time for every instance of
                this application.  This function performs initialization tasks that
                cannot be shared by multiple instances.

                In this case, we save the instance handle in a static variable and
                create and display the main program window.

****************************************************************************/
int loading_stuff = 0;
BOOL InitInstance(
        HINSTANCE          hInstance,
        int             nCmdShow)
{
    WNDCLASS  wc;

    // Fill in window class structure with parameters that describe the
    // main window.

    wc.style         = CS_OWNDC; // Class style(s).
    wc.lpfnWndProc   = (WNDPROC)WndProc;       // Window Procedure
    wc.cbClsExtra    = 0;                      // No per-class extra data.
    wc.cbWndExtra    = 0;                      // No per-window extra data.
    wc.hInstance     = hInstance;              // Owner of this class
    wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APP)); // Icon name from .RC
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);// Cursor
    wc.hbrBackground = NULL;           // Default color
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = szAppName;              // Name to register as

    // Register the window class
    if (!RegisterClass(&wc))
        throw CGameException(
                IDS_STARTUP_ERROR
               );

    // Create a main window for this application instance.

    ghMainWnd = CreateWindow(
            szAppName,           // See RegisterClass() call.
            szTitle,             // Text for window title bar.
            WS_POPUP,       // Window style.
            0,
            0,
            GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN),
            NULL,                // Overlapped windows have no parent.
            NULL,                // Use the window class menu.
            hInstance,           // This instance owns this window.
            NULL                 // We don't use any data in our WM_CREATE
            );

    // If window could not be created, return "failure"
    if (!ghMainWnd)
        throw CGameException(
                IDS_STARTUP_ERROR
               );

    ShowWindow(ghMainWnd, nCmdShow); // Show the window
    UpdateWindow(ghMainWnd);         // Sends WM_PAINT message

    char dirBuf[MAX_PATH];
    char dirPath[MAX_PATH];
    char dataPath[MAX_PATH];
    char *p;
    GetModuleFileName(NULL, dirPath, sizeof(dirPath));
    p = strrchr(dirPath, '\\');
    *p = '\0';


    lstrcpy(dirBuf, dirPath);
    lstrcat(dirBuf , "/iklowns.gam" );

    GetPrivateProfileString("general", "datapath", ".", dataPath
    , sizeof(dataPath), dirBuf);

    lstrcpy(gDataPath, dirPath);
    lstrcat(gDataPath,"\\");
    lstrcat(gDataPath, dataPath);
    SetCurrentDirectory(gDataPath);

    Input = new CGameInput;

    DoIntroScreens(dirBuf);

    pOptionScreen = new COptionScreen;
    if ( ! pOptionScreen ) {
        delete Input;
        return( FALSE );
    }
    pOptionScreen->Init(NULL, OPTION_PLAY_START, CHOICE_QUIT+1, dirBuf
            , Input, CHOICE_SOLO, FIVE_SECONDS);
    {
        BOOL fReturnToOptionScreen;

        do {
            fReturnToOptionScreen = FALSE;

            gGameMode = pOptionScreen->DoOptionScreen();

            if ((gGameMode == -1) || (gGameMode == CHOICE_QUIT))
            {
                pOptionScreen->Shutdown();
                delete pOptionScreen;
                delete Input;
                PostQuitMessage(0); 
                return (FALSE);
            }

            if (gGameMode == CHOICE_NET)
            {
                if ( ! RemoteConnect(IMMORTALKLOWNS_GUID, "Whomever", "KrustyX") ) {
                    fReturnToOptionScreen = TRUE;
                }
            }
        } while ( fReturnToOptionScreen );
    }
    pOptionScreen->Shutdown();
    delete pOptionScreen;
    pOptionScreen = NULL;

    Timer = new CGameTimer;

//  pGameScreen->SetPalette( pBackImage->GetHPalette() );

    gUse_DDraw = (GetPrivateProfileInt("general", "useddraw", 0, dirBuf) == 1);
    gDoubleBuffer = (GetPrivateProfileInt("general", "doublebuffer", 1, dirBuf));

    if (gUse_DDraw)
    {
            //
        // temp hack - verify DirectDraw object can be created
        // before we do our construction, and if it cann't we
        // punt to GDI
        //
            LPDIRECTDRAW    pdd;
        HRESULT     result;
        result = DirectDrawCreate( NULL, &pdd );
        if( result == DD_OK )
        {
            pdd->Release();
            pGameScreen = new CGameDDrawScreen(ghMainWnd, 0, 0);
        }
        else
        {
            gUse_DDraw = 0;
        }
    }
    if (!gUse_DDraw)
    {
#ifdef USE_DIBSECTION
        pGameScreen = new CGameDSScreen(ghMainWnd, SCREEN_WIDTH,
                        SCREEN_HEIGHT);
#else
        pGameScreen = new CGameGDIScreen(ghMainWnd, SCREEN_WIDTH,
                        SCREEN_HEIGHT);
#endif
    }

    gSoundMode = GetPrivateProfileInt("general", "SoundMode", 2, dirBuf);
    if( gSoundMode == 0 )
    {
        LPDIRECTSOUND   pds;
        HRESULT     result;
        result = DirectSoundCreate( NULL, &pds );
        if( result == 0 )
        {
            pds->Release();
        }
        else
        {
            gSoundMode = 1;
        }
    }


{
    
    char        BitmapFile[MAX_PATH];
    char        MidiFile[MAX_PATH];
    POINT       origin;
    TXTCOLOR    color;
    RECT        rect;

    GetPrivateProfileString("LoadingScreens", "Bitmap", "load.bmp"
    , BitmapFile, sizeof(BitmapFile), dirBuf);

    color.main = GetColorFromProfile("LoadingScreens", "DefaultColor"
    , dirBuf);
    color.shadow = GetColorFromProfile("LoadingScreens", "DefaultShadow"
    , dirBuf);
    GetPointFromProfile(origin, "LoadingScreens", "HotSpot", dirBuf);
    GetRectFromProfile(rect, "LoadingScreens", "TextRect", dirBuf);

    CSoundEffect *pSoundStart = GetSoundFromProfile("LoadingScreens", "SoundStart", dirBuf);
    CSoundEffect *pSoundUpdate = GetSoundFromProfile("LoadingScreens", "SoundUpdate", dirBuf);
    CSoundEffect *pSoundEnd = GetSoundFromProfile("LoadingScreens", "SoundEnd", dirBuf);

    GetPrivateProfileString("LoadingScreens", "Music", ""
    , MidiFile, sizeof(MidiFile), dirBuf);

    gLoadingScreen = new CLoadingScreen(BitmapFile, IDS_LOAD_MSG, origin
    , color, rect, pSoundStart, pSoundUpdate, pSoundEnd, MidiFile);
}

    pRumbleLevel = new CGameLevel( dirBuf, "Rumble", Timer, Input, pGameScreen ); 

    delete gLoadingScreen;
    gLoadingScreen = NULL;

    char MidiFile[255];
    GetPrivateProfileString("Rumble", "Music", "", MidiFile
    , sizeof(MidiFile), dirBuf);

    if (gMusicOn)
    {
        playMusic(MidiFile, FALSE);
    }


    PostMessage( ghMainWnd, WPM_ANIMATE, 0, 0 );

    return (TRUE);              // We succeeded...
}

//#define MAX_CREDITS   (CREDIT_END-CREDIT_START+1)
#define MAX_CREDITS 500     // look for gap in resources instead

/****************************************************************************

        FUNCTION:  ShutDownApp()

        PURPOSE:  un-initialize the app as needed

        COMMENTS:

****************************************************************************/
void ShutDownApp()
{
    closeMusic();
#ifdef SHOW_CREDITS
    DoCreditsScreen(NULL, CREDIT_START, MAX_CREDITS
    , pRumbleLevel->GetProfileName(), Input);
#endif

    delete pRumbleLevel;
    delete pGameScreen;
    delete Input;
    delete Timer;

    // force redraw of entire screen
    InvalidateRect( HWND_DESKTOP, NULL, TRUE );
}

/****************************************************************************

        FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)

        PURPOSE:  Processes messages

        MESSAGES:

        WM_COMMAND    - application menu (About dialog box)
        WM_DESTROY    - destroy window

        COMMENTS:

        To process the IDM_ABOUT message, call MakeProcInstance() to get the
        current instance address of the About() function.  Then call Dialog
        box which will create the box according to the information in your
        generic.rc file and turn control over to the About() function.  When
        it returns, free the intance address.

****************************************************************************/

LRESULT CALLBACK WndProc(
                HWND hWnd,         // window handle
                UINT message,      // type of message
                WPARAM uParam,     // additional information
                LPARAM lParam)     // additional information
{
//        FARPROC lpProcAbout;  // pointer to the "About" function
    int wmId, wmEvent;

    switch (message) {
    case WM_ACTIVATE:
        // hook here for min on leaving ...
        if (Timer)
        {
            switch (LOWORD(uParam))
            {
            case WA_INACTIVE:
                if (pRumbleLevel != NULL)
                {
                    pRumbleLevel->StopAnimating();
                }
                ShowWindow(hWnd, SW_MINIMIZE);
                UpdateWindow(hWnd);
                break;

            default:
                ShowWindow(hWnd, SW_RESTORE);
                UpdateWindow(hWnd);
                // restart the animation
                PostMessage( hWnd, WPM_ANIMATE, 0, 0 );
                break;
            } 
            return(0);  
        }
        break;

    case WM_COMMAND:  // message: command from application menu

        wmId    = LOWORD(uParam);
        wmEvent = HIWORD(uParam);

        switch (wmId) {
        case IDM_EXIT:
            DestroyWindow (hWnd);
            break;

        case IDM_HELPCONTENTS:
            if (!WinHelp (hWnd, "GENERIC.HLP", HELP_KEY,(DWORD)(LPSTR)"CONTENTS")) {
            MessageBox (GetFocus(),
            "Unable to activate help",
            szAppName, MB_SYSTEMMODAL|MB_OK|MB_ICONHAND);
            }
            break;

        case IDM_HELPSEARCH:
            if (!WinHelp(hWnd, "GENERIC.HLP", HELP_PARTIALKEY, (DWORD)(LPSTR)"")) {
            MessageBox (GetFocus(),
            "Unable to activate help",
            szAppName, MB_SYSTEMMODAL|MB_OK|MB_ICONHAND);
            }
            break;

        case IDM_HELPHELP:
            if(!WinHelp(hWnd, (LPSTR)NULL, HELP_HELPONHELP, 0)) {
            MessageBox (GetFocus(),
            "Unable to activate help",
            szAppName, MB_SYSTEMMODAL|MB_OK|MB_ICONHAND);
            }
            break;


        default:
            return (DefWindowProc(hWnd, message, uParam, lParam));
        }
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc;

        hdc = BeginPaint(hWnd, &ps);

        if (gLoadingScreen)
            gLoadingScreen->Paint();
        else if ( pOptionScreen ) {
            pOptionScreen->Paint();
        }


        EndPaint(hWnd, &ps);
        return(0);
        break;
    }

    case WM_ERASEBKGND:
        #if 0
        if (gLoadingScreen)
        {
            gLoadingScreen->Paint();
            return(0);
        }
        #endif
        // no need for erasing
        return(1);

    case WPM_ANIMATE:
        // won't return til finished with animation
        if (pRumbleLevel != NULL)
        {
//              SendMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, 1);
            pRumbleLevel->Animate( hWnd, pGameScreen );
        }
        return TRUE;
        break;

    case WM_DESTROY:  // message: window being destroyed
        closeMusic();
        PostQuitMessage(0);
        break;

    default:          // Passes it on if unproccessed
        return (DefWindowProc(hWnd, message, uParam, lParam));
    }
    return (0);
}

/****************************************************************************

        FUNCTION: CenterWindow (HWND, HWND)

        PURPOSE:  Center one window over another

        COMMENTS:

        Dialog boxes take on the screen position that they were designed at,
        which is not always appropriate. Centering the dialog over a particular
        window usually results in a better position.

****************************************************************************/

BOOL CenterWindow (HWND hwndChild, HWND hwndParent)
{
        RECT    rChild, rParent;
        int     wChild, hChild, wParent, hParent;
        int     wScreen, hScreen, xNew, yNew;
        HDC     hdc;

        // Get the Height and Width of the child window
        GetWindowRect (hwndChild, &rChild);
        wChild = rChild.right - rChild.left;
        hChild = rChild.bottom - rChild.top;

        // Get the Height and Width of the parent window
        GetWindowRect (hwndParent, &rParent);
        wParent = rParent.right - rParent.left;
        hParent = rParent.bottom - rParent.top;

        // Get the display limits
        hdc = GetDC (hwndChild);
        wScreen = GetDeviceCaps (hdc, HORZRES);
        hScreen = GetDeviceCaps (hdc, VERTRES);
        ReleaseDC (hwndChild, hdc);

        // Calculate new X position, then adjust for screen
        xNew = rParent.left + ((wParent - wChild) /2);
        if (xNew < 0) {
                xNew = 0;
        } else if ((xNew+wChild) > wScreen) {
                xNew = wScreen - wChild;
        }

        // Calculate new Y position, then adjust for screen
        yNew = rParent.top  + ((hParent - hChild) /2);
        if (yNew < 0) {
                yNew = 0;
        } else if ((yNew+hChild) > hScreen) {
                yNew = hScreen - hChild;
        }

        // Set it, and return
        return SetWindowPos (hwndChild, NULL,
                xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}



/*---------------------------------------------------------------------------*\
|
|       UnHandler
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
void UnHandler(void)
{
    ShutDownApp();
    MessageBox(
            HWND_DESKTOP,
            "Unhandled exception in Immortal Klowns.",
            "Exception",
            MB_ICONSTOP | MB_OK
            );
    abort();
}
