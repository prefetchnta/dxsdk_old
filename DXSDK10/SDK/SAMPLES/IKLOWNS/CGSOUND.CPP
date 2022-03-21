/*===========================================================================*\
|
|  File:        cgsound.cpp
|
|  Description: 
|   Sample Immortal Klowns game sound effects routines.
|
|   Sound effects are implemented as an object which are
|   loaded from a wave file (.WAV) and can be mixed with 
|   other sounds as well as being algorithmically altered
|   (panned from left to right, pitch changes, and volume 
|   changes).
|
|   For demonstration purposes, sounds may be played using a
|   variety of methods.
|       1) DirectSound - the preferred method, requires
|           that a supported sound board be present
|           and that the DirectSound drivers be installed.
|           Offers greatest flexibility and lowest latency.
|           Doesn't support compressed wave files.
|       2) waveOut - standard Windows API for low-level sound
|           support.  Hardest to code for and not as flexible
|           as DirectSound nor as low in latency.  Assumes
|           similar format for all waves.  Only allows playing
|           one sound at a time (without using WaveMix).
|       3) sndPlaySound  - the simpliest interface which
|           yields the least flexibility and worst latency.
|           Only one sound may be played at a time!
|
|   If desired, individual sounds may be played using different
|   methods (there's no advantage to this, it's just for demos).
|
|   It would be nice to implement the WaveMix API which allow
|   mixing of sounds under waveOut.  Perhaps in the next release!
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

//** include files **
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "linklist.h"
#include "cgwave.h"
#include "cgsound.h"

//** local definitions **
// Default volume and panning values
#define DSBSIZE                 0x10000
#define MINFREQ_TB              0
#define MAXFREQ_TB              512
#define MINPAN_TB               0
#define MAXPAN_TB               127
#define MIDPAN_TB               64
#define MINVOL_TB               0
#define MAXVOL_TB               127

//** external functions **
//** external data **
//** public data **

//** private data **

// To prevent having more than one instance of the same
// sound in memory, a list of all sounds is kept.
static CLinkedList  *mpWaveList = NULL;

// Pointer to DirectSound object.
static LPDIRECTSOUND    gpds = NULL;

// Handle of WaveOut device.
static HWAVEOUT     hWaveDevice = NULL;


//** public functions **
//** private functions **

// ----------------------------------------------------------
// SetBufferFormat - set wave format of a direct sound buffer
// ----------------------------------------------------------
void SetBufferFormat(
    DSBUFFERDESC    *dsbd,  // buffer description struct
    DSBUFFERFORMAT  *dsbf   // buffer format structure
)
{
    // Make sure we have a buffer to work on
    if (dsbd->pIDSBuffer != NULL)
    {
        // Set the format (bits/sample, mono, stereo, etc.)
        dsbd->pIDSBuffer->SetFormat(dsbf);

        // Set values for freq and volume
        dsbd->pIDSBuffer->SetFrequency(dsbf->pwfx->nSamplesPerSec);
        dsbd->pIDSBuffer->SetVolume(MAXVOL_TB);
        dsbd->pIDSBuffer->SetPan(MIDPAN_TB);
        dsbd->pIDSBuffer->SetLooping(FALSE);
    }
}   

// ----------------------------------------------------------
// CreatePrimarySoundBuffer - create a primary sound buffer 
//  for direct sound.  Every DirectSound app needs to have
//  one (and only one) DirectSound buffer.
// ----------------------------------------------------------
BOOL CreatePrimarySoundBuffer()
{
    DSBUFFERDESC        dsbd;       // buffer description struct
    DSBUFFERFORMAT      dsbf;       // buffer format struct
    MMRESULT        mmResult;   // result of sound calls
    DWORD           dw;     // size place holder

    // Check if we already have a primary buffer
    if (gpds != NULL)
    {
        return(TRUE);
    }

    // Create the Direct Sound Object
    if (DirectSoundCreate(NULL,&gpds) != 0)
    {
        return(FALSE);
    }

    // Set up the primary direct sound buffer. 
    memset(&dsbd, 0, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.fdwBufferDesc = DSB_DESCRIPTIONF_PRIMARY | DSB_DESCRIPTIONF_LOOPABLE;
    dsbd.cbBufferSize = DSBSIZE;
    dsbd.dsbfFormat.dwSize = sizeof( DSBUFFERFORMAT );
    if ((mmResult = gpds->CreateSoundBuffer(&dsbd)) != 0)
    {
        return(FALSE);
    }

    dsbf.dwSize = sizeof(DSBUFFERFORMAT);
    dsbf.fdwBufferFormat = 0L;

    // Figure out how big a buffer to allocate for wave format info
    if ((mmResult = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &dw)) != 0)    
    {
        return(FALSE);
    }

    if ((dsbf.pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GPTR|GMEM_ZEROINIT, dw)) == NULL)
    {
        return(FALSE);
    }

    dsbf.cbwfx = dsbf.pwfx->cbSize + sizeof(WAVEFORMATEX);

    // Setup the format, frequency, volume, etc.
    dsbf.pwfx->wFormatTag = WAVE_FORMAT_PCM;
    dsbf.pwfx->nChannels = 2;
    dsbf.pwfx->nSamplesPerSec = 22050;
    dsbf.pwfx->nAvgBytesPerSec = 22050*2*2;
    dsbf.pwfx->nBlockAlign = 4;
    dsbf.pwfx->wBitsPerSample = 16;
    dsbf.pwfx->cbSize = 0;
    SetBufferFormat(&dsbd, &dsbf);

    // Need to start playing the primary buffer before any
    // sounds can be mixed into it!
    dsbd.pIDSBuffer->Play( 0, 0 );

    
    return(TRUE);
}

// ----------------------------------------------------------
// InitalizeWaveDevice - be sure wave method is ready for use.
// ----------------------------------------------------------
BOOL InitializeWaveDevice(
    int     WaveMode,   // sndPlaySound, waveOut, or DirectSound
    LPWAVEFORMATEX  pFormat     // default wave format
)
{
    // If we are doing waveOut's then we need a handle to the device
    // driver.
    if (WaveMode == USE_WAVEOUT)
    {

        // If there isn't a wave device already open, open one
        // using the given format
        if (hWaveDevice == NULL)
        {
            if (waveOutOpen((LPHWAVEOUT)&hWaveDevice
            , WAVE_MAPPER, pFormat, NULL, 0L, 0))
            {
                return(FALSE);
            }
        }
    // Must be using DirectSound, make sure we have a primary buffer
    } else {
        // Create a DirectSound primary buffer
        return (CreatePrimarySoundBuffer());
    }
    return(TRUE);
}


// ----------------------------------------------------------
// LoadWaveData - read .WAV file information into appropriate
//  memory buffer for playback.
// ----------------------------------------------------------
LPWAVEHDR LoadWaveData(
    CSoundEffect    *pSound,    // sound effect instance
    LPSTR       WaveName,   // .WAV filename
    int     WaveMode    // sndPlaySound, waveOut or DirectSound
)
{
    LPBYTE      lpData = NULL;
    LPWAVEHDR   lpWaveHdr = NULL;
    DWORD       dwDataSize;
    DSBUFFERDESC    *dsbd = NULL;
    LPWAVEFORMATEX  pwfxInfo = NULL;

    // Check to be sure a non-null name was given
    if ((WaveName == NULL) || (*WaveName == '\0'))
    {
        return(NULL);
    }

    // For sndPlaySound, we just read the whole file into a buffer
    if (WaveMode == USE_SNDPLAY)
    {
        HANDLE  hFile = CreateFile(WaveName, GENERIC_READ, FILE_SHARE_READ
        , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            return(NULL);
        }
            
        dwDataSize = GetFileSize(hFile, NULL);
        if ((dwDataSize == 0xFFFFFFFF) || (dwDataSize == 0))
        {
            CloseHandle(hFile);
            return(NULL);
        }

        // Allocate and lock memory for the waveform data. The memory
        // for waveform data must be globally allocated with
        // GMEM_MOVEABLE and GMEM_SHARE flags.
        if ((lpData = (LPBYTE) GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE
        , dwDataSize)) == NULL)
        {
            CloseHandle(hFile);
            return(NULL);
        }

        // Read the whole wave file in 
        ReadFile(hFile, lpData, dwDataSize, &dwDataSize, NULL);
        CloseHandle(hFile);

    // Either DirectSound or WaveOut
    } else {

        HMMIO       hmmioIn;    
        MMCKINFO    ckInRiff;
        MMCKINFO    ckIn;
        UINT        cbActualRead;
        UINT        cbSize;

        pwfxInfo = NULL;
        cbSize = 0;

        // Use routines in CGWAVE to open the sound file and
        // parse the data in it.    
        if (WaveOpenFile(WaveName, &hmmioIn, &pwfxInfo, &ckInRiff) != 0)
        {
            if (pwfxInfo != NULL)
                GlobalFree(pwfxInfo);
            return(NULL);
        }

        // Be sure we have an output device ready to play the sound
        if (!InitializeWaveDevice(WaveMode, pwfxInfo))
        {
            mmioClose(hmmioIn, 0);
            if (pwfxInfo != NULL)
                GlobalFree(pwfxInfo);
            return(NULL);
        }


        // Position the wave file for reading the wave data
        if (WaveStartDataRead(&hmmioIn, &ckIn, &ckInRiff) != 0)
        {
            mmioClose(hmmioIn, 0);
            if (pwfxInfo != NULL)
                GlobalFree(pwfxInfo);
            return(NULL);
        }

        // Ok, size of wave data is in ckIn, allocate that buffer.
        dwDataSize = ckIn.cksize;

        // waveOut requires that we allocate the data
        if (WaveMode == USE_WAVEOUT)
        {

            if ((lpData = (LPBYTE)GlobalAlloc(GMEM_FIXED
            , dwDataSize)) == NULL)
            {
                mmioClose(hmmioIn, 0);
                if (pwfxInfo != NULL)
                    GlobalFree(pwfxInfo);
                return(NULL);
            }
        // For DirectSound, we let it allocate the buffer for us.
        } else {
            dsbd = new DSBUFFERDESC;
            DWORD   dwBSize;
            DWORD   dwWrapBSize;
            LPVOID  lpWrapPtr;
            
            // Set up the secondary direct sound buffer. 
            memset(dsbd, 0, sizeof(DSBUFFERDESC));
            dsbd->dwSize = sizeof(DSBUFFERDESC);
            dsbd->fdwBufferDesc = 0;    //DSB_DESCRIPTIONF_LOOPABLE;
            dsbd->cbBufferSize = dwDataSize;
            dsbd->dsbfFormat.dwSize = sizeof( DSBUFFERFORMAT );

            if (gpds->CreateSoundBuffer(dsbd) != 0)
            {
                delete dsbd;
                mmioClose(hmmioIn, 0);
                return NULL;
            }

            // Need to lock the buffer so that we can write to it!
            if (dsbd->pIDSBuffer->Lock(
                    0,
                    dwDataSize,
                    (LPLPVOID)&lpData,
                    &dwBSize,
                    &lpWrapPtr,
                    &dwWrapBSize,
                    NULL,
                    0L) != 0)
            {
                delete dsbd;
                mmioClose(hmmioIn, 0);
                return NULL;
            } else {
                dwDataSize = dwBSize;
            }
        }

        // Now read the actual wave data into the data buffer.
        if (WaveReadFile(hmmioIn, dwDataSize, lpData, &ckIn
        , &cbActualRead) != 0)
        {
            // Data didn't get read for some reason!
            if (pwfxInfo != NULL)
                GlobalFree(pwfxInfo);
            if (lpData != NULL)
                GlobalFree(lpData);
        }    

        // Save info on size of data and close the wave file    
        cbSize = cbActualRead;
        dwDataSize = cbSize;
        mmioClose(hmmioIn, 0);

        // If we have a DirectSound buffer, set format & unlock it.
        if (dsbd != NULL)
        {
            DSBUFFERFORMAT  dsbf;

            dsbf.dwSize = sizeof(DSBUFFERFORMAT);
            dsbf.fdwBufferFormat = 0L;

            dsbf.cbwfx = pwfxInfo->cbSize + sizeof(WAVEFORMATEX);
            dsbf.pwfx = pwfxInfo;
            dsbd->pIDSBuffer->Unlock(&lpData, NULL);

            // Set the format of the output buffer while we're at it
            SetBufferFormat(dsbd, &dsbf);
        }

    }

    // Allocate and lock memory for the header. This memory must
    // also be globally allocated with GMEM_MOVEABLE and GMEM_SHARE flags.
    if ((lpWaveHdr = (LPWAVEHDR) GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE
    , (DWORD)sizeof(WAVEHDR))) == NULL) {
        GlobalFreePtr(lpData);
        return(NULL);
    }

    // After allocation, set up and prepare header.  This struct will be
    // used to play the sounds at the appropriate time.
    lpWaveHdr->lpData = (LPSTR) lpData;
    lpWaveHdr->dwBufferLength = dwDataSize;
    lpWaveHdr->dwFlags = 0L;
    lpWaveHdr->dwLoops = 0L;

    pSound->pWaveInfo->dsbd = dsbd;
    pSound->pWaveInfo->pwfxInfo = pwfxInfo;

    return(lpWaveHdr);
}   

// ----------------------------------------------------------
// CSoundEffect constructor
// ----------------------------------------------------------
CSoundEffect::CSoundEffect(
    LPSTR   WaveName,   // name of wave file
    DWORD   Id,
    BOOL    fLoopIt,    // loop the sound when played
    int WaveMode    // sndPlaySound, waveOut or DirectSound 
)
{
    pWaveInfo = NULL;
    fLoop = fLoopIt;
    fPlaying = FALSE;

    // Guard against wise-guys (we don't mind)
    if ((WaveName == NULL) || (*WaveName == '\0'))
        return;

    CSoundEffect::WaveMode = WaveMode;
    curVolume = MAXVOL_TB;
    fMuted = FALSE;
    dsbd = NULL;

    // initialize wave list if necessary
    if (!mpWaveList)
    {
        mpWaveList = new CLinkedList;
    } else {
        // Need to search wave list to see if we already have this
        // wave data cached.
        WAVE_ENTRY  *pCurBuffer = (WAVE_ENTRY *) mpWaveList->GetFirst();

        while (pCurBuffer && !pWaveInfo)
        {
            // Do the name and mode match?
            if ((lstrcmpi( WaveName, pCurBuffer->mpFileName ) == 0 )
            && (pCurBuffer->WaveMode == WaveMode))
            {
                pCurBuffer->mRefCount++;
                pWaveInfo = pCurBuffer;
            } else {
                pCurBuffer = (WAVE_ENTRY *) mpWaveList->GetNext();
            }
        }
    }

    // if we didn't find the wave file in our cache, we need to load it
    if (!pWaveInfo)
    {
        LPWAVEHDR   pWaveHdr;
        pWaveInfo = new WAVE_ENTRY;

        // Load up the wave data            
        if (pWaveHdr = LoadWaveData(this, WaveName, WaveMode))
        {
            // Add 
            pWaveInfo->mpFileName = new char [lstrlen(WaveName)+1];
            lstrcpy(pWaveInfo->mpFileName, WaveName);
            pWaveInfo->pWaveHdr = pWaveHdr;
            pWaveInfo->WaveMode = WaveMode;
            pWaveInfo->hObjectId = Id;
            pWaveInfo->mRefCount = 1;
            mpWaveList->Append( pWaveInfo );
            dsbd = pWaveInfo->dsbd;
        } else {
            delete pWaveInfo;
            pWaveInfo = NULL;
        }
#if 1
    // For DirectSound, we want each instance to have its own
    // buffer so that like sounds can be mixed.
    } else if (WaveMode == USE_DSOUND) {
        dsbd = new DSBUFFERDESC;
        
        // Set up the secondary direct sound buffer based on
        // original
        memset(dsbd, 0, sizeof(DSBUFFERDESC));
        dsbd->dwSize = sizeof(DSBUFFERDESC);
        dsbd->fdwBufferDesc = pWaveInfo->dsbd->fdwBufferDesc;
        dsbd->cbBufferSize = pWaveInfo->dsbd->cbBufferSize;
        dsbd->dsbfFormat.dwSize = sizeof( DSBUFFERFORMAT );

        // Create a new buffer and copy sound data from existing
        // buffer into it.
        if (gpds->CreateSoundBuffer(dsbd) == 0)
        {
            LPSTR   lpDest, lpSrc;
            DWORD   dwDestBSize;
            DWORD   dwDestWrapBSize;
            LPVOID  lpDestWrapPtr;
            DWORD   dwSrcBSize;
            DWORD   dwSrcWrapBSize;
            LPVOID  lpSrcWrapPtr;

            // Need to lock the buffer so that we can write to it!
            if ((dsbd->pIDSBuffer->Lock(
                0,
                dsbd->cbBufferSize,
                (LPLPVOID)&lpDest,
                &dwDestBSize,
                    &lpDestWrapPtr,
                &dwDestWrapBSize,
                NULL,
                0L) == 0)
            && (pWaveInfo->dsbd->pIDSBuffer->Lock(
                0,
                pWaveInfo->dsbd->cbBufferSize,
                (LPLPVOID)&lpSrc,
                &dwSrcBSize,
                    &lpSrcWrapPtr,
                &dwSrcWrapBSize,
                NULL,
                0L) == 0))
            {
                // copy data from one buffer to the other
                memcpy(lpDest, lpSrc, dwDestBSize);

                pWaveInfo->dsbd->pIDSBuffer->Unlock(&lpSrc, NULL);
                dsbd->pIDSBuffer->Unlock(&lpDest, NULL);

                DSBUFFERFORMAT  dsbf;

                dsbf.dwSize = sizeof(DSBUFFERFORMAT);
                dsbf.fdwBufferFormat = 0L;
                dsbf.cbwfx = pWaveInfo->pwfxInfo->cbSize + sizeof(WAVEFORMATEX);
                dsbf.pwfx = pWaveInfo->pwfxInfo;

                // Set the format of the output buffer while we're at it
                SetBufferFormat(dsbd, &dsbf);
            } else {
                delete dsbd;
                dsbd = NULL;
            }

        } else {
            delete dsbd;
            dsbd = NULL;
        }
#else
    } else  {
        dsbd = pWaveInfo->dsbd;
#endif
    }
}

// ----------------------------------------------------------
// CSoundEffect destructor
// ----------------------------------------------------------
CSoundEffect::~CSoundEffect()
{
    Stop();

    if ((pWaveInfo != NULL) && (--pWaveInfo->mRefCount == 0))
    {
        mpWaveList->Remove( pWaveInfo );

        delete pWaveInfo->mpFileName;
        if (dsbd != NULL)
        {
            dsbd->pIDSBuffer->Release();
            delete dsbd;
        } else {
            GlobalFreePtr(pWaveInfo->pWaveHdr->lpData);
        }
        GlobalFreePtr(pWaveInfo->pwfxInfo);
        GlobalFreePtr(pWaveInfo->pWaveHdr);
        delete pWaveInfo;       // causes GP fault why?
        pWaveInfo = NULL;
        // !!! should we delete the list if empty?
    }
}

// ----------------------------------------------------------
// Play - 
// ----------------------------------------------------------
void CSoundEffect::Play()
{
    if (pWaveInfo == NULL) 
    {
        return;
    }

    fPlaying = TRUE;

    switch (WaveMode)
    {
    case USE_WAVEOUT:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            pWaveInfo->pWaveHdr->dwLoops = fLoop;
            waveOutPrepareHeader(hWaveDevice, pWaveInfo->pWaveHdr, sizeof(WAVEHDR));
            waveOutWrite(hWaveDevice, pWaveInfo->pWaveHdr, sizeof(WAVEHDR));
            waveOutUnprepareHeader(hWaveDevice, pWaveInfo->pWaveHdr, sizeof(WAVEHDR));
        }
        break;
    case USE_DSOUND:
        if (dsbd != NULL)
        {
                if( fLoop )
            {
                dsbd->pIDSBuffer->Play( 0, DSBPLAYF_LOOP );
            }
            else
            {
                dsbd->pIDSBuffer->Play( 0, 0 );
            }
        }
        break;
    case USE_SNDPLAY:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            UINT    flags = SND_MEMORY | SND_ASYNC;

            if (fLoop)
                flags |= SND_LOOP;

            sndPlaySound((LPSTR)(pWaveInfo->pWaveHdr->lpData), flags);
        }
        break;
    }
}

// ----------------------------------------------------------
// Stop - 
// ----------------------------------------------------------
void CSoundEffect::Stop()
{
    if ((pWaveInfo == NULL) || (!fPlaying))
    {
        return;
    }

    fPlaying = FALSE;

    switch (WaveMode)
    {
    case USE_WAVEOUT:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            waveOutReset(hWaveDevice);
        }
        break;
    case USE_DSOUND:
        if (dsbd != NULL)
        {
            dsbd->pIDSBuffer->Stop();
        }
        break;
    case USE_SNDPLAY:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            sndPlaySound(NULL, SND_ASYNC);
        }
        break;
    }
}

// ----------------------------------------------------------
// SetPan - 
// ----------------------------------------------------------
void CSoundEffect::SetPan(int PanFactor)
{
//  if ((pWaveInfo == NULL) || (curPan == PanFactor))
    if (pWaveInfo == NULL)
    {
        return;
    }

    curPan = PanFactor;

    switch (WaveMode)
    {
    case USE_WAVEOUT:
#if 0
        if (pWaveInfo->pWaveHdr != NULL)
        {
            DWORD   absVolume;
            DWORD   LeftChannel, RightChannel;

            absVolume = (0xFFFF * curVolume) / 128;
            LeftChannel = (absVolume * (127-curPan)) / 128;
            RightChannel = (absVolume * curPan) / 128;
                        
            waveOutSetVolume((HWAVEOUT)WAVE_MAPPER, MAKELONG(LeftChannel
            , RightChannel));
        }
#endif
        break;
    case USE_DSOUND:
        if (dsbd != NULL)
        {
            dsbd->pIDSBuffer->SetPan(curPan);
        }
        break;
    case USE_SNDPLAY:
        break;
    }
}


// ----------------------------------------------------------
// SetVolume - 
// ----------------------------------------------------------
void CSoundEffect::SetVolume(int Volume)
{
    if ((pWaveInfo == NULL)  || (curVolume == Volume))
    {
        return;
    }

    curVolume = Volume;
    switch (WaveMode)
    {
    case USE_WAVEOUT:
#if 0
        if (pWaveInfo->pWaveHdr != NULL)
        {
            DWORD   absVolume;

            DWORD   LeftChannel, RightChannel;

            absVolume = (0xFFFF * curVolume) / 128;
            LeftChannel = (absVolume * (127-curPan)) / 128;
            RightChannel = (absVolume * curPan) / 128;
                        
            waveOutSetVolume((HWAVEOUT)WAVE_MAPPER, MAKELONG(LeftChannel
            , RightChannel));
        }
#endif
        break;
    case USE_DSOUND:
        if (dsbd != NULL)
        {
            dsbd->pIDSBuffer->SetVolume(curVolume);
        }
        break;
    case USE_SNDPLAY:
        break;
    }
}


// ----------------------------------------------------------
// SetMute - 
// ----------------------------------------------------------
void CSoundEffect::SetMute(BOOL fMute)
{

    if ((pWaveInfo == NULL) || (fMuted == fMute))
    {
        return;
    }
    
    fMuted = fMute;

#if 1
    if (fMuted)
    {
        Stop();
    } else {
        Play();
    }
#else
    DWORD   tmpVolume;
    tmpVolume = fMuted ? 0 : curVolume;

    switch (WaveMode)
    {
    case USE_WAVEOUT:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            DWORD   absVolume;
            DWORD   LeftChannel, RightChannel;

            absVolume = (0xFFFF * tmpVolume) / 128;
            if (fMuted)
            {
                LeftChannel = 0;
                RightChannel = 0;
                
            } else {
                LeftChannel = (absVolume * (127-curPan)) / 128;
                RightChannel = (absVolume * curPan) / 128;
            }
                        
            waveOutSetVolume((HWAVEOUT)WAVE_MAPPER, MAKELONG(LeftChannel
            , RightChannel));
        }
        break;
    case USE_DSOUND:
        if (dsbd != NULL)
        {
            dsbd->pIDSBuffer->SetVolume(tmpVolume);
        }
        break;
    case USE_SNDPLAY:
        break;
    }
#endif
}

