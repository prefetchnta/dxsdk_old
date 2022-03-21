/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:    dddib.cpp
 *  Content: CDIB class.
 *              
 ***************************************************************************/


// dib.cpp : implementation file
//
//

#include "stdafx.h"
#include "dddib.h"
#include "malloc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDIB

// Create a small DIB here so m_pBMI and m_pBits are always valid.
CDIB::CDIB()
{
    m_pBMI       = NULL;
    m_pIDDraw    = NULL;
    m_pIDSurface = NULL;
    m_iNumColors = 0;
    m_iCols      = 1;
    m_iRows      = 1;
    m_iCol       = 0;
    m_iRow       = 0;
    m_iWidth     = 0;
    m_iHeight    = 0;
}

CDIB::~CDIB()
{
    // Free the memory.
    if (m_pIDDraw)
        m_pIDDraw->Release();
    if (m_pIDSurface)
        m_pIDSurface->Release();

    if (m_pBMI)
        free(m_pBMI);
}

/////////////////////////////////////////////////////////////////////////////
// Private functions

static BOOL IsWinDIB(BITMAPINFOHEADER* pBIH)
{
    ASSERT(pBIH);
    if (((BITMAPCOREHEADER*)pBIH)->bcSize == sizeof(BITMAPCOREHEADER))
    {
        return FALSE;
    }
    return TRUE;
}

static int NumDIBColorEntries(LPBITMAPINFO pBmpInfo) 
{
    BITMAPINFOHEADER* pBIH;
    BITMAPCOREHEADER* pBCH;
    int iColors, iBitCount;

    ASSERT(pBmpInfo);

    pBIH = &(pBmpInfo->bmiHeader);
    pBCH = (BITMAPCOREHEADER*) pBIH;

    // Start off by assuming the color table size from
    // the bit-per-pixel field.
    if (IsWinDIB(pBIH))
    {
        iBitCount = pBIH->biBitCount;
    }
    else
    {
        iBitCount = pBCH->bcBitCount;
    }

    switch (iBitCount)
        {
    case 1:
        iColors = 2;
        break;
    case 4:
        iColors = 16;
        break;
    case 8:
        iColors = 256;
        break;
    default:
        iColors = 0;
        break;
        }

    // If this is a Windows DIB, then the color table length
    // is determined by the biClrUsed field if the value in
    // the field is nonzero.
    if (IsWinDIB(pBIH) && (pBIH->biClrUsed != 0))
    {
        iColors = pBIH->biClrUsed;
    }

    return iColors;
}


/////////////////////////////////////////////////////////////////////////////
// CDIB commands

// Create a new empty 8bpp DIB with a 256 entry color table.
// Load a DIB from an open file.

typedef struct
{
    int iWeight;
    int iNumber;
} COLOR_ORDER;

extern "C" int _cdecl WtCompare(const void *pv1, const void *pv2)
{
    COLOR_ORDER *pco1 = (COLOR_ORDER *) pv1;
    COLOR_ORDER *pco2 = (COLOR_ORDER *) pv2;
    return(pco2->iWeight - pco1->iWeight);
}

