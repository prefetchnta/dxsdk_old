/*===========================================================================*\
|
|  File:        cglevel.cpp
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
#include <linklist.h>
#include "cggraph.h"
#include "cglevel.h"
#include "cgtimer.h"
#include "cgchdll.h"
#include "cgchrint.h"
#include "strrec.h"
#include "cginput.h"
#include "cgchar.h"
#include "cgimage.h"
#include "cgmidi.h"
#include "cgremote.h"
#include "cgsound.h"
#include "cgglobl.h"
#include "cgtext.h"
#include "cgrsrce.h"

void dbgprintf(char *fmt,...);

extern LPSTR NewStringResource(
    HINSTANCE   hInst,
    int     idString
);
extern void GetRectFromProfile(RECT &, LPSTR, LPSTR, LPSTR);

extern CGameTimer * Timer;
static RECT WholeScreen = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
static int sleepTime = 4;
HDC LoadBitmapFile (
    LPSTR   pBitmapFile
);
#define BASE_HWND   ghMainWnd

/*---------------------------------------------------------------------------*\
|
|       Class CGameLevel
|
|  DESCRIPTION:
|
|
|
\*---------------------------------------------------------------------------*/

// this is ONLY for the Klown; it must match the corresponding entry in cgkrusty.cpp
typedef struct
{
    int curState;
    int LastMouseX;
    int LastMouseY;
    DWORD   timeLastUpdate;
    int HitsLeft;
    int pushedState;
    CGameCharacter * myPie;
    int type;           // 0 = main; 1=computer; 2= second;
    int IGotKilled;
} KLOWN_DATA;
extern int gGameMode;
CGameLevel::CGameLevel( 
    char *pFileName,
    TCHAR * pLevelName,
    CGameTimer* pTimer,
    CGameInput* pInput,
    CGameScreen* pScreen
 ) : mpGraphics( NULL ),
    mpTimer( pTimer ),
    mpInput( pInput ),
    mpScreen( pScreen ),
    mOffsetX( 0 ),
    mOffsetY( 0 ),
    mFrameTime( 0 ),
    mpGraphicsKey( NULL ),
    mFastKlown( FALSE )
{
    char DllList[256];

    mMainKlown = NULL;
    // ron: first scan internal table of characters:
    LoadCharInfo( NULL );
    // ron: Grab info from the GAM file; look for character DLLs and load them...
    lstrcpy(DllList, "");
    GetPrivateProfileString("General", "DLLS", "", DllList, 255, pFileName);
    sleepTime = GetPrivateProfileInt("General", "Sleep", 4, pFileName);
    if (strlen(DllList) > 0)
    {
        CStringRecord crec(DllList, ",");
        int x;
        char    dlldir[260];
        char   *p;

        lstrcpy( dlldir, pFileName );
        p = strrchr( dlldir, '/' );
        if ( p == NULL )
            p = strrchr( dlldir, '\\' );
        if ( p != NULL )
            lstrcpy( p + 1, "*.dll" );


        for (x=0; x<crec.GetNumFields(); x++)
        {
            LoadMyDLL(dlldir, crec[x]);
        }
    }

    // Load up any sound effects that are designated as preload.
    char    SoundList[255];
    GetPrivateProfileString(pLevelName, "PreloadSounds", "", SoundList, sizeof(SoundList)
    , pFileName);
    if (strlen(SoundList) > 0)
    {
        CStringRecord crec(SoundList, ",");
        for (int x=0; x<crec.GetNumFields(); x++)
        {
            new CSoundEffect(crec[x], 0, FALSE, gSoundMode);
        }
    }

    // init
    mMaxWorldX = GetPrivateProfileInt(pLevelName, "WorldX", SCREEN_HEIGHT, pFileName) / 2;
    mMaxWorldY = GetPrivateProfileInt(pLevelName, "WorldY", SCREEN_WIDTH, pFileName) / 2;

    mOffsetX = GetPrivateProfileInt(pLevelName, "StartX", 0, pFileName);
    mOffsetY = GetPrivateProfileInt(pLevelName, "StartY", 0, pFileName);

    // now create the characters as needed
    pLevName = new char [lstrlen(pLevelName)+1];
    pFilName = new char [lstrlen(pFileName)+1];
    lstrcpy(pLevName, pLevelName);
    lstrcpy(pFilName, pFileName);


    SetCurrentLevel(this);

    {
        
        // set the screen's palette
        char paletteFile[256];
        GetPrivateProfileString(
            pLevelName,
            "Palette",
            "",
            paletteFile,
            sizeof( paletteFile ),
            pFileName
            );
        pScreen->SetPalette( paletteFile );
    }   

    char graphicsName[256];
    GetPrivateProfileString(
        pLevelName,
        "Graphics",
        "",
        graphicsName,
        sizeof( graphicsName ),
        pFileName
        );

    // keep a copy of the section name
    mpGraphicsKey = new char[lstrlen(graphicsName)+1];
    lstrcpy( mpGraphicsKey, graphicsName );

    mpGraphics = new CGameDisplayList(pFileName, graphicsName, this);

    // ron: Add computer opponent(s), second klown, if needed:
    mGameType = gGameMode;
    mNumComputerKlowns =
        mGameType == 0 ?
            1 :
            0 ;
    memset(&mComputerKlowns[0], 0, sizeof(mComputerKlowns));
    GetPrivateProfileString("General", "RoboKlown", "", DllList, sizeof(DllList), pFileName);
    int posx, posy;
    mMainKlown->GetXY(&posx, &posy);
    for (int x=0; x<mNumComputerKlowns; x++)
    {
        // create new klown, computer generated
        mComputerKlowns[x] = new CGameCharacter(pFileName, DllList, graphicsName, this, 
            mMainKlown->GetMinZ(), 
            mMainKlown->GetMaxZ(), posx + mMainKlown->GetCurWidth() * 2, 
            posy, NULL);

        if (mComputerKlowns[x])
        {
            mpGraphics->Insert(mComputerKlowns[x]);
            KLOWN_DATA *data = (KLOWN_DATA *) mComputerKlowns[x]->mpPrivateData;
            if (data)
                data->type = 1; // computer opponent;
        }
    }

    // if playing other person on same machine, create opponent;
    if (mGameType == 1)
    {
        // create second klown
        GetPrivateProfileString("General", "SecondKlown", "", DllList, sizeof(DllList), pFileName);
        mSecondKlown = new CGameCharacter(pFileName, DllList, graphicsName, this, 
            mMainKlown->GetMinZ(), 
            mMainKlown->GetMaxZ(), posx + mMainKlown->GetCurWidth() * 2 , posy, NULL);

        if (mSecondKlown)
        {
            mpGraphics->Insert(mSecondKlown);
            KLOWN_DATA *data = (KLOWN_DATA *) mSecondKlown->mpPrivateData;
            if (data)
                data->type = 2; // second (human) opponent;         
        }

    }
    else
        mSecondKlown = NULL;

    mpUpdateList = new CGameUpdateList;
    mpUpdateList->AddRect(WholeScreen);
}

