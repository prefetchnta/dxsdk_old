/*===========================================================================*\
|
|  File:        cgimage.cpp
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

#include <windows.h>
#include "cgrsrce.h"
#include "cgexcpt.h"
#include "cgdebug.h"
#include "cgglobl.h"
#include "cgscreen.h"
#include "cglevel.h"
#include "cgupdate.h"
#include "cgimage.h"

#define LIMIT(x, low, high) (max(low, min(x, high)))

/*---------------------------------------------------------------------------*\
|
|       Class CGameSkyImage
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/

//#define SKY_COLOR (PALETTERGB( 102, 159, 220 ))
// in our palette, entry 13 is a good sky blue
#define SKY_COLOR ( 13 )

CGameSkyImage::CGameSkyImage(
    char* pFileName
    ) : CGameImage( pFileName, 32760 )
{
    mCurZ = 32760;  // force to background
}

CGameSkyImage::~CGameSkyImage()
{
    
}

void
CGameSkyImage::Update(
    CGameLevel* pLevel,
    CGameUpdateList* pUpdate
    )
{
    
}

void
CGameSkyImage::Render(
    CGameLevel* pLevel,
    CGameScreen* pScreen,
    CGameUpdateList* pUpdate
    )
{
    pScreen->ColorFill(
            pUpdate->GetDirtyRect(),
            SKY_COLOR
            );
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameTiledImage
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/

static char tleExt[] = ".tle";

CGameTiledImage::CGameTiledImage(
        char* pFileBase,        // file base name
        int curz,
        int curx,
        int cury
        ) : CGameImage( pFileBase, curz ),
            mpDIBBuffer( NULL ),
            mTileArray( NULL ),
            mpTransBuffer( NULL )
{
    HANDLE hFile;

    if (!pFileBase)
        return;


    char* pFileName = new char[lstrlen(pFileBase) + lstrlen(tleExt) + 1];

    lstrcpy( pFileName, pFileBase );
    lstrcat( pFileName, tleExt );

    // open the .tle file
    hFile = CreateFile(
        pFileName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );

    delete[] pFileName;

    if (hFile == INVALID_HANDLE_VALUE)
    {
        DB_LOG(DB_PROBLEM, "DEBUG: CreateFile failed in CGameTiledImage");
        throw CGameException(
                IDS_INVALID_FILE
                );
    }

    TileMapStamp stamp;

    DWORD bytesRead;

    // file begins with our stamp
    ReadFile(
        hFile,
        &stamp,
        sizeof( stamp ),
        &bytesRead,
        NULL
        );

    // verify file is valid tile map
    if ((stamp.signature != TILE_MAP_SIGNATURE) ||
        (stamp.version != TILE_MAP_VERSION) ||
        (stamp.tileSize != TILE_SIZE)
        )
    {
        DB_LOG(DB_PROBLEM, "DEBUG: invalid file stamp in CGameTiledImage");
        throw CGameException(
                IDS_INVALID_FILE
                );
    }

    // allocate for dib filename (length includes terminator)
    pFileName = new char[stamp.nameLength];

    // followed by the tile DIB filename
    ReadFile(
        hFile,
        pFileName,
        stamp.nameLength,
        &bytesRead,
        NULL
        );

    // allocate our tile array
    mNumCols = stamp.columns;
    mTileArray = new WORD*[mNumCols];
    mWidth = mNumCols * stamp.tileSize;
    mHeight = stamp.rows * stamp.tileSize;

    mXpos = curx;
    mYpos = cury;

    // read in the tile map
    for (int col = 0; col < mNumCols; col++)
    {
        mTileArray[col] = new WORD[stamp.rows];
        for (int row = 0; row < stamp.rows; row++)
        {
            ReadFile(
                hFile,
                &mTileArray[col][row],
                sizeof(WORD),
                &bytesRead,
                NULL
                );
        }
    }

    CloseHandle( hFile );

    // now create a bit buffer for our DIB
    CGameDIB theDib( pFileName );

    delete[] pFileName;

    if (gUse_DDraw)
    {
        mpDIBBuffer = new CGameDDrawBitBuffer( &theDib );
        if (!mpDIBBuffer->IsValid())
        {
            // we didn't get video memory, so delete that buffer
            delete mpDIBBuffer;
            // & create a system memory one
            mpDIBBuffer = new CGameDSBitBuffer( &theDib );
        }
//      mpDIBBuffer = new CGameDDrawBitBuffer( &theDib );
        if (!mpDIBBuffer->IsValid())
        {
            // we didn't get video memory, so delete that buffer
            delete mpDIBBuffer;
            // & create a system memory one
            mpDIBBuffer = new CGameDSBitBuffer( &theDib );
        }
    }
    else
    {
#ifdef USE_DIBSECTION
        mpDIBBuffer = new CGameDSBitBuffer(
                    &theDib,
                    theDib.GetWidth(),
                    theDib.GetHeight(),
                    theDib.GetPixelColor(0,0)
                    );
#else
        mpDIBBuffer = new CGameGDIBitBuffer(
                    &theDib,
                    theDib.GetWidth(),
                    theDib.GetHeight(),
                    theDib.GetPixelColor(0,0)
                    );
#endif
    }


#ifndef USE_DIBSECTION
    if (!gUse_DDraw)
    {
        // create this image's transparency mask
        mpTransBuffer = new CGameMonoMask( theDib.GetWidth(), theDib.GetHeight() );

        // now create the monochrome mask
        mpTransBuffer->Blt(
                 0,
                 0,
                 theDib.GetWidth(),
                 theDib.GetHeight(),
                 mpDIBBuffer,
                 0,
                 0,
                 SRCCOPY
                 );
    }
#endif
    mCurZ = curz;
}   

CGameTiledImage::~CGameTiledImage()
{
    if (mTileArray)
    {
        for (; mNumCols>0; mNumCols--)
        {
            delete[] mTileArray[mNumCols-1];
        }
        delete[] mTileArray;
    }

    delete mpDIBBuffer;
}

void 
CGameTiledImage::Update(
    CGameLevel* pLevel,
    CGameUpdateList* pUpdate
    )
{
    if (mpNext)
        mpNext->Update(pLevel, pUpdate);
}

void
CGameTiledImage::Render(
    CGameLevel* pLevel,
    CGameScreen* pScreen,
    CGameUpdateList* pUpdate
    )
{
    if (mpNext)
    {
        // recurse down the list to get background filled in
        mpNext->Render(pLevel, pScreen, pUpdate);
    }

    LPRECT pDirty = pUpdate->GetDirtyRect();

    int width = pDirty->right - pDirty->left + 1;
    int height = pDirty->bottom - pDirty->top + 1;

    RECT worldUpdate;

    worldUpdate.left = pLevel->Screen2WorldX( pDirty->left, mXParallax );
    worldUpdate.top = pLevel->Screen2WorldY( pDirty->top, mYParallax );
    worldUpdate.right = worldUpdate.left + width - 1;
    worldUpdate.bottom = worldUpdate.top + height - 1;

    RECT image;

    image.left = mXpos;
    image.top = mYpos;
    image.right = mXpos + mWidth - 1;
    image.bottom = mYpos + mHeight - 1;

    RECT realImage;

    if (IntersectRect( &realImage, &image, &worldUpdate ))
    {
        int offsetX = realImage.left>mXpos?(realImage.left-mXpos) % TILE_SIZE :0;
        int tileWidth = TILE_SIZE - offsetX;

        for (
            int worldPixelX = realImage.left;
            worldPixelX <= realImage.right;
            worldPixelX += tileWidth, tileWidth = TILE_SIZE
            )
        {
            int colIndex = (worldPixelX - mXpos) / TILE_SIZE;
            int screenX = pLevel->World2ScreenX( worldPixelX, mXParallax );

            int offsetY = realImage.top>mYpos? (realImage.top-mYpos) % TILE_SIZE:0;
            int tileHeight = TILE_SIZE - offsetY;

            for (
                int worldPixelY = realImage.top;
                worldPixelY <= realImage.bottom;
                worldPixelY += tileHeight, tileHeight = TILE_SIZE
                )
            {
                int rowIndex = (worldPixelY - mYpos) / TILE_SIZE;
                int screenY = pLevel->World2ScreenY( worldPixelY, mYParallax );

                WORD tileNum = mTileArray[colIndex][rowIndex];

                if (!IS_TRANSPARENT(tileNum))
                {
                    int srcX = (TILE_INDEX(tileNum) * TILE_SIZE) + offsetX;
                    int srcY = offsetY;

                    if (HAS_TRANSPARENCY(tileNum))
                    {
                        pScreen->TransRender(
                            screenX,
                            screenY,
                            min(tileWidth, realImage.right - worldPixelX + 1),
                            min(tileHeight, realImage.bottom - worldPixelY + 1),
                            mpDIBBuffer,
                            srcX,
                            srcY,
                            mpTransBuffer,
                            srcX,
                            srcY
                            );
                    }
                    else
                    {
                        pScreen->Render(
                            screenX,
                            screenY,
                            min(tileWidth, realImage.right - worldPixelX + 1),
                            min(tileHeight, realImage.bottom - worldPixelY + 1),
                            mpDIBBuffer,
                            srcX,
                            srcY,
                            SRCCOPY
                            );
                    }
                }

                offsetY = 0;
            }

            offsetX = 0;
        }
    }

}

