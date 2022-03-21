/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *  Copyright (C) 1994-1995 ATI Technologies Inc. All Rights Reserved.
 *
 *  File:       gfx.c
 *  Content:    graphics API
 *
 ***************************************************************************/
#include "foxbear.h"

#define BMP_STRIDE      4

/*
 * gfxRectXfer_HostToVram
 */
static BOOL gfxRectXfer_HostToVram(
                GFX_HBM hbmDest,
                GFX_RECT *prcDest,
                GFX_HBM hbmSrc,
                GFX_POINT *pptSrc )
{
    GFX_BITMAP_VRAM     *pvramDest;
    GFX_BITMAP_HOST     *phostSrc;
    DWORD               dwWidth;
    DWORD               dwHeight;
    BYTE                *pjBits;
    LONG                lDelta;
    LONG                xDst;
    LONG                xSrc;
    LONG                align;
    BYTE                *pTarget;
    DDSURFACEDESC       ddsd;
    HRESULT             ddrval;
    DWORD               src_stride;

    pvramDest = (GFX_BITMAP_VRAM *) hbmDest;
    phostSrc  = (GFX_BITMAP_HOST *) hbmSrc;


    dwWidth  = prcDest->dwRight - prcDest->dwLeft;
    dwHeight = prcDest->dwBottom - prcDest->dwTop;

    /*
     * Calc dwX, dwY, new dwWidth, new dwHeight here
     */
    align = pptSrc->dwX % BMP_STRIDE;
    xDst = prcDest->dwLeft - align;
    xSrc = pptSrc->dwX - align;
    dwWidth += align;

    align = dwWidth % BMP_STRIDE;
    if( align != 0 )
    {
        dwWidth += BMP_STRIDE - align;
    }
    lDelta = phostSrc->dwDelta * BMP_STRIDE;
    pjBits = (BYTE *) phostSrc->pdwBits + pptSrc->dwY * dwWidth + xSrc;

    ddsd.dwSize = sizeof(ddsd);
    ddrval = pvramDest->lpSurface->lpVtbl->Lock(
                pvramDest->lpSurface,
                NULL,
                &ddsd,
                0,
                NULL );

    if( ddrval != DD_OK )
    {
        return FALSE;
    }

    pTarget = ddsd.lpSurface;
    src_stride = (DWORD) ddsd.lPitch;

    /*
     * transfer the bits
     */
    while( dwHeight-- )
    {
        MoveMemory( pTarget, pjBits, dwWidth );
        pTarget += src_stride;
        pjBits += lDelta;
    }

    pvramDest->lpSurface->lpVtbl->Unlock(
                pvramDest->lpSurface,
                NULL );
    return TRUE;

} /* gfxRectXfer_HostToVram */

/*
 * gfxRectXfer_VramToVram 
 */
static BOOL gfxRectXfer_VramToVram(
                GFX_HBM hbmDest,
                GFX_RECT *prcDest,
                GFX_HBM hbmSrc,
                GFX_POINT *pptSrc )
{
    GFX_BITMAP_VRAM     *pvramDest;
    GFX_BITMAP_VRAM     *pvramSrc;
    RECT                rcRect;

    pvramDest = (GFX_BITMAP_VRAM *) hbmDest;
    pvramSrc  = (GFX_BITMAP_VRAM *) hbmSrc;

    rcRect.left = pptSrc->dwX;
    rcRect.top = pptSrc->dwY;
    rcRect.right = rcRect.left+prcDest->dwRight - prcDest->dwLeft;
    rcRect.bottom = rcRect.top+prcDest->dwBottom - prcDest->dwTop;
    
    lpBackBuffer->lpVtbl->BltFast(
                    lpBackBuffer,
                    prcDest->dwLeft,
                    prcDest->dwTop,
                    pvramSrc->lpSurface,
                    &rcRect,
                    FALSE );

    return TRUE;

} /* gfxRectXfer_VramToVram */

/*
 * gfxRectXfer
 *
 * transfer a rectangle from one bitmap to another
 */