CGameCharacter * CGameLevel::Add (
                char *name, 
                int curz, 
                int curx, 
                int  cury,
                void *pNewObjID)
{
    CGameCharacter * newchar;

    if (mpGraphics == NULL)
        return(NULL);

    newchar = new CGameCharacter(pFilName, name, mpGraphicsKey, this, curz, curz, curx, cury, pNewObjID);
    if (newchar)
    {
        mpGraphics->Insert(newchar);
    }
    return(newchar);    
}

CGameLevel::~CGameLevel(  )
{
#if 0
    delete  mpScene;

    // delete all the character objects
    if ( mpCharacters ) {
        for ( 
            CGameCharacter * pChar = ( CGameCharacter * ) mpCharacters->RemoveFirst(  );
            pChar;
            pChar = ( CGameCharacter * ) mpCharacters->RemoveFirst(  )
             ) {
            delete  pChar;
        }
    }
    delete  mpCharacters;

#endif
    delete mpGraphics;
    delete pLevName;
    delete pFilName;
    delete mpGraphicsKey;
    delete mpUpdateList;
}

BOOL gameover = FALSE;
BOOL quit = FALSE;
BOOL showing = FALSE;
BOOL showFrameRate = FALSE;
void CGameLevel::GameOver()
{
    gameover = TRUE;
}

void
CGameLevel::StopAnimating()
{
    showing = FALSE;
}

