/*==========================================================================;
 *
 *  Copyright (C) 1994-1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ddraw.h
 *  Content:    DirectDraw include file
 *
 ***************************************************************************/

#ifndef __DDRAW_INCLUDED__
#define __DDRAW_INCLUDED__
#ifdef _WIN32
#define COM_NO_WINDOWS_H
#include <objbase.h>
#endif

#define _FAC    0x876
#define MAKE_DDHRESULT( code )  MAKE_HRESULT( 1, _FAC, code )

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 *
 * DirectDraw Structures
 *
 * Various structures used to invoke DirectDraw.
 *
 *==========================================================================*/

#ifdef __cplusplus
/* 'struct' not 'class' per the way DECLARE_INTERFACE_ is defined */
struct IDirectDraw;
struct IDirectDrawSurface;
struct IDirectDrawPalette;
typedef struct IDirectDraw              FAR *LPDIRECTDRAW;
typedef struct IDirectDrawSurface       FAR *LPDIRECTDRAWSURFACE;
typedef struct IDirectDrawPalette       FAR *LPDIRECTDRAWPALETTE;
#else                         
typedef struct IDirectDraw              FAR *LPDIRECTDRAW;
typedef struct IDirectDrawSurface       FAR *LPDIRECTDRAWSURFACE;
typedef struct IDirectDrawPalette       FAR *LPDIRECTDRAWPALETTE;
#endif

typedef struct _DDFXROP                 FAR *LPDDFXROP;
typedef struct _DDSURFACEDESC           FAR *LPDDSURFACEDESC;
typedef struct _DDVERSIONINFO           FAR *LPDDVERSIONINFO;

/*
 * Create API
 */
extern HRESULT WINAPI DirectDrawCreate( LPSTR lpszDriverName, LPDIRECTDRAW FAR *lplpDD );
#define DDCREATE_HARDWAREONLY           0x00000001l
#define DDCREATE_EMULATIONONLY          0x00000002l

typedef HRESULT (FAR PASCAL * LPDDENUMMODESCALLBACK)(LPDDSURFACEDESC, LPVOID);
typedef HRESULT (FAR PASCAL * LPDDENUMSURFACESCALLBACK)(LPDDSURFACEDESC, LPVOID);

/*
 * DDCOLORKEY
 */
typedef struct _DDCOLORKEY
{
    DWORD       dwColorSpaceLowValue;   // low boundary of color space that is to 
                                        // be treated as Color Key, inclusive
    DWORD       dwColorSpaceHighValue;  // high boundary of color space that is 
                                        // to be treated as Color Key, inclusive
} DDCOLORKEY;

typedef DDCOLORKEY FAR* LPDDCOLORKEY;

/*
 * DDBLTFX
 * Used to pass override information to the DIRECTDRAWSURFACE callback Blt.
 */
typedef struct _DDBLTFX
{
    DWORD       dwSize;                         // size of structure
    DWORD       dwDDFX;                         // FX operations
    DWORD       dwROP;                          // Win32 raster operations
    DWORD       dwDDROP;                        // Raster operations new for DirectDraw
    DWORD       dwRotationAngle;                // Rotation angle for blt
    DWORD       dwZBufferOpCode;                // ZBuffer compares
    DWORD       dwZBufferLow;                   // Low limit of Z buffer
    DWORD       dwZBufferHigh;                  // High limit of Z buffer
    DWORD       dwZBufferBaseDest;              // Destination base value
    DWORD       dwConstZDestBitDepth;           // Bit depth used to specify Z constant for destination
    union
    {
        DWORD   dwConstZDest;                   // Constant to use as Z buffer for dest
        LPDIRECTDRAWSURFACE lpDDSZBufferDest;   // Surface to use as Z buffer for dest
    };
    DWORD       dwConstZSrcBitDepth;            // Bit depth used to specify Z constant for source
    union
    {
        DWORD   dwConstZSrc;                    // Constant to use as Z buffer for src
        LPDIRECTDRAWSURFACE lpDDSZBufferSrc;    // Surface to use as Z buffer for src
    };
    DWORD       dwAlphaEdgeBlendBitDepth;       // Bit depth used to specify constant for alpha edge blend
    DWORD       dwAlphaEdgeBlend;               // Alpha for edge blending
    DWORD       dwConstAlphaDestBitDepth;       // Bit depth used to specify alpha constant for destination
    union
    {
        DWORD   dwConstAlphaDest;               // Constant to use as Alpha Channel 
        LPDIRECTDRAWSURFACE lpDDSAlphaDest;     // Surface to use as Alpha Channel
    };
    DWORD       dwConstAlphaSrcBitDepth;        // Bit depth used to specify alpha constant for source
    union
    {
        DWORD   dwConstAlphaSrc;                // Constant to use as Alpha Channel 
        LPDIRECTDRAWSURFACE lpDDSAlphaSrc;      // Surface to use as Alpha Channel
    };
    union
    {
        DWORD   dwFillColor;                    // color in RGB or Palettized
        LPDIRECTDRAWSURFACE lpDDSPattern;       // Surface to use as pattern
    };
    DDCOLORKEY  ddckDestColorkey;               // DestColorkey override
    DDCOLORKEY  ddckSrcColorkey;                // SrcColorkey override
} DDBLTFX;

typedef DDBLTFX FAR* LPDDBLTFX;

/*
 * DDCOMPOSEFX
 * Used to pass override information to the DIRECTDRAWSURFACE callback Compose.
 */
typedef DWORD   (FAR PASCAL *LPCOMPOSECALLBACK)(DWORD);
typedef struct _DDCOMPOSEFX
{
    DWORD       dwSize;                         // size of structure
    DWORD       dwDDFX;                         // FX operations
    DWORD       dwROP;                          // Win32 raster operations
    DWORD       dwDDROP;                        // Raster operations new for DirectDraw
    DWORD       dwRotationAngle;                // Rotation angle for blt
    DWORD       dwZBufferOpCode;                // ZBuffer compares
    DWORD       dwZBufferLow;                   // Low limit of Z buffer
    DWORD       dwZBufferHigh;                  // High limit of Z buffer
    DWORD       dwZBufferBaseDest;              // Destination base value
    DWORD       dwConstZDestBitDepth;           // Bit depth used to specify Z constant for destination
    union
    {
        DWORD   dwConstZDest;                   // Constant to use as Z buffer for dest
        LPDIRECTDRAWSURFACE lpDDSZBufferDest;   // Surface to use as Z buffer for dest
    };
    DWORD       dwConstZSrcBitDepth;            // Bit depth used to specify Z constant for source
    union
    {
        DWORD   dwConstZSrc;                    // Constant to use as Z buffer for src
        LPDIRECTDRAWSURFACE lpDDSZBufferSrc;    // Surface to use as Z buffer for src
    };
    DWORD       dwAlphaEdgeBlendBitDepth;       // Bit depth used to specify constant for alpha edge blend
    DWORD       dwAlphaEdgeBlend;               // Alpha for edge blending
    DWORD       dwConstAlphaDestBitDepth;       // Bit depth used to specify alpha constant for destination
    union
    {
        DWORD   dwConstAlphaDest;               // Constant to use as Alpha Channel 
        LPDIRECTDRAWSURFACE lpDDSAlphaDest;     // Surface to use as Alpha Channel
    };
    DWORD       dwConstAlphaSrcBitDepth;        // Bit depth used to specify alpha constant for source
    union
    {
        DWORD   dwConstAlphaSrc;                // Constant to use as Alpha Channel 
        LPDIRECTDRAWSURFACE lpDDSAlphaSrc;      // Surface to use as Alpha Channel
    };
    union
    {
        DWORD   dwFillColor;                    // color in RGB or Palettized
        LPDIRECTDRAWSURFACE lpDDSPattern;       // Surface to use as pattern
    };
    DDCOLORKEY  ddckDestColorkey;               // DestColorkey override
    DDCOLORKEY  ddckSrcColorkey;                // SrcColorkey override
    DWORD       dwRepeat;                       // how many times to repeat the composition command
    LPCOMPOSECALLBACK   lpfnCallback;           // function to call prior to execution of each compose command
} DDCOMPOSEFX;

typedef DDCOMPOSEFX FAR* LPDDCOMPOSEFX;

/*
 * DDSCAPS
 */
typedef struct _DDSCAPS
{
    DWORD       dwCaps;         // capabilities of surface wanted
} DDSCAPS;

typedef DDSCAPS FAR* LPDDSCAPS; 

/*
 * DDCAPS
 */
#define DD_ROP_SPACE            (256/32)        // space required to store ROP array
typedef struct _DDCAPS
{
    DWORD       dwSize;                 // size of the DDDRIVERCAPS structure
    DWORD       dwCaps;                 // driver specific capabilities
    DWORD       dwCKeyCaps;             // color key capabilities of the surface
    DWORD       dwFXCaps;               // driver specific stretching and effects capabilites
    DWORD       dwPalCaps;              // palette capabilities
    DWORD       dwSVCaps;               // stereo vision capabilities
    DWORD       dwAlphaBltConstBitDepths;       // DDBD_2,4,8
    DWORD       dwAlphaBltPixelBitDepths;       // DDBD_1,2,4,8
    DWORD       dwAlphaBltSurfaceBitDepths;     // DDBD_1,2,4,8
    DWORD       dwAlphaOverlayConstBitDepths;   // DDBD_2,4,8
    DWORD       dwAlphaOverlayPixelBitDepths;   // DDBD_1,2,4,8
    DWORD       dwAlphaOverlaySurfaceBitDepths; // DDBD_1,2,4,8
    DWORD       dwZBufferBitDepths;             // DDBD_8,16,24,32
    DWORD       dwVidMemTotal;          // total amount of video memory
    DWORD       dwVidMemFree;           // amount of free video memory
    DWORD       dwMaxVisibleOverlays;   // maximum number of visible overlays
    DWORD       dwCurrVisibleOverlays;  // current number of visible overlays
    DWORD       dwNumFourCCCodes;       // number of four cc codes
    DWORD       dwAlignBoundarySrc;     // source rectangle alignment
    DWORD       dwAlignSizeSrc;         // source rectangle byte size
    DWORD       dwAlignStrideAlign;     // stride alignment
    DDSCAPS     ddsCaps;                // DDSCAPS structure has all the general capabilities
    DWORD       dwRops[DD_ROP_SPACE];   // ROPS supported
} DDCAPS;

typedef DDCAPS FAR* LPDDCAPS;

/*
 * IDirectDraw 
 */