CDIB* CDIB::NewDib(IDirectDraw *pIDD, CFile* fp, BOOL bResort)
{

    if (!pIDD || !fp)
        return(NULL);

    CDIB *pDib = NULL;

    BOOL        bIsPM    = FALSE;
    BITMAPINFO *pBmpInfo = NULL;
    BYTE       *pBits    = NULL;
    int         ii, jj;
    LPRGBQUAD lpRGB;

    // Get the current file position.
    DWORD dwFileStart = fp->GetPosition();

    // Read the file header to get the file size and to
    // find out where the bits start in the file.
    BITMAPFILEHEADER BmpFileHdr;
    int iBytes;
    iBytes = fp->Read(&BmpFileHdr, sizeof(BmpFileHdr));
    if (iBytes != sizeof(BmpFileHdr))
    {
        TRACE("Failed to read file header");
        goto $abort;
    }

    // Check that we have the magic 'BM' at the start.
    if (BmpFileHdr.bfType != 0x4D42)
    {
        TRACE("Not a bitmap file");
        goto $abort;
    }

    // Make a wild guess that the file is in Windows DIB
    // format and read the BITMAPINFOHEADER. If the file turns
    // out to be a PM DIB file we'll convert it later.
    BITMAPINFOHEADER BmpInfoHdr;
    iBytes = fp->Read(&BmpInfoHdr, sizeof(BmpInfoHdr)); 
    if (iBytes != sizeof(BmpInfoHdr))
    {
        TRACE("Failed to read BITMAPINFOHEADER");
        goto $abort;
    }   

    // Check that we got a real Windows DIB file.
    if (BmpInfoHdr.biSize != sizeof(BITMAPINFOHEADER))
    {
        if (BmpInfoHdr.biSize != sizeof(BITMAPCOREHEADER))
        {
            TRACE(" File is not Windows or PM DIB format");
            goto $abort;
        }

        // Set a flag to convert PM file to Win format later.
        bIsPM = TRUE;

        // Back up the file pointer and read the BITMAPCOREHEADER
        // and create the BITMAPINFOHEADER from it.
        fp->Seek(dwFileStart + sizeof(BITMAPFILEHEADER), CFile::begin);
        BITMAPCOREHEADER BmpCoreHdr;
        iBytes = fp->Read(&BmpCoreHdr, sizeof(BmpCoreHdr)); 
        if (iBytes != sizeof(BmpCoreHdr))
        {
            TRACE("Failed to read BITMAPCOREHEADER");
            goto $abort;
        }

        BmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
        BmpInfoHdr.biWidth = (int) BmpCoreHdr.bcWidth;
        BmpInfoHdr.biHeight = (int) BmpCoreHdr.bcHeight;
        BmpInfoHdr.biPlanes = BmpCoreHdr.bcPlanes;
        BmpInfoHdr.biBitCount = BmpCoreHdr.bcBitCount;
        BmpInfoHdr.biCompression = BI_RGB;
        BmpInfoHdr.biSizeImage = 0;
        BmpInfoHdr.biXPelsPerMeter = 0;
        BmpInfoHdr.biYPelsPerMeter = 0;
        BmpInfoHdr.biClrUsed = 0;
        BmpInfoHdr.biClrImportant = 0;
    }



    // Work out how much memory we need for the BITMAPINFO
    // structure, color table and then for the bits.  
    // Allocate the memory blocks.
    // Copy the BmpInfoHdr we have so far,
    // and then read in the color table from the file.
    int iColors;
    int iColorTableSize;

    iColors = NumDIBColorEntries((LPBITMAPINFO) &BmpInfoHdr);
    iColorTableSize = iColors * sizeof(RGBQUAD);

    int iBitsSize;
    int iBISize;

    //
    // always allocate enough room for 256 entries
    //
    iBISize = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
    iBitsSize = BmpFileHdr.bfSize - BmpFileHdr.bfOffBits;

    //
    // Allocate the memory for the header.
    //
    pBmpInfo = (LPBITMAPINFO) malloc(iBISize);
    if (!pBmpInfo)
    {
        TRACE("Out of memory for DIB header");
        goto $abort;
    }

    memset(pBmpInfo, 0, iBISize);

    // Copy the header we already have.
    memcpy(pBmpInfo, &BmpInfoHdr, sizeof(BITMAPINFOHEADER));

    // Now read the color table in from the file.
    if (bIsPM == FALSE && iColorTableSize)
    {
        // Read the color table from the file.
        iBytes = fp->Read(((LPBYTE) pBmpInfo) + sizeof(BITMAPINFOHEADER),
                             iColorTableSize);
        if (iBytes != iColorTableSize)
        {
            TRACE("Failed to read color table");
            goto $abort;
        }
    }
    else
    {
        // Read each PM color table entry in turn and convert it
        // to Win DIB format as we go.
        lpRGB = (LPRGBQUAD) ((LPBYTE) pBmpInfo + sizeof(BITMAPINFOHEADER));
        int i;
        RGBTRIPLE rgbt;
        for (i=0; i<iColors; i++)
        {
            iBytes = fp->Read(&rgbt, sizeof(RGBTRIPLE));
            if (iBytes != sizeof(RGBTRIPLE))
            {
                TRACE("Failed to read RGBTRIPLE");
                goto $abort;
            }
            lpRGB->rgbBlue = rgbt.rgbtBlue;
            lpRGB->rgbGreen = rgbt.rgbtGreen;
            lpRGB->rgbRed = rgbt.rgbtRed;
            lpRGB->rgbReserved = 0;
            lpRGB++;
        }
    }


    pDib = new CDIB;

    ASSERT(pDib);

    pDib->m_pIDDraw = pIDD;
    pDib->m_pIDDraw->AddRef();


    //
    // Create an offscreen surface to put the DIB in.
    //
    DDSURFACEDESC dddesc;
    memset((void *) &dddesc, 0x00, sizeof(dddesc));
    dddesc.dwSize = sizeof(DDSURFACEDESC);
    dddesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    dddesc.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    dddesc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
    dddesc.lpDDSurface = NULL;
    dddesc.ddpfPixelFormat.dwFlags = DDPF_RGB;

    dddesc.dwHeight = BmpInfoHdr.biHeight;
    dddesc.dwWidth  = BmpInfoHdr.biWidth;
    dddesc.ddpfPixelFormat.dwRGBBitCount = BmpInfoHdr.biBitCount;

    if (BmpInfoHdr.biBitCount < 9)
        dddesc.ddpfPixelFormat.dwFlags |= DDPF_PALETTEINDEXED8;

    HRESULT ddrVal;

    ddrVal = pDib->m_pIDDraw->CreateSurface(&dddesc);
    if (ddrVal != DD_OK || dddesc.lpDDSurface == NULL)
    {
        TRACE("CreateSurface failed.");
        goto $abort;
    }

    pDib->m_pIDSurface = (IDirectDrawSurface *) dddesc.lpDDSurface;

    int iFileStride;
    int iSurfaceStride;
    int iWidth;
    int iDelta;
    int iHeight;
    LPBYTE lpByte;

    //
    // Lock the bits down so we can load them into the buffer.
    //
    if (pDib->m_pIDSurface->Lock(NULL, &dddesc, 0, NULL) != DD_OK)
    {
        TRACE("Lock failed.");
        goto $abort;
    }

    //
    // Calculate Strides.
    //
    iFileStride    = (BmpInfoHdr.biWidth + 3) & ~3;
    iSurfaceStride = dddesc.lPitch;
    iWidth         = min(iFileStride, iSurfaceStride);
    iDelta         = max(0, iFileStride - iWidth);

    lpByte = (LPBYTE) dddesc.lpSurface;

    // Seek to the bits in the file.
    fp->Seek(dwFileStart + BmpFileHdr.bfOffBits, CFile::begin);

    iHeight = BmpInfoHdr.biHeight;

    if (iHeight > 0)
    {
        lpByte += (iHeight - 1) * iSurfaceStride;
        iSurfaceStride = -iSurfaceStride;
    }
    else
        iHeight = -iHeight;

    while (iHeight--)
    {
        if (fp->Read((LPVOID) lpByte, iWidth) != (DWORD) iWidth)
        {
            TRACE("Failed to read bits");
            goto $abort;
        }
        if (iDelta)
            fp->Seek(iDelta, CFile::current);

        lpByte += iSurfaceStride;
    }

    //
    // Do some initialization if we have been asked to resort.
    //
    COLOR_ORDER  aco[256];
    if (bResort)
    {
        for (ii = 0; ii < 256; ii++)
        {
            aco[ii].iWeight =  0;
            aco[ii].iNumber = ii;
        }
    }
    lpRGB = (LPRGBQUAD) ((LPBYTE) pBmpInfo + sizeof(BITMAPINFOHEADER));
    lpByte = (LPBYTE) dddesc.lpSurface;

    //
    // How many colors to we really need?
    //
    iHeight = dddesc.dwHeight;
    for (ii = 0; ii < iHeight; ii++)
    {
        for (jj = 0; jj < BmpInfoHdr.biWidth; jj++)
        {
            lpRGB[*lpByte].rgbReserved = 1;
            aco[*lpByte].iWeight++;
            lpByte++;
        }

        lpByte += dddesc.lPitch - dddesc.dwWidth;
    }

    pDib->m_iNumColors = 0;
    pDib->m_iHighColor = 0;

    LPBYTE lpLine;
    lpRGB = (LPRGBQUAD) ((LPBYTE) pBmpInfo + sizeof(BITMAPINFOHEADER));
    for (ii = 0; ii < 256; ii++)
    {
        if (lpRGB->rgbReserved == 1)
        {
            lpRGB->rgbReserved = 0;
            pDib->m_iNumColors++;
            pDib->m_iHighColor;
        }
        else
        {
            //
            // If we didn't use it, set it to black.
            //

            memset((LPVOID)lpRGB, 0x00, sizeof(RGBQUAD));
        }
        lpRGB++;
    }

    //
    // Resort color table if asked.
    //
    if (bResort)
    {
        qsort(aco, 256, sizeof(COLOR_ORDER), WtCompare);

        BYTE abTrans[256];
        RGBQUAD rgbOld[256];

        lpRGB = (LPRGBQUAD) ((LPBYTE) pBmpInfo + sizeof(BITMAPINFOHEADER));
        memcpy( (LPVOID) rgbOld, lpRGB, sizeof(RGBQUAD) * 256);

        for (ii = 0; ii < 256; ii++)
        {
            abTrans[(aco[ii].iNumber & 0x000000ff)] = ii;
            memcpy((LPVOID) &lpRGB[ii], (LPVOID) &rgbOld[(aco[ii].iNumber & 0x000000ff)], sizeof(RGBQUAD));
        }

        lpLine = (LPBYTE) dddesc.lpSurface;
        for (jj = 0; jj < dddesc.dwHeight; jj++)
        {
            lpByte = lpLine;
            for (ii = 0; ii < dddesc.dwWidth; ii++)
            {
                *lpByte = abTrans[*lpByte];
                lpByte++;
            }
            lpLine += dddesc.lPitch;
            }
    }


    //
    // Remember default color key.
    //
    lpLine = (LPBYTE) dddesc.lpSurface;
    pDib->m_iPalColorKey = (int) (*lpLine & 0x000000ff);

    if (pDib->m_pIDSurface->Unlock(NULL) != DD_OK)
    {
        TRACE("UnLock failed.");
        goto $abort;
    }

    //
    // We keep color table information in PaleteEntry format.
    //
    lpRGB = (LPRGBQUAD) ((LPBYTE) pBmpInfo + sizeof(BITMAPINFOHEADER));
    for (ii = 0; ii < 256; ii++)
    {
        pDib->m_ape[ii].peFlags  = 0;
        pDib->m_ape[ii].peBlue   = lpRGB[ii].rgbBlue;
        pDib->m_ape[ii].peRed    = lpRGB[ii].rgbRed;
        pDib->m_ape[ii].peGreen  = lpRGB[ii].rgbGreen;
    }

    // Everything went OK.
    pDib->m_pBMI    = pBmpInfo;
    pDib->m_iWidth  = pBmpInfo->bmiHeader.biWidth;
    pDib->m_iHeight = pBmpInfo->bmiHeader.biHeight;
    return pDib;
                
$abort: // Something went wrong.
    if (pDib)
        delete pDib;

    if (pBmpInfo)
        free(pBmpInfo);

    if (pBits)
        free(pBits);

    return(NULL);    
}
LPRECT CDIB::GetCellRect()
{
    m_CellRect.left   = m_iWidth  * m_iCol;
    m_CellRect.right  = m_iWidth  * (m_iCol + 1);
    m_CellRect.top    = m_iHeight * m_iRow;
    m_CellRect.bottom = m_iHeight * (m_iRow + 1);
    return(&m_CellRect);
}
BOOL CDIB::SetCell(int iRow, int iCol)
{
    if (iRow >= m_iRows || iCol >= m_iCols)
        return(FALSE);

    m_iRow = iRow;
    m_iCol = iCol;

    return(TRUE);
}

