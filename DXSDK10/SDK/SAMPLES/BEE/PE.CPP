/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:    pe.cpp
 *  Content: CPE implementation.
 *              
 ***************************************************************************/


// pe.cpp : implementation file
//
//

#include "stdafx.h"
#include "pe.h"
#include "malloc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPE

//
// Get reserved palette entries.
//
CPE* CPE::NewCPE(DWORD dwFlags)
{
    CDC cDCScreen;
    int iSysColors;
    int iPalEntries;
    CPE *pCpe = NULL;
    DWORD dwOldPaletteUse;

    int     ii;

    cDCScreen.CreateDC("DISPLAY", NULL, NULL, NULL);
    if (!cDCScreen.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
    {
        TRACE("Not a Palettized Device");        
        return(NULL);
    }

    iSysColors  = cDCScreen.GetDeviceCaps(NUMCOLORS);
    iPalEntries = cDCScreen.GetDeviceCaps(SIZEPALETTE);

    if (iSysColors > 256 || iPalEntries != 256)
        return(NULL);

    if (!(pCpe = new CPE))
        return(NULL);

    dwOldPaletteUse = SetSystemPaletteUse(cDCScreen.GetSafeHdc(), dwFlags);

    GetSystemPaletteEntries(cDCScreen.GetSafeHdc(), 0, 256, pCpe->m_ape);

    for (ii = 0; ii < iSysColors / 2; ii++)
    {
        pCpe->m_abFlags[ii]       = 1;
        pCpe->m_iColorsAvail--;
    }

    for (ii = 256 - iSysColors/2; ii < 256; ii++)
    {
        pCpe->m_abFlags[ii]       = 1;
        pCpe->m_iColorsAvail--;
    }
    
    SetSystemPaletteUse(cDCScreen.GetSafeHdc(), dwOldPaletteUse);

    pCpe->m_iFirstFree = iSysColors/2;

    return(pCpe);


}

#define EqualPE(a,b) (memcmp((LPVOID)&(a),(LPVOID)&(b),sizeof(PALETTEENTRY))==0)


//
// Find a place for this palette Entry.
//
int CPE::InsertPE(PALETTEENTRY pe)
{
    int ii;
    int iClose;
    int iDiff;
    int iDelta;
    int iMatch;

    for (ii = 0; ii < 256; ii++)
    {
        //
        // Don't do the compare if we haven't set the color.
        //
        if (m_abFlags[ii] && EqualPE(m_ape[ii], pe))
            return(ii);
    }

    if (m_iColorsAvail)
    {
        memcpy( (LPVOID) &m_ape[m_iFirstFree], (LPVOID) &pe, sizeof(PALETTEENTRY));
        m_iColorsAvail--;
        m_abFlags[m_iFirstFree]   = 2;
        return(m_iFirstFree++);
    }

    iMatch = 0;
    iClose = 0x7fffffff;
    for (ii = 0; ii < 256; ii++)
    {
        iDiff  = 0;
        iDelta =   (((int)        pe.peRed) & 0x000000ff)
                 - (((int) m_ape[ii].peRed) & 0x000000ff);
        iDiff += iDelta * iDelta;
        iDelta =   (((int)        pe.peBlue) & 0x000000ff)
                 - (((int) m_ape[ii].peBlue) & 0x000000ff);
        iDiff += iDelta * iDelta;
        iDelta =   (((int)        pe.peGreen) & 0x000000ff)
                 - (((int) m_ape[ii].peGreen) & 0x000000ff);
        iDiff += iDelta * iDelta;
        if (iClose > iDiff)
        {
            iClose = iDiff;
            iMatch = ii;
        }
    }
    return(iMatch);

}

//
// Find places for a a list of colors.
//
void CPE::SetNewColors(LPPALETTEENTRY apeNew, int iColors, LPBYTE aTrans)
{
    int ii;

    for (ii = 0; ii < 256; ii++)
    {
        aTrans[ii] = ii;
    }

    for (ii = 0; ii < iColors; ii++)
    {
        aTrans[ii] = ii;
        if (!EqualPE(apeNew[ii], m_ape[ii]))
            aTrans[ii] = InsertPE(apeNew[ii]);
    }
}


CPE::CPE()
{
    int ii;

    for (ii = 0; ii < 256; ii++)
    {
        m_ape[ii].peRed      =   0;
        m_ape[ii].peBlue     =   0;
        m_ape[ii].peGreen    =   0;
        m_ape[ii].peFlags    =   0;
        m_abFlags[ii]        =   0;
    }

    m_iColorsAvail = 256;
    m_iFirstFree   = 0;
}
CPE::~CPE()
{
}


