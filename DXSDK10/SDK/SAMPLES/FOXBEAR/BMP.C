/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *  Copyright (C) 1994-1995 ATI Technologies Inc. All Rights Reserved.
 *
 *  File:       bmp.c
 *  Content:    Bitmap reader
 *
 ***************************************************************************/
#include "foxbear.h"

/*
 * read1BPP
 */
static GFX_BITMAP_HOST *read1BPP(
                HFASTFILE pfile,
                LPBITMAPFILEHEADER pbf,
                LPBITMAPINFOHEADER pbi )
{
    LONG                lWidth;
    LONG                lHeight;
    LONG                lStart;
    LONG                lStop;
    LONG                lInc;
    LONG                x;
    LONG                y;
    DWORD               *pdwData;
    GFX_BITMAP_HOST     *phost;

    lWidth = ((pbi->biWidth + 31) & ~0x1F) / 32;
    lHeight = pbi->biHeight;

    if( lHeight < 0 )
    {
        lHeight = -lHeight;

        lStart = 0;
        lStop = lHeight;
        lInc = 1;
    }
    else
    {
        lStart = lHeight - 1;
        lStop = -1;
        lInc = -1;
    }

    if( !FastFileSeek( pfile, pbf->bfOffBits, SEEK_SET ) )
    {
        return NULL;
    }

    pdwData = MemAlloc( 4 * lWidth * lHeight );
    if( pdwData == NULL )
    {
        return NULL;
    }

    for( y = lStart; y != lStop; y += lInc ) {
        for( x = 0; x < lWidth; ++x ) {
            DWORD dwData;

            if( !FastFileRead( pfile, &dwData, sizeof dwData ) )
            {
                MemFree( pdwData );
                return NULL;
            }
            *(pdwData + y * lWidth + x) = dwData;
        }
    }

    phost = MemAlloc( sizeof *phost );
    if( phost == NULL )
    {
        MemFree( pdwData );
        return NULL;
    }

    phost->dwType = GFX_BITMAP_TYPE_HOST;
    phost->dwFlags = 0;

    phost->size.dwWidth = pbi->biWidth;
    phost->size.dwHeight = labs( pbi->biHeight );

    phost->bmf = GFX_BMF_1BPP;

    phost->pdwBits = pdwData;
    phost->dwDelta = lWidth;

    return phost;

} /* read1BPP */

/*
 * read8BPP
 */
static GFX_BITMAP_HOST *read8BPP(
                HFASTFILE pfile,
                LPBITMAPFILEHEADER pbf,
                LPBITMAPINFOHEADER pbi )
{
    RGBQUAD             lut[256];
    LONG                lWidth;
    LONG                lHeight;
    LONG                lStart;
    LONG                lStop;
    LONG                lInc;
    LONG                x;
    LONG                y;
    BYTE                *pjData;
    GFX_BITMAP_HOST     *phost;

    if( !FastFileSeek( pfile, sizeof (BITMAPFILEHEADER) +
                sizeof (BITMAPINFOHEADER), SEEK_SET ) )
    {
        return NULL;
    }

    if( !FastFileRead( pfile, lut, sizeof lut ) )
    {
        return NULL;
    }

    lWidth = (pbi->biWidth + 3) & ~0x3;
    lHeight = pbi->biHeight;

    if( lHeight < 0 )
    {
        lHeight = -lHeight;
        lStart = 0;
        lStop = lHeight;
        lInc = 1;
    }
    else
    {
        lStart = lHeight - 1;
        lStop = -1;
        lInc = -1;
    }

    if( !FastFileSeek( pfile, pbf->bfOffBits, SEEK_SET ) )
    {
        return NULL;
    }

    pjData = MemAlloc( lWidth * lHeight );
    if( pjData == NULL )
    {
        return NULL;
    }

    for( y = lStart; y != lStop; y += lInc )
    {
        for( x = 0; x < lWidth; ++x )
        {
            BYTE index;

            if( !FastFileRead( pfile, &index, sizeof index ) )
            {
                MemFree( pjData );
                return NULL;
            }

            /*
             * hack to make sure fox off-white doesn't become
             * pure white (which is transparent)
             */
            if( lut[index].rgbRed == 0xff &&
                lut[index].rgbGreen == 0xff &&
                lut[index].rgbBlue == 224 ) {
                *(pjData + y * lWidth + x) = 0xfe;
            }
            else if( lut[index].rgbRed == 251 &&
                lut[index].rgbGreen == 243 &&
                lut[index].rgbBlue == 234 )
            {
                *(pjData + y * lWidth + x) = 0xfd;
            }
            else
            {
                *(pjData + y * lWidth + x) =
                    ((lut[index].rgbRed   >> 0) & 0xE0 ) |
                    ((lut[index].rgbGreen >> 3) & 0x1C ) |
                    ((lut[index].rgbBlue  >> 6) & 0x03 );
            }
        }
    }

    phost = MemAlloc( sizeof( *phost ) );
    if( phost == NULL )
    {
        MemFree( pjData );
        return NULL;
    }

    phost->dwType = GFX_BITMAP_TYPE_HOST;
    phost->dwFlags = 0;

    phost->size.dwWidth = pbi->biWidth;
    phost->size.dwHeight = labs( pbi->biHeight );

    phost->bmf = GFX_BMF_8BPP;

    phost->pdwBits = (DWORD *) pjData;
    phost->dwDelta = lWidth / 4;

    return phost;

} /* read8BPP */