#undef INTERFACE
#define INTERFACE IDirectDraw
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectDraw, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDraw methods ***/
    STDMETHOD(CreatePalette)(THIS_ DWORD, LPDIRECTDRAWPALETTE FAR*,LPPALETTEENTRY ) PURE;
    STDMETHOD(CreateSurface)(THIS_  LPDDSURFACEDESC) PURE;
    STDMETHOD(DefWindowProc)(THIS_ HWND, UINT, WPARAM, LPARAM ) PURE;
    STDMETHOD(DuplicateSurface)( THIS_ LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * ) PURE;
    STDMETHOD(EnumDisplayModes)( THIS_ DWORD, LPDDSURFACEDESC FAR*,LPVOID, LPDDENUMMODESCALLBACK ) PURE;
    STDMETHOD(EnumSurfaces)(THIS_ DWORD,DWORD, LPDDSURFACEDESC FAR*,LPVOID,LPDDENUMSURFACESCALLBACK ) PURE;
    STDMETHOD(FlipToGDISurface)(THIS) PURE;
    STDMETHOD(FreeAllSurfaces)(THIS) PURE;
    STDMETHOD(GarbageCollect)(THIS) PURE;
    STDMETHOD(GetCaps)( THIS_ LPDDCAPS) PURE;
    STDMETHOD(GetDisplayMode)( THIS_ LPDDSURFACEDESC) PURE;
    STDMETHOD(GetExclusiveModeOwner)(THIS_ LPDWORD,LPDWORD) PURE;
    STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD, LPDWORD ) PURE;
    STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE FAR *) PURE;
    STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetScanLine)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetVersion)(THIS_ LPDDVERSIONINFO) PURE;
    STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL ) PURE;
    STDMETHOD(RestoreDisplayMode)(THIS) PURE;
    STDMETHOD(SetExclusiveModeOwner)(THIS_ DWORD,BOOL) PURE;
    STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD,DWORD) PURE;
    STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE ) PURE;
};
#endif

/*
 * DDOVERLAYFX
 */
typedef struct _DDOVERLAYFX
{
    DWORD       dwSize;                         // size of structure
    DWORD       dwZOrder;                       // Override for overlay surface Z order
    DWORD       dwAlphaEdgeBlendBitDepth;       // Bit depth used to specify constant for alpha edge blend
    DWORD       dwAlphaEdgeBlend;               // Constant to use as alpha for edge blend
    DWORD       dwConstAlphaDestBitDepth;       // Bit depth used to specify alpha constant for destination
    union
    {
        DWORD   dwConstAlphaDest;               // Constant to use as alpha channel for dest
        LPDIRECTDRAWSURFACE lpDDSAlphaDest;     // Surface to use as alpha channel for dest
    };
    DWORD       dwConstAlphaSrcBitDepth;        // Bit depth used to specify alpha constant for source
    union
    {
        DWORD   dwConstAlphaSrc;                // Constant to use as alpha channel for src
        LPDIRECTDRAWSURFACE lpDDSAlphaSrc;      // Surface to use as alpha channel for src
    };
    DDCOLORKEY  dckDestColorkey;                // DestColorkey override
    DDCOLORKEY  dckSrcColorkey;                 // DestColorkey override
} DDOVERLAYFX;

typedef DDOVERLAYFX FAR *LPDDOVERLAYFX;

/*
 * IDirectDrawPalette
 */
#undef INTERFACE
#define INTERFACE IDirectDrawPalette
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectDrawPalette, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDrawPalette methods ***/
    STDMETHOD(GetCaps)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetEntries)(THIS_ DWORD,DWORD,DWORD,LPPALETTEENTRY) PURE;
    STDMETHOD(SetEntries)(THIS_ DWORD,DWORD,DWORD,LPPALETTEENTRY) PURE;
};
#endif

/*
 * DDPIXELFORMAT
 */
typedef struct _DDPIXELFORMAT
{
    DWORD       dwSize;                 // size of structure
    DWORD       dwFlags;                // pixel format flags
    DWORD       dwFourCC;               // (FOURCC code)
    union
    {
        DWORD   dwRGBBitCount;          // how many bits per pixel (BD_4,8,16,24,32)
        DWORD   dwYUVBitCount;          // how many bits per pixel (BD_4,8,16,24,32)
        DWORD   dwZBufferBitDepth;      // how many bits for z buffers (BD_8,16,24,32)
        DWORD   dwAlphaBitDepth;        // how many bits for alpha channels (BD_1,2,4,8)
    };
    union
    {
        DWORD   dwRBitMask;             // mask for red bit
        DWORD   dwYBitMask;             // mask for Y bits
    };
    union
    {
        DWORD   dwGBitMask;             // mask for green bits
        DWORD   dwUBitMask;             // mask for U bits
    };
    union
    {
        DWORD   dwBBitMask;             // mask for blue bits
        DWORD   dwVBitMask;             // mask for V bits
    };
    union
    {
        DWORD   dwRGBAlphaBitMask;      // mask for alpha channel
        DWORD   dwYUVAlphaBitMask;      // mask for alpha channel
    };
} DDPIXELFORMAT;

typedef DDPIXELFORMAT FAR* LPDDPIXELFORMAT;

/*
 * IDirectDrawSurface
 */
typedef DWORD   (FAR PASCAL *LPCLIPCALLBACK)(DWORD);
typedef DWORD   (FAR PASCAL *LPSTREAMCALLBACK)(DWORD);
#undef INTERFACE
#define INTERFACE IDirectDrawSurface
#ifdef _WIN32
DECLARE_INTERFACE_( IDirectDrawSurface, IUnknown )
{
    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID * ppvObj) PURE;
    STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
    STDMETHOD_(ULONG,Release) (THIS) PURE;
    /*** IDirectDrawSurface methods ***/
    STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(AddSurfaceDependency)(THIS_ LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(Blt)(THIS_ LPRECT,LPDIRECTDRAWSURFACE, LPRECT,DWORD, LPDDBLTFX) PURE;
    STDMETHOD(BltFast)(THIS_ DWORD,DWORD,LPDIRECTDRAWSURFACE, LPRECT,BOOL) PURE;
    STDMETHOD(Compose)(THIS_ LPRECT,LPDIRECTDRAWSURFACE,LPRECT,DWORD,LPDDCOMPOSEFX) PURE;
    STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD,LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(DeleteSurfaceDependency)(THIS_ DWORD,LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(DestLock)(THIS) PURE;
    STDMETHOD(DestUnlock)(THIS) PURE;
    STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK) PURE;
    STDMETHOD(EnumSurfaceDependencies)(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK) PURE;
    STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE) PURE;
    STDMETHOD(Flush)(THIS_ DWORD) PURE;
    STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS, LPDIRECTDRAWSURFACE FAR *) PURE;
    STDMETHOD(GetBltStatus)(THIS_ DWORD) PURE;
    STDMETHOD(GetCaps)(THIS_ LPDDSCAPS) PURE;
    STDMETHOD(GetClipList)(THIS_ LPRGNDATA) PURE;
    STDMETHOD(GetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(GetCompositionOrder)(THIS_ LPDWORD) PURE;
    STDMETHOD(GetDC)(THIS_ HDC FAR *) PURE;
    STDMETHOD(GetFlipStatus)(THIS_ DWORD) PURE;
    STDMETHOD(GetHWnd)(THIS_ HWND FAR *) PURE;
    STDMETHOD(GetOverlayPosition)(THIS_  LPDWORD, LPDWORD ) PURE;
    STDMETHOD(GetOverlayZOrder)(THIS_  LPDWORD ) PURE;
    STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE FAR*) PURE;
    STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT) PURE;
    STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC) PURE;
    STDMETHOD(InsertOverlayZOrder)(THIS_ DWORD) PURE;
    STDMETHOD(Lock)(THIS_ LPRECT,LPDDSURFACEDESC,DWORD,HANDLE) PURE;
    STDMETHOD(ReleaseDC)(THIS_ HDC) PURE;
    STDMETHOD(SetClipList)(THIS_ LPRGNDATA,DWORD) PURE;
    STDMETHOD(SetClippingNotificationCallback)(THIS_ DWORD,LPCLIPCALLBACK) PURE;
    STDMETHOD(SetColorKey)(THIS_ DWORD, LPDDCOLORKEY) PURE;
    STDMETHOD(SetCompositionOrder)(THIS_ DWORD) PURE;
    STDMETHOD(SetHWnd)(THIS_ HWND) PURE;
    STDMETHOD(SetOverlayPosition)(THIS_ DWORD, DWORD ) PURE;
    STDMETHOD(SetOverlayZOrder)(THIS_ DWORD ) PURE;
    STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE) PURE;
    STDMETHOD(SetStreamingNotificationCallback)(THIS_ DWORD,LPSTREAMCALLBACK) PURE;
    STDMETHOD(Unlock)(THIS_ LPVOID) PURE;
    STDMETHOD(UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE,LPRECT,DWORD, LPDDOVERLAYFX) PURE;
};
#endif

/*
 * DDSURFACEDESC
 */
typedef struct _DDSURFACEDESC
{
    DWORD               dwSize;                 // size of the DDSURFACEDESC structure
    DWORD               dwFlags;                // determines what fields are valid
    DDSCAPS             ddsCaps;                // direct draw surface capabilities
    DWORD               dwHeight;               // height of surface to be created
    DWORD               dwWidth;                // width of input surface
    LONG                lPitch;                 // distance to start of next line (return value)
    DWORD               dwBackBufferCount;      // number of back buffers requested
    DWORD               dwZBufferBitDepth;      // depth of Z buffer requested
    DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
    DWORD               dwCompositionOrder;     // blt order for the surface, 0 is background
    DWORD               dwOverlayZOrder;        // z order when it is being overlayed
    DWORD               hWnd;                   // window handle associated with surface
    LPVOID              lpSurface;              // pointer to an associated surface memory
    DDPIXELFORMAT       ddpfPixelFormat;        // pixel format description of the surface
    DDCOLORKEY          ddckCKDestOverlay;      // color key for destination overlay use
    DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
    DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
    DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
    LPRGNDATA           lpClipList;             // clip list (return value)
    LPDIRECTDRAWSURFACE lpDDSurface;            // pointer to DirectDraw Surface struct (return value)
} DDSURFACEDESC;

/*
 * ddsCaps field is valid.
 */
#define DDSD_CAPS               0x00000000l     // default

/*
 * dwHeight field is valid.
 */
#define DDSD_HEIGHT             0x00000002l

/*
 * dwWidth field is valid.
 */
#define DDSD_WIDTH              0x00000004l

/*
 * lPitch is valid.
 */
#define DDSD_PITCH              0x00000008l

/*
 * dwBackBufferCount is valid.
 */