BOOL CDIB::SetNumRowsAndColumns(int iRows, int iCols)
{
    int iT1;
    int iT2;

    iT1 = m_iWidth  / iCols;
    iT2 = m_iHeight / iRows;
    if (iT1 < 1 || iT2 < 1)
        return(FALSE);

    m_iWidth  = iT1;
    m_iHeight = iT2;
    m_iRows   = iRows;
    m_iCols   = iCols;
    return(TRUE);
}

//
// Adjust CDIB for a particular color table.
//
void CDIB::ResetColors(CPE *pcPE)
{
    BYTE abTrans[256];
    LPBYTE    lpByte;
    LPBYTE lpLine;
    int ii;
    int jj;

    if (!pcPE)
        return;

    pcPE->SetNewColors(&m_ape[0], m_iNumColors, abTrans);

    DDSURFACEDESC dddesc;
    memset((void *) &dddesc, 0x00, sizeof(dddesc));
    dddesc.dwSize = sizeof(DDSURFACEDESC);

    if (m_pIDSurface->Lock(NULL, &dddesc, 0, NULL) != DD_OK)
        return;

    lpLine = (LPBYTE) dddesc.lpSurface;
    m_iPalColorKey = (int) (abTrans[*lpLine] & 0x000000ff);

    for (jj = 0; jj < dddesc.dwHeight; jj++)
        {
        lpByte = lpLine;
        for (ii = 0; ii < dddesc.dwWidth; ii++)
        {
            *lpByte = abTrans[*lpByte];
            lpByte++;
        }
        lpLine += dddesc.lPitch;
        }





    memcpy( (LPVOID) m_ape, (LPVOID) pcPE->GetAPE(), sizeof(PALETTEENTRY) * 256);

    if (m_pIDSurface->Unlock(NULL) != DD_OK)
    {
        TRACE("Lock failed.");
    }

}