/*
 * readBITMAPINFOHEADER
 */
static LPBITMAPINFOHEADER readBITMAPINFOHEADER(
                HFASTFILE pfile,
                LPBITMAPFILEHEADER pbf )
{
    BITMAPINFOHEADER    bi;
    LPBITMAPINFOHEADER  pbi;

    if( pbf->bfSize <
        (sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER)) )
    {
        return NULL;
    }

    if( !FastFileSeek( pfile, sizeof (BITMAPFILEHEADER), SEEK_SET ) )
    {
        return NULL;
    }

    if( !FastFileRead( pfile, &bi, sizeof bi ) )
    {
        return NULL;
    }

    /*
     * sanity test (perhaps a bit lengthy...
     */
    if( bi.biSize != sizeof bi )
    {
        return NULL;
    }
    else if( bi.biWidth > GFX_MAX_WIDTH )
    {
        return NULL;
    }
    else if( labs( bi.biHeight ) > GFX_MAX_HEIGHT )
    {
        return NULL;
    }
    else if( bi.biPlanes != 1 )
    {
        return NULL;
    }
    else if( (bi.biBitCount != 1) && (bi.biBitCount != 4) &&
        (bi.biBitCount != 8) && (bi.biBitCount != 16) &&
        (bi.biBitCount != 24) && (bi.biBitCount != 32) )
    {
        return NULL;
    }
    else if( bi.biCompression > BI_BITFIELDS )
    {
        return NULL;
    }
    else if( (bi.biCompression == BI_RGB) &&
        (bi.biBitCount != 1) && (bi.biBitCount != 4) &&
        (bi.biBitCount != 8) && (bi.biBitCount != 24) )
    {
        return NULL;
    }
    else if( (bi.biCompression == BI_RLE8) && (bi.biBitCount != 8) )
    {
        return NULL;
    }
    else if( (bi.biCompression == BI_RLE4) && (bi.biBitCount != 4) )
    {
        return NULL;
    }
    else if( (bi.biCompression == BI_BITFIELDS) &&
        (bi.biBitCount != 16) && (bi.biBitCount != 32) )
    {
        return NULL;
    }
    else if( (bi.biSizeImage == 0) && (bi.biCompression != BI_RGB) )
    {
        return NULL;
    }
    else if( bi.biXPelsPerMeter < 0 )
    {
        return NULL;
    }
    else if( bi.biYPelsPerMeter < 0 )
    {
        return NULL;
    }

    pbi = MemAlloc( sizeof bi );
    if( pbi == NULL ) {
        return NULL;
    }

    memcpy( pbi, &bi, sizeof bi );
    return pbi;

} /* readBITMAPINFOHEADER */

/*
 * readBITMAPFILEHEADER 
 */
static LPBITMAPFILEHEADER readBITMAPFILEHEADER( HFASTFILE pfile )
{
    BITMAPFILEHEADER    bf;
    LPBITMAPFILEHEADER  pbf;
    long                pos;

    if( !FastFileSeek( pfile, 0, SEEK_END ) )
    {
        return NULL;
    }

    pos = FastFileTell( pfile );
    if( pos < sizeof( bf ) )
    {
        return NULL;
    }

    if( !FastFileSeek( pfile, 0, SEEK_SET ) )
    {
        return NULL;
    }

    if( !FastFileRead( pfile, &bf, sizeof bf ) )
    {
        return NULL;
    }

    if( bf.bfType != 'MB' )
    {
        return NULL;
    }
    else if( bf.bfSize != (DWORD) pos )
    {
        return NULL;
    }
    else if( bf.bfReserved1 != 0 )
    {
        return NULL;
    }
    else if( bf.bfReserved2 != 0 )
    {
        return NULL;
    }
    else if( bf.bfOffBits >= (DWORD) pos )
    {
        return NULL;
    }

    pbf = MemAlloc( sizeof bf );
    if( pbf == NULL )
    {
        return NULL;
    }

    memcpy( pbf, &bf, sizeof bf );

    return pbf;

} /* readBITMAPFILEHEADER */

/*
 * gfxLoadBitmap
 */
GFX_HBM gfxLoadBitmap( LPSTR szFileName )
{
    HFASTFILE           pfile;
    LPBITMAPFILEHEADER  pbf;
    LPBITMAPINFOHEADER  pbi;
    GFX_BITMAP_HOST     *phost;
    GFX_BITMAP_HOST     *(*pfn_win)(HFASTFILE,LPBITMAPFILEHEADER,LPBITMAPINFOHEADER);

    pfile = FastFileOpen( szFileName );
    if( pfile == NULL )
    {
        return NULL;
    }

    pbf = readBITMAPFILEHEADER( pfile );
    if( pbf == NULL )
    {
        FastFileClose( pfile );
        return NULL;
    }

    pbi = readBITMAPINFOHEADER( pfile, pbf );
    if( pbi == NULL )
    {
        FastFileClose( pfile );
        return NULL;
    }
    else
    {
        switch( pbi->biBitCount )
        {
        case 1:
            pfn_win = read1BPP;
            break;
        case 8:
            pfn_win = read8BPP;
            break;
        default:
            FastFileClose( pfile );
            return NULL;
        }
        phost = (*pfn_win)( pfile, pbf, pbi );
    }

    if( phost == NULL )
    {
        FastFileClose( pfile );
        return NULL;
    }

    FastFileClose( pfile );

    return (GFX_HBM) phost;

} /* gfxLoadBitmap */