#define DDSD_BACKBUFFERCOUNT    0x00000020l

/*
 * dwZBufferBitDepth is valid.
 */
#define DDSD_ZBUFFERBITDEPTH    0x00000040l

/*
 * dwAlphaBitDepth is valid.
 */
#define DDSD_ALPHABITDEPTH      0x00000080l

/*
 * dwCompositionOrder is valid.
 */
#define DDSD_COMPOSITIONORDER   0x00000100l

/* 
 * dwOverlayZOrder is valid.
 */
#define DDSD_OVERLAYZORDER      0x00000200l

/*
 * hWnd is valid.
 */
#define DDSD_HWND               0x00000400l

/*
 * lpSurface is valid.
 */
#define DDSD_LPSURFACE          0x00000800l

/*
 * ddpfPixelFormat is valid.
 */
#define DDSD_PIXELFORMAT        0x00001000l

/*
 * ddckCKDestOverlay is valid.
 */
#define DDSD_CKDESTOVERLAY      0x00002000l

/*
 * ddckCKDestBlt is valid.
 */
#define DDSD_CKDESTBLT          0x00004000l

/*
 * ddckCKSrcOverlay is valid.
 */
#define DDSD_CKSRCOVERLAY       0x00008000l

/*
 * ddckCKSrcBlt is valid.
 */
#define DDSD_CKSRCBLT           0x00010000l

/*
 * lpClipList is valid.
 */
#define DDSD_LPCLIPLIST         0x00020000l

/*
 * lpDDSurface is valid.
 */
#define DDSD_LPDDSURFACE        0x00040000l

/*
 * All input fields are valid. lpClipList 
 * and lpDDSurface are return values, there validity
 * is specified independently by the member function.
 */
#define DDSD_ALL                0x0003ffeel

/*
 * DDMODEDESC
 */
typedef struct _DDMODEDESC
{
    DWORD               dwSize;                 // size of structure
    DWORD               dwFlags;                // enumeration flags
    DWORD               dwMonitorFrequency;     // frequency of monitor in mode
    DDSURFACEDESC       ddsdSurfaceDesc;        // surface being enumerated
} DDMODEDESC;

typedef DDMODEDESC FAR* LPDDMODEDESC;

/*
 * DDVERSIONINFO
 */
#define MAX_VERSION_STR 40
typedef struct _DDVERSIONINFO
{
    DWORD       dwSize;                 // size of the DDVERSIONINFO structure
    DWORD       dwVersion;              // Version of DirectDraw
    DWORD       dwBuildNumber;          // Driver build number
    DWORD       dwDate;                 // Build date
    char        szCompanyName[MAX_VERSION_STR];
    char        szPhoneContact[MAX_VERSION_STR];
    char        szFaxContact[MAX_VERSION_STR];
    char        szEmailContact1[MAX_VERSION_STR];
    char        szEmailContact2[MAX_VERSION_STR];
} DDVERSIONINFO; 

#define DD_VERSION              0x00000200l

/*============================================================================
 *
 * Direct Draw Capability Flags
 *
 * These flags are used to describe the capabilities of a given Surface.
 * All flags are bit flags. 
 *
 *==========================================================================*/

/****************************************************************************
 *
 * DIRECTDRAWSURFACE CAPABILITY FLAGS
 *
 ****************************************************************************/
/*
 * Indicates that this surface is a front buffer, back buffer, or
 * texture map that is being used in conjunction with a 3DDDI or
 * Direct3D HAL.
 */
#define DDSCAPS_3D                              0x00000001l

/*
 * Indicates that this surface contains alpha information.  The pixel
 * format must be interrogated to determine whether this surface
 * contains only alpha information or alpha information interlaced
 * with pixel color data (e.g. RGBA or YUVA).
 */
#define DDSCAPS_ALPHA                           0x00000002l

/*
 * Indicates that this surface is a backbuffer.  It is generally
 * set by CreateSurface when the DDSCAPS_FLIP capability bit is set.
 * It indicates that this surface is THE back buffer of a surface
 * flipping structure.  DirectDraw supports N surfaces in a
 * surface flipping structure.  Only the surface that immediately
 * precedeces the DDSCAPS_FRONTBUFFER has this capability bit set.
 * The other surfaces are identified as back buffers by the presence
 * of the DDSCAPS_FLIP capability, their attachment order, and the
 * absence of the DDSCAPS_FRONTBUFFER and DDSCAPS_BACKBUFFER
 * capabilities.  The bit is sent to CreateSurface when a standalone
 * back buffer is being created.  This surface could be attached to
 * a front buffer and/or back buffers to form a flipping surface
 * structure after the CreateSurface call.  See AddAttachments for
 * a detailed description of the behaviors in this case.
 */
#define DDSCAPS_BACKBUFFER                      0x00000004l

/*
 * Indicates a complex surface structure is being described.  A
 * complex surface structure results in the creation of more than
 * one surface.  The additional surfaces are attached to the root
 * surface.  The complex structure can only be destroyed by
 * destroying the root.
 */
#define DDSCAPS_COMPLEX                         0x00000008l

/*
 * Indicates that this surface is a part of a surface flipping structure.
 * When it is passed to CreateSurface the DDSCAPS_FRONTBUFFER and
 * DDSCAP_BACKBUFFER bits are not set.  They are set by CreateSurface
 * on the resulting creations.  The dwBackBufferCount field in the
 * DDSURFACEDESC structure must be set to at least 1 in order for
 * the CreateSurface call to succeed.  The DDSCAPS_COMPLEX capability
 * must always be set with creating multiple surfaces through CreateSurface.
 */
#define DDSCAPS_FLIP                            0x00000010l

/*
 * Indicates that this surface is THE front buffer of a surface flipping
 * structure.  It is generally set by CreateSurface when the DDSCAPS_FLIP
 * capability bit is set.
 * If this capability is sent to CreateSurface then a standalonw front buffer
 * is created.  This surface will not have the DDSCAPS_FLIP capability.
 * It can be attached to other back buffers to form a flipping structure.
 * See AddAttachments for a detailed description of the behaviors in this
 * case.
 */
#define DDSCAPS_FRONTBUFFER                     0x00000020l

/*
 * Indicates that this surface is any offscreen surface that is not an overlay,
 * texture, zbuffer, front buffer, back buffer, or alpha surface.  It is used
 * to identify plain vanilla surfaces.
 */
#define DDSCAPS_OFFSCREENPLAIN                  0x00000040l

/*
 * Indicates that this surface is an overlay.  It may or may not be directly visible
 * depending on whether or not it is currently being overlayed onto the primary 
 * surface.  DDSCAPS_VISIBLE can be used to determine whether or not it is being 
 * overlayed at the moment. 
 */
#define DDSCAPS_OVERLAY                         0x00000080l

/*
 * Indicates that unique DirectDrawPalette objects can be created and
 * attached to this surface.
 */
#define DDSCAPS_PALETTE                         0x00000100l

/*
 * Indicates that this surface is the primary surface.  The primary
 * surface represents what the user is seeing at the moment.
 */
#define DDSCAPS_PRIMARYSURFACE                  0x00000200l

/*
 * Indicates that this surface is the primary surface for the left eye.
 * The primary surface for the left eye represents what the user is seeing
 * at the moment with the users left eye.  When this surface is created the
 * DDSCAPS_PRIMARYSURFACE represents what the user is seeing with the users
 * right eye.
 */
#define DDSCAPS_PRIMARYSURFACELEFT              0x00000400l

/*
 * Indicates that this surface memory was allocated in system memory
 */
#define DDSCAPS_SYSTEMMEMORY                    0x00000800l

/*
 * Indicates that this surface can be used as a 3D texture.  It does not
 * indicate whether or not the surface is being used for that purpose.
 */
#define DDSCAPS_TEXTUREMAP                      0x00001000l

/*
 * Indicates that this surface can be accessed by multiple threads and
 * processes safely.
 */
#define DDSCAPS_THREADSAFE                      0x00002000l

/*
 * Indicates that this surface exists in video memory.
 */
#define DDSCAPS_VIDEOMEMORY                     0x00004000l

/*
 * Indicates that changes made to this surface are immediately visible.  
 * It is always set for the primary surface and is set for overlays while
 * they are bing overalyed and texture maps while they are being textured. 
 */
#define DDSCAPS_VISIBLE                         0x00008000l

/*
 * Indicates that only writes are permitted to the surface.  Read accesses 
 * from the surface may or may not generate a protection fault, but the 
 * results of a read from this surface will not be meaningful.  READ ONLY.
 */
#define DDSCAPS_WRITEONLY                       0x00010000l

/*
 * Indicates that this surface is the z buffer.  The z buffer does not contain
 * displayable information.  Instead it contains bit depth information that is
 * used to determine which pixels are visible and which are obscured. 
 */
#define DDSCAPS_ZBUFFER                         0x00020000l


/****************************************************************************
 *
 * DIRECTDRAW DRIVER CAPABILITY FLAGS
 *
 ****************************************************************************/

/*
 * Display hardware has 3D acceleration.
 */
#define DDCAPS_3D                       0x00000001l

/*
 * Indicates that DirectDraw will support only source rectangles that are aligned
 * on DIRECTDRAWCAPS.dwAlignBoundarySrc boundaries of the surface, respectively.
 * READ ONLY.
 */
#define DDCAPS_ALIGNBOUNDARY            0x00000002l

/*
 * Indicates that DirectDraw will support only source rectangles  whose sizes in 
 * BYTEs are DIRECTDRAWCAPS.dwAlignSizeSrc multiples, respectively.  READ ONLY.
 */
#define DDCAPS_ALIGNSIZE                0x00000004l

/*
 * Indicates that DirectDraw will create video memory surfaces that have a stride 
 * alignment equal to DIRECTDRAWCAPS.dwAlignStride.  READ ONLY.
 */
#define DDCAPS_ALIGNSTRIDE              0x00000008l

/*
 * Display hardware is capable of blt operations.
 */
#define DDCAPS_BLT                      0x00000010l

/*
 * Display hardware is capable of asynchronous blt operations.
 */
#define DDCAPS_BLTASYNC                 0x00000020l

/*
 * Display hardware is capable of color space conversions during the blt operation.
 */
#define DDCAPS_BLTFOURCC                0x00000040l

/*
 * Display hardware is capable of stretching during blt operations.
 */
#define DDCAPS_BLTSTRETCH               0x00000080l

/*
 * Display hardware is shared with GDI.
 */
#define DDCAPS_GDI                      0x00000100l

/*
 * Display hardware can overlay.
 */
#define DDCAPS_OVERLAY                  0x00000200l

/*
 * Set if display hardware supports overlays but can not clip them.
 */
