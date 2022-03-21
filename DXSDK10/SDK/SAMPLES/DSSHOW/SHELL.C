/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       shell.c
 *  Content:    Direct Sound show-off.
 *  This app basically uses the direct sound api's and pops up some
 *  controls that the user can play with at runtime to change
 *  the sound frequency, panning, volume, etc.   It has a few
 *  other functions built in.

 *
 ***************************************************************************/
    
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>

#include <mmreg.h>
#include <msacm.h>
#include <dsound.h>


#include "wassert.h"
#include "wave.h"

#include "resource.h"
#include "shell.h"

/* Procedure called when the application is loaded for the first time */

BOOL ClassInit( hInstance )
HANDLE hInstance;
{
    WNDCLASS    myClass;
            
    myClass.hCursor             = LoadCursor( NULL, IDC_ARROW );
    myClass.hIcon               = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON3));
    myClass.lpszMenuName        = MAKEINTRESOURCE(IDR_MAINMENU);
    myClass.lpszClassName       = (LPSTR)szAppName;
    myClass.hbrBackground       = (HBRUSH)(COLOR_WINDOW);
    myClass.hInstance           = hInstance;
    myClass.style               = CS_HREDRAW | CS_VREDRAW;
    myClass.lpfnWndProc         = WndProc;
    myClass.cbClsExtra          = 0;
    myClass.cbWndExtra          = 0;

    if (!RegisterClass( &myClass ) )
       return FALSE;

    return TRUE;        /* Initialization succeeded */
}


int PASCAL WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )
HANDLE hInstance, hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG   msg;
    HWND  hWnd;

    if (!hPrevInstance) {
        /* Call initialization procedure if this is the first instance */
    if (!ClassInit( hInstance ))
            return FALSE;
        }

    
    hWnd = CreateWindow((LPSTR)szAppName,
                        (LPSTR)szMessage,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,    
                        CW_USEDEFAULT,    
                        DX_MINWINDOW,     
                        DY_MINWINDOW,     
                        (HWND)NULL,        
                        (HMENU)NULL,      
                        (HANDLE)hInstance, 
                        (LPSTR)NULL        
                        );

    if (!hWnd) return (int)msg.wParam;

    // Make a long line across the top.
    CreateWindow(
        "STATIC", 
        "", 
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 
        0,
        0,
        8000, 
        2,              
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL);
        

    /* Save instance handle for DialogBox */
    hInst = hInstance;
    
    ShowWindow( hWnd, cmdShow );
    
    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }
    
    DestroyWindow(hWnd);
    UnregisterClass(szAppName, hInstance);
    return (int)msg.wParam;
}

/*  This routine will set up everything needed for the app to run.

    Input:
        hWnd                - App main window handle

    Output:
        None.

*/

void AppInit(
                    HWND        hWnd
                    )

{

    UINT                cT;
    DSBUFFERDESC        dsbd;
    DSBUFFERFORMAT      dsbf;
    HRESULT         hr;
    MMRESULT            mmr;
    DWORD               dw;

    // Set up the global window handle.
    hWndMain = hWnd;

    // Set up the file info header
    FileInfoFirst.pNext = NULL;
    FileInfoFirst.pwfx = NULL;
    FileInfoFirst.cox = COX_STARTCONTROL;
    FileInfoFirst.coy = COY_STARTCONTROL;

    // Clear the coordinate buffer.  This is used to find the next available
    // position to use for a new control.  -1 is the invalid value.
    for (cT=0; cT<MAXCONTROLS; cT++)
        rgfcoxAvail[cT] = FALSE;

    // Setup the timer...
    if ((dwTimer = SetTimer(hWnd, 1, TIMERPERIOD, NULL)) == 0)
        {
        hr = ER_TIMERALLOC;
        MessageBox(hWnd, "Cannot allocate. timer, aborting", "App Error", MB_OK|MB_ICONSTOP);
        goto ERROR_DONE_ROUTINE;
        }

    // Now set up all the direct sound stuff...

    // Get the largest waveformatex structure.
    if ((mmr = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &dw)) != 0)    
        {
        hr = mmr;
        MessageBox(hWnd, "ACM Metrics failed, aborting", "App Error", MB_OK|MB_ICONSTOP);
        goto ERROR_DONE_ROUTINE;
        }


    // Setup the format, frequency, volume, etc.
    if ((FileInfoFirst.pwfx = GlobalAllocPtr(GPTR, dw)) == NULL)
        {
        MessageBox(hWnd, "Memory startup error.", "Out of Memory", MB_OK|MB_ICONSTOP);
        goto ERROR_DONE_ROUTINE;
        }
#ifndef STARTEIGHTBITS
    FileInfoFirst.pwfx->wFormatTag = WAVE_FORMAT_PCM;
    FileInfoFirst.pwfx->nChannels = 2;
    FileInfoFirst.pwfx->nSamplesPerSec = 22050;
    FileInfoFirst.pwfx->nAvgBytesPerSec = 22050*2*2;
    FileInfoFirst.pwfx->nBlockAlign = 4;
    FileInfoFirst.pwfx->wBitsPerSample = 16;
    FileInfoFirst.pwfx->cbSize = 0;
#else
    FileInfoFirst.pwfx->wFormatTag = WAVE_FORMAT_PCM;
    FileInfoFirst.pwfx->nChannels = 2;
    FileInfoFirst.pwfx->nSamplesPerSec = 22050;
    FileInfoFirst.pwfx->nAvgBytesPerSec = 22050*1*2;
    FileInfoFirst.pwfx->nBlockAlign = 2;
    FileInfoFirst.pwfx->wBitsPerSample = 8;
    FileInfoFirst.pwfx->cbSize = 0;
#endif
    // First create the direct sound object.
    if (DirectSoundCreate(NULL,&gpds) != 0 )
        {
        MessageBox(hWnd, "Direct Sound Creation Failed", "Critical Error", MB_OK|MB_ICONSTOP);
        goto ERROR_DONE_ROUTINE;        
        }

    // Set up the primary direct sound buffer. 
    memset(&dsbd, 0, sizeof(DSBUFFERDESC));
    dsbd.dwSize                 = sizeof(DSBUFFERDESC);
    dsbd.fdwBufferDesc          = DSB_DESCRIPTIONF_PRIMARY;
    dsbd.fdwBufferDesc         |= DSB_DESCRIPTIONF_LOOPABLE;
    dsbd.cbBufferSize           = DSBSIZE;
    dsbd.dsbfFormat.dwSize  = sizeof( DSBUFFERFORMAT );
    
    if ((hr = gpds->lpVtbl->CreateSoundBuffer(gpds, &dsbd)) != 0)
        {
        MessageBox(hWnd, "Cannot create primary buffer", "Direct Sound Error", MB_OK|MB_ICONSTOP);
        goto ERROR_DONE_ROUTINE;
        }

    FileInfoFirst.pDSB = dsbd.pIDSBuffer;   

    // Ok, set the format of the output buffer.
    dsbf.dwSize          = sizeof(DSBUFFERFORMAT);
    dsbf.fdwBufferFormat = 0L;
    dsbf.pwfx            = FileInfoFirst.pwfx;
    dsbf.cbwfx           = FileInfoFirst.pwfx->cbSize + sizeof(WAVEFORMATEX);
    if ((hr = FileInfoFirst.pDSB->lpVtbl->SetFormat(FileInfoFirst.pDSB,&dsbf)) != 0)
        {
        MessageBox(hWnd, "Cannot set format on primary buffer", "Direct Sound Error", MB_OK);
        goto ERROR_DONE_ROUTINE;
        }    

    if ((hr = FileInfoFirst.pDSB->lpVtbl->SetFrequency(FileInfoFirst.pDSB, 22050)) != 0)
        {
        MessageBox(hWnd, "Cannot set frequency on primary buffer", "Direct Sound Error", MB_OK);
        goto ERROR_DONE_ROUTINE;
        }

    if ((hr = FileInfoFirst.pDSB->lpVtbl->SetVolume(FileInfoFirst.pDSB, MAXVOL_TB)) != 0)
        {
        MessageBox(hWnd, "Cannot set volume on primary buffer", "Direct Sound Error", MB_OK);
        goto ERROR_DONE_ROUTINE;
        }

    if ((hr = FileInfoFirst.pDSB->lpVtbl->SetPan(FileInfoFirst.pDSB, MIDPAN_TB)) != 0)
        {
        MessageBox(hWnd, "Cannot set panning on primary buffer", "Direct Sound Error", MB_OK);      
        goto ERROR_DONE_ROUTINE;
        }

    if ((hr = FileInfoFirst.pDSB->lpVtbl->Play(FileInfoFirst.pDSB, 0, 0)) != 0)
        {
        MessageBox(hWnd, "Cannot start playing primary buffer", "Direct Sound Error", MB_OK);
        goto ERROR_DONE_ROUTINE;
        }

    if ((hr = FileInfoFirst.pDSB->lpVtbl->SetLooping(FileInfoFirst.pDSB, FALSE)) != 0)          
        {
        MessageBox(hWnd, "Cannot set looping on primary buffer", "Direct Sound Error", MB_OK);
        goto ERROR_DONE_ROUTINE;        
        }



    goto DONE_ROUTINE;

