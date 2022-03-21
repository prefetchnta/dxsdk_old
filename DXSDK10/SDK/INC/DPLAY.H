/*==========================================================================;
 *
 *  Copyright (C) 1994-1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dplay.h
 *  Content:	DirectPlay include file
 *
 ***************************************************************************/

#ifndef __DPLAY_INCLUDED__
#define __DPLAY_INCLUDED__
#ifdef _WIN32
/* for DECLARE_INTERFACE and HRESULT.   find better way later*/
#include <ole2.h>
#endif

#define	_FACDP	0x877
#define MAKE_DPHRESULT( code )	MAKE_HRESULT( 1, _FACDP, code )

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 *
 * DirectPlay Structures
 *
 * Various structures used to invoke DirectPlay.
 *
 *==========================================================================*/

#ifdef __cplusplus
/* 'struct' not 'class' per the way DECLARE_INTERFACE_ is defined */
struct IDirectPlay;
typedef struct IDirectPlay		FAR *LPDIRECTPLAY;
#else			      
typedef struct IDirectPlay		FAR *LPDIRECTPLAY;
#endif
typedef struct _DPCAPS			FAR *LPDPCAPS;
/* DPID is the player id */
typedef DWORD	DPID, FAR *LPDPID;
/* DPMID is the message handle (for replies) */
typedef DWORD	DPMID, FAR *LPDPMID;


/*
 * Create API
 */
typedef HRESULT (FAR PASCAL * LPDPENUMDPCALLBACK)(LPGUID, LPSTR, LPDPCAPS, LPVOID);
typedef HRESULT (FAR PASCAL * LPDPENUMSESSIONSCALLBACK)(
                                DWORD dwSession,
                                LPSTR pSessionName,
                                LPVOID pContextLPDWORD, LPSTR, LPDWORD, LPVOID);
extern HRESULT WINAPI DirectPlayCreate( LPGUID lpGUID, LPDIRECTPLAY FAR *lplpDP );
extern HRESULT WINAPI DirectPlayEnumerate( LPDPENUMDPCALLBACK, LPVOID );

#define DPOPEN_OPENSPECIFICSESSION	0x00000001

#define DPOPENSTATUS_INITIATOR		0x00000001
#define DPOPENSTATUS_INVITED		0x00000002

#define	DPSEND_BLOCKING			0x00000001
#define DPSEND_GUARANTEE		0x00000002
#define DPSEND_HIGHPRIORITY		0x00000004
#define DPSEND_REPLYREQUESTED		0x00000008
#define DPSEND_TRYONCE			0x00000010

#define DPRECEIVE_HIGHPRIORITY		0x00000002
#define DPRECEIVE_NORMAL		0x00000004
#define DPRECEIVE_PEEK			0x00000008
#define DPRECEIVE_TOPLAYER		0x00000010
#define DPRECEIVE_TRYONCEFIRST		0x00000020

#define DPRECEIVESTATUS_REPLYREQUESTED 0x00000001

#define DPSYSMSG_CREATEPLAYER		0x00000001
#define DPSYSMSG_DESTROYPLAYER		0x00000002
#define DPSYSMSG_GAMEINFO		0x00000004
#define DPSYSMSG_REPLYDENIED		0x00000008