#define DDCAPS_OVERLAYCANTCLIP          0x00000400l

/*
 * Indicates that overlay hardware is capable of color space conversions during
 * the overlay operation.
 */
#define DDCAPS_OVERLAYFOURCC            0x00000800l

/*
 * Indicates that stretching can be done by the overlay hardware.
 */
#define DDCAPS_OVERLAYSTRETCH           0x00001000l

/*
 * Indicates that unique DirectDrawPalettes can be created for DirectDrawSurfaces
 * other than the primary surface.
 */
#define DDCAPS_PALETTE                  0x00002000l

/*
 * Indicates that palette changes can be syncd with the veritcal refresh.
 */
#define DDCAPS_PALETTEVSYNC             0x00004000l

/*
 * Display hardware can return the current scan line.
 */
#define DDCAPS_READSCANLINE             0x00008000l

/*
 * Display hardware has stereo vision capabilities.  DDSCAPS_PRIMARYSURFACELEFT 
 * can be created.
 */
#define DDCAPS_STEREOVIEW               0x00010000l

/*
 * Display hardware is capable of generating a vertical blank interrupt.
 */
#define DDCAPS_VBI                      0x00020000l

/*
 * Supports the use of z buffers with blt operations.
 */
#define DDCAPS_ZBLTS                    0x00040000l

/*
 * Supports the use of OverlayZOrder as a z value for overlays to control their 
 * layering.
 */
#define DDCAPS_ZOVERLAYS                0x00080000l

#define DDCAPS_COLORKEY                 0x00100000l
#define DDCAPS_ALPHA                    0x00200000l

/****************************************************************************
 *
 * DIRECTDRAW FX CAPABILITY FLAGS
 *
 ****************************************************************************/

/*
 * Supports alpha blending around the edge of a source color keyed surface.
 */
#define DDFXCAPS_ALPHAEDGEBLEND         0x00000001l

/*
 * Supports alpha information in the pixel format.  The bit depth of alpha 
 * information in the pixel format can be 1,2,4, or 8.  The alpha value becomes
 * more opaque as the alpha value increases.  (0 is transparent.)
 */
#define DDFXCAPS_ALPHAPIXELS            0x00000002l

/*
 * Supports alpha information in the pixel format.  The bit depth of alpha 
 * information in the pixel format can be 1,2,4, or 8.  The alpha value 
 * becomes more transparent as the alpha value increases.  (0 is opaque.) 
 * This flag can only be set if DDFXCAPS_ALPHAPIXELS is set.
 */
#define DDFXCAPS_ALPHAPIXELSNEG         0x00000004l

/*
 * Supports alpha only surfaces.  The bit depth of an alpha only surface can be
 * 1,2,4, or 8.  The alpha value becomes more opaque as the alpha value increases.
 * (0 is transparent.)
 */
#define DDFXCAPS_ALPHASURFACES          0x00000008l

/*
 * The depth of the alpha channel data can range can be 1,2,4, or 8.  
 * The NEG suffix indicates that this alpha channel becomes more transparent
 * as the alpha value increases. (0 is opaque.)  This flag can only be set if
 * DDFXCAPS_ALPHASURFACES is set.
 */
#define DDFXCAPS_ALPHASURFACESNEG       0x00000010l

/*
 * Uses arithmetic operations to stretch and shrink surfaces during blt
 * rather than pixel doubling techniques.  Along the Y axis.
 */
#define DDFXCAPS_BLTARITHSTRETCHY       0x00000020l

/*
 * Supports mirroring left to right in blt.
 */
#define DDFXCAPS_BLTMIRRORLEFTRIGHT     0x00000040l

/*
 * Supports mirroring top to bottom in blt.
 */
#define DDFXCAPS_BLTMIRRORUPDOWN        0x00000080l

/*
 * Supports arbitrary rotation for blts.
 */
#define DDFXCAPS_BLTROTATION            0x00000100l

/*
 * Supports 90 degree rotations for blts.
 */
#define DDFXCAPS_BLTROTATION90          0x00000200l

/*
 * DirectDraw supports arbitrary shrinking of a surface along the
 * x axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSHRINKX             0x00000400l

/*
 * DirectDraw supports integer shrinking (1x,2x,) of a surface
 * along the x axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSHRINKXN            0x00000800l

/*
 * DirectDraw supports arbitrary shrinking of a surface along the
 * y axis (horizontal direction) for blts.  
 */
#define DDFXCAPS_BLTSHRINKY             0x00001000l

/*
 * DirectDraw supports integer shrinking (1x,2x,) of a surface
 * along the y axis (vertical direction) for blts.  
 */
#define DDFXCAPS_BLTSHRINKYN            0x00002000l

/*
 * DirectDraw supports arbitrary stretching of a surface along the
 * x axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSTRETCHX            0x00004000l

/*
 * DirectDraw supports integer stretching (1x,2x,) of a surface
 * along the x axis (horizontal direction) for blts.
 */
#define DDFXCAPS_BLTSTRETCHXN           0x00008000l

/*
 * DirectDraw supports arbitrary stretching of a surface along the
 * y axis (horizontal direction) for blts.  
 */
#define DDFXCAPS_BLTSTRETCHY            0x00010000l

/*
 * DirectDraw supports integer stretching (1x,2x,) of a surface
 * along the y axis (vertical direction) for blts.  
 */
#define DDFXCAPS_BLTSTRETCHYN           0x00020000l

/*
 * Uses arithmetic operations to stretch and shrink surfaces during 
 * overlay rather than pixel doubling techniques.  Along the Y axis 
 * for overlays.
 */
#define DDFXCAPS_OVERLAYARITHSTRETCHY   0x00040000l

/*
 * DirectDraw supports arbitrary shrinking of a surface along the
 * x axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSHRINKX         0x00080000l

/*
 * DirectDraw supports integer shrinking (1x,2x,) of a surface
 * along the x axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSHRINKXN        0x00100000l

/*
 * DirectDraw supports arbitrary shrinking of a surface along the
 * y axis (horizontal direction) for overlays.  
 */
#define DDFXCAPS_OVERLAYSHRINKY         0x00200000l

/*
 * DirectDraw supports integer shrinking (1x,2x,) of a surface
 * along the y axis (vertical direction) for overlays.  
 */
#define DDFXCAPS_OVERLAYSHRINKYN        0x00400000l

/*
 * DirectDraw supports arbitrary stretching of a surface along the
 * x axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSTRETCHX        0x00800000l

/*
 * DirectDraw supports integer stretching (1x,2x,) of a surface
 * along the x axis (horizontal direction) for overlays.
 */
#define DDFXCAPS_OVERLAYSTRETCHXN       0x01000000l

/*
 * DirectDraw supports arbitrary stretching of a surface along the
 * y axis (horizontal direction) for overlays.  
 */
#define DDFXCAPS_OVERLAYSTRETCHY        0x02000000l

/*
 * DirectDraw supports integer stretching (1x,2x,) of a surface
 * along the y axis (vertical direction) for overlays.  
 */
#define DDFXCAPS_OVERLAYSTRETCHYN       0x04000000l

/****************************************************************************
 *
 * DIRECTDRAW STEREO VIEW CAPABILITIES
 *
 ****************************************************************************/

/*
 * The stereo view is accomplished via enigma encoding.
 */
#define DDSVCAPS_ENIGMA                 0x00000001l

/*
 * The stereo view is accomplished via high frequency flickering.
 */
#define DDSVCAPS_FLICKER                0x00000002l

/*
 * The stereo view is accomplished via red and blue filters applied
 * to the left and right eyes.  All images must adapt their colorspaces
 * for this process.
 */
#define DDSVCAPS_REDBLUE                0x00000004l

/****************************************************************************
 *
 * DIRECTDRAWPALETTE CAPABILITIES
 *
 ****************************************************************************/

/*
 * Index is 4 bits.  There are sixteen color entries in the palette table.
 */
#define DDPCAPS_4BIT                    0x00000001l

/*
 * Index is onto a 8 bit color index.  This field is only valid with the
 * DDPCAPS_4BIT capability and the target surface is in 8bpp. Each color
 * entry is one byte long and is an index into destination surface's 8bpp
 * palette.
 */
#define DDPCAPS_8BITENTRIES             0x00000002l

/*
 * Index is 8 bits.  There are 256 color entries in the palette table.
 */
#define DDPCAPS_8BIT                    0x00000004l

/*
 * Indicates that this DIRECTDRAWPALETTE should use the palette color array
 * passed into the lpDDColorArray parameter to initialize the DIRECTDRAWPALETTE
 * object.
 */
#define DDPCAPS_INITIALIZE              0x00000008l

/*
 * This palette is the one attached to the primary surface.  Changing this
 * table has immediate effect on the display unless DDPSETPAL_VSYNC is specified
 * and supported.
 */
#define DDPCAPS_PRIMARYSURFACE          0x00000010l

/*
 * This palette is the one attached to the primary surface left.  Changing
 * this table has immediate effect on the display for the left eye unless
 * DDPSETPAL_VSYNC is specified and supported.
 */
#define DDPCAPS_PRIMARYSURFACELEFT      0x00000020l

/*
 * This palette is safe to use for a multi-threaded access.
 */
#define DDPCAPS_THREADSAFE              0x00000040l

/*
 * This palette can have modifications to it synced with the monitors
 * refresh rate.
 */
#define DDPCAPS_VSYNC                   0x00000080l

/****************************************************************************
 *
 * DIRECTDRAWPALETTE SETENTRY CONSTANTS
 *
 ****************************************************************************/

/*
 * Palette changes take effect immediately.
 */
#define DDPSETPAL_IMMEDIATE             0x00000000l     // default

/*
 * Palette changes should take effect during the vertical blank to avoid
 * palette tearing.
 */
#define DDPSETPAL_VSYNC                 0x00000002l

/****************************************************************************
 *
 * DIRECTDRAWPALETTE GETENTRY CONSTANTS
 *
 ****************************************************************************/

/* 0 is the only legal value */

/****************************************************************************
 *
 * DIRECTDRAWSURFACE SETPALETTE CONSTANTS
 *
 ****************************************************************************/

/*
 * Palette change takes effect immediately.
 */
#define DDSETPAL_IMMEDIATE              0x00000000l     // default

/*
 * Palette change should take effect during the vertical blank to avoid
 * palette tearing.
 */
#define DDSETPAL_VSYNC                  0x00000002l

/****************************************************************************
 *
 * DIRECTDRAW BITDEPTH CONSTANTS
 *
 ****************************************************************************/

/*
 * 1 bit per pixel.
 */
#define DDBD_1                  0x00000001l

