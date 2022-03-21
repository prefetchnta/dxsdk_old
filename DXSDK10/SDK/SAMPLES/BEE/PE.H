// pe.h : header file
//
// CPE class
//

#ifndef __PE__
#define __PE__
#include "ddraw.h"


class CPE : public CObject
{
public:
    static CPE* NewCPE(DWORD dwFlags = SYSPAL_STATIC);
    ~CPE();
    PALETTEENTRY* GetAPE() {return &m_ape[0];}
    int GetNumColorsAvail() {return m_iColorsAvail;}
    void SetNewColors(PALETTEENTRY* ape, int iColors, LPBYTE aTrans);

private:
    int InsertPE(PALETTEENTRY pe);
    CPE();
    PALETTEENTRY m_ape[256];
    BYTE    m_abFlags[256];
    int     m_iColorsAvail;
    int     m_iFirstFree;

};

#endif // __PE__


