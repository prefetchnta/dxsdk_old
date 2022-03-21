/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsound.h
 *  Content:    DirectSound include file
 *
 ***************************************************************************/

#ifndef __DSOUND_INCLUDED__
#define __DSOUND_INCLUDED__

#ifdef _WIN32
#define COM_NO_WINDOWS_H
#include <objbase.h>
#endif

#define _FACDS  0x878
#define MAKE_DSHRESULT( code )  MAKE_HRESULT( 1, _FACDS, code )

#ifdef __cplusplus
extern "C" {
#endif

// DirectSound 279afa83-4981-11ce-a521-0020af0be560
DEFINE_GUID(IDIRECTSOUND_IID,0x279AFA83,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
// DirectSound3D 279afa84-4981-11ce-a521-0020af0be560
DEFINE_GUID(IDIRECTSOUND3D_IID,0x279AFA84,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
// DirectSoundBuffer 279afa85-4981-11ce-a521-0020af0be560
DEFINE_GUID(IDIRECTSOUNDBUFFER_IID,0x279AFA85,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);
// DirectSoundBuffer3D 279afa86-4981-11ce-a521-0020af0be560
DEFINE_GUID(IDIRECTSOUNDBUFFER3D_IID,0x279AFA86,0x4981,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60);

//==========================================================================;
//
//                               Flags...
//
//==========================================================================;

#define DSBPLAYF_LOOP           0x00000001
#define DSB_LOCKF_WRITEPOS      0x00000001

//==========================================================================;
//
//                            Structures...
//
//==========================================================================;
#ifdef __cplusplus
/* 'struct' not 'class' per the way DECLARE_INTERFACE_ is defined */
struct IDirectSound;
struct IDirectSound3D;
struct IDirectSoundBuffer;
struct IDirectSoundBuffer3D;
#endif

typedef struct IDirectSound           *LPDIRECTSOUND;        
typedef struct IDirectSound3D         *LPDIRECTSOUND3D;      
typedef struct IDirectSoundBuffer     *LPDIRECTSOUNDBUFFER;  
typedef struct IDirectSoundBuffer3D   *LPDIRECTSOUNDBUFFER3D;

typedef HRESULT DSVAL;

typedef struct dspath3d_tag
{
    DWORD       dwControl1;
    DWORD       dwControl2;
    DWORD       dwControl3;
    DWORD       dwControl4;
} DSPATH3D, *LPDSPATH3D;

typedef struct  dscaps_tag
{
    DWORD       dwCaps;
} DSCAPS, *LPDSCAPS;

typedef struct dsbuffercaps_tag
{
    DWORD       dwCaps;
} DSBUFFERCAPS, *LPDSBUFFERCAPS;

typedef struct dsbufferformat_tag
{
    DWORD           dwSize;
    DWORD           fdwBufferFormat;
    LPWAVEFORMATEX  pwfx;
    DWORD           cbwfx;
} DSBUFFERFORMAT, *LPDSBUFFERFORMAT;

typedef struct dsbufferdesc_tag
{
    DWORD                   dwSize;
    DWORD                   fdwBufferDesc;
    DWORD                   cbBufferSize;
    DWORD                   dwPriority;
    DSBUFFERFORMAT          dsbfFormat;
    LPDIRECTSOUNDBUFFER     pIDSBuffer;
} DSBUFFERDESC, *LPDSBUFFERDESC;

typedef struct dspos3d_tag
{
    LONG        lX;
    LONG        lY;
    LONG        lZ;
} DSPOS3D, *LPDSPOS3D;

typedef struct dsversion_tag
{
    DWORD   dwSize;
    DWORD   dwDirectSound;
    DWORD   dwDriverMajor;
    DWORD   dwDriverMinor;
    LPSTR   lpDriverName;
    LPSTR   lpContactName;
    LPSTR   lpContactPhone;
    LPSTR   lpContactFax;
    LPSTR   lpContactBBS;
    LPSTR   lpContactEmail;
} DSVERSION, *LPDSVERSION;

typedef struct dsreflect_tag
{
    DWORD       dwXNeg;
    DWORD       dwXPos;
    DWORD       dwYNeg;
    DWORD       dwYPos;
    DWORD       dwZNeg;
    DWORD       dwZPos;
} DSREFLECT, *LPDSREFLECT;

typedef struct dsbwavebltbuffer_tag
{
    LPDIRECTSOUNDBUFFER     pdsb;    
    DWORD                   fdwWaveBltBuffer;
    DWORD                   dwPosition;
    DWORD                   cbCopyLength;
    DWORD                   dwReserved[8];
} DSBWAVEBLTBUFFER, *LPDSBWAVEBLTBUFFER;

typedef struct dsbwaveblt_tag
{
    DWORD               dwSize;
    DWORD               fdwWaveBlt;
    DWORD               dwPosition;
    DWORD               cbCopyLength;
    DWORD               dwCount;
    DSBWAVEBLTBUFFER    adswbb[1];
} DSBWAVEBLT, *LPDSBWAVEBLT;

typedef struct dsbufferinfo3d_tag
{
    DWORD       dwSize;
    DSPOS3D     dspDirection;
    DSPOS3D     dspOrientation;
    DSPOS3D     dspPosition;
    DSPATH3D    dsaPath;
    DWORD       dwAmbientVolume;
    DWORD       dwEffectPriority;
    DWORD       dwSoundCone;
    DWORD       dwSoundPressure;
    DWORD       dwSoundSizeX;
    DWORD       dwSoundSizeY;
    DWORD       dwVelocity;
} DSBUFFERINFO3D, *LPDSBUFFERINFO3D;

typedef LPVOID* LPLPVOID;

typedef DSVAL (FAR PASCAL * LPDSENUMMODESCALLBACK)(LPDSBUFFERDESC, DWORD, LPVOID);
typedef DSVAL (FAR PASCAL * LPDSENUMBUFFERSCALLBACK)(LPDSBUFFERDESC, LPVOID);
typedef DSVAL (FAR PASCAL * LPDSPATHCALLBACK)(LPDSPATH3D,DWORD,LPVOID);

//
// IDirectSound
//
#undef INTERFACE
#define INTERFACE IDirectSound
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectSound, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectSound methods ***/

    STDMETHOD(ChangePlayingSpeed)(THIS_ LONG ) PURE;
    STDMETHOD( CreateSoundBuffer)(THIS_ LPDSBUFFERDESC ) PURE;
    STDMETHOD(         EnumModes)(THIS_ DWORD,LPDSBUFFERDESC,LPDSENUMMODESCALLBACK,LPVOID ) PURE;
    STDMETHOD(  EnumSoundBuffers)(THIS_ DWORD,LPDSBUFFERDESC,LPDSENUMBUFFERSCALLBACK,LPVOID ) PURE;
    STDMETHOD(    FreeAllBuffers)(THIS ) PURE;
    STDMETHOD(           GetCaps)(THIS_ LPDSCAPS ) PURE;
    STDMETHOD(       GetCPULimit)(THIS_ LPDWORD ) PURE;
    STDMETHOD(  GetExclusiveMode)(THIS_ LPHANDLE ) PURE;
    STDMETHOD(           GetMode)(THIS_ LPDWORD ) PURE;
    STDMETHOD(        GetVersion)(THIS_ LPDSVERSION ) PURE;
    STDMETHOD(  GetSpeakerConfig)(THIS_ LPDWORD ) PURE;
    STDMETHOD(       SetCPULimit)(THIS_ DWORD ) PURE;
    STDMETHOD(  SetExclusiveMode)(THIS_ HANDLE,DWORD ) PURE;
    STDMETHOD(           SetMode)(THIS_ DWORD ) PURE;
    STDMETHOD(  SetSpeakerConfig)(THIS_ DWORD ) PURE;
};
#endif

//
// IDirectSound3D
//
#undef INTERFACE
#define INTERFACE IDirectSound3D
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectSound3D, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectSound3D methods ***/

    STDMETHOD(  GetListenersDirection)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(GetListenersOrientation)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(       GetListenersPath)(THIS_ LPDSPATH3D) PURE;
    STDMETHOD(   GetListenersPosition)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(   GetListenersVelocity)(THIS_ LPDWORD ) PURE;
    STDMETHOD(        GetReflectivity)(THIS_ LPDSREFLECT ) PURE;
    STDMETHOD(            GetRoomSize)(THIS_ LPDWORD ) PURE;
    STDMETHOD(    GetSoundDissapation)(THIS_ LPDWORD ) PURE;
    STDMETHOD(        GetSpeedOfSound)(THIS_ LPDWORD ) PURE;
    STDMETHOD(  SetListenersDirection)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(SetListenersOrientation)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(       SetListenersPath)(THIS_ LPDSPATH3D ) PURE;
    STDMETHOD(   SetListenersPosition)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(   SetListenersVelocity)(THIS_ DWORD ) PURE;
    STDMETHOD(        SetReflectivity)(THIS_ LPDSREFLECT ) PURE;
    STDMETHOD(            SetRoomSize)(THIS_ DWORD ) PURE;
    STDMETHOD(    SetSoundDissapation)(THIS_ DWORD ) PURE;
    STDMETHOD(        SetSpeedOfSound)(THIS_ DWORD ) PURE;
};
#endif

//
// IDirectSoundBuffer
//
#undef INTERFACE
#define INTERFACE IDirectSoundBuffer
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectSoundBuffer, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectSoundBuffer methods ***/

    STDMETHOD(           GetCaps)(THIS_ LPDSBUFFERCAPS ) PURE;
    STDMETHOD(GetCurrentPosition)(THIS_ LPDWORD,LPDWORD,LPDWORD ) PURE;
    STDMETHOD(         GetFormat)(THIS_ LPDSBUFFERFORMAT ) PURE;
    STDMETHOD(         GetVolume)(THIS_ LPDWORD ) PURE;
    STDMETHOD(            GetPan)(THIS_ LPDWORD ) PURE;
    STDMETHOD(      GetFrequency)(THIS_ LPDWORD ) PURE;
    STDMETHOD(        GetLooping)(THIS_ LPDWORD ) PURE;
    STDMETHOD(         GetStatus)(THIS_ LPDWORD ) PURE;
    STDMETHOD(              Lock)(THIS_ DWORD,DWORD,LPVOID,LPDWORD,LPVOID,LPDWORD,HANDLE,DWORD ) PURE;
    STDMETHOD(              Play)(THIS_ DWORD,DWORD ) PURE;
    STDMETHOD(SetCurrentPosition)(THIS_ DWORD ) PURE;
    STDMETHOD(         SetFormat)(THIS_ LPDSBUFFERFORMAT ) PURE;
    STDMETHOD(         SetVolume)(THIS_ DWORD ) PURE;
    STDMETHOD(            SetPan)(THIS_ DWORD ) PURE;
    STDMETHOD(      SetFrequency)(THIS_ DWORD ) PURE;
    STDMETHOD(        SetLooping)(THIS_ DWORD ) PURE;
    STDMETHOD(              Stop)(THIS  ) PURE;
    STDMETHOD(            Unlock)(THIS_ LPVOID, LPVOID ) PURE;
    STDMETHOD(            WavBlt)(THIS_ LPDSBWAVEBLT ) PURE;
};
#endif

