/*===========================================================================*\
|
|  File:        cgtext.h
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
#ifndef _CGTEXT_H
#define _CGTEXT_H
#include "linklist.h"

#define COLOR_RED   (PALETTERGB(255,0,0))
#define COLOR_GREEN (PALETTERGB(0,255,0))
#define COLOR_BLUE  (PALETTERGB(0,0,255))
#define COLOR_BLACK (PALETTERGB(0,0,0))
#define COLOR_WHITE (PALETTERGB(255,255,255))
#define COLOR_YELLOW    (PALETTERGB(255, 255, 0))
#define COLOR_GREY  (PALETTERGB(149, 149, 149))

#define NO_SHADOW   ((COLORREF)-1)

class CGameText {
private:
    int     maxWidth;
    int     maxHeight;
    RECT        rect;
    int     nLines;
    int     maxLines;
    int     spacing;
    CLinkedList *pLines;
    HDC     hdcText;
    HFONT       hFont;

public:
    CGameText(HDC, LPRECT, int maxLines=100, int lineSpacing=2);
    ~CGameText();

    int AddLine(LPSTR, COLORREF Color = COLOR_RED
    , COLORREF DropColor = COLOR_GREY);
    void ChangeColor(int, COLORREF, COLORREF DropColor = COLOR_GREY);
    LPSTR GetText(int);
    int TextBlt(int ScrollPos=0);
};
#endif