#define EqualPE(a,b) (memcmp((LPVOID)&(a),(LPVOID)&(b),sizeof(PALETTEENTRY))==0)
HRESULT CDIB::SetColorKey(PALETTEENTRY *lppe)
{
    int iIndex;
    int ii;
    int iMatch;
    int iClose;
    int iDiff;
    int iDelta;

    if (lppe)
    {

        iMatch = 0;
        iClose = 0x7fffffff;
        ii = 0;
        while(iClose && ii < 256)
        {
            iDiff  = 0;
            iDelta =   (((int)        lppe->peRed) & 0x000000ff)
                     - (((int)     m_ape[ii].peRed) & 0x000000ff);
            iDiff += iDelta * iDelta;
            iDelta =   (((int)        lppe->peBlue) & 0x000000ff)
                     - (((int)     m_ape[ii].peBlue) & 0x000000ff);
            iDiff += iDelta * iDelta;
            iDelta =   (((int)        lppe->peGreen) & 0x000000ff)
                     - (((int)     m_ape[ii].peGreen) & 0x000000ff);
            iDiff += iDelta * iDelta;
            if (iClose > iDiff)
            {
                iClose = iDiff;
                iMatch = ii;
            }
            ii++;
        }
        iIndex = iMatch;
    }
    else
        iIndex = m_iPalColorKey;

    DDCOLORKEY DDColorKey;

    DDColorKey.dwColorSpaceLowValue  = (DWORD) iIndex;
    DDColorKey.dwColorSpaceHighValue = (DWORD) iIndex;

    return(m_pIDSurface->SetColorKey(DDCKEY_SRCBLT, &DDColorKey));


}