//
// IDirectSoundBuffer
//
#undef INTERFACE
#define INTERFACE IDirectSoundBuffer3D
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectSoundBuffer3D, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectSoundBuffer3D methods ***/

    STDMETHOD(          GetInfo)(THIS_ LPDSBUFFERINFO3D ) PURE;
    STDMETHOD( GetAmbientVolume)(THIS_ LPDWORD ) PURE;
    STDMETHOD(     GetDirection)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(GetEffectPriority)(THIS_ LPDWORD ) PURE;
    STDMETHOD(   GetOrientation)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(          GetPath)(THIS_ LPDSPATH3D,LPDWORD,LPDSPATHCALLBACK ) PURE;
    STDMETHOD(      GetPosition)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(     GetSoundCone)(THIS_ LPDWORD ) PURE;
    STDMETHOD( GetSoundPressure)(THIS_ LPDWORD ) PURE;
    STDMETHOD(     GetSoundSize)(THIS_ LPDWORD,LPDWORD ) PURE;
    STDMETHOD(      GetVelocity)(THIS_ LPDWORD ) PURE;
    STDMETHOD( SetAmbientVolume)(THIS_ DWORD ) PURE;
    STDMETHOD(     SetDirection)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(SetEffectPriority)(THIS_ DWORD ) PURE;
    STDMETHOD(          SetInfo)(THIS_ DWORD,LPDSBUFFERINFO3D ) PURE;
    STDMETHOD(   SetOrientation)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(          SetPath)(THIS_ DWORD,LPDSPATH3D,DWORD,LPDSPATHCALLBACK ) PURE;
    STDMETHOD(      SetPosition)(THIS_ LPDSPOS3D ) PURE;
    STDMETHOD(     SetSoundCone)(THIS_ DWORD ) PURE;
    STDMETHOD( SetSoundPressure)(THIS_ DWORD ) PURE;
    STDMETHOD(     SetSoundSize)(THIS_ DWORD,DWORD ) PURE;
    STDMETHOD(      SetVelocity)(THIS_ DWORD ) PURE;
    STDMETHOD(           WavBlt)(THIS_ DWORD,DWORD,LPDIRECTSOUNDBUFFER3D,DWORD,DWORD,LPDSBWAVEBLT ) PURE;
};
#endif