ERROR_DONE_ROUTINE: 
    PostMessage(hWnd, WM_CLOSE, 0, 0);
    
DONE_ROUTINE:
    return; 

}

/*  This will destroy all the created objects, allocated memory, etc.  Must be called
    before termination of app.

    Input:
        hWnd                - Window handle of main window

    Output:
        None.

*/
void AppDestroy(
                    HWND        hWnd
                    )

{

    HRESULT     hr          = 0;

    if (dwTimer != 0)
        {
        KillTimer(hWnd, dwTimer);
        dwTimer = 0;
        }


    StopAllDSounds(hWnd, &FileInfoFirst);
    FreeAllList(hWnd, &FileInfoFirst);

    if( (FileInfoFirst.pDSB != NULL)  &&
        ((hr = FileInfoFirst.pDSB->lpVtbl->Stop(FileInfoFirst.pDSB))
             != 0) ) {
        MessageBox(hWnd, "Cannot stop playing primary buffer", "Direct Sound Error", MB_OK);
    }


    // Destroy the direct sound buffer.
    if(FileInfoFirst.pDSB != NULL) 
        {
        FileInfoFirst.pDSB->lpVtbl->Release(FileInfoFirst.pDSB);
        FileInfoFirst.pDSB = NULL;
        }

    // Destroy the direct sound object.
    if (gpds != NULL)
        {
        gpds->lpVtbl->Release(gpds);
        gpds = NULL;
        }

    if (FileInfoFirst.pwfx != NULL)
        {
        GlobalFreePtr(FileInfoFirst.pwfx);
        FileInfoFirst.pwfx = NULL;
        }



}

/* Procedures which make up the window class. */
long FAR PASCAL WndProc( hWnd, message, wParam, lParam )
HWND hWnd;
unsigned message;
WPARAM wParam;
LPARAM lParam;
{



    switch (message)
        {

        case WM_CREATE:
            AppInit(hWnd);
            break;

        case WM_TIMER:  
            if (!UIMainWindowTimerHandler(hWnd, wParam, lParam))
                return(DefWindowProc(hWnd, message, wParam, lParam));                           
            break;
            
    
        case WM_HSCROLL:
            if (!UIMainWindowHSBHandler(hWnd, wParam, lParam))
                return(DefWindowProc(hWnd, message, wParam, lParam));
                    
            break;

        case WM_VSCROLL:
            if (!UIMainWindowVSBHandler(hWnd, wParam, lParam))
                return(DefWindowProc(hWnd, message, wParam, lParam));
            break;
                    

        case WM_COMMAND:
            if (!UIMainWindowCMDHandler(hWnd, wParam, lParam))
                return(DefWindowProc(hWnd, message, wParam, lParam));
            break;
            
            break;
        
        /*case WM_PAINT:
            {           
            
            break;
            }*/


        case WM_DESTROY:
            AppDestroy(hWnd);
            PostQuitMessage( 0 );
            break;
        
        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
            break;
            
        }
    
    return(0L);
}

/*  This routine will pop up the open file dialog and open a file, and make any internal
    arrangements so we know the file is loaded.

    Input:
        hWnd            -   Handle of parent window.

    Output:
        None.

*/
void PD_FileOpen(
                    HWND hWnd
                    )
{

    char            szFileName[MAX_PATH];
    UINT            cSamples;
    FILEINFO        *pFileInfo                  = NULL;
    int             nFileName;

    if (GetNumControls(&FileInfoFirst) >= MAXCONTROLS)
        {
        MessageBox(hWnd, "No more controls allowed", "Hold on a sec...", MB_OK);
        return;
        }

    // Open the file, and check its format, etc.
    if (OpenFileDialog(hWnd, szFileName, &nFileName))
        {

        // Allocate the memory for the structure.
        if ((pFileInfo = GlobalAllocPtr(GPTR, sizeof(FILEINFO))) == NULL)
            {
            MessageBox(hWnd, "Cannot add this file", "Out of Memory", MB_OK|MB_ICONSTOP);
            goto ERROR_DONE_ROUTINE;
            }

        pFileInfo->pbData   = NULL;
        pFileInfo->pwfx     = NULL;
        pFileInfo->pDSB     = NULL;
        strcpy(pFileInfo->szFileName, szFileName);

        if (WaveLoadFile(szFileName, &pFileInfo->cbSize, &cSamples, &pFileInfo->pwfx, &pFileInfo->pbData) != 0)
            {
            MessageBox(hWnd, "Bad wave file or file too big to fit in memory", "Cannot load wave", MB_OK|MB_ICONSTOP);
            goto ERROR_DONE_ROUTINE;
            }

        GetNextControlCoords(&FileInfoFirst, &pFileInfo->cox, &pFileInfo->coy);

        if (NewDirectSoundBuffer(pFileInfo) != 0)
            {
            MessageBox(hWnd, "Cannot create new buffer", "Direct Sound Error", MB_OK|MB_ICONSTOP);
            goto ERROR_DONE_ROUTINE;
            }
        
        // Ok, now we can release the memory for the wave file, its copied into the
        // direct sound secondary buffer.
        Assert(pFileInfo->pbData != NULL);
        if (pFileInfo->pbData != NULL)
            {
            GlobalFree(pFileInfo->pbData);
            pFileInfo->pbData = NULL;
            }


        // If we fail after this, make sure to update the list!!!
        if (AddToList(&FileInfoFirst, pFileInfo) != 0)
            {
            MessageBox(hWnd, "Cannot add file to list", "Out of Memory", MB_OK|MB_ICONSTOP);
            goto ERROR_DONE_ROUTINE;
            }

        pFileInfo->nFileName = nFileName;
        CreateControl(hWnd, pFileInfo, pFileInfo->pwfx->nSamplesPerSec, (MAXPAN_TB-MINPAN_TB)/2, (MAXVOL_TB-MINVOL_TB)/2);
        ChangeOutputVol(pFileInfo);
        ChangeOutputFreq(pFileInfo);
        ChangeOutputPan(pFileInfo);
    
        }

        goto DONE_ROUTINE;
           
ERROR_DONE_ROUTINE:
    if (pFileInfo != NULL)
        {
        
        ReleaseDirectSoundBuffer(pFileInfo);

        if (pFileInfo->pwfx != NULL)
            {
            GlobalFree(pFileInfo->pwfx);
            
            }
        if (pFileInfo->pbData != NULL)
            {
            GlobalFree(pFileInfo->pbData);          
            }

        GlobalFreePtr(pFileInfo);
        pFileInfo = NULL;
        }

DONE_ROUTINE:
    return;

}

/*  This routine will initialize a new direct sound buffer, set the data in the buffer, 
    set the rate, format, etc...

    Input:
        pFileInfo               -   Pointer to file info with all nessecary info filled, 
                                    like pbData, cbData, etc...

    Output:
        0 if successful, else the error code.

*/