/*
 * 2 bits per pixel.
 */
#define DDBD_2                  0x00000002l

/*
 * 4 bits per pixel.
 */
#define DDBD_4                  0x00000004l

/*
 * 8 bits per pixel.
 */
#define DDBD_8                  0x00000008l

/*
 * 16 bits per pixel.
 */
#define DDBD_16                 0x00000010l

/*
 * 24 bits per pixel.
 */
#define DDBD_24                 0X00000020l

/*
 * 32 bits per pixel.
 */
#define DDBD_32                 0x00000040l

/****************************************************************************
 *
 * DIRECTDRAWSURFACE SET/GET COLOR KEY FLAGS
 *
 ****************************************************************************/

/*
 * Set if the structure contains a color space.  Not set if the structure
 * contains a single color key.
 */
#define DDCKEY_COLORSPACE       0x00000001l

/*
 * Set if the structure specifies a color key or color space which is to be
 * used as a destination color key for blt operations.
 */
#define DDCKEY_DESTBLT          0x00000002l

/*
 * Set if the structure specifies a color key or color space which is to be
 * used as a destination color key for overlay operations.
 */
#define DDCKEY_DESTOVERLAY      0x00000004l

/*
 * Set if the structure specifies a color key or color space which is to be
 * used as a source color key for blt operations.
 */
#define DDCKEY_SRCBLT           0x00000008l

/*
 * Set if the structure specifies a color key or color space which is to be
 * used as a source color key for overlay operations.
 */
#define DDCKEY_SRCOVERLAY       0x00000010l


/****************************************************************************
 *
 * DIRECTDRAW COLOR KEY CAPABILITY FLAGS
 *
 ****************************************************************************/

/*
 * Supports transparent blting using a color key to identify the replaceable 
 * bits of the destination surface for RGB colors.
 */
#define DDCKEYCAPS_DESTBLT                      0x00000001l

/*
 * Supports transparent blting using a color space to identify the replaceable
 * bits of the destination surface for RGB colors.
 */
#define DDCKEYCAPS_DESTBLTCLRSPACE              0x00000002l

/*
 * Supports transparent blting using a color space to identify the replaceable
 * bits of the destination surface for YUV colors.
 */
#define DDCKEYCAPS_DESTBLTCLRSPACEYUV           0x00000004l

/*
 * Supports transparent blting using a color key to identify the replaceable 
 * bits of the destination surface for YUV colors.
 */
#define DDCKEYCAPS_DESTBLTYUV                   0x00000008l

/*
 * Supports overlaying using colorkeying of the replaceable bits of the surface
 * being overlayed for RGB colors.
 */
#define DDCKEYCAPS_DESTOVERLAY                  0x00000010l

/*
 * Supports a color space as the color key for the destination for RGB colors.
 */
#define DDCKEYCAPS_DESTOVERLAYCLRSPACE          0x00000020l

/*
 * Supports a color space as the color key for the destination for YUV colors.
 */
#define DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV       0x00000040l

/*
 * Supports only one active destination color key value for visible overlay
 * surfaces.
 */
#define DDCKEYCAPS_DESTOVERLAYONEACTIVE         0x00000080l

/*
 * Supports overlaying using colorkeying of the replaceable bits of the 
 * surface being overlayed for YUV colors.
 */
#define DDCKEYCAPS_DESTOVERLAYYUV               0x00000100l

/*
 * Supports transparent blting using the color key for the source with
 * this surface for RGB colors.
 */
#define DDCKEYCAPS_SRCBLT                       0x00000200l

/*
 * Supports transparent blting using a color space for the source with
 * this surface for RGB colors.
 */
#define DDCKEYCAPS_SRCBLTCLRSPACE               0x00000400l

/*
 * Supports transparent blting using a color space for the source with
 * this surface for YUV colors.
 */
#define DDCKEYCAPS_SRCBLTCLRSPACEYUV            0x00000800l

/*
 * Supports transparent blting using the color key for the source with
 * this surface for YUV colors.
 */
#define DDCKEYCAPS_SRCBLTYUV                    0x00001000l

/*
 * Supports overlays using the color key for the source with this
 * overlay surface for RGB colors.
 */
#define DDCKEYCAPS_SRCOVERLAY                   0x00002000l

/*
 * Supports overlays using a color space as the source color key for
 * the overlay surface for RGB colors.
 */
#define DDCKEYCAPS_SRCOVERLAYCLRSPACE           0x00004000l

/*
 * Supports overlays using a color space as the source color key for
 * the overlay surface for YUV colors.
 */
#define DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV        0x00008000l

/*
 * Supports only one active source color key value for visible
 * overlay surfaces.
 */
#define DDCKEYCAPS_SRCOVERLAYONEACTIVE          0x00010000l

/*
 * Supports overlays using the color key for the source with this
 * overlay surface for YUV colors.
 */
#define DDCKEYCAPS_SRCOVERLAYYUV                0x00020000l

/****************************************************************************
 *
 * DIRECTDRAW PIXELFORMAT FLAGS
 *
 ****************************************************************************/

/*
 * The surface has alpha channel information in the pixel format.
 */
#define DDPF_ALPHAPIXELS                        0x00000001l

/*
 * The pixel format contains alpha only information
 */
#define DDPF_ALPHA                              0x00000002l

/*
 * The FourCC code is valid.
 */
#define DDPF_FOURCC                             0x00000004l

/*
 * The surface is 4-bit color indexed.
 */
#define DDPF_PALETTEINDEXED4                    0x00000008l

/*
 * The surface is 4-bit color indexed to an 8-bit palette.
 */
#define DDPF_PALETTEINDEXED4TO8                 0x00000010l

/*
 * The surface is 8-bit color indexed.
 */
#define DDPF_PALETTEINDEXED8                    0x00000020l

/*
 * The RGB data in the pixel format structure is valid.
 */
#define DDPF_RGB                                0x00000040l

/*
 * The surface will accept pixel data in the format specified
 * and compress it during the write.
 */
#define DDPF_COMPRESSED                         0x00000080l

/*
 * The surface will accept RGB data and translate it during
 * the write to YUV data.  The format of the data to be written
 * will be contained in the pixel format structure.  The DDPF_RGB
 * flag will be set. 
 */
#define DDPF_RGBTOYUV                           0x00000100l

/*
 * pixel format is YUV - YUV data in pixel format struct is valid
 */
#define DDPF_YUV                                0x00000200l

/*
 * pixel format is a z buffer only surface
 */
#define DDPF_ZBUFFER                            0x00000400l

/*===========================================================================
 *
 *
 * DIRECTDRAW CALLBACK FLAGS
 *
 *
 *==========================================================================*/

/****************************************************************************
 *
 * DIRECTDRAW FLIP FLAGS FLAGS
 *
 ****************************************************************************/
#define DDFLIP_WAITFORVERTICALBLANKEND          0x00000001l

/****************************************************************************
 *
 * DIRECTDRAW ENUMSURFACES FLAGS
 *
 ****************************************************************************/

/*
 * Enumerate all of the surfaces that meet the search criterion.
 */
#define DDENUMSURFACES_ALL                      0x00000001l

/*
 * A search hit is a surface that matches the surface description.
 */
#define DDENUMSURFACES_MATCH                    0x00000002l

/*
 * A search hit is a surface that does not match the surface description.
 */
#define DDENUMSURFACES_NOMATCH                  0x00000004l

/*
 * Enumerate the first surface that can be created which meets the search criterion.
 */
#define DDENUMSURFACES_CANBECREATED             0x00000008l

/*
 * Enumerate the surfaces that already exist that meet the search criterion.
 */
#define DDENUMSURFACES_DOESEXIST                0x00000010l

/****************************************************************************
 *
 * DIRECTDRAW SETEXCLUSIVEMODEOWNER FLAGS
 *
 ****************************************************************************/

/*
 * Exclusive mode owner will be responsible for the entire primary surface.
 * GDI can be ignored.
 */
#define DDSEMO_FULLSCREEN                       0x00000001l

/****************************************************************************
 *
 * DIRECTDRAW BLT FLAGS
 *
 ****************************************************************************/

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the destination surface as the alpha channel for this blt.
 */
#define DDBLT_ALPHADEST                         0x00000001l

/*
 * Use the dwConstAlphaDest field in the DDBLTFX structure as the alpha channel
 * for the destination surface for this blt.
 */
#define DDBLT_ALPHADESTCONSTOVERRIDE            0x00000002l

/*
 * The NEG suffix indicates that the destination surface becomes more
 * transparent as the alpha value increases. (0 is opaque)
 */
#define DDBLT_ALPHADESTNEG                      0x00000004l

/*
 * Use the lpDDSAlphaDest field in the DDBLTFX structure as the alpha
 * channel for the destination for this blt.
 */
#define DDBLT_ALPHADESTSURFACEOVERRIDE          0x00000008l

/*
 * Use the dwAlphaEdgeBlend field in the DDBLTFX structure as the alpha channel
 * for the edges of the image that border the color key colors.
 */
#define DDBLT_ALPHAEDGEBLEND                    0x00000010l

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the source surface as the alpha channel for this blt.
 */
#define DDBLT_ALPHASRC                          0x00000020l

/*
 * Use the dwConstAlphaSrc field in the DDBLTFX structure as the alpha channel
 * for the source for this blt.
 */
#define DDBLT_ALPHASRCCONSTOVERRIDE             0x00000040l

/*
 * The NEG suffix indicates that the source surface becomes more transparent
 * as the alpha value increases. (0 is opaque)
 */
#define DDBLT_ALPHASRCNEG                       0x00000080l

/*
 * Use the lpDDSAlphaSrc field in the DDBLTFX structure as the alpha channel
 * for the source for this blt. 
 */
#define DDBLT_ALPHASRCSURFACEOVERRIDE           0x00000100l

/*
 * Do this blt asynchronously through the FIFO in the order received.  If
 * there is no room in the hardware FIFO fail the call.
 */
#define DDBLT_ASYNC                             0x00000200l

/*
 * Uses the dwFillColor field in the DDBLTFX structure as the RGB color
 * to fill the destination rectangle on the destination surface with.
 */
#define DDBLT_COLORFILL                         0x00000400l

/*
 * Uses the dwDDFX field in the DDBLTFX structure to specify the effects
 * to use for the blt.
 */
#define DDBLT_DDFX                              0x00000800l

/*
 * Uses the dwDDROPS field in the DDBLTFX structure to specify the ROPS
 * that are not part of the Win32 API.
 */
#define DDBLT_DDROPS                            0x00001000l

/*
 * Use the color key associated with the destination surface.
 */
#define DDBLT_KEYDEST                           0x00002000l

