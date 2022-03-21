/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:    animate.cpp
 *  Content: Sprite based animation with background sound.   
 *              
 ***************************************************************************/


#include "stdafx.h"
#include "ddraw.h"
#include "animate.h"
#include "dddib.h"
#include <initguid.h>
#include "wav.h"
#include "stdafx.h"
#include "ddraw.h"
#include "dddib.h"
#include <initguid.h>
#include "wav.h"
#include <stdio.h>
#include <stdlib.h>

HINSTANCE hSound = NULL;
HINSTANCE hDraw  = NULL;
typedef HRESULT (__stdcall *FP_DSND)(LPSTR, LPDIRECTSOUND *);
FP_DSND        fp_DSnd = NULL;


typedef HRESULT (WINAPI *FP_DRAW)( LPSTR lpszDriverName, LPDIRECTDRAW FAR *lplpDD );
FP_DRAW fp_DDraw = NULL;


typedef enum
{
    LOG,
    ABORT,
    EXIT,
    INFO
} LOG_TYPE;



void LogIt(char *chMsg, UINT uiLine, LOG_TYPE log)
{
    char *achLog;
    char  chBuffer[256];

    switch(log)
        {
        case LOG:
            achLog = "Log";
            break;
        case ABORT:
            achLog = "Abort";
            break;
        case EXIT:
            achLog = "Exit";
            break;
        case INFO:
            achLog = "Info";
            break;

        default:
            achLog = "UNKNOWN";
        }

        sprintf( chBuffer, "%s Line %d: %s\n", achLog, uiLine, chMsg);
        TRACE(chBuffer);
}


#define JMH_LOG(a)   LogIt((a), __LINE__, LOG)  ; bb = FALSE
#define JMH_INFO(a)  LogIt((a), __LINE__, INFO)  
#define JMH_ABORT(a) LogIt((a), __LINE__, ABORT); bb = FALSE; goto abort 
#define JMH_EXIT(a)  LogIt((a), __LINE__, EXIT); return(FALSE) 


BOOL bEnableDraw()
{
    BOOL bb;

    if (!hDraw)
    {
        JMH_ABORT("DirectDraw not supported on this machine.");
    }

    if (!fp_DDraw)
    {
        fp_DDraw = (FP_DRAW) GetProcAddress(hDraw, "DirectDrawCreate");
    }

    if (!fp_DDraw)
    {
        JMH_ABORT("DirectDraw not supported on this machine.");
    }

    return(TRUE);

abort:
    return(bb);
}
BOOL bEnableSound()
{
    BOOL bb;

    if (!hSound)
    {
        JMH_ABORT("DirectSound not supported on this machine.");
    }

    if (!fp_DSnd)
    {
        fp_DSnd = (FP_DSND) GetProcAddress(hSound, "DirectSoundCreate");
    }

    if (!fp_DSnd)
    {
        JMH_ABORT("DirectSound not supported on this machine.");
    }

    return(TRUE);

abort:
    return(bb);
}

//
// Bee.bmp organization.
//
#define BEE_ROWS  4
#define BEE_COLS  5

//
// Animation Path
//
typedef struct _CELL
{
    int     iRow;
    int     iCol;
    DWORD   dwX;
    DWORD   dwY;
} CELL;


#define BEE_CELLS 30

CELL aBeeCell[BEE_CELLS] =
{
    0, 0,  75, 300,          
    0, 1, 150, 320,          
    0, 2, 250, 340,          
    0, 3, 350, 360,          
    0, 4, 375, 380,          
    1, 0, 400, 395,          
    1, 1, 425, 375,          
    1, 2, 450, 350,          
    1, 3, 425, 330,          
    1, 4, 400, 310,          
    2, 0, 375, 290,          
    2, 1, 350, 260,          
    2, 2, 250, 240,          
    2, 3, 250, 240,          
    2, 4, 150, 260,          
    3, 0,  25, 300,          
    3, 1, 100, 320,          
    3, 2, 150, 340,          
    3, 3, 250, 350,          
    3, 3, 350, 360,          
    1, 0, 375, 355,          
    1, 1, 400, 330,          
    1, 2, 425, 310,          
    1, 3, 400, 290,          
    1, 4, 350, 270,          
    2, 0, 250, 250,          
    2, 1, 150, 270,          
    2, 2, 100, 290,          
    2, 3,  70, 310,          
    2, 4,  50, 330,          
};

