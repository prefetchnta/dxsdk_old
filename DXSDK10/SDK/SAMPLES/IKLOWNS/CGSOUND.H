/*===========================================================================*\
|
|  File:        cgsound.h
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
#ifndef _CGSOUND_H
#define _CGSOUND_H

#include <mmreg.h>
#include <msacm.h>
extern "C" {
    #include "dsound.h"
};

#define USE_DSOUND  0
#define USE_WAVEOUT 1
#define USE_SNDPLAY 2


// Master wave information structure -- one for each unique
// sound (not each instance).
typedef struct _WAVE_ENTRY
{
    LPSTR           mpFileName;
    DSBUFFERDESC        *dsbd;
    LPWAVEFORMATEX      pwfxInfo;
    LPWAVEHDR       pWaveHdr;
    DWORD           hObjectId;
    int         WaveMode;
    int         mRefCount;
} WAVE_ENTRY, *LPWAVE_ENTRY;


class CSoundEffect
{
private:
    LPWAVE_ENTRY    pWaveInfo;  // ptr to master wav info
    DSBUFFERDESC    *dsbd;      // instance DS buffer
    int     WaveMode;   // method for playing
    int     curVolume;  // 0-127
    int     curPan;     // 0=left, 127=right
    BOOL        fLoop;
    BOOL        fPlaying;
    BOOL        fMuted;

public:
    CSoundEffect(LPSTR WaveFile, DWORD UniqueId=0, BOOL fLoopIt=FALSE
    , int Mode=USE_DSOUND); 
    ~CSoundEffect();

    void Play();
    void Stop();
    void SetPan(int PanFactor);
    void SetVolume(int Volume);
    void SetMute(BOOL);
    void SetFreq();

    friend LPWAVEHDR LoadWaveData(CSoundEffect *, LPSTR, int);

};
#endif
