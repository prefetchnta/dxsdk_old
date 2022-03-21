/*===========================================================================*\
|
|  File:        cgoption.h
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
#ifndef _CGOPTION_H
#define _CGOPTION_H

HDC LoadBitmapFile (
    LPSTR   pBitmapFile
);

// Class for the options screen, to allow clean repaints
// from the base window when the option screen is up

// Forward declaration of CGameText (protected member
// needn't force include of new header file)
class CGameText;

class COptionScreen
{
    public:
        COptionScreen();

        BOOL Init(
            LPSTR       pBitmapName,    // name of .BMP file or NULL to use default
            int     idStringBase,   // first resource id of text lines
            int     nChoices,   // number of text lines to display
            LPSTR       ProfileName,    // filename of game config file
            CGameInput  *Input=NULL,        // ptr to input object or NULL if no input
            int     defSelect=0,    // line to hilight first or -1 if none
            int     timeout=-1  // maximum time to wait or -1 forever
        );
        int DoOptionScreen( void );
        void Shutdown( void );
        void Paint( void );
    protected:
        RECT    rect;
        HDC mHdcScreen;
        HDC mHdcIntro; // DC screen is assembled in
        COLORREF    colorDefault;
        COLORREF    colorSelected;
        COLORREF    colorDefaultShadow;
        COLORREF    colorSelectedShadow;
        LPSTR       pChoice;           
        int         CurSel;    
        CGameInput  *mInput;
        int         mnChoices;
        CGameText   *pText;
        int         mTimeout;
};

void DoCreditsScreen(
    LPSTR       pBitmapName,    // name of .BMP file or NULL to use default
    int     idStringBase,   // first resource id of text lines
    int     nCredits,   // number of text lines to display
    LPSTR       ProfileName,    // filename of game config file
    CGameInput  *Input=NULL,    // ptr to input object or NULL if no input
    int     scrollRate=0    // rate to scroll text
);

void DoIntroScreens(
    LPSTR       ProfileName // filename of game config file
);

#endif