#define DP_OK 				MAKE_DPHRESULT( 10 )
#define DPERR_ACTIVEPLAYERS		MAKE_DPHRESULT( 20 )
#define DPERR_BOTHBUFFERSTOOSMALL	MAKE_DPHRESULT( 30 )
#define DPERR_CANTCREATEPLAYER		MAKE_DPHRESULT( 40 )
#define DPERR_CAPSNOTAVAILABLEYET	MAKE_DPHRESULT( 50 )
#define DPERR_FORMALBUFFERTOOSMALL	MAKE_DPHRESULT( 60 )
#define DPERR_FRIENDLYBUFFERTOOSMALL	MAKE_DPHRESULT( 70 )
#define DPERR_INVALIDDPID		MAKE_DPHRESULT( 80 )
#define DPERR_INVALIDDPMID		MAKE_DPHRESULT( 90 )
#define DPERR_INVALIDFLAGS		MAKE_DPHRESULT( 100 )
#define DPERR_INVALIDOBJECTS		MAKE_DPHRESULT( 110 )
#define DPERR_INVALIDPARAM		MAKE_DPHRESULT( 120 )
#define DPERR_MESSAGEBUFFERTOOSMALL	MAKE_DPHRESULT( 130 )
#define DPERR_NOCAPS			MAKE_DPHRESULT( 140 )
#define DPERR_NOCONNECTION		MAKE_DPHRESULT( 150 )
#define DPERR_NOMESSAGES		MAKE_DPHRESULT( 160 )
#define DPERR_NONAMESERVERFOUND		MAKE_DPHRESULT( 170 )
#define DPERR_NOPLAYERS			MAKE_DPHRESULT( 180 )
#define DPERR_NOSESSIONS		MAKE_DPHRESULT( 190 )
#define DPERR_NOTSUPPORTED		MAKE_DPHRESULT( 200 )
#define DPERR_REPLYNOTSUPPORTED		MAKE_DPHRESULT( 210 )
#define DPERR_TIMEOUT			MAKE_DPHRESULT( 220 )
#define DPERR_NOMEMORY			MAKE_DPHRESULT( 230 )
#define DPERR_FAIL_GENERIC			MAKE_DPHRESULT( 240 )

/*
 * DPCAPS
 */
typedef struct _DPCAPS
{
  DWORD	dwCaps;
  DWORD dwLatency;
  DWORD	dwBandwidth;
} DPCAPS;

/* Defines for current maximum sizes of formal name and friendly name. */
#define LEN_FORMALNAME 80
#define LEN_FRIENDLYNAME 80

/* Max length of gamename currently */
#define LEN_GAMENAME 80

/* Player ID to use when broadcasting to all other players */
#define DP_BROADCAST_ID	0xFFFFFFFF

/* Left as readable value as debugging aid */
#define SYSTEM_MESSAGE '*'
/* System message parsing structures */
typedef struct {
    BYTE sysFlag;
    BYTE id;
} SysMsgHeader;

typedef struct {
    SysMsgHeader hdr;
    DPID playerID;
    char fullName[1];
} SysMsgCreatePlayer;

typedef struct {
    SysMsgCreatePlayer CreatePlayer;
} Class1SysMsg;

typedef union {
    SysMsgHeader hdr;
    Class1SysMsg msg;    
} SysMsg;

/* Player enumeration callback prototype */
typedef HRESULT (FAR PASCAL *LPDPENUMPLAYERSCALLBACK)(
                                DPID playerID,
                                LPSTR pFriendlyName,
                                LPSTR pFormalName,
                                LPVOID pContext );

/*
 * IDirectPlay
 */
#undef INTERFACE
#define INTERFACE IDirectPlay
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectPlay, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectPlay methods ***/
    STDMETHOD(Close)(THIS) PURE;
    STDMETHOD(CreatePlayer)(THIS_ LPDPID,LPSTR,LPSTR) PURE;
    STDMETHOD(DestroyPlayer)(THIS_ DPID) PURE;
    STDMETHOD(EnumPlayers)(THIS_ LPDPENUMPLAYERSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(EnumPreviousSessions)(THIS_ LPDPENUMSESSIONSCALLBACK,LPVOID,DWORD) PURE;
    STDMETHOD(GetCaps)(THIS_ LPDPCAPS) PURE;
    STDMETHOD(GetPlayerName)(THIS_ DPID,LPDWORD,LPSTR,LPDWORD,LPSTR) PURE;
    STDMETHOD(Open)(THIS_ LPGUID,LPSTR,LPDWORD,LPDWORD,DWORD,LPDWORD) PURE;
    STDMETHOD(Send)(THIS_ DPID, DPID, DWORD,DWORD,LPSTR,LPDWORD,LPSTR) PURE;
    STDMETHOD(Receive)(THIS_ LPDPID,LPDPID,DWORD,LPDWORD,LPSTR,LPDPMID,LPDWORD) PURE;
    STDMETHOD(Reply)(THIS_ DPID,DWORD,DWORD,LPSTR,DPMID) PURE;
};
#endif

/****************************************************************************
 *
 * DIRECTPLAY ERRORS
 *
 * Errors are represented by negative values and cannot be combined.
 *
 ****************************************************************************/

#ifdef __cplusplus
};
#endif

#endif