int NewDirectSoundBuffer(
                    FILEINFO *pFileInfo
                    )
{

    DSBUFFERDESC        dsbd;
    DSBUFFERFORMAT      dsbf;
    HRESULT         hr;
    BYTE            *pbData         = NULL;
    BYTE            *pbData2        = NULL;
    DWORD           dwLength;
    DWORD           dwLength2;

    // Set up the direct sound buffer. 
    memset(&dsbd, 0, sizeof(DSBUFFERDESC));
    dsbd.dwSize                 = sizeof(DSBUFFERDESC);
    dsbd.fdwBufferDesc          = 0;            //DSB_DESCRIPTIONF_LOOPABLE;
    dsbd.cbBufferSize           = pFileInfo->cbSize;
    dsbd.dsbfFormat.dwSize  = sizeof( DSBUFFERFORMAT );
    if ((hr = gpds->lpVtbl->CreateSoundBuffer(gpds, &dsbd)) != 0)
        {
        goto ERROR_IN_ROUTINE;
        }
    pFileInfo->pDSB = dsbd.pIDSBuffer;

    // Ok, lock the sucker down, and copy the memory to it.
    if ((hr = pFileInfo->pDSB->lpVtbl->Lock(pFileInfo->pDSB,
                        0,
                        pFileInfo->cbSize,
                        &pbData,
                        &dwLength,
                        &pbData2,
                        &dwLength2,
                        NULL, 0L)) != 0)
        {
        goto ERROR_IN_ROUTINE;
        }

    Assert(pbData != NULL);
    memcpy(pbData, pFileInfo->pbData, pFileInfo->cbSize);

    // Ok, now unlock the buffer, we don't need it anymore.
    if ((hr = pFileInfo->pDSB->lpVtbl->Unlock(pFileInfo->pDSB, pbData,NULL)) != 0)
        {
        goto ERROR_IN_ROUTINE;
        }

    pbData = NULL;

    // Ok, set the format of the output buffer.
    dsbf.dwSize          = sizeof(DSBUFFERFORMAT);
    dsbf.fdwBufferFormat = 0L;
    dsbf.pwfx            = pFileInfo->pwfx;
    dsbf.cbwfx           = pFileInfo->pwfx->cbSize + sizeof(WAVEFORMATEX);
    if ((hr = pFileInfo->pDSB->lpVtbl->SetFormat(pFileInfo->pDSB,&dsbf)) != 0)
        {
        goto ERROR_IN_ROUTINE;
        }    

    if ((hr = pFileInfo->pDSB->lpVtbl->SetFrequency(pFileInfo->pDSB, pFileInfo->pwfx->nSamplesPerSec)) != 0)
        {
        goto ERROR_IN_ROUTINE;
        }

    if ((hr = pFileInfo->pDSB->lpVtbl->SetVolume(FileInfoFirst.pDSB, MAXVOL_TB)) != 0)
        {
        goto ERROR_IN_ROUTINE;
        }

    if ((hr = pFileInfo->pDSB->lpVtbl->SetPan(FileInfoFirst.pDSB, MIDPAN_TB)) != 0)
        {
        goto ERROR_IN_ROUTINE;
        }

    if ((hr = pFileInfo->pDSB->lpVtbl->SetLooping(pFileInfo->pDSB, FALSE)) != 0)            
        {       
        goto ERROR_IN_ROUTINE;      
        }


    goto DONE_ROUTINE;

ERROR_IN_ROUTINE:
    if (pbData != NULL)
        {
        hr = pFileInfo->pDSB->lpVtbl->Unlock(pFileInfo->pDSB, pbData,NULL);
        pbData = NULL;
        }

    if (pFileInfo->pDSB != NULL)
        {
        pFileInfo->pDSB->lpVtbl->Release(pFileInfo->pDSB);
        pFileInfo->pDSB = NULL;
        }
    
DONE_ROUTINE:

    return(hr); 

}

/*  This routine will release a direct sound buffer, freeing up memory, resources, 
    whatever.

    Input:
        pFileInfo               -   Pointer to the file info, with the proper stuff
                                    set.

    Output: 
        0 if successful, else the error code.

*/
int ReleaseDirectSoundBuffer(
                    FILEINFO *pFileInfo
                    )

{

    if (pFileInfo->pDSB != NULL)
        {
        pFileInfo->pDSB->lpVtbl->Release(pFileInfo->pDSB);
        pFileInfo->pDSB = NULL; 
        }

    return(0);

}

/*  This routine will find the next x and y coordinates to write the control to.
    The rgfcoxAvail is an array of booleans.  If false, then than index can be 
    used as an x coordinate.

    Input:
        pFileInfoHead           -   Header of the linked list.
        pcox, pcoy              -   Filled upon return with next coordinates to use.
        
    Output:
        Only pcox and pcoy change.
        
*/
        
void GetNextControlCoords(                     
                    FILEINFO    *pFileInfoHead, 
                    int         *pcox, 
                    int         *pcoy
                    )
{

    UINT            cT;

    for (cT=0; cT<MAXCONTROLS; cT++)
        {
        if (rgfcoxAvail[cT] == FALSE)
            {
            rgfcoxAvail[cT] = TRUE;
            break;
            }
            
        }

    if (cT == MAXCONTROLS)
        {
        Assert(FALSE);
        // Couldn't find a place to put control, shouldn't happen though.
        cT = 666;       // Well, at least put it off screen.
        }

    *pcox = cT*DX_CONTROLSPACING+COX_STARTCONTROL;
    *pcoy = COY_STARTCONTROL;
    

}

/*  This will create the control used for the window, actually it is a bundle of controls
    put together.  I was thinking of a good way to figure out id codes for the controls
    but found no good way except a "funny" way...I'm going to use the x coordinate of the
    control as the id for the first control, then id+1 for the second control.  Since
    all the controls have different x coordinates, this is fine, as long as the # of
    windows in the control is not more than the spacing of the controls.

    Input:
        hWnd                -   Parent Window.
        pFileInfo           -   Pointer to FileInfo structure with the cox and coy filled.
        dwFreq, dwPan, dwVol-   Default track bar values.

    Output:
        0 if successful, else the error code.

*/
int CreateControl(
                    HWND        hWnd, 
                    FILEINFO    *pFileInfo,
                    DWORD       dwFreq,
                    DWORD       dwPan,
                    DWORD       dwVol
                    )

