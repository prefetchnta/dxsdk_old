// ddib.h : header file
//
// CDIB class
//

#ifndef __DDDIB__
#define __DDDIB__
#include "ddraw.h"
#include "pe.h"


class CDIB : public CObject
{
public:
    IDirectDrawSurface *m_pIDSurface;

    static CDIB *NewDib(IDirectDraw *pIDD, CFile *fp, BOOL bResort = FALSE);
    ~CDIB();

    PALETTEENTRY* GetAPE() {return(m_ape);}

    void ResetColors(CPE *pcPE);
    BOOL SetNumRowsAndColumns(int iRows, int iCols);
    BOOL   SetCell(int iRow, int iCol);
    LPRECT GetCellRect();
    HRESULT SetColorKey(PALETTEENTRY* lppe = NULL);


protected:

private:
    BITMAPINFO* m_pBMI;         // Pointer to BITMAPINFO struct
    IDirectDraw        *m_pIDDraw;
    int                 m_iNumColors;
    int                 m_iHighColor;
    int                 m_iRows;
    int                 m_iCols;
    int                 m_iWidth;
    int                 m_iHeight;
    int                 m_iRow;
    int                 m_iCol;
    RECT                m_CellRect;
    int                 m_iPalColorKey;
    PALETTEENTRY        m_ape[256];

    CDIB();
    int DibWidth()
        {return m_iWidth;}
    int DibHeight() 
        {return m_iHeight;}
};


#endif // __DDDIB__