HRESULT WINAPI DirectSoundCreate(LPSTR pszName,LPDIRECTSOUND * ppDS );

/*
 * Return Codes
 */

#define DS_OK                           0x0000
#define DSERR_NOTSUPPORTED              0x0008
#define DSERR_INVALIDPARAM              0x000B
#define DSERR_GENERIC                   0x0001
#define DSERR_OUTOFMEMORY               0x0007

/*
 * FLAGS
 */

#define DSB_STATUSF_STOPPED             0x0001
#define DSB_STATUSF_PLAYING             0x0002

#define DSB_DESCRIPTIONF_PRIMARY        0x0001
#define DSB_DESCRIPTIONF_HARDWARE       0x0004
#define DSB_DESCRIPTIONF_NOMIXIN        0x0008
#define DSB_DESCRIPTIONF_PLAYABLE       0x0010
#define DSB_DESCRIPTIONF_LOOPABLE       0x0020
#define DSB_DESCRIPTIONF_NOLOOPABLE     0x0040


#define DSB_BLTF_SRCCURRENTPOS          0x0001
#define DSB_BLTF_DSTCURRENTPOS          0x0002
#define DSB_BLTF_LOOPSRC                0x0004
#define DSB_BLTF_COPY                   0x0008
#define DSB_BLTF_MIX                    0x0010
#define DSB_BLTF_USEDSTLENGTH           0x0020

#ifdef __cplusplus
};
#endif

#endif  /* __DSOUND_INCLUDED__ */