{

    int             cox, 
                    coy;
    int             coxOld,
                    coyOld;
    int             nError              = 0;
    DWORD           idBase;
    SIZE            Size;       
    HDC             hDC                 = NULL;
/*  int             cT;
    RECT            rc;*/

    /* Figure out the values of dwFreq, dwPan and dwVol that the track bars like */
    dwFreq = (dwFreq - FREQADD)/FREQMUL;

    idBase = pFileInfo->cox;
    Assert(pFileInfo != NULL);
    cox = pFileInfo->cox+DX_TEXTSPACING;
    coy = pFileInfo->coy+DY_TEXTSPACING;

    coxOld = cox;
    coyOld = coy;

    if ((hDC = GetDC(hWnd)) == NULL)
        {
        nError = ER_CANNOTALLOCATEDC;       
        goto DONE_ROUTINE;
        }


    if (!GetTextExtentPoint32(hDC, pFileInfo->szFileName+pFileInfo->nFileName, strlen(pFileInfo->szFileName+pFileInfo->nFileName), &Size))
        {
        nError = ER_APIFAILED;      
        goto DONE_ROUTINE;
        }


    if ((pFileInfo->hWndFileName_TXT = CreateWindow(
        "STATIC", 
        pFileInfo->szFileName+pFileInfo->nFileName, 
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
        cox,
        coy,
        DX_FILENAME_TXT, 
        Size.cy + DY_TEXTSPACING,               
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    cox += DX_LOOPEDSPACING;
    coy += Size.cy + DY_TEXTSPACING + DY_LOOPEDSPACING;

    if ((pFileInfo->hWndFileName_EDGE = CreateWindow(
        "STATIC", 
        "", 
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 
        cox,
        coy - (DY_LOOPEDSPACING+DY_TEXTSPACING)/2,
        DX_LINEEDGE, 
        DY_LINEEDGE,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    // Now create status if required.
    
    #ifdef SHOWSTATUS   

    if (!GetTextExtentPoint32(hDC, szPlaying, strlen(szPlaying), &Size))
        {
        nError = ER_APIFAILED;      
        goto DONE_ROUTINE;
        }


    if ((pFileInfo->hWndStatus_TXT = CreateWindow(
        "STATIC", 
        szStopped, 
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
        cox,
        coy,
        DX_STATUS_TXT, 
        Size.cy, // + DY_TEXTSPACING,               
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    cox += DX_LOOPEDSPACING;
    coy += Size.cy + DY_TEXTSPACING + DY_LOOPEDSPACING;

    if ((pFileInfo->hWndStatus_EDGE = CreateWindow(
        "STATIC", 
        "", 
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 
        cox,
        coy - (DY_LOOPEDSPACING+DY_TEXTSPACING)/2,
        DX_LINEEDGE, 
        DY_LINEEDGE,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   
    
    #endif

    if (!GetTextExtentPoint32(hDC, szLooped, strlen(szLooped), &Size))
        {
        nError = ER_APIFAILED;      
        goto DONE_ROUTINE;
        }

    // Looped checkbox.
    if ((pFileInfo->hWndLooped_BN = CreateWindow(
        "BUTTON", 
        szLooped, 
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
        cox,
        coy,
        DX_LOOPED_TXT, 
        Size.cy + DY_TEXTSPACING,               
        hWnd, 
        (HMENU)(idBase+idLoopedBN), 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }       

    cox += DX_FREQSPACING;
    coy += Size.cy + DY_TEXTSPACING + DY_FREQSPACING;

    if ((pFileInfo->hWndLooped_EDGE = CreateWindow(
        "STATIC", 
        "", 
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 
        cox,
        coy - (DY_FREQSPACING+DY_TEXTSPACING)/2,
        DX_LINEEDGE, 
        DY_LINEEDGE,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   
    
        
        
    if (!GetTextExtentPoint32(hDC, szFreq, strlen(szFreq), &Size))
        {
        nError = ER_APIFAILED;      
        goto DONE_ROUTINE;
        }

    // Make the frequency text there.
    if ((pFileInfo->hWndFreq_TXT = CreateWindow(
        "STATIC", 
        szFreq, 
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
        cox,
        coy,
        DX_FREQ_TXT, 
        Size.cy,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    coy += Size.cy;

    // Make the frequency trackbar.
    if ((pFileInfo->hWndFreq_TB = CreateWindow(
        TRACKBAR_CLASS, 
        "", 
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_BOTH, 
        cox,
        coy,
        DX_FREQ_TB, 
        DY_FREQ_TB,             
        hWnd, 
        (HMENU)(idBase+idFreqTB), 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    SendMessage(pFileInfo->hWndFreq_TB, TBM_SETRANGE, FALSE, MAKELONG(MINFREQ_TB, MAXFREQ_TB)); 
    SendMessage(pFileInfo->hWndFreq_TB, TBM_SETPOS, TRUE, dwFreq);
    pFileInfo->dwFreq = dwFreq;


    coy += DY_FREQ_TB+DY_PANSPACING;    

    if ((pFileInfo->hWndFreq_EDGE = CreateWindow(
        "STATIC", 
        "", 
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 
        cox,
        coy - (DY_PANSPACING+DY_TEXTSPACING)/2,
        DX_LINEEDGE, 
        DY_LINEEDGE,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   
    

    if (!GetTextExtentPoint32(hDC, szPan, strlen(szPan), &Size))
        {
        nError = ER_APIFAILED;      
        goto DONE_ROUTINE;
        }



    // Make the pan text there.
    if ((pFileInfo->hWndPan_TXT = CreateWindow(
        "STATIC", 
        szPan, 
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
        cox,
        coy,
        DX_PAN_TXT, 
        Size.cy,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    coy += Size.cy;

    // Make the pan trackbar.
    if ((pFileInfo->hWndPan_TB = CreateWindow(
        TRACKBAR_CLASS, 
        "", 
        WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_BOTH, 
        cox,
        coy,
        DX_PAN_TB, 
        DY_PAN_TB,              
        hWnd, 
        (HMENU)(idBase+idPanTB), 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    SendMessage(pFileInfo->hWndPan_TB, TBM_SETRANGE, FALSE, MAKELONG(MINPAN_TB, MAXPAN_TB)); 
    SendMessage(pFileInfo->hWndPan_TB, TBM_SETPOS, TRUE, dwPan);
    pFileInfo->dwPan = dwPan;


    coy += DY_PAN_TB + DY_VOLSPACING;

    if ((pFileInfo->hWndPan_EDGE = CreateWindow(
        "STATIC", 
        "", 
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 
        cox,
        coy - (DY_VOLSPACING+DY_TEXTSPACING)/2,
        DX_LINEEDGE, 
        DY_LINEEDGE,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   
    


    if (!GetTextExtentPoint32(hDC, szVolume, strlen(szVolume), &Size))
        {
        nError = ER_APIFAILED;      
        goto DONE_ROUTINE;
        }

    // Make the volume text there.
    if ((pFileInfo->hWndVol_TXT = CreateWindow(
        "STATIC", 
        szVolume, 
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
        cox,
        coy,
        DX_VOL_TXT, 
        Size.cy,                
        hWnd, 
        (HMENU)idBase, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    coy += Size.cy;

    // Make the volume trackbars.
    // Create main volume bar.
    if ((pFileInfo->hWndVolM_TB = CreateWindow(
        TRACKBAR_CLASS, 
        "", 
        WS_CHILD | WS_VISIBLE | TBS_VERT | TBS_BOTH, 
        cox,
        coy,
        DX_VOL_TB, 
        DY_VOL_TB,              
        hWnd, 
        (HMENU)(idBase+idVolMTB), 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    SendMessage(pFileInfo->hWndVolM_TB, TBM_SETRANGE, FALSE, MAKELONG(MINVOL_TB, MAXVOL_TB)); 
    SendMessage(pFileInfo->hWndVolM_TB, TBM_SETPOS, TRUE, dwVol);
    pFileInfo->dwVol = dwVol;



    // Now the left volume.
    if ((pFileInfo->hWndVolL_TB = CreateWindow(
        TRACKBAR_CLASS, 
        "", 
        WS_CHILD | WS_VISIBLE |WS_DISABLED| TBS_VERT | TBS_BOTH, 
        cox+DX_VOL_TB+DX_VOLSPACING_TB,
        coy,
        DX_VOL_TB, 
        DY_VOL_TB,              
        hWnd, 
        (HMENU)(idBase+idVolLTB), 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    SendMessage(pFileInfo->hWndVolL_TB, TBM_SETRANGE, FALSE, MAKELONG(MINVOL_TB, MAXVOL_TB)); 
    SendMessage(pFileInfo->hWndVolL_TB, TBM_SETPOS, TRUE, (MAXVOL_TB-MINVOL_TB)/2);

    if ((pFileInfo->hWndVolL_TXT = CreateWindow(
        "STATIC", 
        "L", 
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
        cox+DX_VOL_TB*3/2+DX_VOLSPACING_TB/2,
        coy+DY_VOL_TB+DY_VOLSPACINGY,
        DX_VOLUMECHAR, 
        Size.cy,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   


    // And right volume.
    if ((pFileInfo->hWndVolR_TB = CreateWindow(
        TRACKBAR_CLASS, 
        "", 
        WS_CHILD | WS_VISIBLE | WS_DISABLED | TBS_VERT | TBS_BOTH, 
        cox+DX_VOL_TB*2+DX_VOLSPACING_TB*2,
        coy,
        DX_VOL_TB, 
        DY_VOL_TB,              
        hWnd, 
        (HMENU)(idBase+idVolRTB), 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    SendMessage(pFileInfo->hWndVolR_TB, TBM_SETRANGE, FALSE, MAKELONG(MINVOL_TB, MAXVOL_TB)); 
    SendMessage(pFileInfo->hWndVolR_TB, TBM_SETPOS, TRUE, (MAXVOL_TB-MINVOL_TB)/2);

    if ((pFileInfo->hWndVolR_TXT = CreateWindow(
        "STATIC", 
        "R", 
        WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
        cox+DX_VOL_TB*5/2+DX_VOLSPACING_TB/2+2,     // +2 to look nice.
        coy+DY_VOL_TB+DY_VOLSPACINGY,
        DX_VOLUMECHAR, 
        Size.cy,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

 



    coy += DY_VOL_TB + DY_BEFOREFIRSTBUTTON;

    if ((pFileInfo->hWndVol_EDGE = CreateWindow(
        "STATIC", 
        "", 
        WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 
        cox,
        coy - (DY_BEFOREFIRSTBUTTON)/2,
        DX_LINEEDGE, 
        DY_LINEEDGE,                
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   



    if (!GetTextExtentPoint32(hDC, szPlay, strlen(szPlay), &Size))
        {
        nError = ER_APIFAILED;      
        goto DONE_ROUTINE;
        }


    if ((pFileInfo->hWndPlay_BN = CreateWindow(
        "BUTTON", 
        szPlay, 
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
        cox,
        coy,
        DX_BUTTONSPACING, 
        Size.cy + DY_BUTTONSPACING,             
        hWnd, 
        (HMENU)(idBase+idPlayBN), 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }       

    coy += Size.cy + DY_BUTTONSPACING + DY_BETWEENBUTTONS;
    if (!GetTextExtentPoint32(hDC, szPlay, strlen(szPlay), &Size))
        {
        nError = ER_APIFAILED;      
        goto DONE_ROUTINE;
        }
     
    if ((pFileInfo->hWndRemove_BN = CreateWindow(
        "BUTTON", 
        szRemove, 
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
        cox,
        coy,
        DX_BUTTONSPACING, 
        Size.cy + DY_BUTTONSPACING,             
        hWnd, 
        (HMENU)(idBase+idRemoveBN), 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }       

    // Don't need the between buttons spacing because there are no more controls.
    coy += Size.cy + DY_BUTTONSPACING; //+ DY_BETWEENBUTTONS;

    if ((pFileInfo->hWndWhole_EDGE = CreateWindow(
        "STATIC", 
        "", 
        WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME, 
        coxOld-DX_FRAMEEDGE,
        coyOld-DY_FRAMEEDGE,
        DX_CONTROLSPACING-DX_FRAMEEDGEINNER, 
        coy - coyOld + DY_FRAMEEDGE*2,              
        hWnd, 
        (HMENU)0, 
        hInst, 
        NULL)) == NULL)
        {
        nError = ER_CANNOTCREATEWINDOW;     
        goto DONE_ROUTINE;
        }   

    
    SetAllText(pFileInfo);
    UpdateLRVolume(pFileInfo);

/*  cT = GetNumControls(&FileInfoFirst);    
    GetWindowRect(hWnd, &rc);
    if (rc.right - rc.left < DX_MINWINDOW)
        rc.right = rc.left + DX_MINWINDOW;
    if (rc.bottom - rc.top < DY_MINWINDOW)
        rc.bottom = rc.top + DY_MINWINDOW;
    
    MoveWindow(hWnd, rc.left, rc.top, DX_CONTROLSPACING*cT+DX_WIN_BORDER, coy-coyOld+DY_WIN_BORDER, TRUE);
  */
    
DONE_ROUTINE:   
    if (hDC != NULL)
        {
        if (ReleaseDC(hWnd, hDC) == 0)
            {
            nError = ER_CANNOTRELEASEDC;            
            goto DONE_ROUTINE;
            }
        }

    return(nError);

}

/*  This will add to the linked list of FileInfo's.  The FileInfo's keep track of the
    files loaded, and this is done in a linked list format

    Input:
        pFileInfoHead           -   Top of linked list.
        pFileInfo               -   Pointer to entry to add.

    Output:
        0 if successful, else the error code.

*/      

int AddToList(
                    FILEINFO *pFileInfoHead, 
                    FILEINFO *pFileInfo
                    )
{

    pFileInfo->pNext = NULL;    
    pFileInfo->fPlaying = FALSE;

    while (pFileInfoHead->pNext != NULL)
        {
        pFileInfoHead = pFileInfoHead->pNext;
        }

    pFileInfoHead->pNext = pFileInfo;

    return(0);

}

/*  This routine will get the number of controls in the window.
    Can be used to determine new size of window.

    Input:
        pFileInfoHead           -   Header of linked list.

    Output:
        # of controls.

*/

int GetNumControls(
                    FILEINFO *pFileInfoHead
                    )
{

    int             cT              = 0;

    while (pFileInfoHead->pNext != NULL)
        {
        pFileInfoHead = pFileInfoHead->pNext;
        cT++;
        }

    return(cT);

}

/*  This routine will free the whole linked list in pFileInfoFirst, including all the
    memory used by the wave file, waveformatex structure, etc.

*/
int FreeAllList(
                    HWND hWnd, 
                    FILEINFO *pFileInfoFirst
                    )
{

    FILEINFO        *pFileInfo, 
                    *pFileNext;
    UINT            cT;

    Assert(pFileInfoFirst != NULL);
    pFileInfo = pFileInfoFirst->pNext;

    while (pFileInfo != NULL)
        {
        ReleaseDirectSoundBuffer(pFileInfo);
        GlobalFree(pFileInfo->pwfx);
        pFileNext = pFileInfo->pNext;
        GlobalFreePtr(pFileInfo);
        pFileInfo = pFileNext;
        }

    for (cT=0; cT<MAXCONTROLS; cT++)
        rgfcoxAvail[cT] = FALSE;



    return(0);          


}

/*  This routine will remove an entry from the list, i.e. will remove
    pFileInfo and all its allocated memory from the list pointed by the header
    by pFileInfoHead

    Input:
        pFileInfo               -   Pointer to entry to remove.
        pFileInfoHead           -   Head, first entry.

    Output:
        0 if successful, else the error.

*/
int RemoveFromList(
                    FILEINFO *pFileInfo, 
                    FILEINFO *pFileInfoHead
                    )
{

    FILEINFO        *pFileNext;

    Assert(pFileInfoHead != NULL);

    // This used to be pFileInfoHead != NULL
    while (pFileInfoHead->pNext != NULL)
        {
        if (pFileInfoHead->pNext == pFileInfo)
            {
            Assert(pFileInfo->cox/DX_CONTROLSPACING < MAXCONTROLS);
            rgfcoxAvail[pFileInfo->cox/DX_CONTROLSPACING] = FALSE;
           
            DestroyWindow(pFileInfo->hWndFileName_TXT); 
            DestroyWindow(pFileInfo->hWndFreq_TB);      
            DestroyWindow(pFileInfo->hWndFreq_TXT);     
            DestroyWindow(pFileInfo->hWndPan_TB);           
            DestroyWindow(pFileInfo->hWndPan_TXT);      
            DestroyWindow(pFileInfo->hWndVol_TXT);      
            DestroyWindow(pFileInfo->hWndVolL_TB);      
            DestroyWindow(pFileInfo->hWndVolR_TB);      
            DestroyWindow(pFileInfo->hWndVolM_TB);      
            DestroyWindow(pFileInfo->hWndLooped_BN);        
            DestroyWindow(pFileInfo->hWndPlay_BN);      
            DestroyWindow(pFileInfo->hWndRemove_BN);
            DestroyWindow(pFileInfo->hWndFileName_EDGE);
            DestroyWindow(pFileInfo->hWndLooped_EDGE);  
            DestroyWindow(pFileInfo->hWndFreq_EDGE);        
            DestroyWindow(pFileInfo->hWndPan_EDGE);     
            DestroyWindow(pFileInfo->hWndVol_EDGE);     
            DestroyWindow(pFileInfo->hWndWhole_EDGE);       
            DestroyWindow(pFileInfo->hWndVolL_TXT);     
            DestroyWindow(pFileInfo->hWndVolR_TXT);     
            #ifdef SHOWSTATUS
            DestroyWindow(pFileInfo->hWndStatus_TXT);       
            DestroyWindow(pFileInfo->hWndStatus_EDGE);      
            #endif




            GlobalFree(pFileInfoHead->pNext->pwfx);
            pFileNext = pFileInfoHead->pNext->pNext;
            GlobalFreePtr(pFileInfoHead->pNext);
            pFileInfoHead->pNext = pFileNext;                                                         
            break;
            }
        pFileInfoHead = pFileInfoHead->pNext;
        }

    return(0);
}

/*  This will pop up the open file dialog and allow the user to pick one file. 
    
    Input:  
        hWnd            -   Handle of parent window.
        pszFileName         -   String to store filename in, must be at least MAX_PATH long.


    Output:
        TRUE if a file was  picked successfully, else FALSE (user didn't pick a file)

 */
BOOL OpenFileDialog(
                    HWND            hWnd,
                    LPSTR           pszFileName,
                    int             *nFileName
                    )
{

    BOOL            fReturn,
                    fValid;
    OPENFILENAME    ofn;                

    pszFileName[0]          = 0;

    ofn.lStructSize         = sizeof(ofn);
    ofn.hwndOwner           = hWnd;
    ofn.hInstance           = hInst;
    ofn.lpstrFilter         = "Wave Files\0*.wav\0All Files\0*.*\0\0";
    ofn.lpstrCustomFilter   = NULL;
    ofn.nMaxCustFilter      = 0;
    ofn.nFilterIndex        = 1;
    ofn.lpstrFile           = pszFileName;
    ofn.nMaxFile            = MAX_PATH;
    ofn.lpstrFileTitle      = NULL;
    ofn.nMaxFileTitle       = 0;
    ofn.lpstrInitialDir     = ".";
    ofn.lpstrTitle          = "File Open";
    ofn.Flags               = OFN_FILEMUSTEXIST;
    ofn.nFileOffset         = 0;
    ofn.nFileExtension      = 0;
    ofn.lpstrDefExt         = "wav";
    ofn.lCustData           = 0;
    ofn.lpfnHook            = NULL;
    ofn.lpTemplateName      = NULL;
                                
    fValid = FALSE;
    do 
        {    
        
        if (fReturn = GetOpenFileName(&ofn))
            {                               
            fValid = IsValidWave(pszFileName);
            if (!fValid)
                MessageBox(hWnd, "Wave files must be PCM format!", "Invalid Wave File", MB_OK|MB_ICONSTOP);
            else
                *nFileName = ofn.nFileOffset;
            }
        else fValid = TRUE;         // Force break out of loop.

        } while (!fValid);

    return(fReturn);     

}

/*  This function will determine if the filename passed in is a valid wave for this
    app, that is a PCM wave.

    Input:
        pszFileName             -   FileName to check.

    Output:
        FALSE if not a valid wave, TRUE if it is.
    
*/
BOOL IsValidWave(
                    LPSTR       pszFileName
                    )

{ 
    BOOL            fReturn     = FALSE;
    int             nError      = 0;
    HMMIO           hmmio;
    MMCKINFO        mmck;
    WAVEFORMATEX    *pwfx;

    if ((nError = WaveOpenFile(pszFileName, &hmmio, &pwfx, &mmck)) != 0)
        {       
        goto ERROR_IN_ROUTINE;
        }

    if (pwfx->wFormatTag != WAVE_FORMAT_PCM) 
        {
        goto ERROR_IN_ROUTINE;
        }

    WaveCloseReadFile(&hmmio, &pwfx);

    fReturn = TRUE;

ERROR_IN_ROUTINE:
    return(fReturn);    

}


BOOL UIMainWindowVSBHandler(
                    HWND hWnd, 
                    WPARAM wParam, 
                    LPARAM lParam
                    )
{

    FILEINFO        *pFileInfo;
    BOOL            fReturn             = FALSE;

    pFileInfo = FileInfoFirst.pNext;

    Assert(pFileInfo != NULL);

    while (pFileInfo != NULL)
        {

        if ((HWND)LOWORD(lParam) == pFileInfo->hWndVolM_TB)
            {
            pFileInfo->dwVol = MAXVOL_TB - SendMessage(pFileInfo->hWndVolM_TB, TBM_GETPOS, 0, 0);
            ChangeOutputVol(pFileInfo);
            SetAllText(pFileInfo);
            UpdateLRVolume(pFileInfo);
            fReturn = TRUE;
            }

        pFileInfo = pFileInfo->pNext;

        }

    return (fReturn);

}


/*  This routine will handle all the calls to the WM_HSCROLL for the main window, that
    is, all the horizontal scrollbar (and trackbar) messages.

    Input:
        Standard parameters (minus the "message" parameter) for a window callback, though
        this is called from the window callback.

    Output:
        FALSE if the message isn't processed, else TRUE if it is.  If FALSE, the
        return procedure should call the default windows procedure.
        

*/
BOOL UIMainWindowHSBHandler(
                    HWND hWnd, 
                    WPARAM wParam, 
                    LPARAM lParam
                    )
{

    FILEINFO        *pFileInfo;
    BOOL            fReturn             = FALSE;

    pFileInfo = FileInfoFirst.pNext;

    Assert(pFileInfo != NULL);

    while (pFileInfo != NULL)
        {

        if ((HWND)LOWORD(lParam) == pFileInfo->hWndFreq_TB)
            {
            pFileInfo->dwFreq = SendMessage(pFileInfo->hWndFreq_TB, TBM_GETPOS, 0, 0);
            ChangeOutputFreq(pFileInfo);
            SetAllText(pFileInfo);          
            fReturn = TRUE;
            }

        else if ((HWND)LOWORD(lParam) == pFileInfo->hWndPan_TB)
            {
            pFileInfo->dwPan = SendMessage(pFileInfo->hWndPan_TB, TBM_GETPOS, 0, 0);
            ChangeOutputPan(pFileInfo);
            SetAllText(pFileInfo);
            UpdateLRVolume(pFileInfo);
            fReturn = TRUE;
            }

        pFileInfo = pFileInfo->pNext;

        }

    return (fReturn);
        


}

/*  This routine will handle all the calls to the WM_COMMAND for the main window.

    Input:
        Standard parameters (minus the "message" parameter) for a window callback, though
        this is called from the window callback.

    Output:
        FALSE if the message isn't processed, else TRUE if it is.  If FALSE, the
        return procedure should call the default windows procedure.
        

*/
BOOL UIMainWindowCMDHandler(
                    HWND hWnd, 
                    WPARAM wParam, 
                    LPARAM lParam
                    )
{

    BOOL            fReturn             = FALSE;
    FILEINFO        *pFileInfo;
    FILEINFO        *pFileInfoNext;

    pFileInfo = FileInfoFirst.pNext;
    
    while (pFileInfo != NULL)
        {
    
        pFileInfoNext = pFileInfo->pNext;

        if ((HWND)lParam == pFileInfo->hWndLooped_BN)
            {
            pFileInfo->fLooped = SendMessage(pFileInfo->hWndLooped_BN, BM_GETCHECK, 0, 0);          
            pFileInfo->pDSB->lpVtbl->SetLooping(pFileInfo->pDSB, pFileInfo->fLooped);           
            fReturn = TRUE;
            }
        else if ((HWND)lParam == pFileInfo->hWndPlay_BN)
            {
            if (pFileInfo->fPlaying)
                {
                if (StopDSound(hWnd, pFileInfo) == 0)
                    {
                    SendMessage((HWND)lParam, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szPlay);

                    #ifdef SHOWSTATUS
                    SendMessage(pFileInfo->hWndStatus_TXT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szStopped);
                    #endif

                    fReturn = TRUE;
                    break;
                    }
                                
                }
            else            
                {
                if (StartDSound(hWnd, pFileInfo) == 0)
                    {
                    SendMessage((HWND)lParam, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szStop);
                    #ifdef SHOWSTATUS
                    SendMessage(pFileInfo->hWndStatus_TXT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szPlaying);
                    #endif
                    
                    fReturn = TRUE;
                    break;
                    }
                
                }
            fReturn = TRUE;
            }
        
        else if ((HWND)lParam == pFileInfo->hWndRemove_BN)
            {
            ReleaseDirectSoundBuffer(pFileInfo);
            RemoveFromList(pFileInfo, &FileInfoFirst);
            
            fReturn = TRUE;
            }
        

        pFileInfo = pFileInfoNext;

        }
    
    if (!fReturn)
        {

        switch(wParam)
            {

            case IDPD_FILE_EXIT:    
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;

            case IDPD_FILE_OPEN:
                PD_FileOpen(hWnd);
                break;
            
            case IDPD_HELP_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)DLGHelpAbout);
                break;

            case IDPD_OPTIONS_OUTPUTTYPE:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_OUTPUTBUFFERTYPE), hWnd, (DLGPROC)DLGOutputBufferType);
                break;

            case IDPD_CHECKLATENCY:
                StopAllDSounds(hWnd, &FileInfoFirst);
                // Now fake that we're on in each voice so the timer will update the 
                // strings in the window.
                pFileInfo = FileInfoFirst.pNext;
                while (pFileInfo != NULL)
                    {                                           
                    pFileInfo->fPlaying = TRUE;
                    pFileInfo = pFileInfo->pNext;       
                    }

                DialogBox(hInst, MAKEINTRESOURCE(IDD_CHECKLATENCY), hWnd, (DLGPROC)DLGCheckLatency);
                break;


            default:
                return(FALSE);

            }
        }

    return(TRUE);


}

/*  This routine will handle the timer messages.

    Input:
        Standard input.

    Output: 
        TRUE if processed message, otherwise FALSE

*/
BOOL UIMainWindowTimerHandler(
                    HWND hWnd, 
                    WPARAM wParam, 
                    LPARAM lParam
                    )
{

    FILEINFO        *pFileInfo;
    BOOL            fReturn             = FALSE;
    DWORD           dwStatus            = 0;

    pFileInfo = FileInfoFirst.pNext;
        
    while (pFileInfo != NULL)
        {
        pFileInfo->pDSB->lpVtbl->GetStatus(pFileInfo->pDSB, &dwStatus);

        if ((pFileInfo->fPlaying) && (dwStatus == DSB_STATUSF_STOPPED))
            {
            if (StopDSound(hWnd, pFileInfo) == 0)
                {
                SendMessage(pFileInfo->hWndPlay_BN, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szPlay);

                #ifdef SHOWSTATUS
                SendMessage(pFileInfo->hWndStatus_TXT, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)szStopped);
                #endif

                fReturn = TRUE;
                }
            }
        pFileInfo = pFileInfo->pNext;
        }

    return (fReturn);

}



/*  This routine will start a sound to be played.  

    Input:
        hWnd                -   Of parent window.
        pFileInfo           -   Pointer to file to start, which is loaded and the
                                data is filled in the structure, such as pbData, 
                                etc.

    Output:
        0 if successful, else the error code.

*/
int StartDSound(
                    HWND hWnd, 
                    FILEINFO *pFileInfo
                    )
{

    HRESULT     hr              = 0;
    DWORD           dwLooped;
    DWORD           dwStatus                = 0;

    // Already playing?
/*  if (pFileInfo->fPlaying)
        return(0);*/
        
    

    // Start sound here....
    dwLooped = 0;
    if (pFileInfo->fLooped)
        dwLooped = DSBPLAYF_LOOP;
                    

    if ((hr = pFileInfo->pDSB->lpVtbl->GetStatus(pFileInfo->pDSB, &dwStatus)) == 0)
        {
        if ((dwStatus&DSB_STATUSF_PLAYING) == DSB_STATUSF_PLAYING)
            {
            // Don't bother playing, just restart
            if ((hr = pFileInfo->pDSB->lpVtbl->SetCurrentPosition(pFileInfo->pDSB, 0)) != 0)
                {
                MessageBox(hWnd, "Cannot set current position", "Direct Sound Error", MB_OK);
                }
            }
        // Yes gotos are bad but this is real life not school.
        else goto PLAY_THE_THING;           
        }
    
    else
        {
        PLAY_THE_THING:
        if ((hr = pFileInfo->pDSB->lpVtbl->Play(pFileInfo->pDSB, 0, dwLooped)) != 0)
            {
            MessageBox(hWnd, "Cannot start playing", "Direct Sound Error", MB_OK);
            }
        else
            pFileInfo->fPlaying = TRUE;
        }

    return(hr);


}

/*  This routine will stop a sound which is playing.

    Input:
        hWnd                - Of parent window.
        pFileInfo           - Pointer to file to stop playing.

    Output:
        0 if successful, else the error code.

*/
int StopDSound(
                    HWND hWnd, 
                    FILEINFO *pFileInfo
                    )

{

    HRESULT     hr          = 0;

    if (!pFileInfo->fPlaying)
        return(0);
           

    // Stop sound here...
    if ((hr = pFileInfo->pDSB->lpVtbl->Stop(pFileInfo->pDSB)) != 0) 
        {
        MessageBox(hWnd, "Cannot stop sound", "Direct Sound Error", MB_OK);     
        }
    else
        pFileInfo->fPlaying = FALSE;    

    return(hr);

}

/*  This routine will stop all the sounds which are playing.

    Input:
        hWnd                - Of parent window.
        pFileInfo           - Pointer to file to stop playing.(i.e. the head)

    Output:
        0 if successful, else the error code.

*/
int StopAllDSounds(
                    HWND hWnd, 
                    FILEINFO *pFileInfo
                    )

{

    while (pFileInfo->pNext != NULL)
        {
        StopDSound(hWnd, pFileInfo->pNext);
        pFileInfo = pFileInfo->pNext;       
        }

    return(0);

}



/*  This routine will set the freq, vol and pan slider text according to the value 
    passed in.

    Input:
        pFileInfo       -   File pointer to set frequency for.

    The dwFreq in the pFileInfo structure must be set.  This also uses the window handle
    in the pFileInfo structure.
    
    Output:
        None.

*/
void SetAllText(
                    FILEINFO    *pFileInfo
                    )
{
    char            szBufT[128];

    sprintf(szBufT, "%s: %lu Hz     ", szFreq, pFileInfo->dwFreq*FREQMUL+FREQADD);
    SetWindowText(pFileInfo->hWndFreq_TXT, szBufT);

    sprintf(szBufT, "%s: %lu", szPan, pFileInfo->dwPan);
    SetWindowText(pFileInfo->hWndPan_TXT, szBufT);

    sprintf(szBufT, "%s: %lu", szVolume, pFileInfo->dwVol);
    SetWindowText(pFileInfo->hWndVol_TXT, szBufT);


}

/*  This routine will update the left and right volume according to main volume 
    and pan.

    Input:
        pFileInfo           -   Pointer to fileinfo to update.

    Output:
        Nothing worth using.
                    

*/
void UpdateLRVolume(
                    FILEINFO *pFileInfo
                    )
{

    int             volLeft, 
                    volRight;

    if (pFileInfo->dwPan < MIDPAN_TB)
        {
        volLeft = pFileInfo->dwVol;
        volRight = (((int)pFileInfo->dwPan)*(int)pFileInfo->dwVol)/((int)MIDPAN_TB);
        }
    else
        {
        volLeft = ((((int)pFileInfo->dwPan - MAXPAN_TB)*-1)*(int)pFileInfo->dwVol)/((int)MIDPAN_TB);
        volRight = pFileInfo->dwVol;
        }

        

//  volRight = (((int)pFileInfo->dwPan)*(int)pFileInfo->dwVol)/((int)MAXPAN_TB);
    SendMessage(pFileInfo->hWndVolL_TB, TBM_SETPOS, TRUE, MAXVOL_TB-volLeft);
    SendMessage(pFileInfo->hWndVolR_TB, TBM_SETPOS, TRUE, MAXVOL_TB-volRight);
    
        

}

/*  This will change the output panning position for a certain FILEINFO.  This is 
    done by sending messages to the direct sound driver 

    Input:  
        pFileInfo                   -   FileInfo to set.  This must contain the
                                        panning value to set.

    Output:
        0 if successful, else the error code.

*/
int ChangeOutputPan(
                    FILEINFO *pFileInfo
                    )

{

    HRESULT     hr      = 0;


    if ((hr = pFileInfo->pDSB->lpVtbl->SetPan(pFileInfo->pDSB, pFileInfo->dwPan)) != 0)
        {
        goto ERROR_DONE_ROUTINE;
        }

ERROR_DONE_ROUTINE:
    return(hr);

}

/*  This will change the output freq for a certain FILEINFO.  This is 
    done by sending messages to the direct sound driver 

    Input:  
        pFileInfo                   -   FileInfo to set.  This must contain the
                                        freq value to set.

    Output:
        0 if successful, else the error code.

*/
int ChangeOutputFreq(
                    FILEINFO *pFileInfo
                    )

{

    HRESULT     hr      = 0;


    if ((hr = pFileInfo->pDSB->lpVtbl->SetFrequency(pFileInfo->pDSB, pFileInfo->dwFreq*FREQMUL+FREQADD)) != 0)
        {
        goto ERROR_DONE_ROUTINE;
        }

ERROR_DONE_ROUTINE:
    return(hr);

}



/*  This will change the output vol for a certain FILEINFO.  This is 
    done by sending messages to the direct sound driver 

    Input:  
        pFileInfo                   -   FileInfo to set.  This must contain the
                                        freq value to set.

    Output:
        0 if successful, else the error code.

*/
int ChangeOutputVol(
                    FILEINFO *pFileInfo
                    )

{

    HRESULT     hr      = 0;


    if ((hr = pFileInfo->pDSB->lpVtbl->SetVolume(pFileInfo->pDSB, pFileInfo->dwVol)) != 0)
        {
        goto ERROR_DONE_ROUTINE;
        }

ERROR_DONE_ROUTINE:
    return(hr);

}


/*  This is the dialog box handler for the check latency dialog box.

    Input:
        Standard dialog box input.

    Output:
        Standard dialog box output.

*/

long FAR PASCAL DLGCheckLatency(
                    HWND hWnd, 
                    UINT uMsg, 
                    WPARAM wParam, 
                    LPARAM lParam
                    )
{

    static HWND             hWndFiles_LB;
    FILEINFO                *pFileInfo              = NULL;
    int                     nSelected;
    int                     cT;

             
    switch(uMsg)
        {
        case WM_INITDIALOG:
            hWndFiles_LB = GetDlgItem(hWnd, IDC_FILES_LB);
            
            pFileInfo = FileInfoFirst.pNext;
            while (pFileInfo != NULL)
                {               
                SendMessage(hWndFiles_LB, LB_ADDSTRING, 0, (LPARAM)(pFileInfo->szFileName+pFileInfo->nFileName));
                pFileInfo = pFileInfo->pNext;       
                }

            break;      
        
        case WM_COMMAND:
            switch(wParam)
                {
                case ID_DONE:                   
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;

                case ID_PLAY:                       
                    if ((nSelected = SendMessage(hWndFiles_LB, LB_GETCURSEL, 0, 0)) != LB_ERR)
                        {
                        for (cT=0, pFileInfo = FileInfoFirst.pNext; pFileInfo != NULL; pFileInfo = pFileInfo->pNext, cT++)
                            {
                            if (cT == nSelected)
                                {
                                StartDSound(hWnd, pFileInfo);
                                break;
                                }
                            }
                            
                        }
                    
                    break;
                    
                case ID_STOP:
                    StopAllDSounds(hWnd, &FileInfoFirst);
                    break;

                default:
                    break;

                }
            break;

        case WM_CLOSE:
            StopAllDSounds(hWnd, &FileInfoFirst);
            EndDialog(hWnd, 0);
            break;

        default:
            return(0);
            break;               

        }
                
    return(1);

}


/*  The help about dialog procedure.  
    
    Input:
        Standard windows dialog procedure.

    Output:
        Standard windows dialog procedure.

*/
long FAR PASCAL DLGHelpAbout(
                    HWND hWnd, 
                    UINT uMsg, 
                    WPARAM wParam, 
                    LPARAM lParam
                    )
{

             
    switch(uMsg)
        {
        case WM_INITDIALOG:
            break;      
        
        case WM_COMMAND:
            switch(wParam)
                {
                case ID_OK:                 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;

                default:
                    break;

                }
            break;

        case WM_CLOSE:
            EndDialog(hWnd, 0);
            break;

        default:
            return(0);
            break;               

        }
                
    return(1);

}


/*  The help about dialog procedure.  
    
    Input:
        Standard windows dialog procedure.

    Output:
        Standard windows dialog procedure.

*/

long FAR PASCAL DLGOutputBufferType(
                    HWND hWnd, 
                    UINT uMsg, 
                    WPARAM wParam, 
                    LPARAM lParam
                    )
{

    static HWND                 hWndFormats_LB          = NULL;
    
    int                         cT;
    int                         nSelection;

             
    switch(uMsg)
        {
        case WM_INITDIALOG:
            // Get the windows we need.
            hWndFormats_LB = GetDlgItem(hWnd, IDC_FORMATS);

            // Put the strings in the list box.
            for (cT=0; cT<C_DROPDOWNPCMFORMATS; cT++)
                SendMessage(hWndFormats_LB, LB_ADDSTRING, 0, (LPARAM)rgszTypes[cT]);

            // Get the current format and highlight it in the list box.
            if ((nSelection = FormatToIndex(hWnd, &FileInfoFirst)) != LB_ERR)
                {
                SendMessage(hWndFormats_LB, LB_SETCURSEL, nSelection, 0);
                }


            break;      
        
        case WM_COMMAND:
            switch(wParam)
                {
                case ID_OK:                 
                    if ((nSelection = SendMessage(hWndFormats_LB, LB_GETCURSEL, 0, 0)) != LB_ERR)
                        {
                        if (IndexToFormat(hWnd, &FileInfoFirst, nSelection) == 0)                           
                            PostMessage(hWnd, WM_CLOSE, 0, 0);                          
                        }
                    break;
                
                case ID_CANCEL:                 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;

                case ID_APPLY:                  
                    if ((nSelection = SendMessage(hWndFormats_LB, LB_GETCURSEL, 0, 0)) != LB_ERR)                       
                        IndexToFormat(hWnd, &FileInfoFirst, nSelection);
                        
                    break;


                default:
                    break;

                }
            break;

        case WM_CLOSE:
            EndDialog(hWnd, 0);
            break;

        default:
            return(0);
            break;               

        }
                
    return(1);

}

/*  This routine will determine the output format in terms of an integer from the
    current output rate, type, etc. stored in the direct sound routines.   Integer
    values designate the string # in rgszTypes, i.e. index 0 is 8000kHz, 8 bit mono, 
    etc...

    Input:
        hWnd                    - Handle of the current window.
        pFileInfo               - Pointer to the file info to retrieve the format for.

    Output:
        The index of the format, LB_ERR if undetermined.

*/
int FormatToIndex(
                    HWND        hWnd, 
                    FILEINFO    *pFileInfo
                    )

{

    DSBUFFERFORMAT              dsbf;
    WAVEFORMATEX                wfx;
    DWORD                       dwWaveStyle;
    int                         nError              = 0;

    wfx.cbSize = 0;
    dsbf.pwfx = &wfx;
    dsbf.cbwfx = sizeof(wfx);

    // Get the format.
    if ((nError = pFileInfo->pDSB->lpVtbl->GetFormat(pFileInfo->pDSB, &dsbf)) != 0)
        {
        goto ERROR_IN_ROUTINE;
        }


    // Change wfx to an integer.  Assume theres an error and check all parameters to 
    // see if its valid.
    nError = LB_ERR;
    dwWaveStyle = 0;

    if (wfx.wFormatTag != WAVE_FORMAT_PCM)
           goto ERROR_IN_ROUTINE;

    // Check the channels
    if (wfx.nChannels == 1);
    else if (wfx.nChannels == 2)
        dwWaveStyle |= 1;
    else
        goto ERROR_IN_ROUTINE;

    // Check the bits...
    if (wfx.wBitsPerSample == 8);
    else if (wfx.wBitsPerSample == 16)
        dwWaveStyle |= 2;
    else
        goto ERROR_IN_ROUTINE;

    // Check the rate.
    if (wfx.nSamplesPerSec == 8000);
    else if (wfx.nSamplesPerSec == 11025)
        dwWaveStyle |= 4;
    else if (wfx.nSamplesPerSec == 22050)
        dwWaveStyle |= 8;
    else if (wfx.nSamplesPerSec == 44100)
        dwWaveStyle |= 12;
    else
        goto ERROR_IN_ROUTINE;

    nError = (int)dwWaveStyle;

ERROR_IN_ROUTINE:
    return(nError);
}


/*  This will convert an index (from a list box for instance) to a format by passing
    in the format to direct sound.

    Input:
        hWnd                -   Handle to window.
        pFileInfo           -   Pointer to current file info.
        index               -   Index value to convert to a waveformat structure.

    Output:
        0 if successful, else the error code.

*/
int IndexToFormat(
                    HWND        hWnd, 
                    FILEINFO    *pFileInfo,
                    int         index
                    )

{

    int         nError      = 0;
    DSBUFFERFORMAT  dsbf;


    pFileInfo->pwfx->wFormatTag = WAVE_FORMAT_PCM;

    pFileInfo->pwfx->nChannels = 2;                                     // Assume stereo.
    if ((index%2) == 0)
    pFileInfo->pwfx->nChannels = 1;                                 // Its mono.
        
    // Assume 16 bit    
    pFileInfo->pwfx->nBlockAlign = 2*pFileInfo->pwfx->nChannels;
    pFileInfo->pwfx->wBitsPerSample = 16;
    if ((index%4) < 2) {
    // Its 8 bit.
    pFileInfo->pwfx->nBlockAlign = 1*pFileInfo->pwfx->nChannels;
    pFileInfo->pwfx->wBitsPerSample = 8;
    }
    
    pFileInfo->pwfx->nSamplesPerSec = 44100;    // Assume 44.1 kHz
    if (index < 4)
        pFileInfo->pwfx->nSamplesPerSec = 8000;
    else if (index < 8)
        pFileInfo->pwfx->nSamplesPerSec = 11025;
    else if (index < 12)
        pFileInfo->pwfx->nSamplesPerSec = 22050;
    
    
    pFileInfo->pwfx->nAvgBytesPerSec = pFileInfo->pwfx->nSamplesPerSec *
                       pFileInfo->pwfx->nBlockAlign;                                        
    pFileInfo->pwfx->cbSize = 0;


    dsbf.dwSize             = sizeof(DSBUFFERFORMAT);
    dsbf.fdwBufferFormat    = 0L;
    dsbf.pwfx           = pFileInfo->pwfx;
    dsbf.cbwfx          = sizeof(WAVEFORMATEX);
    
    if ((nError = pFileInfo->pDSB->lpVtbl->Stop(pFileInfo->pDSB))
            != 0)   {
    MessageBox(hWnd, "Cannot Stop buffer",
           "Direct Sound Error", MB_OK);
    goto ERROR_DONE_ROUTINE;
    }
    if ((nError = pFileInfo->pDSB->lpVtbl->SetFormat(pFileInfo->pDSB,&dsbf))
            != 0)   {
    MessageBox(hWnd, "Cannot set format buffer",
           "Direct Sound Error", MB_OK);
    pFileInfo->pDSB->lpVtbl->Play(pFileInfo->pDSB,0, 0);
    goto ERROR_DONE_ROUTINE;
    }
    if ((nError = pFileInfo->pDSB->lpVtbl->Play(pFileInfo->pDSB,0, 0))
            != 0)   {
    MessageBox(hWnd, "Cannot Play buffer",
           "Direct Sound Error", MB_OK);
    goto ERROR_DONE_ROUTINE;
    }

ERROR_DONE_ROUTINE:
    return(nError);

}