DWORD dwSpeed = 6;
int GetNextCellPos(LPDWORD pdwX, LPDWORD pdwY)
{
    static int ii = -1;
    static int   iX0 = 0;
    static int   iY0 = 0;
    static int   iX1 = 0;
    static int   iY1 = 0;
    static int   idX;
    static int   idY;
    static int   isX;
    static int   isY;
    static int   iInter;
    static int   ie;
    static int   iCount;

    int          iTemp;
    int          jj;
    int          kk;

    //
    // Reset.
    //
    if (!pdwX || !pdwY)
    {
        ii = -1;
        iX0 = iX1;
        return(0);
    }

    //
    // We have previously reached an endpoint.  Calculate
    // constants for new line segment.
    //
    if (iX0 == iX1 )
    {
        if (ii == -1)
        {
            ii = 0;
        }
        else
            ii++;

        jj = ii + 1;
        ii = ii % BEE_CELLS;
        jj = jj % BEE_CELLS;

        iX0 = aBeeCell[ii].dwX;
        iY0 = aBeeCell[ii].dwY;
        iX1 = aBeeCell[jj].dwX;
        iY1 = aBeeCell[jj].dwY;
        idX  = abs(iX1 - iX0);
        idY  = abs(iY1 - iY0);
        isX  = ((iX1 - iX0) > 0) ? 1 : -1;
        isY  = ((iY1 - iY0) > 0) ? 1 : -1;

        if (idY > idY)
        {
            iTemp  = idY;
            idX    = idY;
            idY    = iTemp;
            iInter = 1;
        }
        else
            iInter = 0;

        ie = (2 * idY) - idX;
        iCount = idX;
    }

    //
    // Move along line segment in dwSpeed chunks, but stop
    // at end points defined by iCount.
    //
    for (kk = 0; (kk < (int) dwSpeed) && iCount; kk++, iCount--)
    {
        while (ie >= 0)
        {
            if (iInter)
                iX0 += isX;
            else
                iY0 += isY;

            ie -= 2 * idX;
        }

        if (iInter)
            iY0 += isY;
        else
            iX0 += isX;

        ie += 2 * idY;
    }
    
    *pdwX = iX0;
    *pdwY = iY0;

    return(ii);
}