/*
 * Use the dckDestColorkey field in the DDBLTFX structure as the color key
 * for the destination surface.
 */
#define DDBLT_KEYDESTOVERRIDE                   0x00004000l

/*
 * Use the color key associated with the source surface.
 */
#define DDBLT_KEYSRC                            0x00008000l

/*
 * Use the dckSrcColorkey field in the DDBLTFX structure as the color key
 * for the source surface.
 */
#define DDBLT_KEYSRCOVERRIDE                    0x00010000l

/*
 * Use the dwROP field in the DDBLTFX structure for the raster operation
 * for this blt.  These ROPs are the same as the ones defined in the Win32 API.
 */
#define DDBLT_ROP                               0x00020000l

/*
 * Use the dwRotationAngle field in the DDBLTFX structure as the angle
 * (specified in 1/100th of a degree) to rotate the surface.
 */
#define DDBLT_ROTATIONANGLE                     0x00040000l

/*
 * Z-buffered blt using the z-buffers attached to the source and destination
 * surfaces and the dwZBufferOpCode field in the DDBLTFX structure as the
 * z-buffer opcode.
 */
#define DDBLT_ZBUFFER                           0x00080000l

/*
 * Z-buffered blt using the dwConstDest Zfield and the dwZBufferOpCode field
 * in the DDBLTFX structure as the z-buffer and z-buffer opcode respectively
 * for the destination.
 */
#define DDBLT_ZBUFFERCONSTDESTOVERRIDE          0x00100000l

/*
 * Z-buffered blt using the lpDDSDestZBuffer field and the dwZBufferOpCode
 * field in the DDBLTFX structure as the z-buffer and z-buffer opcode
 * respectively for the destination.
 */
#define DDBLT_ZBUFFERDESTOVERRIDE               0x00200000l

/*
 * Z-buffered blt using the dwConstSrcZ field and the dwZBufferOpCode field
 * in the DDBLTFX structure as the z-buffer and z-buffer opcode respectively
 * for the source.
 */
#define DDBLT_ZBUFFERCONSTSRCOVERRIDE           0x00400000l

/*
 * Z-buffered blt using the lpDDSSrcZBuffer field and the dwZBufferOpCode
 * field in the DDBLTFX structure as the z-buffer and z-buffer opcode
 * respectively for the source.
 */
#define DDBLT_ZBUFFERSRCOVERRIDE                0x00800000l

/****************************************************************************
 *
 * DIRECTDRAW COMP FLAGS
 *
 ****************************************************************************/

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the destination surface as the alpha channel for this blt.
 */
#define DDCOMP_ALPHADEST                        0x00000001l

/*
 * Use the dwConstAlphaDest field in the DDCOMPOSEFX structure as the alpha channel
 * for the destination surface for this blt.
 */
#define DDCOMP_ALPHADESTCONSTOVERRIDE           0x00000002l

/*
 * The NEG suffix indicates that the destination surface becomes more
 * transparent as the alpha value increases. (0 is opaque)
 */
#define DDCOMP_ALPHADESTNEG                     0x00000004l

/*
 * Use the lpDDSAlphaDest field in the DDCOMPOSEFX structure as the alpha
 * channel for the destination for this blt.
 */
#define DDCOMP_ALPHADESTSURFACEOVERRIDE         0x00000008l

/*
 * Use the dwAlphaEdgeBlend field in the DDCOMPOSEFX structure as the alpha channel
 * for the edges of the image that border the color key colors.
 */
#define DDCOMP_ALPHAEDGEBLEND                   0x00000010l

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the source surface as the alpha channel for this blt.
 */
#define DDCOMP_ALPHASRC                         0x00000020l

/*
 * Use the dwConstAlphaSrc field in the DDCOMPOSEFX structure as the alpha channel
 * for the source for this blt.
 */
#define DDCOMP_ALPHASRCCONSTOVERRIDE            0x00000040l

/*
 * The NEG suffix indicates that the source surface becomes more transparent
 * as the alpha value increases. (0 is opaque)
 */
#define DDCOMP_ALPHASRCNEG                      0x00000080l

/*
 * Use the lpDDSAlphaSrc field in the DDCOMPOSEFX structure as the alpha channel
 * for the source for this blt. 
 */
#define DDCOMP_ALPHASRCSURFACEOVERRIDE          0x00000100l

/*
 * Before performing this blt invoke the callback specified by the lpfnCallback
 * field in the DDCOMPOSEFX structure.
 */
#define DDCOMP_CALLBACK                         0x00000200l

/*
 * Uses the dwFillColor field in the DDCOMPOSEFX structure as the RGB color
 * to fill the destination rectangle on the destination surface with.
 */
#define DDCOMP_COLORFILL                        0x00000400l

/*
 * Uses the dwDDFX field in the DDCOMPOSEFX structure to specify the effects
 * to use for the blt.
 */
#define DDCOMP_DDFX                             0x00000800l

/*
 * Uses the dwDDROPS field in the DDCOMPOSEFX structure to specify the ROPS
 * that are not part of the Win32 API.
 */
#define DDCOMP_DDROPS                           0x00001000l

/*
 * Use the color key associated with the destination surface.
 */
#define DDCOMP_KEYDEST                          0x00002000l

/*
 * Use the dckDestColorkey field in the DDCOMPOSEFX structure as the color key
 * for the destination surface.
 */
#define DDCOMP_KEYDESTOVERRIDE                  0x00004000l

/*
 * Use the color key associated with the source surface.
 */
#define DDCOMP_KEYSRC                           0x00008000l

/*
 * Use the dckSrcColorkey field in the DDCOMPOSEFX structure as the color key
 * for the source surface.
 */
#define DDCOMP_KEYSRCOVERRIDE                   0x00010000l

/*
 * Do this blt when DestUnlock is called for the last time on the source surface.
 */
#define DDCOMP_ONDESTUNLOCK                     0x00020000l

/*
 * Do this blt when Flip is called on the source surface.
 */
#define DDCOMP_ONFLIP                           0x00040000l

/*
 * Do this blt when Unlock is called for the last time on the source surface.
 */
#define DDCOMP_ONUNLOCK                         0x00080000l

/*
 * Repeat this composition blt the number of times specified in the dwRepeat
 * field of the DDCOMPOSEFX structure
 */
#define DDCOMP_REPEAT                           0x00100000l

/*
 * Use the dwROP field in the DDCOMPOSEFX structure for the raster operation
 * for this blt.  These ROPs are the same as the ones defined in the Win32 API.
 */
#define DDCOMP_ROP                              0x00200000l

/*
 * Use the dwRotationAngle field in the DDCOMPOSEFX structure as the angle
 * (specified in 1/100th of a degree) to rotate the surface.
 */
#define DDCOMP_ROTATIONANGLE                    0x00400000l

/*
 * Z-buffered blt using the z-buffers attached to the source and destination
 * surfaces and the dwZBufferOpCode field in the DDBLTFX structure as the
 * z-buffer opcode.
 */
#define DDCOMP_ZBUFFER                          0x00800000l

/*
 * Z-buffered blt using the dwConstDest Zfield and the dwZBufferOpCode field
 * in the DDCOMPOSEFX structure as the z-buffer and z-buffer opcode respectively
 * for the destination.
 */
#define DDCOMP_ZBUFFERCONSTDESTOVERRIDE         0x01000000l

/*
 * Z-buffered blt using the lpDDSDestZBuffer field and the dwZBufferOpCode
 * field in the DDCOMPOSEFX structure as the z-buffer and z-buffer opcode
 * respectively for the destination.
 */
#define DDCOMP_ZBUFFERDESTOVERRIDE              0x02000000l

/*
 * Z-buffered blt using the dwConstSrcZ field and the dwZBufferOpCode field
 * in the DDCOMPOSEFX structure as the z-buffer and z-buffer opcode respectively
 * for the source.
 */
#define DDCOMP_ZBUFFERCONSTSRCOVERRIDE          0x04000000l

/*
 * Z-buffered blt using the lpDDSSrcZBuffer field and the dwZBufferOpCode
 * field in the DDCOMPOSEFX structure as the z-buffer and z-buffer opcode
 * respectively for the source.
 */
#define DDCOMP_ZBUFFERSRCOVERRIDE               0x08000000l

/****************************************************************************
 *
 * DIRECTDRAW SURFACE OVERLAY FLAGS
 *
 ****************************************************************************/

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the destination surface as the alpha channel for the
 * destination overlay.
 */
#define DDOVER_ALPHADEST                        0x00000001l

/*
 * Use the dwConstAlphaDest field in the DDOVERLAYFX structure as the
 * destination alpha channel for this overlay.
 */
#define DDOVER_ALPHADESTCONSTOVERRIDE           0x00000002l

/*
 * The NEG suffix indicates that the destination surface becomes more
 * transparent as the alpha value increases. 
 */
#define DDOVER_ALPHADESTNEG                     0x00000004l

/*
 * Use the lpDDSAlphaDest field in the DDOVERLAYFX structure as the alpha
 * channel destination for this overlay.
 */
#define DDOVER_ALPHADESTSURFACEOVERRIDE         0x00000008l

/*
 * Use the dwAlphaEdgeBlend field in the DDOVERLAYFX structure as the alpha
 * channel for the edges of the image that border the color key colors.
 */
#define DDOVER_ALPHAEDGEBLEND                   0x00000010l

/*
 * Use the alpha information in the pixel format or the alpha channel surface
 * attached to the source surface as the source alpha channel for this overlay.
 */
#define DDOVER_ALPHASRC                         0x00000020l

/*
 * Use the dwConstAlphaSrc field in the DDOVERLAYFX structure as the source
 * alpha channel for this overlay.
 */
#define DDOVER_ALPHASRCCONSTOVERRIDE            0x00000040l

/*
 * The NEG suffix indicates that the source surface becomes more transparent
 * as the alpha value increases.
 */
#define DDOVER_ALPHASRCNEG                      0x00000080l

/*
 * Use the lpDDSAlphaSrc field in the DDOVERLAYFX structure as the alpha channel
 * source for this overlay.
 */
#define DDOVER_ALPHASRCSURFACEOVERRIDE          0x00000100l

/*
 * Turn this overlay off.
 */
#define DDOVER_HIDE                             0x00000200l

/*
 * Use the color key associated with the destination surface.
 */
#define DDOVER_KEYDEST                          0x00000400l

/*
 * Use the dckDestColorkey field in the DDOVERLAYFX structure as the color key
 * for the destination surface
 */
#define DDOVER_KEYDESTOVERRIDE                  0x00000800l

/*
 * Use the color key associated with the source surface.
 */
#define DDOVER_KEYSRC                           0x00001000l

