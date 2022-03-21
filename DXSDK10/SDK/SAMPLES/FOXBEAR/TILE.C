/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *  Copyright (C) 1994-1995 ATI Technologies Inc. All Rights Reserved.
 *
 *  File:       tile.c
 *  Content:    tile loading and initialization functions
 *
 ***************************************************************************/
#include "foxbear.h"

/*
 * CreateTiles
 */
HBITMAPLIST *CreateTiles( HBITMAPLIST *phBitmapList, USHORT n )
{
    HBITMAPLIST *hTileList;
    GFX_HBM      hBM_dest;
    GFX_HBM      hBM_src;
    GFX_SIZE     size;
    GFX_RECT     rect;
    GFX_POINT    point;
    USHORT       i;

    hTileList = CMemAlloc( n, sizeof (HBITMAPLIST) );
    if( hTileList == NULL )
    {
        ErrorMessage( "hTileList in CreateTiles" );
    }

    size.dwWidth = C_TILE_W;         // THESE NEED
    size.dwHeight = C_TILE_H;         // CHANGING !
    point.dwX = 0;
    point.dwY = 0;
    rect.dwLeft = point.dwX;
    rect.dwTop = point.dwY;
    rect.dwRight = point.dwX + size.dwWidth;
    rect.dwBottom = point.dwY + size.dwHeight;
    
    for( i = 0; i < n; ++i )
    {
        hBM_src = phBitmapList[i].hBM;
        if( hBM_src != NULL )
        {
            hBM_dest = gfxCreateVramBitmap( &size, GFX_BMF_8BPP );
            gfxRectXfer( hBM_dest, &rect, hBM_src, &point, GFX_MIX_S );
            hTileList[i].hBM = hBM_dest;
        }
        else
        {
            hTileList[i].hBM = NULL;
        }

        hTileList[i].hMask = phBitmapList[i].hMask;
    }
    return hTileList;

} /* CreateTiles */

/*
 * DestroyTiles
 */
BOOL DestroyTiles( HBITMAPLIST *phTileList )
{
    MemFree( phTileList );

    return TRUE;

} /* DestroyTiles */


/*
 * CreatePosList
 */
HPOSLIST *CreatePosList( LPSTR fileName, USHORT width, USHORT height )
{
    HPOSLIST    *hPosList;
    USHORT      pos;
    FILE        *pFile;
    CHAR        number[4];
        
    pFile = fopen( fileName, "r" );

    if( pFile == NULL )
    {
        ErrorMessage( "pFile in CreatePosList" );
    }

    hPosList = CMemAlloc( width * height, sizeof (USHORT) );

    if( hPosList == NULL )
    {
        fclose( pFile );
        ErrorMessage( "posList in CreatePosList" );
    }

    for( pos = 0; pos < width * height; ++pos )
    {
        fscanf( pFile, "%s\n", number );
        hPosList[pos] = (USHORT) atoi( number ) - 1;
    }

    fclose( pFile );

    return hPosList;
}    

/*
 * CreateSurfaceList
 */
HSURFACELIST *CreateSurfaceList( LPSTR fileName, USHORT width, USHORT height )
{
    HSURFACELIST        *hSurfaceList;
    USHORT              pos;
    FILE                *pFile;
    char                number[4];
    USHORT              value;
        
    pFile = fopen( fileName, "r" );
    if( pFile == NULL )
    {
        ErrorMessage( "pFile in CreateSurfaceList" );
    }

    hSurfaceList = CMemAlloc( width * height, sizeof (HSURFACELIST) );
    if( hSurfaceList == NULL )
    {
        fclose( pFile );
        ErrorMessage( "posList in CreateSurfaceList" );
    }

    for( pos = 0; pos < width * height; ++pos )
    {
        fscanf( pFile, "%s\n", number );
        value = (USHORT) atoi( number );

        if( value == 0 )
        {
            hSurfaceList[pos] = FALSE;
        }
        else
        {
            hSurfaceList[pos] = TRUE;
        }
    }
    fclose( pFile );

    return hSurfaceList;

} /* CreateSurfaceList */

/*
 * DestoryPosList
 */
BOOL DestroyPosList ( HPOSLIST *posList )
{
    if( posList == NULL )
    {
        ErrorMessage( "posList in DestroyPosList" );
    }

    MemFree( posList );
    return TRUE;

} /* DestroyPosList */
