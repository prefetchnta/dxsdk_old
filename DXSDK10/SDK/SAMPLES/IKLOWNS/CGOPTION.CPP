/*===========================================================================*\
|
|  File:        cgoption.cpp
|
|  Description: 
|   Routines to display lines of text (from resource strings) overlayed on 
|   a bitmap.  The text will be sized appropriately to fit within the 
|   rectangle specified in the profile.  The user may hilight a particular 
|   line and the index of the line will be returned to the caller.
|
|   Also, routines to scroll text across a bitmap image (useful for
|   story lines and credits).
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

#include <windows.h>
#include "cgglobl.h"
#include "cgdib.h"
#include "strrec.h"
#include "cginput.h"
#include "cgtext.h"
#include "cgmidi.h"
#include "cgimage.h"
#include "cgrsrce.h"
#include "cgoption.h"

#define MIN_JOY_MOVE    15
#define MIN_MOUSE_MOVE  50

#define SOURCEX SCREEN_WIDTH
#define SOURCEY SCREEN_HEIGHT
#define SCREENX GetSystemMetrics(SM_CXSCREEN)
#define SCREENY GetSystemMetrics(SM_CYSCREEN)
#define CenterX (GetSystemMetrics(SM_CXSCREEN) / 2)
#define CenterY (GetSystemMetrics(SM_CXSCREEN) / 2)

#define PIXEL_STEP  2

#define BASE_HWND   ghMainWnd
#define NUM_ENTRIES(x)  (sizeof(x)/sizeof(x[0]))

//** external functions **
extern void GetRectFromProfile(RECT &, LPSTR, LPSTR, LPSTR);
COLORREF GetColorFromProfile(LPSTR, LPSTR, LPSTR);

//** external data **
//** public data **

//** private data **
static HDC hdcIntro;
static CGameDIB *pDib;
static HPALETTE hPal;

static int iMouseMove;
static int iJoyMove;
static int iCycleRate;

//** public functions **
//** private functions **

// ----------------------------------------------------------
// LoadBitmap - load bitmap from a file into a DC and setup
//  palette.
// ----------------------------------------------------------
HDC LoadBitmapFile (
    LPSTR   pBitmapFile // Name of .BMP file
)
{
    HDC     hdcBase, hdcBitmap;

    // Loadup bitmap
    pDib = new CGameDIB(pBitmapFile);
    
    // Create a DC and copy bitmap to it
    hdcBase = GetDC(BASE_HWND);
    hdcBitmap = CreateCompatibleDC(hdcBase);
    SelectObject(hdcBitmap, pDib->GetHBitmap());

    // Set palette so image looks ok!
    hPal = pDib->CreatePalette();
    SelectPalette(hdcBase, hPal, FALSE);
    RealizePalette(hdcBase);

    ReleaseDC( BASE_HWND, hdcBase );

    return(hdcBitmap);
}

// ----------------------------------------------------------
// GetUpDownDone - poll user input devices looking to see if
//  there are any selection changes.
// ----------------------------------------------------------
void GetUpDownDone(CGameInput *Input, BOOL &up, BOOL &down, BOOL &done)
{
    JOYINFO joy;
    int x, y, buttons;

    up = down = done = FALSE;

    // Check for keyboard input
    down = (Input->GetKeyboard(VK_DOWN) !=0) ||
           (Input->GetKeyboard(VK_NUMPAD2) != 0);
    up = (Input->GetKeyboard(VK_UP) != 0) ||
           (Input->GetKeyboard(VK_NUMPAD8) != 0);
    done = ((Input->GetKeyboard(VK_RETURN)) || (Input->GetKeyboard(VK_SPACE)));

    // No keyboard input?
    if (!(up || down || done))
    {

        // Check for mouse input
        if (Input->GetMouse(x, y, buttons))
        {
            down = (y - CenterY > iMouseMove);
            up   = (y - CenterY < -iMouseMove);
            done = (buttons & 1);

            // Keep mouse in center of screen
            SetCursorPos(CenterX, CenterY);
        }
        
        // No mouse input?
        if (!(up || down || done))
        {
            // Check for joystick input
            Input->UpdateJoystick();
            if (Input->GetJoystick(1, &joy))
            {
                down = ((int)joy.wYpos >= iJoyMove);
                up = ((int)joy.wYpos <= -iJoyMove);
                done = (joy.wButtons & JOY_BUTTON1);
            }           
        }
    }
}   

// ----------------------------------------------------------
// GetStringResource -  Load string from resource into memory
// ----------------------------------------------------------
LPSTR NewStringResource(
    HINSTANCE   hInst,
    int     idString
)
{
    DWORD   nBytes;
    LPSTR   pStr=NULL;

// Wouldn't it be nice if we could use SizeofResource to know how much space to 
// alloc for each string?  Sigh -- unfortunately this doesn't seem to work!
#if 0
    HRSRC   hRsrcString;

    if ((hRsrcString = FindResource(hInst, MAKEINTRESOURCE(idString), RT_STRING)) != NULL)
    {
        nBytes = SizeofResource(hInst, hRsrcString) + 1;
        pStr = new char [nBytes];
        LoadString(ghInst, idString, pStr, nBytes);
    }

// No choice but to alloc some fixed storage size for each string.
#else
    nBytes = 128;
    pStr = new char [nBytes];
    if (LoadString(ghInst, idString, pStr, nBytes) <= 0)
    {
        delete pStr;
        pStr = NULL;
    }
#endif

    return(pStr);
}


// Options screen object for displaying option screen
// while allowing repaints to occur
COptionScreen::COptionScreen()
{
    mHdcScreen = NULL;
    mHdcIntro = NULL;
}

// Init - initialises context for option screen
//  NOTE: resource strings are assumed to be sequential!
BOOL COptionScreen::Init(
    LPSTR       pBitmapName,    // name of .BMP file or NULL to use default
    int     idStringBase,   // first resource id of text lines
    int     nChoices,   // number of text lines to display
    LPSTR       ProfileName,    // filename of game config file
    CGameInput  *Input,     // ptr to input object or NULL if no input
    int     defSelect,  // line to hilight first or -1 if none
    int     timeout     // maximum time to wait or -1 forever
)
{
    // Set up member variables
    rect.left = 50;
    rect.top = 50;
    rect.right = SCREEN_WIDTH-50;
    rect.bottom = SCREEN_HEIGHT-50;
    colorDefault=COLOR_RED;
    colorSelected=COLOR_WHITE;
    colorDefaultShadow=NO_SHADOW;
    colorSelectedShadow=NO_SHADOW;
    char        dataBuf[256];
    CurSel = defSelect;

    // Save parameters needed
    mInput = Input;
    mnChoices = nChoices;

    mHdcScreen = GetDC(BASE_HWND);

    PatBlt(mHdcScreen, 0, 0,    SCREENX, SCREENY, BLACKNESS);

    // If no bitmap is specified, get name out of profile
    if (pBitmapName == NULL)
    {
        GetPrivateProfileString("OptionScreens", "Bitmap", "option.bmp"
        , dataBuf, sizeof(dataBuf), ProfileName);
        pBitmapName = dataBuf;
    }

    // Load the bitmap
    if ((mHdcIntro = LoadBitmapFile(pBitmapName)) == NULL)
    {
        return (FALSE);
    }


    GetRectFromProfile(rect, "OptionScreens", "TextRect", ProfileName);

    colorDefault = GetColorFromProfile("OptionScreens", "DefaultColor"
    , ProfileName);
    colorDefaultShadow = GetColorFromProfile("OptionScreens", "DefaultShadow"
    , ProfileName);
    colorSelected = GetColorFromProfile("OptionScreens", "SelectColor"
    , ProfileName);
    colorSelectedShadow = GetColorFromProfile("OptionScreens", "SelectShadow"
    , ProfileName);

    // Initialize sensitivity of input devices
    iMouseMove = GetPrivateProfileInt("OptionScreens", "MouseSensitivity"
    , MIN_MOUSE_MOVE, ProfileName);
    iJoyMove = GetPrivateProfileInt("OptionScreens", "JoystickSensitivity"
    , MIN_JOY_MOVE, ProfileName);
    iCycleRate = GetPrivateProfileInt("OptionScreens", "CycleRate"
    , 250, ProfileName);

    mTimeout = GetPrivateProfileInt("OptionScreens", "Timeout"
    , 15, ProfileName) * 1000;

    // Load up the text for overlaying on the bitmap
    RECT rect2;
    rect2.top = SCREEN_HEIGHT - 75;
    rect2.bottom = SCREEN_HEIGHT-15;
    rect2.left = 10;
    rect2.right = SCREEN_WIDTH-10;

    CGameText   *pText2 = new CGameText(mHdcIntro, &rect2);
    pChoice = NewStringResource(ghInst, IDS_INSTRUCTION_1);
    pText2->AddLine(pChoice, COLOR_YELLOW);
    pChoice = NewStringResource(ghInst, IDS_INSTRUCTION_2);
    pText2->AddLine(pChoice, COLOR_YELLOW);
    pText2->TextBlt();
    delete pText2;

    pText = new CGameText(mHdcIntro, &rect);

    for (int ii=0; ii < nChoices; ii++)
    {
        pChoice = NewStringResource(ghInst, idStringBase+ii);
        if (ii != defSelect)
            pText->AddLine(pChoice, colorDefault, colorDefaultShadow);
        else
            pText->AddLine(pChoice, colorSelected, colorSelectedShadow);
    }

    // Overlay text on bitmap
    pText->TextBlt();

    // Display complete option screen!
//  BitBlt(mHdcScreen, 0, 0,    SCREENX, SCREENY, mHdcIntro, 0, 0, SRCCOPY);
    return( TRUE );
}

// Shutdown - closes down context
void COptionScreen::Shutdown( void )
{
    // Cleanup
    if ( ! mHdcIntro ) {
        DeleteDC(mHdcIntro);
        mHdcIntro = NULL;
    }
    if ( ! pText ) {
        delete pText;   // destructor will delete our strings for us!
    }
    if ( ! pDib ) {
        delete pDib;
    }
    if ( ! mHdcScreen ) {
        ReleaseDC(BASE_HWND, mHdcScreen);
        mHdcScreen = NULL;
    }
}

// ----------------------------------------------------------
// DoOptionScreen - display bitmap & text and allow user to 
//  make a selection.
// ----------------------------------------------------------
int COptionScreen::DoOptionScreen( void )
{
    // Allow user to change options
    BOOL    done = FALSE;
    BOOL    up, down;
    MSG msg;
    DWORD   startTime = timeGetTime();

    // Hide the cursor and center it
    ShowCursor(FALSE);
    SetCursorPos(CenterX, CenterY);

    // Display current option screen
    Paint();

    if (mInput != NULL)
        mInput->Flush();

    while (!done)
    {

        // Be polite (kinda)
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        Sleep(iCycleRate);

        // Get user input and hilight selections as appropriate
        if (mInput != NULL)
        {
            GetUpDownDone(mInput, up, down, done);

            // Did user press escape?
            if (mInput->GetKeyboard(VK_ESCAPE))
            {
                done = TRUE;
                CurSel = -1;
                break;
            }

            // Was there a selection change?
            if (up || down)
            {
                pText->ChangeColor(CurSel+1, colorDefault, colorDefaultShadow);
                if (down)
                    CurSel = (CurSel + 1) % mnChoices;
                else
                    CurSel = (CurSel + (2*mnChoices-1)) % mnChoices;

                pText->ChangeColor(CurSel+1, colorSelected, colorSelectedShadow);

                // Redisplay screen (we could probably do a dirty 
                // rectangle thing here -- but it's most likely
                // not worth it).
#if 0
                BitBlt(mHdcScreen, 0, 0,    SCREENX, SCREENY, mHdcIntro
                , 0, 0, SRCCOPY);
#endif
                Paint();  // member function to refresh screen
                startTime = timeGetTime();
            }
        }

        
        // See if we should timeout
        if ((mTimeout > 0) && (timeGetTime() - startTime > (DWORD)mTimeout))
        {
            done = TRUE;
            break;
        }
    }

    if (mInput != NULL)
        mInput->Flush();

    ShowCursor(TRUE);

    return(CurSel);
}


void COptionScreen::Paint( void )
{
    // We repaint the screen using our
    // member variables
    if ( mHdcScreen && mHdcIntro ) {
        StretchBlt(mHdcScreen,
                   0, 0, SCREENX, SCREENY,
                   mHdcIntro,
                   0, 0, SOURCEX, SOURCEY,
                   SRCCOPY);
    }
}

// ----------------------------------------------------------
// DoCreditsScreen - 
// ----------------------------------------------------------
void DoCreditsScreen(
    LPSTR       pBitmapName,    // name of .BMP file or NULL to use default
    int     idStringBase,   // first resource id of text lines
    int     nCredits,   // total number of text lines to display
    LPSTR       ProfileName,    // filename of game config file
    CGameInput  *Input,     // ptr to input object or NULL if no input
    int     scrollRate  // rate to scroll text
)
{
    RECT        rect= {50,50,SCREEN_WIDTH-50,SCREEN_HEIGHT-50};
    HDC     hdcBitmap;
    HDC     hdcScreen = GetDC(BASE_HWND);
    COLORREF    colorDefault=COLOR_RED;
    COLORREF    colorDefaultShadow=NO_SHADOW;
    LPSTR       pLine;
    char        dataBuf[256];
    int     nLinesPerScreen;
    int     pixelStep;
    int         ii;

    ShowCursor(FALSE);
    PatBlt(hdcScreen, 0, 0, SCREENX, SCREENY, BLACKNESS);

    // If no bitmap is specified, get name out of profile
    if (pBitmapName == NULL)
    {
        GetPrivateProfileString("CreditScreens", "Bitmap", "option.bmp"
        , dataBuf, sizeof(dataBuf), ProfileName);
        pBitmapName = dataBuf;
    }

    // Load the bitmap
    if ((hdcBitmap = LoadBitmapFile(pBitmapName)) == NULL)
    {
        return;
    }
    delete pDib;    // Is this ok?

    // We need to copies of the bitmap, one to refresh the image
    // and another to scribble over with text.  We want a flashy
    // screen, not a flashing one!
    if ((hdcIntro = LoadBitmapFile(pBitmapName)) == NULL)
    {
        DeleteDC(hdcBitmap);
        return;
    }

    GetRectFromProfile(rect, "CreditScreens", "TextRect", ProfileName);

    colorDefault = GetColorFromProfile("CreditScreens", "DefaultColor"
    , ProfileName);
    colorDefaultShadow = GetColorFromProfile("CreditScreens", "DefaultShadow"
    , ProfileName);

    // Initialize number of lines per screen  -- this will determine
    // how big the text is.
    nLinesPerScreen = GetPrivateProfileInt("CreditScreens", "LinesPerScreen"
    , 15, ProfileName);

    // Initialize number of pixels per scroll
    pixelStep = GetPrivateProfileInt("CreditScreens", "ScrollRate"
    , PIXEL_STEP, ProfileName);

//  BitBlt(hdcScreen, 0, 0, SCREENX, SCREENY, hdcBitmap, 0, 0, SRCCOPY);
    StretchBlt(hdcScreen, 0, 0, SCREENX, SCREENY, hdcBitmap, 0, 0, SOURCEX, SOURCEY, SRCCOPY);


    char MidiFile[255];
    GetPrivateProfileString("CreditScreens", "Music", "", MidiFile
    , sizeof(MidiFile), ProfileName);

    if (gMusicOn)
    {
        playMusic( MidiFile, TRUE);
    }

    // Load up the text for overlaying on the bitmap
    CGameText   *pText = new CGameText(hdcIntro, &rect, nLinesPerScreen, 1);

    for (ii=0; ii < nCredits; ii++)
    {
        pLine = NewStringResource(ghInst, idStringBase+ii);

        // Stop loading strings when we reach a gap in the resources
        if (pLine == NULL)
            break;
        pText->AddLine(pLine, colorDefault, colorDefaultShadow);
    }
    nCredits = ii;

    if (Input != NULL)
        Input->Flush();


    // Now scroll the text across the background!
    // @BEGIN_MSINTERNAL
    // NOTE: the rate of scroll is going to be dependant on the
    // speed of the PC, perhaps if we switch to DirectDraw we'll
    // have enough spare CPU cycles to allow us to slow the scroll
    // to a consistent rate!
    // @END_MSINTERNAL
    int nDisplayed=0;
    BOOL Done=FALSE, DisplayedSomething=FALSE;
    int nPages = (nCredits / nLinesPerScreen) + 1;
    for (int scroll=SOURCEY; !Done; scroll -= pixelStep)
    {
//      BitBlt(hdcIntro, 0, 0, SCREENX, SCREENY, hdcBitmap, 0, 0, SRCCOPY);
        BitBlt(hdcIntro, rect.left, rect.top, rect.right, rect.bottom
        , hdcBitmap, rect.left, rect.top, SRCCOPY);
        nDisplayed = pText->TextBlt(scroll);
//      BitBlt(hdcScreen, 0, 0, SCREENX, SCREENY, hdcIntro, 0, 0, SRCCOPY);
#if 0
        BitBlt(hdcScreen, rect.left, rect.top, rect.right, rect.bottom
        , hdcIntro, rect.left, rect.top, SRCCOPY);
#else
        StretchBlt(hdcScreen, 0, 0, SCREENX, SCREENY,
        hdcIntro, 0, 0,
        SOURCEX, SOURCEY,
        SRCCOPY);
#endif


        // We want to stop when we don't have anything left to display
        // but first we need to make sure we have started displaying 
        // something!
        if (nDisplayed > 0)
            DisplayedSomething = TRUE;
        Done = (DisplayedSomething && (nDisplayed == 0));
//          || (scroll > -(SOURCEY*nPages));


        // I guess we shouldn't FORCE the user to read our credits.
        // Allow them to break out with a keypress.
        if ((!Done) && (Input != NULL))
        {
            Done = ((Input->GetKeyboard(VK_ESCAPE)) 
            || (Input->GetKeyboard(VK_RETURN)) 
            || (Input->GetKeyboard(VK_SPACE)));
        }

        //Sleep(0);
    }


    ShowCursor(TRUE);

    // Cleanup
    DeleteDC(hdcIntro);
    hdcIntro = NULL;
    DeleteDC(hdcBitmap);
    delete pText;   // destructor will delete our strings for us!
    delete pDib;
    ReleaseDC(BASE_HWND, hdcScreen);
    closeMusic();
}


// ----------------------------------------------------------
// DoIntroScreens - 
// ----------------------------------------------------------
void DoIntroScreens(
    LPSTR       ProfileName
)
{
    HDC     hdcBitmap;
    HDC     hdcScreen = GetDC(BASE_HWND);
    char        dataBuf[256];
    DWORD       Delay;

    ShowCursor(FALSE);
    PatBlt(hdcScreen, 0, 0, SCREENX, SCREENY, BLACKNESS);

    GetPrivateProfileString("IntroScreens", "Bitmap", ""
    , dataBuf, sizeof(dataBuf), ProfileName);

    Delay = (DWORD)GetPrivateProfileInt("IntroScreens", "Delay", 2, ProfileName)
        * 1000;

    CStringRecord fields( dataBuf, "," );
    for (int ii=0; ii < fields.GetNumFields(); ii++)
    {
        if ((hdcBitmap = LoadBitmapFile(fields[ii])) != NULL)
        {
            StretchBlt(hdcScreen, 0, 0, SCREENX, SCREENY, hdcBitmap
            , 0, 0, SOURCEX, SOURCEY, SRCCOPY);
            delete pDib;
            Sleep(Delay);
        }
    }


    // Cleanup
    ShowCursor(TRUE);
    DeleteDC(hdcBitmap);
    ReleaseDC(BASE_HWND, hdcScreen);
}