/*
 * Use the dckSrcColorkey field in the DDOVERLAYFX structure as the color key
 * for the source surface.
 */
#define DDOVER_KEYSRCOVERRIDE                   0x00002000l

/*
 * Turn this overlay on.
 */
#define DDOVER_SHOW                             0x00004000l

/*
 * Use the specified z order to determine the z order of the overlays being
 * displayed.
 */
#define DDOVER_ZORDER                           0x00008000l

/*
 * Use the dwZOrder field in the DDOVERLAYFX structure as the z order for the
 * display of this overlay.
 */
#define DDOVER_ZORDEROVERRIDE                   0x00010000l

/****************************************************************************
 *
 * DIRECTDRAWSURFACE LOCK FLAGS
 *
 ****************************************************************************/

/*
 * The default.  Set to indicate that Lock should return a valid memory pointer
 * to the top of the specified rectangle.  If no rectangle is specified then a
 * pointer to the top of the surface is returned.
 */
#define DDLOCK_SURFACEMEMORYPTR                 0x00000000l     // default

/*
 * Set if an event handle is being passed to Lock.  Lock will trigger the event
 * when it can return the surface memory pointer requested.
 */
#define DDLOCK_EVENT                            0x00000002l

/*
 * Set if a streaming lock is needed.  A streaming lock requires that a streaming
 * callback notification function has already been established.
 */
#define DDLOCK_STREAMING                        0x00000004l

/****************************************************************************
 *
 * DIRECTDRAWSURFACE BLT FX FLAGS
 *
 ****************************************************************************/

/*
 * If stretching, use arithmetic stretching along the Y axis for this blt.
 */
#define DDBLTFX_ARITHSTRETCHY                   0x00000001l

/*
 * Do this blt mirroring the surface left to right.  Spin the
 * surface around its y-axis.
 */
#define DDBLTFX_MIRRORLEFTRIGHT                 0x00000002l

/*
 * Do this blt mirroring the surface up and down.  Spin the surface
 * around its x-axis.
 */
#define DDBLTFX_MIRRORUPDOWN                    0x00000004l

/*
 * Schedule this blt to avoid tearing.
 */
#define DDBLTFX_NOTEARING                       0x00000008l

/*
 * Do this blt rotating the surface one hundred and eighty degrees.
 */
#define DDBLTFX_ROTATE180                       0x00000010l

/*
 * Do this blt rotating the surface two hundred and seventy degrees.
 */
#define DDBLTFX_ROTATE270                       0x00000020l

/*
 * Do this blt rotating the surface ninety degrees.
 */
#define DDBLTFX_ROTATE90                        0x00000040l

/*
 * Do this z blt using dwZBufferLow and dwZBufferHigh as  range values
 * specified to limit the bits copied from the source surface.
 */
#define DDBLTFX_ZBUFFERRANGE                    0x00000080l

/*
 * Do this z blt adding the dwZBufferBaseDest to each of the sources z values
 * before comparing it with the desting z values.
 */
#define DDBLTFX_ZBUFFERBASEDEST                 0x00000100l

/****************************************************************************
 *
 * DIRECTDRAWSURFACE COMP FX FLAGS
 *
 ****************************************************************************/

/*
 * If stretching, use arithmetic stretching along the Y axis for this blt.
 */
#define DDCOMPFX_ARITHSTRETCHY                  0x00000001l

/*
 * Do this blt mirroring the surface left to right.  Spin the
 * surface around its y-axis.
 */
#define DDCOMPFX_MIRRORLEFTRIGHT                0x00000002l

/*
 * Do this blt mirroring the surface up and down.  Spin the surface
 * around its x-axis.
 */
#define DDCOMPFX_MIRRORUPDOWN                   0x00000004l

/*
 * Schedule this blt to avoid tearing.
 */
#define DDCOMPFX_NOTEARING                      0x00000008l

/*
 * Do this blt rotating the surface one hundred and eighty degrees.
 */
#define DDCOMPFX_ROTATE180                      0x00000010l

/*
 * Do this blt rotating the surface two hundred and seventy degrees.
 */
#define DDCOMPFX_ROTATE270                      0x00000020l

/*
 * Do this blt rotating the surface ninety degrees.
 */
#define DDCOMPFX_ROTATE90                       0x00000040l

/*
 * Do this z blt using dwZBufferLow and dwZBufferHigh as  range values
 * specified to limit the bits copied from the source surface.
 */
#define DDCOMPFX_ZBUFFERRANGE                   0x00000080l

/*
 * Do this z blt adding the dwZBufferBaseDest to each of the sources z values
 * before comparing it with the desting z values.
 */
#define DDCOMPFX_ZBUFFERBASEDEST                0x00000100l

/****************************************************************************
 *
 * DIRECTDRAWSURFACE OVERLAY FX FLAGS
 *
 ****************************************************************************/

/*
 * If stretching, use arithmetic stretching along the Y axis for this overlay.
 */
#define DDOVERFX_ARITHSTRETCHY                  0x00000001l

/****************************************************************************
 *
 * DIRECTDRAW WAITFORVERTICALBLANK FLAGS
 *
 ****************************************************************************/

/*
 * return when the vertical blank interval begins
 */
#define DDWAITVB_BLOCKBEGIN                     0x00000001l

/*
 * set up an event to trigger when the vertical blank begins
 */
#define DDWAITVB_BLOCKBEGINEVENT                0x00000002l

/*
 * return when the vertical blank interval ends and display begins
 */
#define DDWAITVB_BLOCKEND                       0x00000004l

/****************************************************************************
 *
 * DIRECTDRAW GETFLIPSTATUS FLAGS
 *
 ****************************************************************************/

/*
 * is it OK to flip now?
 */
#define DDGFS_CANFLIP                   0x00000000l

/*
 * is the last flip finished?
 */
#define DDGFS_ISFLIPDONE                0x00000001l

/****************************************************************************
 *
 * DIRECTDRAW GETBLTSTATUS FLAGS
 *
 ****************************************************************************/

/*
 * is it OK to blt now?
 */
#define DDGBS_CANBLT                    0x00000000l

/*
 * is the blt to the surface finished?
 */
#define DDGBS_ISBLTDONE                 0x00000001l

/*===========================================================================
 *
 *
 * DIRECTDRAW RETURN CODES
 *
 * The return values from DirectDraw Commands and Surface that return an HRESULT
 * are codes from DirectDraw concerning the results of the action
 * requested by DirectDraw.
 *
 *==========================================================================*/

/*
 * Status is OK
 *
 * Issued by: DirectDraw Commands and all callbacks
 */
#define DD_OK                                   0

/****************************************************************************
 *
 * DIRECTDRAW ENUMCALLBACK RETURN VALUES
 *
 * EnumCallback returns are used to control the flow of the DIRECTDRAW and
 * DIRECTDRAWSURFACE object enumerations.   They can only be returned by
 * enumeration callback routines.
 *
 ****************************************************************************/

/*
 * stop the enumeration
 */
#define DDENUMRET_CANCEL                        0

/*
 * continue the enumeration
 */
#define DDENUMRET_OK                            1

/****************************************************************************
 *
 * DIRECTDRAW ERRORS
 *
 * Errors are represented by negative values and cannot be combined.
 *
 ****************************************************************************/

/*
 * This surface can not be attached to the requested surface.
 */
#define DDERR_CANNOTATTACHSURFACE               MAKE_DDHRESULT( 10 )

/*
 * Support is currently not available.
 */
#define DDERR_CURRENTLYNOTAVAIL                 MAKE_DDHRESULT( 40 )

/*
 * No dest color key specified for this operation.
 */
#define DDERR_DESTCLRKEYNOTSET                  MAKE_DDHRESULT( 50 )

/*
 * Returned when the exclusive mode is being released by an application that
 * doesn't have it.
 */
#define DDERR_EXCLUSIVEMODENOTOWNED             MAKE_DDHRESULT( 60 )

/*
 * Returned when the exclusive mode is being requested and another
 * application already has taken it.
 */
#define DDERR_EXCLUSIVEMODEUNAVAILABLE          MAKE_DDHRESULT( 70 )

/*
 * Generic failure.
 */
#define DDERR_GENERIC                           MAKE_DDHRESULT( 80 )

/*
 * Height of rectangle provided is not a DWORD multiple.
 */
#define DDERR_HEIGHTALIGN                       MAKE_DDHRESULT( 90 )

/*
 * One or more of the caps bits passed to the callback are incorrect.
 */
#define DDERR_INVALIDCAPS                       MAKE_DDHRESULT( 100 )

/*
 * DirectDraw does not support provided Cliplist.
 */
#define DDERR_INVALIDCLIPLIST                   MAKE_DDHRESULT( 110 )

/*
 * DirectDraw received a pointer that was an invalid DIRECTDRAW object.
 */
#define DDERR_INVALIDDRIVER                     MAKE_DDHRESULT( 120 )

/*
 * DirectDraw does not support the requested mode
 */
#define DDERR_INVALIDMODE                       MAKE_DDHRESULT( 125 )

/*
 * DirectDraw received a pointer that was an invalid DIRECTDRAWPALETTE object.
 */
#define DDERR_INVALIDPALETTE                    MAKE_DDHRESULT( 130 )

/*
 * One or more of the parameters passed to the callback function are
 * incorrect.
 */
#define DDERR_INVALIDPARAMS                     MAKE_DDHRESULT( 140 )

/*
 * pixel format was invalid as specified
 */
#define DDERR_INVALIDPIXELFORMAT                MAKE_DDHRESULT( 145 )

/*
 * Rectangle provided was invalid.
 */
#define DDERR_INVALIDRECT                       MAKE_DDHRESULT( 150 )

/*
 * DirectDraw received a pointer that was an invalid DIRECTDRAWSURFACE object.
 */
#define DDERR_INVALIDSURFACE                    MAKE_DDHRESULT( 160 )

/*
 * There is no 3D present.
 */
#define DDERR_NO3D                              MAKE_DDHRESULT( 170 )

/*
 * Operation could not be carried out because there is no alpha accleration
 * hardware present or available.
 */
#define DDERR_NOALPHAHW                         MAKE_DDHRESULT( 180 )

/*
 * Operation could not be carried out because there is no hardware support
 * for synchronizing blts to avoid tearing.
 */
#define DDERR_NOANTITEARHW                      MAKE_DDHRESULT( 190 )

/*
 * Operation could not be carried out because there is no hardware support
 * for asynchronous blting.
 */
#define DDERR_NOASYNCHW                         MAKE_DDHRESULT( 200 )

/*
 * no clip list available
 */
#define DDERR_NOCLIPLIST                        MAKE_DDHRESULT( 205 )

