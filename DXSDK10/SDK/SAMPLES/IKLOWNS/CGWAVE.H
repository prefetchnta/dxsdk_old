/*===========================================================================*\
|
|  File:        cgwave.h
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
#ifndef _CGWAVE_H
#define _CGWAVE_H
#include <mmsystem.h>

#define WAVEVERSION 1

#ifndef ER_MEM
#define ER_MEM          0xe000
#endif

#ifndef ER_CANNOTOPEN
#define ER_CANNOTOPEN       0xe100
#endif

#ifndef ER_NOTWAVEFILE
#define ER_NOTWAVEFILE      0xe101
#endif

#ifndef ER_CANNOTREAD
#define ER_CANNOTREAD       0xe102
#endif

#ifndef ER_CORRUPTWAVEFILE
#define ER_CORRUPTWAVEFILE  0xe103
#endif

#ifndef ER_CANNOTWRITE
#define ER_CANNOTWRITE      0xe104
#endif


int WaveOpenFile(
    char        *pszFileName,
    HMMIO       *phmmioIn,
    WAVEFORMATEX    **ppwfxInfo,
    MMCKINFO    *pckInRIFF
);

int WaveStartDataRead(
    HMMIO       *phmmioIn,
    MMCKINFO    *pckIn,
    MMCKINFO    *pckInRIFF
);

int WaveReadFile(
    HMMIO       hmmioIn,
    UINT        cbRead,
    BYTE        *pbDest,
    MMCKINFO    *pckIn,
    UINT        *cbActualRead
);
#endif

