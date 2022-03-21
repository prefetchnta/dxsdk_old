/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *  Copyright (C) 1994-1995 ATI Technologies Inc. All Rights Reserved.
 *
 *  File:       gfx.h
 *  Content:    graphics routines include file
 *
 ***************************************************************************/
#ifndef __GFX_INCLUDED__
#define __GFX_INCLUDED__

#define GFX_MAX_WIDTH  2048
#define GFX_MAX_HEIGHT 2048

///////////////
//           //
//  GFX_BMF  //
//           //
///////////////

typedef enum enum_GFX_BMF
{
    GFX_BMF_DEVICE,
    GFX_BMF_1BPP,
    GFX_BMF_4BPP,
    GFX_BMF_8BPP,
    GFX_BMF_16BPP,
    GFX_BMF_24BPP,
    GFX_BMF_32BPP,
    GFX_BMF_4RLE,
    GFX_BMF_8RLE,
    GFX_BMF_COUNT
}
GFX_BMF;


/////////////////
//             //
//  GFX_COLOR  //
//             //
/////////////////

typedef DWORD GFX_COLOR;


///////////////
//           //
//  GFX_HBM  //
//           //
///////////////

typedef VOID *GFX_HBM;
typedef GFX_HBM *LPGFX_HBM;


///////////////
//           //
//  GFX_MIX  //
//           //
///////////////

typedef enum enum_GFX_MIX
{
    GFX_MIX_0,     // 0x0: ROP2_BLACK
    GFX_MIX_DSon,  // 0x1: ROP2_NOTMERGEPEN
    GFX_MIX_DSna,  // 0x2: ROP2_MASKNOTPEN
    GFX_MIX_Sn,    // 0x3: ROP2_NOTCOPYPEN
    GFX_MIX_SDna,  // 0x4: ROP2_MASKPENNOT
    GFX_MIX_Dn,    // 0x5: ROP2_NOT
    GFX_MIX_DSx,   // 0x6: ROP2_XORPEN
    GFX_MIX_DSan,  // 0x7: ROP2_NOTMASKPEN
    GFX_MIX_DSa,   // 0x8: ROP2_MASKPEN
    GFX_MIX_DSxn,  // 0x9: ROP2_NOTXORPEN
    GFX_MIX_D,     // 0xA: ROP2_NOP
    GFX_MIX_DSno,  // 0xB: ROP2_MERGENOTPEN
    GFX_MIX_S,     // 0xC: ROP2_COPYPEN
    GFX_MIX_SDno,  // 0xD: ROP2_MERGEPENNOT
    GFX_MIX_DSo,   // 0xE: ROP2_MERGEPEN
    GFX_MIX_1,     // 0xF: ROP2_WHITE
    GFX_MIX_COUNT
}
GFX_MIX;


/////////////////
//             //
//  GFX_POINT  //
//             //
/////////////////

typedef struct struct_GFX_POINT
{
    DWORD dwX;
    DWORD dwY;
}
GFX_POINT;


////////////////
//            //
//  GFX_RECT  //
//            //
////////////////

typedef struct struct_GFX_RECT
{
    DWORD dwTop;
    DWORD dwLeft;
    DWORD dwRight;
    DWORD dwBottom;
}
GFX_RECT;


////////////////
//            //
//  GFX_SIZE  //
//            //
////////////////

typedef struct struct_GFX_SIZE
{
    DWORD dwWidth;
    DWORD dwHeight;
}
GFX_SIZE;


///////////////////////
//                   //
//  GFX_BITMAP_TYPE  //
//                   //
///////////////////////

typedef enum enum_GFX_BITMAP_TYPE
{
    GFX_BITMAP_TYPE_NONE,
    GFX_BITMAP_TYPE_HOST,
    GFX_BITMAP_TYPE_PDEV,
    GFX_BITMAP_TYPE_VRAM
}
GFX_BITMAP_TYPE;


///////////////////////
//                   //
//  GFX_BITMAP_HOST  //
//                   //
///////////////////////

typedef struct struct_GFX_BITMAP_HOST
{
    DWORD dwType;
    DWORD dwFlags;

    GFX_SIZE size;
    GFX_BMF  bmf;

    DWORD *pdwBits;
    DWORD  dwDelta;
}
GFX_BITMAP_HOST;


///////////////////////
//                   //
//  GFX_BITMAP_PDEV  //
//                   //
///////////////////////

typedef struct struct_GFX_BITMAP_PDEV
{
    DWORD dwType;
    DWORD dwFlags;

    GFX_SIZE size;
    GFX_BMF  bmf;

    DWORD dwPixWidth;
    DWORD dwOffPitch;

    DWORD dwOffPitch0;
    DWORD dwOffPitch1;
}
GFX_BITMAP_PDEV;


///////////////////////
//                   //
//  GFX_BITMAP_VRAM  //
//                   //
///////////////////////

typedef struct struct_GFX_BITMAP_VRAM
{
    DWORD dwType;
    DWORD dwFlags;

    GFX_SIZE size;
    GFX_BMF  bmf;

    LPDIRECTDRAWSURFACE lpSurface;
}
GFX_BITMAP_VRAM;


//
// Prototypes
//

GFX_HBM gfxLoadBitmap(CHAR*);
BOOL gfxRectXfer(GFX_HBM,GFX_RECT*,GFX_HBM,GFX_POINT*,GFX_MIX);
BOOL gfxMaskXfer(GFX_HBM,GFX_RECT*,GFX_HBM,GFX_POINT*,GFX_HBM,GFX_POINT*,GFX_MIX,GFX_MIX);
GFX_HBM gfxCreateHostBitmap(GFX_SIZE*,GFX_BMF);
GFX_HBM gfxCreateVramBitmap(GFX_SIZE*,GFX_BMF);
BOOL gfxDestroyBitmap(GFX_HBM);
BOOL gfxSwapBuffers(GFX_HBM);
GFX_HBM gfxBegin(VOID);
BOOL gfxEnd(GFX_HBM);


#endif