/*
 * Operation could not be carried out because there is no color conversion
 * hardware present or available.
 */
#define DDERR_NOCOLORCONVHW                     MAKE_DDHRESULT( 210 )

/*
 * Surface doesn't currently have a color key
 */
#define DDERR_NOCLRKEY                          MAKE_DDHRESULT( 215 )

/*
 * Operation could not be carried out because there is no hardware support
 * of the dest color key.
 */
#define DDERR_NODESTCLRKEYHW                    MAKE_DDHRESULT( 220 )

/*
 * process does not have exclusive mode
 */
#define DDERR_NOEXCLUSIVEMODE                   MAKE_DDHRESULT( 225 )

/*
 * Flipping visible surfaces is not supported.
 */
#define DDERR_NOFLIPHW                          MAKE_DDHRESULT( 230 )

/*
 * There is no GDI present.
 */
#define DDERR_NOGDI                             MAKE_DDHRESULT( 240 )

/*
 * Operation could not be carried out because there is no hardware present
 * or available.
 */
#define DDERR_NOMIRRORHW                        MAKE_DDHRESULT( 250 )

/*
 * Requested item was not found
 */
#define DDERR_NOTFOUND                          MAKE_DDHRESULT( 255 )

/*
 * Operation could not be carried out because there is no overlay hardware
 * present or available.
 */
#define DDERR_NOOVERLAYHW                       MAKE_DDHRESULT( 260 )

/*
 * Operation could not be carried out because there is no appropriate raster
 * op hardware present or available.
 */
#define DDERR_NORASTEROPHW                      MAKE_DDHRESULT( 280 )

/*
 * Operation could not be carried out because there is no rotation hardware
 * present or available.
 */
#define DDERR_NOROTATEHW                        MAKE_DDHRESULT( 290 )

/*
 * Operation could not be carried out because there is no hardware support
 * for the src color key.
 */
#define DDERR_NOSRCCLRKEYHW                     MAKE_DDHRESULT( 300 )

/*
 * Operation could not be carried out because there is no hardware support
 * for stretching
 */
#define DDERR_NOSTRETCHHW                       MAKE_DDHRESULT( 310 )

/*
 * DirectDrawSurface is not in 4 bit color palette and the requested operation
 * requires 4 bit color palette.
 */
#define DDERR_NOT4BITCOLOR                      MAKE_DDHRESULT( 316 )

/*
 * DirectDrawSurface is not in 4 bit color index palette and the requested
 * operation requires 4 bit color index palette.
 */
#define DDERR_NOT4BITCOLORINDEX                 MAKE_DDHRESULT( 317 )

/*
 * DirectDraw Surface is not in 8 bit color mode and the requested operation
 * requires 8 bit color.
 */
#define DDERR_NOT8BITCOLOR                      MAKE_DDHRESULT( 320 )

/*
 * Operation could not be carried out because there is no texture mapping
 * hardware present or available.
 */
#define DDERR_NOTEXTUREHW                       MAKE_DDHRESULT( 330 )

/*
 * Operation could not be carried out because there is no hardware support
 * for vertical blank synchronized operations.
 */
#define DDERR_NOVSYNCHW                         MAKE_DDHRESULT( 335 )

/*
 * Operation could not be carried out because there is no hardware support
 * for zbuffer blting.
 */
#define DDERR_NOZBUFFERHW                       MAKE_DDHRESULT( 340 )

/*
 * Overlay surfaces could not be z layered based on their BltOrder because
 * the hardware does not support z layering of overlays.
 */
#define DDERR_NOZHW                             MAKE_DDHRESULT( 350 )

/*
 * The hardware needed for the requested operation has already been
 * allocated.
 */
#define DDERR_OUTOFCAPS                         MAKE_DDHRESULT( 360 )

/*
 * DirectDraw does not have enough memory to perform the operation.
 */
#define DDERR_OUTOFMEMORY                       MAKE_DDHRESULT( 370 )

/*
 * DirectDraw does not have enough memory to perform the operation.
 */
#define DDERR_OUTOFVIDEOMEMORY                  MAKE_DDHRESULT( 380 )

/*
 * Access to this palette is being refused because the palette is already
 * locked by another thread.
 */
#define DDERR_OVERLAYCANTCLIP                   MAKE_DDHRESULT( 382 )

/*
 * Access to this palette is being refused because the palette is already
 * locked by another thread.
 */
#define DDERR_OVERLAYCLRKEYONLYONEACTIVE        MAKE_DDHRESULT( 384 )

/*
 * Access to this palette is being refused because the palette is already
 * locked by another thread.
 */
#define DDERR_PALETTEBUSY                       MAKE_DDHRESULT( 387 )

/*
 * The primary surface that is being created does not match the primary
 * surface that already exists.  The mode may need to be changed or another
 * application may have created primary surface that has additional
 * capabilities that have not been requested by this call to CreateSurface.
 */
#define DDERR_PRIMARYDOESNOTMATCH               MAKE_DDHRESULT( 390 )

/*
 * No src color key specified for this operation.
 */
#define DDERR_SRCCLRKEYNOTSET                   MAKE_DDHRESULT( 400 )

/*
 * This surface is already attached to the surface it is being attached to.
 */
#define DDERR_SURFACEALREADYATTACHED            MAKE_DDHRESULT( 410 )

/*
 * This surface is already a dependency of the surface it is being made a
 * dependency of.
 */
#define DDERR_SURFACEALREADYDEPENDENT           MAKE_DDHRESULT( 420 )

/*
 * Access to this surface is being refused because the surface is already
 * locked by another thread.
 */
#define DDERR_SURFACEBUSY                       MAKE_DDHRESULT( 430 )

/*
 * Access to Surface refused because Surface is obscured.
 */
#define DDERR_SURFACEISOBSCURED                 MAKE_DDHRESULT( 440 )

/*
 * Access to this surface is being refused because the surface is gone.
 * The DIRECTDRAWSURFACE object representing this surface should be Released and
 * a new one should be created.  This can only be returned for 
 * DDSCAPS_PRIMARYSURFACE objects that were not created for exclusive access.
 */
#define DDERR_SURFACELOST                       MAKE_DDHRESULT( 450 )

/*
 * The requested surface is not attached.
 */
#define DDERR_SURFACENOTATTACHED                MAKE_DDHRESULT( 460 )

/*
 * Height requested by DirectDraw is too large.
 */
#define DDERR_TOOBIGHEIGHT                      MAKE_DDHRESULT( 470 )

/*
 * Size requested by DirectDraw is too large --  The individual height and
 * width are OK.
 */
#define DDERR_TOOBIGSIZE                        MAKE_DDHRESULT( 480 )

/*
 * Width requested by DirectDraw is too large.
 */
#define DDERR_TOOBIGWIDTH                       MAKE_DDHRESULT( 490 )

/*
 * Action not supported.
 */
#define DDERR_UNSUPPORTED                       MAKE_DDHRESULT( 500 )

/*
 * FOURCC format requested is unsupported by DirectDraw
 */
#define DDERR_UNSUPPORTEDFORMAT                 MAKE_DDHRESULT( 510 )

/*
 * Bitmask in the pixel format requested is unsupported by DirectDraw
 */
#define DDERR_UNSUPPORTEDMASK                   MAKE_DDHRESULT( 520 )

/*
 * Version provided is not supported by DirectDraw
 */
#define DDERR_UNSUPPORTEDVERSION                MAKE_DDHRESULT( 530 )

/*
 * vertical blank is in progress
 */
#define DDERR_VERTICALBLANKINPROGRESS           MAKE_DDHRESULT( 537 )

/*
 * Informs DirectDraw that the previous Blt which is transfering information
 * to or from this Surface is incomplete.
 */
#define DDERR_WASSTILLDRAWING                   MAKE_DDHRESULT( 540 )

/*
 * Width of rectangle provided is not a DWORD multiple.
 */
#define DDERR_WIDTHALIGN                        MAKE_DDHRESULT( 550 )

/*
 * Rectangle provided was not horizontally aligned on a DWORD boundary.
 */
#define DDERR_XALIGN                            MAKE_DDHRESULT( 560 )

/*
 * Rectangle provided was not on a DWORD boundary in both the horizontal and
 * vertical directions.
 */
#define DDERR_XYALIGN                           MAKE_DDHRESULT( 570 )

/*
 * Rectangle provided was not vertically aligned on a DWORD boundary.
 */
#define DDERR_YALIGN                            MAKE_DDHRESULT( 580 )

/* Alpha bit depth constants */

/*
 * 1 bit alpha constants
 */

/*
 * Completely transparent. Colors being blended with this constant do not effect
 * the final result.
 */
#define DDALPHA1_TRANSPARENT                    0

/*
 * Completely opaque. Colors being blended with this constant are the final result.
 */
#define DDALPHA1_OPAQUE                         1

/*
 * 2 bit alpha constants
 */

/*
 * Completely transparent. Colors being blended with this constant do not effect
 * the final result.
 */
#define DDALPHA2_TRANSPARENT                    0

/*
 * 50-50 blend.  Colors being blended with this constant effect the final result
 * with 50% of their color.
 */
#define DDALPHA2_50                             2

/*
 * Completely opaque. Colors being blended with this constant are the final result.
 */
#define DDALPHA2_OPAQUE                         3

/*
 * 4 bit alpha constants
 */

/*
 * Completely transparent. Colors being blended with this constant do not effect
 * the final result.
 */
#define DDALPHA4_TRANSPARENT                    0

/*
 * Opaque modifier is multiplied by any value between 0 and _OPAQUE to
 * establish gradients of alpha blending.
 */
#define DDALPHA4_OPAQUEMOD                      1

/*
 * 50-50 blend.  Colors being blended with this constant effect the final result
 * with 50% of their color.
 */
#define DDALPHA4_50                             8

/*
 * Completely opaque. Colors being blended with this constant are the final result.
 */
#define DDALPHA4_OPAQUE                         15

/*
 * 8 bit alpha constants
 */

/*
 * Completely transparent. Colors being blended with this constant do not effect
 * the final result.
 */
#define DDALPHA8_TRANSPARENT                    0

/*
 * Opaque modifier is multiplied by any value between 0 and _OPAQUE to
 * establish gradients of alpha blending.
 */
#define DDALPHA8_OPAQUEMOD                      1

/*
 * 50-50 blend.  Colors being blended with this constant effect the final result
 * with 50% of their color.
 */
#define DDALPHA8_50                             128
/*
 * Completely opaque. Colors being blended with this constant are the final result.
 */
#define DDALPHA8_OPAQUE                         255

#ifdef __cplusplus
};
#endif

#endif