BOOL gfxRectXfer(
                GFX_HBM hbmDest,
                GFX_RECT *prcDest,
                GFX_HBM hbmSrc,
                GFX_POINT *pptSrc,
                GFX_MIX mix )
{
    DWORD       dwTypeDest;
    DWORD       dwTypeSrc;
    BOOL        (*pfn)( GFX_HBM, GFX_RECT *, GFX_HBM, GFX_POINT * );

    dwTypeDest = *((DWORD *) hbmDest);
    dwTypeSrc  = *((DWORD *) hbmSrc);

    switch( dwTypeDest ) {
    case GFX_BITMAP_TYPE_PDEV:
    case GFX_BITMAP_TYPE_VRAM:
        switch( dwTypeSrc ) {
        case GFX_BITMAP_TYPE_HOST:
            pfn = gfxRectXfer_HostToVram;
            break;
        case GFX_BITMAP_TYPE_PDEV:
        case GFX_BITMAP_TYPE_VRAM:
            pfn = gfxRectXfer_VramToVram;
            break;
        default:
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }

    if( !(*pfn)( hbmDest, prcDest, hbmSrc, pptSrc ) )
    {
        return FALSE;
    }

    return TRUE;

} /* gfxRectXfer */

/*
 * gfxMaskXfer
 *
 * Mask transfer.  Right now, does just transparency.
 */
BOOL gfxMaskXfer(
            GFX_HBM hbmDest,
            GFX_RECT *prcDest,
            GFX_HBM hbmSrc,
            GFX_POINT *pptSrc,
            GFX_HBM hbmMask,
            GFX_POINT *pptMask,
            GFX_MIX mix0,
            GFX_MIX mix1 )
{
    GFX_BITMAP_VRAM     *pDst;
    GFX_BITMAP_VRAM     *pSrc;
    RECT                rcRect;


    pDst = (GFX_BITMAP_VRAM *) hbmDest;
    pSrc = (GFX_BITMAP_VRAM *) hbmSrc;

    rcRect.left = pptSrc->dwX;
    rcRect.top = pptSrc->dwY;
    rcRect.right = rcRect.left+(prcDest->dwRight-prcDest->dwLeft);
    rcRect.bottom = rcRect.top+(prcDest->dwBottom-prcDest->dwTop);

    lpBackBuffer->lpVtbl->BltFast(
                lpBackBuffer,
                prcDest->dwLeft,
                prcDest->dwTop,
                pSrc->lpSurface,
                &rcRect,
                TRUE );

    return TRUE;

} /* gfxMaskXfer */

/*
 * gfxCreateVramBitmap
 */
GFX_HBM gfxCreateVramBitmap( GFX_SIZE *psize, GFX_BMF bmf )
{
    GFX_BITMAP_VRAM     *pvram;

    pvram = MemAlloc( sizeof( *pvram ) );
    if( pvram == NULL )
    {
        return NULL;
    }
 
    pvram->dwType  = GFX_BITMAP_TYPE_VRAM;
    pvram->dwFlags = 0;

    pvram->size = *psize;
    pvram->bmf  = bmf;

    pvram->lpSurface = DDCreateSurface( psize->dwWidth, psize->dwHeight );
    if( pvram->lpSurface == NULL )
    {
        return NULL;
    }
        
    return (GFX_HBM) pvram;

} /* gfxCreateVramBitmap */

/*
 * gfxDestroyBitmap
 */
BOOL gfxDestroyBitmap ( GFX_HBM hbm )
{
    DWORD       dwType;
    
    if( hbm == NULL )
    {
        return FALSE;
    }
    
    dwType = *((DWORD *) hbm);
    
    switch( dwType )
    {
    case GFX_BITMAP_TYPE_HOST:
        MemFree( ((GFX_BITMAP_HOST *) hbm)->pdwBits );
    case GFX_BITMAP_TYPE_VRAM:
        break;
    case GFX_BITMAP_TYPE_NONE:
    case GFX_BITMAP_TYPE_PDEV:
    default:
        return FALSE;
    }
    
    *((DWORD *) hbm) = GFX_BITMAP_TYPE_NONE;
    MemFree( (VOID *) hbm );
    
    return TRUE;
    
} /* gfxDestroyBitmap */

/*
 * gfxSwapBuffers
 */
BOOL gfxSwapBuffers( GFX_HBM hbm )
{
    HRESULT     ddrval;
    extern BOOL bDoubleBuffer;

    do
    {
        ddrval = lpFrontBuffer->lpVtbl->Flip( lpFrontBuffer, NULL );
    } while( ddrval == DDERR_WASSTILLDRAWING );

    if( ddrval != DD_OK )
    {
        Msg( "Flip FAILED, rc=%08lx", ddrval );
        return FALSE;
    }
    /*
     * wait for flip to finish - do real game stuff here  rather than
     * just burn CPU, but hey this is just a demo
     */
    if( bDoubleBuffer )
    {
        while( lpBackBuffer->lpVtbl->GetFlipStatus( lpBackBuffer,
                                DDGFS_ISFLIPDONE ) == DDERR_WASSTILLDRAWING );
    }

    return TRUE;

} /* gfxSwapBuffers */

/*
 * gfxBegin
 */
GFX_HBM gfxBegin( void )
{
    GFX_BITMAP_PDEV *ppdev;

    if( !DDEnable() )
    {
        return NULL;
    }
    if( !DDCreateFlippingSurface() ) {
        lpDD->lpVtbl->Release( lpDD );
        lpDD = NULL;
        return NULL;
    }
    Splash();

    ppdev = MemAlloc( sizeof( *ppdev ) );
    if( ppdev == NULL )
    {
        lpDD->lpVtbl->Release( lpDD );
        return NULL;
    }

    ppdev->dwType  = GFX_BITMAP_TYPE_PDEV;
    ppdev->dwFlags = 0;

    ppdev->size.dwWidth  = C_SCREEN_W;
    ppdev->size.dwHeight = C_SCREEN_H;

    ppdev->bmf = GFX_BMF_8BPP;

    return (GFX_HBM) ppdev;

} /* gfxBegin */

/*
 * gfxEnd
 */
BOOL gfxEnd ( GFX_HBM hbm )
{
    MemFree( (LPVOID) hbm );
    if( lpDD != NULL )
    {
        lpDD->lpVtbl->Release( lpDD );
        lpDD = NULL;
    }
    return TRUE;

} /* gfxEnd */