void    CGameLevel::Animate( 
            HWND hwndParent,
            CGameScreen * pScreen
 )
{
    SetCapture( hwndParent );   // so we get mouse clicks

    // turn off the cursor
    HCURSOR hOldCursor = SetCursor( NULL );

    pScreen->SetMode( SCREEN_WIDTH, SCREEN_HEIGHT, 8 );

    showing = TRUE;
    MSG     msg;

    Timer->Time = timeGetTime(); // * 60 / 1000;

    UINT    lastTime = Timer->Time;
    mFrameTime = lastTime;
    UINT elapsed = 0;

#define DEBOUNCE_FRAMES 12
    static int debounceF2 = 0;
    static int debounceF5 = 0;
    static int debounceF9 = 0;

#define FRAMERATE
#define SHOWSCORES

#ifdef SHOWSCORES
#define SCORE_WIDTH 64
#define SCORE_HEIGHT 64

    CGameDSBitBuffer* pScoreFrameBuffer;

    HDC hdcWindow = GetDC(HWND_DESKTOP);
    HDC hdcScore = CreateCompatibleDC(hdcWindow);
    ReleaseDC(HWND_DESKTOP, hdcWindow);

    pScoreFrameBuffer = new CGameDSBitBuffer( SCORE_WIDTH, SCORE_HEIGHT);
    SelectObject(hdcScore, pScoreFrameBuffer->GetHBitmap());
    SetBkMode(hdcScore, TRANSPARENT);

    //    set up  our cool font
    LOGFONT logFont;
    HANDLE hFont;
    memset(&logFont, 0, sizeof(LOGFONT));
    logFont.lfHeight = SCORE_HEIGHT; //maxHeight;
    logFont.lfPitchAndFamily = FF_ROMAN;
    hFont = CreateFontIndirect(&logFont);
    SelectObject(hdcScore, hFont);
    SetTextColor(hdcScore, COLOR_YELLOW);
#endif            

#ifdef FRAMERATE
#define FR_WIDTH 32
#define FR_HEIGHT 32

    UINT frames = 0;
    UINT frameTime = timeGetTime();

    // create a memory bitmap for our frame text
    CGameDSBitBuffer* pFrameBuffer;

    HDC hdc = GetDC( HWND_DESKTOP );
    HDC hdcFrame = CreateCompatibleDC( hdc );

    pFrameBuffer = new CGameDSBitBuffer( FR_WIDTH, FR_HEIGHT);
    SelectObject( hdcFrame, pFrameBuffer->GetHBitmap() );
    //    set up  our cool font
    LOGFONT logFont2;
    HANDLE hFont2;
    memset(&logFont2, 0, sizeof(LOGFONT));
    logFont2.lfHeight = FR_HEIGHT; //maxHeight;
    logFont2.lfPitchAndFamily = FF_ROMAN;
    hFont2 = CreateFontIndirect(&logFont2);
    SelectObject(hdcFrame, hFont2);
    SetTextColor(hdcFrame, COLOR_RED);
    SetBkMode(hdcFrame, TRANSPARENT);

    memset(pFrameBuffer->GetBits(), 1, FR_WIDTH * FR_HEIGHT);
    TextOut( hdcFrame, 0,0, "30", 2 );

    frames = 0;
#endif

    Timer->Resume();
    if (gMusicOn)
    {
        resumeMusic();
    }
#ifndef DEBUG
    HANDLE hprocess;
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    SetPriorityClass(hprocess, HIGH_PRIORITY_CLASS );
    CloseHandle(hprocess);
#endif

    while ( showing ) {
        Timer->Time = timeGetTime();
        UINT    time = Timer->Time;

        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD ) ) {
            TranslateMessage(&msg);
            switch (msg.message)
            {
                case WM_ACTIVATE:
                case WM_NCACTIVATE:
                case WM_ACTIVATEAPP:
                    showing = FALSE;
                    continue;

                case MM_MCINOTIFY:
                    DispatchMessage(&msg);
                    continue;

                default:
                    break;
            }
        }

        if (mpInput->GetKeyboard(VK_ESCAPE) ||
            mpInput->GetKeyboard(VK_F12))
        {
            quit = TRUE;
            showing = FALSE;
            continue;               
        }
        if (mpInput->GetKeyboard(VK_F2) && (debounceF2 == 0))
        {
            // mute!
            gMusicOn = !gMusicOn;
            if (gMusicOn)
            {
                resumeMusic();
            } else {
                pauseMusic();
            }
            debounceF2 = DEBOUNCE_FRAMES;
        }

        if (debounceF2)
            --debounceF2;

        // toggles need to be debounced
        if (mpInput->GetKeyboard(VK_F9) && (debounceF9 == 0)) 
        {
            mFastKlown = !mFastKlown;
            debounceF9 = DEBOUNCE_FRAMES;
        }

        if (debounceF9)
            --debounceF9;

#ifdef FRAMERATE
        if (mpInput->GetKeyboard(VK_F5) && (debounceF5 == 0)) 
        {
            showFrameRate = !showFrameRate;
            debounceF5 = DEBOUNCE_FRAMES;
        }

        if (debounceF5)
            --debounceF5;
