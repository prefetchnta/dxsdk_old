/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       fastfile.c
 *  Content:    Fast file I/O for large numbers of files.
 *              Uses a single file built using FFCREATE.EXE; this file
 *              contains a directory + all the files.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 ***************************************************************************/
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <search.h>
#include "fastfile.h"
#include "ffent.h"

#ifdef DEBUG
#define ODS( a ) OutputDebugString( a )
#else
#define ODS( a )
#endif

typedef struct {
    BOOL        inuse;
    LONG        pos;
    LONG        size;
    LPFILEENTRY pfe;
} FILEHANDLE, FAR *LPFILEHANDLE;

static HANDLE           hFile;
static HANDLE           hFileMapping;
static LPFILEENTRY      pFE;
static LPSTR            pBase;
static DWORD            dwFECnt;
static LPFILEHANDLE     lpFH;
static DWORD            dwFHCnt;
static long             lFileEnd;

/*
 * Compare 
 *
 * bsearch comparison routine
 */
int Compare( LPFILEENTRY p1, LPFILEENTRY p2 )
{
    return( _stricmp( (p1)->name,(p2)->name ) );

} /* Compare */

/*
 * FastFileInit
 *
 * Initialize for fast file access. The master file and maximum number
 * of open "files" are specified.
 */
BOOL FastFileInit( LPSTR fname, int max_handles )
{
    /*
     * get a file handle array
     */
    lpFH = LocalAlloc( LPTR, max_handles * sizeof( FILEHANDLE ) );
    if( lpFH == NULL ) {
        return FALSE;
    }
    dwFHCnt = max_handles;

    /*
     * create a memory mapped file for the master file
     */
    hFile = CreateFile( fname, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                        FILE_FLAG_RANDOM_ACCESS, 0 );

    if( hFile == NULL ) {
        ODS( "FastFileInit: CreateFile(" ); ODS( fname ); ODS( ")\r\n" );
        FastFileFini();
        return FALSE;
    }
    hFileMapping = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
    if( hFileMapping == NULL ) {
        ODS( "FastFileInit: CreateFileMapping Failed\r\n" );
        FastFileFini();
        return FALSE;
    }
    pBase = MapViewOfFile( hFileMapping, FILE_MAP_READ, 0, 0, 0 );
    if( pBase == NULL ) {
        ODS( "FastFileInit: MapViewOfFile Failed\r\n" );
        FastFileFini();
        return FALSE;
    }

    /*
     * get initial data from the memory mapped file
     */
    dwFECnt = *((DWORD *) pBase);
    pFE = (LPFILEENTRY) (pBase + 4);
    lFileEnd = pFE[dwFECnt-1].offset;

    return TRUE;

} /* FastFileInit */

/*
 * FastFileFini
 *
 * Clean up resources
 */
void FastFileFini( void )
{
    if( hFileMapping != NULL ) {
        CloseHandle( hFileMapping );
        hFileMapping = NULL;
    }
    if( hFile != NULL ) {
        CloseHandle( hFile );
        hFile = NULL;
    }
    if( lpFH != NULL ) {
        LocalFree( lpFH );
        lpFH = NULL;
    }
    dwFHCnt = 0;
    pBase = NULL;
    dwFECnt = 0;
    pFE = NULL;

} /* FastFileFini */

/*
 * FastFileOpen
 *
 * Search the directory for the file, and return a file handle if found.
 */
HFASTFILE FastFileOpen( LPSTR name )
{
    FILEENTRY   fe;
    LPFILEENTRY pfe;

    if( pFE == NULL ) {
        ODS( "FastFileOpen: not initialized\r\n" );
        return NULL;
    }
    if( name == NULL || name[0] == 0 ) {
        ODS( "FastFileOpen: invalid name\r\n" );
        return NULL;
    }

    strcpy( fe.name, name );
    pfe = bsearch( &fe, pFE, dwFECnt, sizeof( FILEENTRY ), (LPVOID) Compare );
    if( pfe != NULL ) {
        DWORD   i;
        for( i=0;i<dwFHCnt;i++ ) {
            if( !lpFH[i].inuse ) {
                lpFH[i].inuse = TRUE;
                lpFH[i].pos = pfe->offset;
                lpFH[i].size = (pfe+1)->offset - pfe->offset;
                lpFH[i].pfe = pfe;
                return &lpFH[i];
            }
        }
        ODS( "FastFileOpen: Out of file handles\r\n" );
    } else {
        ODS( "FastFileOpen: File \"" ); ODS( name ); ODS( "\" not found\r\n" );
    }

    return NULL;

} /* FastFileOpen */

/*
 * FastFileClose
 *
 * Mark a fast file handle as closed
 */
BOOL FastFileClose( LPFILEHANDLE pfh )
{
    if( pfh == NULL || pfh->inuse != TRUE ) {
        ODS( "FastFileClose: invalid handle\r\n" );
        return FALSE;
    }
    pfh->inuse = FALSE;
    return TRUE;

} /* FastFileClose */

/*
 * FastFileRead
 *
 * read from a fast file (memcpy!)
 */
BOOL FastFileRead( LPFILEHANDLE pfh, LPVOID ptr, int size )
{
    if( pfh == NULL || pfh->inuse != TRUE ) {
        ODS( "FastFileRead: invalid handle\r\n" );
        return FALSE;
    }
    if( size < 0 ) {
        ODS( "FastFileRead: invalid size\r\n" );
        return FALSE;
    }
    if( (pfh->pos + size) > ((pfh->pfe)+1)->offset ) {
        ODS( "FastFileRead: read past end of file\r\n" );
        return FALSE;
    }
    memcpy( ptr, pBase + pfh->pos, size );
    pfh->pos += size;
    return TRUE;

} /* FastFileRead */

/*
 * FastFileSeek
 *
 * Set to a new position in a fast file.  Uses standard SEEK_SET, SEEK_CUR,
 * SEEK_END definitions.
 */
BOOL FastFileSeek( LPFILEHANDLE pfh, int off, int how )
{
    LPFILEENTRY pfe;

    if( pfh == NULL || pfh->inuse != TRUE ) {
        ODS( "FastFileSeek: invalid handle\r\n" );
        return FALSE;
    }
    pfe = pfh->pfe;
    if( how == SEEK_SET ) {
        if( off < 0 || off >= pfh->size ) {
            ODS( "FastFileSeek: Invalid offset\r\n" );
            return FALSE;
        }
        off += pfe->offset;
    } else if( how == SEEK_END ) {
        if( off < 0 || off >= pfh->size ) {
            ODS( "FastFileSeek: Invalid offset\r\n" );
            return FALSE;
        }
        off = (pfe+1)->offset - off;
    } else if( how == SEEK_CUR ) {
        off = pfh->pos + off;
        if( off < pfe->offset || off >= (pfe+1)->offset ) {
            ODS( "FastFileSeek: Invalid offset\r\n" );
            return FALSE;
        }
    } else {
        ODS( "FastFileSeek: Invalid 'how' field\r\n" );
        return FALSE;
    }
    pfh->pos = off;
    return TRUE;

} /* FastFileSeek */

/*
 * FastFileTell
 *
 * Get the current position in a fast file
 */
long FastFileTell( LPFILEHANDLE pfh )
{
    if( pfh == NULL || pfh->inuse != TRUE ) {
        ODS( "FastFileTell: invalid handle\r\n" );
        return -1;
    }
    return pfh->pos - pfh->pfe->offset;

} /* FastFileTell */