BOOL bStop       = FALSE;
BOOL bContinuous = FALSE;
BOOL Animate(char *chWavFile)
{
    IDirectDraw          *pIDirectDraw     = NULL;
    IDirectDrawSurface   *pIDSPrimary      = NULL;
    IDirectDrawSurface   *pIDSBackBuff     = NULL;
    IDirectDrawSurface   *pIDSGDI          = NULL;
    IDirectDrawPalette   *pIDirectPalette  = NULL;


    IDirectSound         *pIDSnd         = NULL;
    CWAV                 *pWavPrimary    = NULL;
    CWAV                 *pWavSound      = NULL;
    IDirectSoundBuffer   *pIDSndBPrimary = NULL;
    IDirectSoundBuffer   *pIDSndBSound   = NULL;
    CDIB                 *pDib = NULL;
    CPE                  *pCPE = NULL;

    CFile                 fp;

    BOOL                  bb               = TRUE;
    DWORD                 dwTickLStart   = 0;
    DWORD                 dwBuffer       = 0;
    DWORD                 dwLoops        = 0;
    DWORD                 dwCopyTickS    = 0;
    DWORD                 dwCopyTickMax  = 0;
    BOOL                  bUnDone        = TRUE;

    DWORD                 dwX, dwY;
    char                  chBuffer[128];
    HRESULT               ddrVal;
    int                   ii;
    int                   iCell;




    //
    // Enable Draw and Sound
    //
    if (!bEnableDraw())
    {
        JMH_ABORT("DirectDraw not supported on this machine.");
        }

    if (!bEnableSound())
    {
        JMH_ABORT("DirectSound not supported on this machine.");
        }

    if (!fp.Open("Bee.bmp", CFile::modeRead))
    {
        AfxMessageBox("Couldn't find bee.bmp in current directory.");
        JMH_ABORT("Couldn't find bee.bmp in current directory.");
    }

    //
    // Create Basic DirectDraw Object.
    //
    pIDirectDraw = NULL;
    ddrVal = (fp_DDraw)(NULL, &pIDirectDraw);
    if (!pIDirectDraw)
    {
        JMH_EXIT("Inital create failed");
    }

    //
    // Get Primary surfaces.
    //
    DDSURFACEDESC dddesc;
    memset((void *) &dddesc, 0x00, sizeof(dddesc));
    dddesc.dwSize = sizeof(DDSURFACEDESC);
    dddesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    dddesc.dwBackBufferCount = 2;
    dddesc.dwFlags = DDSD_BACKBUFFERCOUNT;

    ddrVal = pIDirectDraw->CreateSurface(&dddesc);
    if (ddrVal != DD_OK || dddesc.lpDDSurface == NULL)
    {
        JMH_ABORT("CreateSurface failed.");
    }

    pIDSPrimary = (IDirectDrawSurface *) dddesc.lpDDSurface;

    DDSCAPS ddsCaps;
    ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;


    //
    // Get a pointer to our Middle Back Buffer.
    //
    if (pIDSPrimary->GetAttachedSurface(&ddsCaps, &pIDSBackBuff) != DD_OK)
    {
        JMH_ABORT("GetAttachedSurface failed.");
    }

    pDib = CDIB::NewDib(pIDirectDraw, &fp, FALSE);

    if (!pDib)
    {
        JMH_ABORT("Couldn't create class for phased image.");
    }

    //
    // Get the system palette Entries.
    //
    pCPE = CPE::NewCPE(SYSPAL_STATIC);

    if (pCPE)
    {
        //
        // Figure out what an identity palette would look like.
        //
        pDib->ResetColors(pCPE);
    }
    else
    {
        TRACE("CPE init failed.");
    }

    //
    // Create an identity palette.
    //
    if (pIDirectDraw->CreatePalette(DDPCAPS_8BIT,
                                    &pIDirectPalette,
                                    pDib->GetAPE() ) != DD_OK)
    {
        JMH_ABORT("CreatePalette failed.");
    }

    if (pIDSPrimary->SetPalette(pIDirectPalette) != DD_OK)
    {
        JMH_LOG("SetPalette failed.");
    }

    //
    // Set phased image rows and columns.
    //
    pDib->SetNumRowsAndColumns(BEE_ROWS, BEE_COLS);

    //
    // Set the ColorKey we are going to use for transparent blting.
    //
    if ((ddrVal = pDib->SetColorKey()) != DD_OK)
    {
        JMH_LOG("SetColorKey Failed.");
    }


    //
    // DirectDraw surfaces initialized, now for DirectSound.
    //
    if ((fp_DSnd)(NULL, &pIDSnd) != 0)
    {
        JMH_ABORT("Couldn't create IDirectSound object.");
    }

    if ((pWavSound = CWAV::NewWav(pIDSnd, chWavFile,
                             DSB_DESCRIPTIONF_NOMIXIN
                             )) == NULL)
    {
        JMH_ABORT("Couldn't place wave file in buffer");
    }

    if ((pWavPrimary = CWAV::NewWav(pIDSnd, pWavSound->GetFmtAddress(),
                               DSB_DESCRIPTIONF_PRIMARY
                             | DSB_DESCRIPTIONF_LOOPABLE
                             | DSB_DESCRIPTIONF_PLAYABLE
                             )) == NULL)
    {
        JMH_ABORT("Couldn't place wave file in buffer");
    }


    pIDSndBPrimary = pWavPrimary->GetSoundBuffer();
    pIDSndBSound   = pWavSound->GetSoundBuffer();

    //
    // The Primary buffer will loop.  Here, we tell the secondary buffer
    // to loop back to the beginning and reload data.
    //
    if (bContinuous)
        pWavSound->bSetFileWrap();

    //
    // Do it twice, the BLT has a max blt size of 32K.
    //
    pWavPrimary->Copy(pWavSound, BUFFER_SIZE);
    pWavPrimary->Copy(pWavSound, BUFFER_SIZE);
    pWavPrimary->SetLooping(TRUE);

    //
    // Final Surface preperation for Animation loop.
    // Copy the GDI surface, flip it to the back of the chain,
    // and get a pointer to it.
    //
    ddrVal = pIDSBackBuff->BltFast(0, 0, pIDSPrimary, NULL, FALSE);
    while (pIDSPrimary->Flip(NULL) != DD_OK);
    if (    pIDirectDraw->GetGDISurface(&pIDSGDI) != DD_OK)
    {
        JMH_ABORT("GetGDISurface failed.");
    }

    while (pIDSBackBuff->GetFlipStatus(DDGFS_ISFLIPDONE) != DD_OK);

    ii = -1;
    dwLoops = 0;
    GetNextCellPos(NULL, NULL);

    pWavPrimary->Play();


    //
    // The Animation Loop
    //
    while (!pWavSound->bFinished() && !bStop)
    {
        dwLoops++;
        ii++;

        //
        // Copy GDI Surface.
        //
        ddrVal = pIDSBackBuff->BltFast(0, 0, pIDSGDI, NULL, FALSE);

        //
        // Determine location and type of bee to put on back buffer.
        //
        iCell = GetNextCellPos(&dwX, &dwY);

        //
        // Set our phased sprite to the correct phase.
        //
        pDib->SetCell(aBeeCell[iCell].iRow, aBeeCell[iCell].iCol);

        //
        // Put the right bee in the right place.
        //
        pIDSBackBuff->BltFast( dwX, dwY,
            pDib->m_pIDSurface, pDib->GetCellRect(), TRUE);

        //
        // Flip.  Don't take no for an answer, keep at it until it works.
        //
        while (pIDSPrimary->Flip(pIDSBackBuff) != DD_OK);

        //
        // Keep up with sound until we are ready to blt again.
        //
        do
        {
            if (! pWavPrimary->GetInvalidSize(&dwBuffer))
            {
                sprintf( chBuffer, "Buffer Over-Run Loop %d", dwLoops);
                LogIt(chBuffer, __LINE__, ABORT);
                bb = FALSE;
                goto abort;
            }
            else
            {
                dwCopyTickS = GetTickCount();
                pWavPrimary->Copy( pWavSound, dwBuffer);
                dwCopyTickMax = max(dwCopyTickMax, GetTickCount() - dwCopyTickS);
            }
        } while (   (pIDSBackBuff->GetFlipStatus(DDGFS_ISFLIPDONE) != DD_OK)
                     && (bUnDone = !pWavSound->bFinished()));

        //
        // If we are kicked out because the sound ended, make sure the flip
        // is done.
        //
        if (!bUnDone)
            while (pIDSBackBuff->GetFlipStatus(DDGFS_ISFLIPDONE) != DD_OK);
    }

    //
    // Stop the world and clean up.
    //


    pWavPrimary->CleanStop();



abort:

    if (pIDSBackBuff && pIDirectDraw)
    {

        //
        // Once more, make sure flipping is OK.
        //
        for (ii = 0; ii < 10; ii++)
            if (pIDSBackBuff->GetFlipStatus(DDGFS_ISFLIPDONE) != DD_OK)
                Sleep(100);

        pIDirectDraw->FlipToGDISurface();

        for (ii = 0; ii < 10; ii++)
            if (pIDSBackBuff->GetFlipStatus(DDGFS_ISFLIPDONE) != DD_OK)
                Sleep(100);

    }

    if (pWavSound)
        delete pWavSound;

    if (pWavPrimary)
        delete pWavPrimary;

    if (pIDSndBSound)
        if ( pIDSndBSound->Release() != 0)
        {
            JMH_LOG("Unexpected Ref Count other than 0");
        }

    if (pIDSndBPrimary)
        if ( pIDSndBPrimary->Release() != 0)
        {
            JMH_LOG("Unexpected Ref Count other than 0");
        }

    if (pIDSnd)
        if ( pIDSnd->Release() != 0)
        {
            JMH_LOG("Unexpected Ref Count other than 0");
        }

    if (pCPE)
        delete pCPE;

    if (pIDirectPalette)
    {
        if (pIDirectPalette->Release() != 0)
        {
            JMH_LOG("Unexpected Ref Count other than 0");
        }
    }
    if (pDib)
        delete pDib;

    if (pIDSPrimary)
    {
        if (pIDSPrimary->Release() != 0)
        {
            JMH_LOG("Unexpected Ref Count other than 0");
        }
    }

    if (pIDirectDraw)
        if ( pIDirectDraw->Release() != 0)
        {
            JMH_LOG("Unexpected Ref Count other than 0");
        }


    return(bb);
}