#endif

        lastTime = time;

        {
            extern void PollForRemoteReceive( void );

            if ( mGameType > 1 ) {
                PollForRemoteReceive();
            }
        }
        {
            mFrameTime = time;
            static int counter = 1;

            // let each object update position, get input, etc
//          if ((counter++ % 2) != 0)
            {
                mpGraphics->Update(this, mpUpdateList);
            }
//          else
            {
                // let each object render its graphical image
                mpGraphics->Render(this, pScreen, mpUpdateList);
                mpUpdateList->Clear();

#ifdef SHOWSCORES
                // get the correct score to show... there is always at least *one*
                KLOWN_DATA * pKlown;
                RECT score_rect;
                char    scorebuf[5];
                pKlown = (KLOWN_DATA *) mMainKlown->mpPrivateData;

                score_rect.left = 10;
                score_rect.right = 10+SCORE_WIDTH;
                score_rect.top = 10;
                score_rect.bottom = 10+SCORE_HEIGHT;
                mpUpdateList->AddRect(score_rect);

                itoa(pKlown->HitsLeft, scorebuf, 10);

                memset(pScoreFrameBuffer->GetBits(), 1, SCORE_WIDTH * SCORE_HEIGHT);

                TextOut(hdcScore, 0, 0, scorebuf, lstrlen(scorebuf));
                pScreen->TransRender(10,10,SCORE_WIDTH, SCORE_HEIGHT,
                    pScoreFrameBuffer,0,0,NULL,0,0);        

                switch (mGameType)
                {
                    case 0: // against computer
                        pKlown = (KLOWN_DATA *) mComputerKlowns[0]->mpPrivateData;
                        break;

                    case 1:
                        pKlown = (KLOWN_DATA *) mSecondKlown->mpPrivateData;
                        break;

                    default:
                        pKlown = NULL;
                        break;
                }
                if (pKlown != NULL) 
                {
                    score_rect.left = SCREEN_WIDTH -10 - SCORE_WIDTH;
                    score_rect.right = SCREEN_WIDTH -10;
                    mpUpdateList->AddRect(score_rect);

                    itoa(pKlown->HitsLeft, scorebuf, 10);
                    memset(pScoreFrameBuffer->GetBits(), 1,
                                        SCORE_WIDTH * SCORE_HEIGHT);
                    TextOut(hdcScore, 0, 0, scorebuf, lstrlen(scorebuf));
                    pScreen->TransRender(SCREEN_WIDTH -10-SCORE_WIDTH,10,
                            SCORE_WIDTH, SCORE_HEIGHT,
                    pScoreFrameBuffer,0,0,NULL,0,0);

                }   
#endif
#ifdef FRAMERATE
                if (showFrameRate)
                {
                    ++frames;
                    UINT newTime = timeGetTime();
                    UINT dTime = newTime - frameTime;
                    if (dTime >= 1000 )
                    {
                        char buf[4];

                        memset(pFrameBuffer->GetBits(), 1, FR_WIDTH * FR_HEIGHT);
                        itoa( frames / (dTime / 1000) , buf, 10 );
    //                  itoa( elapsed, buf, 10 );
                        //PatBlt( hdcFrame, 0,0,FR_WIDTH,FR_HEIGHT,WHITENESS );
    //                  if (elapsed > 20)
                        TextOut( hdcFrame, 0,0, buf, lstrlen(buf) );

                        frames = 0;
                        frameTime = newTime;
                    }
                    pScreen->TransRender(
                            320 - (FR_WIDTH >> 1),
                            10,
                            FR_WIDTH,
                            FR_HEIGHT,
                            pFrameBuffer,
                            0,
                            0,
                            NULL,
                            0,0
                            );
                }
                else
                {
                    // keep frametime updated
                    frameTime = time;
                }
#endif

                // update the screen
                pScreen->PageFlip();
                mpInput->UpdateJoystick();

                if (gameover)
                {
                    RECT rect;
                    char tempstring[20];
                    pKlown = (KLOWN_DATA *) mMainKlown->mpPrivateData;

                    Sleep(2000);

                    GetPrivateProfileString( pLevName, "WinLose", "end.bmp", tempstring, 19, pFilName);

                    //HDC hdcEnd = LoadBitmapFile(tempstring);
                    CGameDIB * myDib = new CGameDIB(tempstring);
                    CGameDSBitBuffer * myBuf = new CGameDSBitBuffer(myDib);

                    if (myBuf)
                    {
                        RECT rectdtop;

                        HDC hdcWindow = GetDC(HWND_DESKTOP);
                        HDC hdc = CreateCompatibleDC(hdcWindow);
                        ReleaseDC(HWND_DESKTOP, hdcWindow);

                        rectdtop.left = 0;
                        rectdtop.right = SCREEN_WIDTH;
                        rectdtop.top = 0;
                        rectdtop.bottom = SCREEN_HEIGHT;
                        mpUpdateList->AddRect(rectdtop);

                        SelectObject(hdc, myBuf->GetHBitmap());
                        // make the gameover bitmap fill current screen
                        rect = rectdtop;

                        CGameText * pCtext = new CGameText (hdc, &rect, 12,1);

                        if (pCtext)
                        {
                            int ix, ixend;
                            pKlown = (KLOWN_DATA *) mMainKlown->mpPrivateData;
                            pKlown->curState = 0;

                            ix = pKlown->IGotKilled ? IDS_LOSE_START : IDS_WIN_START;
                            ixend = ix + 8;
                            // load strings...
                            while (ix <= ixend)
                            {
                                char *pChoice = NewStringResource(ghInst, ix);
                                pCtext->AddLine(pChoice, COLOR_YELLOW);
                                ++ix;
                            }

                            // Overlay text on bitmap
                            pCtext->TextBlt();
                            delete pCtext;
                        }

                        //
                        // Display option screen!
                        pScreen->Render(
                            0,
                            0,
                            SCREEN_WIDTH,
                            SCREEN_HEIGHT,
                            myBuf,
                            0,
                            0,
                            SRCCOPY
                            );

                        pScreen->PageFlip();
                        Sleep(5000);
                        pScreen->PageFlip();
                        DeleteDC(hdc);
                        delete myBuf;
                    }
                    delete myDib;

                    // reset igotkilled flag...
                    pKlown->IGotKilled = 0;
                    if (mComputerKlowns[0])
                    {
                        pKlown = (KLOWN_DATA *) mComputerKlowns[0]->mpPrivateData;
                        if (pKlown) 
                        {
                            pKlown->curState = 0;                           
                            pKlown->IGotKilled = 0;
                        }

                    }
                    if (mSecondKlown)
                    {
                        pKlown = (KLOWN_DATA *) mSecondKlown->mpPrivateData;
                        if (pKlown) 
                        {
                            pKlown->curState = 0;                           
                            pKlown->IGotKilled = 0;
                        }
                    }

                    gameover = FALSE;
                }
                if (sleepTime != -1)
                    Sleep(sleepTime);
            }
        }
    }

    Timer->Pause();
    SetCursor( hOldCursor );
    ReleaseCapture(  );

    if (gMusicOn)
    {
        pauseMusic();
    }
#ifndef DEBUG
    hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    SetPriorityClass(hprocess, IDLE_PRIORITY_CLASS );
    CloseHandle(hprocess);
#endif

    pScreen->RestoreMode();

#ifdef FRAMERATE
    DeleteObject( hdcFrame );
    DeleteObject(hFont2);
    delete pFrameBuffer;
#endif

#ifdef SHOWSCORES
    DeleteObject(hdcScore);
    DeleteObject(hFont);
    delete pScoreFrameBuffer;
#endif

    if (quit)
        PostQuitMessage(0); 
}

void CGameLevel::ForceOnScreen(int *x, int *y, int wide, int high, int primary)
{
    // if the x or y coords are off screen:
    // 1) force screen to be there - unless
    // 2) the coords are impossible, then adjust them!
    int widebound, highbound;

    widebound = SCREEN_WIDTH-wide;
    highbound = SCREEN_HEIGHT-high;

    if (*x < -mMaxWorldX)
        *x = -mMaxWorldX;
    else
    {
        if (*x > (mMaxWorldX-wide))
            *x = mMaxWorldX-wide;
    }

    if (*y < -mMaxWorldY)
        *y = -mMaxWorldY;
    else
    {
        if (*y > (mMaxWorldY-wide))
            *y = mMaxWorldY-wide;
    }

    if (primary)
    {
        if (*x < Screen2WorldX(0))
        {
            mOffsetX = *x;
            mpUpdateList->AddRect(WholeScreen);
        }
        else
        {
            if (*x > Screen2WorldX(widebound))
            {
                mOffsetX += (*x - Screen2WorldX(widebound));            
                mpUpdateList->AddRect(WholeScreen);
            }
        }

        if (*y < Screen2WorldY(0))
        {       
            mOffsetY = *y;
            mpUpdateList->AddRect(WholeScreen);
        }
        else
        {
            if (*y > Screen2WorldY(highbound))
            {           
                mOffsetY += (*y - Screen2WorldY(highbound));
                mpUpdateList->AddRect(WholeScreen);
            }
        }       
    }

}



