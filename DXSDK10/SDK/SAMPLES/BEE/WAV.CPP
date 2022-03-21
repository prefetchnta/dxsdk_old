/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:    wav.cpp
 *  Content: Sound wav class.
 *              
 ***************************************************************************/


//
// CWAV implementation.
//


#include "stdafx.h"
#include "wav.h"
#include "malloc.h"
#include "dsound.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CWAV::CWAV()
{
    m_pIDSound         = NULL;
    m_pIDSoundBuffer   = NULL;
    m_pwfmt            = NULL;
    m_dwExtraData      = 0;
    m_hmmio            = NULL;
    m_dwOffsetUsed     = 0;
    m_dwBufferSize     = 0;
    m_dwDataStart      = 0;
    m_dwWavSize        = 0;
    m_bWrapIt          = FALSE;
}

CWAV::~CWAV()
{
    if (m_pIDSound)
        m_pIDSound->Release();

    if (m_pIDSoundBuffer)
        m_pIDSoundBuffer->Release();

    if (m_pwfmt)
        free(m_pwfmt);

    if (m_hmmio)
        mmioClose(m_hmmio, 0);

}

#define WAVE_FORMAT_8BIT (  WAVE_FORMAT_1M08 \
                          | WAVE_FORMAT_1S08 \
                          | WAVE_FORMAT_2M08 \
                          | WAVE_FORMAT_2S08 \
                          | WAVE_FORMAT_4M08 \
                          | WAVE_FORMAT_4S08)

CWAV* CWAV::NewWav(IDirectSound *pIDS, PSTR pchFile, DWORD fFlags, DWORD dwSize)
{
    CWAV               *pWav              = NULL;
    HMMIO               hmmio             = NULL; // file handle for open file
    MMCKINFO            mmckinfoParent;           // parent chunk information structure
    MMCKINFO            mmckinfoSubchunk;         // subchunk information structure
    int                 iFmtSize ;                // size of "fmt " chunk
    DWORD               dwDataSize;               // size of "data" chunk
    HPSTR               lpData;                   // pointer to memory for "data" chunk
    IDirectSoundBuffer *pIDSoundBuffer    = NULL;
    BOOL                bExtraData;
    WAVEFORMATEX        wfmt;
    BOOL                bLock             = FALSE;
    DWORD               dwDataBlock;
    BYTE                byte;
    DWORD               dwSize2;

    if ((fFlags & DSB_DESCRIPTIONF_PRIMARY) && (dwSize > BUFFER_SIZE))
        dwSize = BUFFER_SIZE;       // Hardware limitation on buffer size;

    if (pchFile == NULL)
        return(NULL);

    if(!(hmmio = mmioOpen(pchFile, NULL, MMIO_READ | MMIO_ALLOCBUF)))
    {
        TRACE("Failed to open file.\n");
        return(NULL);
    }

    mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL,
                    MMIO_FINDRIFF))
    {
        TRACE("This is not a WAVE file.\n");
        mmioClose(hmmio, 0);
        return(NULL);
    }

    //
    // Find the "fmt " chunk (form type "fmt "); it must be
    // a subchunk of the "RIFF" parent chunk
    //
    mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent,
                    MMIO_FINDCHUNK))
    {
        TRACE("WAVE file has no \"fmt \" chunk.\n");
        mmioClose(hmmio, 0);
        return(NULL);
    }

    iFmtSize  = mmckinfoSubchunk.cksize;
    if (iFmtSize  != sizeof(PCMWAVEFORMAT))
    {
        TRACE( "WAVE Format structure unrecognized size\n");
        goto abort;
    }

    //
    // Read the "fmt " chunk
    //
    if (mmioRead(hmmio, (HPSTR) &wfmt, iFmtSize ) != iFmtSize )
    {
        TRACE("Failed to read format chunk.\n");
        goto abort;
    }

    //
    // Ascend out of the "fmt " subchunk
    //
    mmioAscend(hmmio, &mmckinfoSubchunk, 0);

    //
    // Find the data subchunk. The current file position should be at the
    // beginning of the data chunk, however, you should not make this                             * assumption--use mmioDescend to locate the data chunk.
    //
    mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent,
                    MMIO_FINDCHUNK))
    {
        TRACE("WAVE file has no data chunk.\n");
        goto abort;
    }


    /* Get the size of the data subchunk
     */
    dwDataSize = mmckinfoSubchunk.cksize;
    if (dwDataSize == 0L)
    {
        TRACE("The data chunk contains no data.\n");
        goto abort;
    }
   
    if (dwDataSize <= dwSize)
    {
        dwSize = dwDataSize;
        bExtraData = FALSE;
    }
    else
    {
        bExtraData = TRUE;
    }

    dwSize +=   BUFFER_4K - 1;
    dwSize &= ~(BUFFER_4K - 1);         // DMA buffers multiple of 4K

    dwDataBlock = min(dwSize, dwDataSize);

    DSBUFFERDESC dSndBuffDesc;
    memset(&dSndBuffDesc, 0x00, sizeof(dSndBuffDesc));
    dSndBuffDesc.dwSize                     = sizeof(dSndBuffDesc);
    dSndBuffDesc.fdwBufferDesc              = fFlags;

    dSndBuffDesc.cbBufferSize               = dwSize;
    dSndBuffDesc.dwPriority                 = 0;
    dSndBuffDesc.dsbfFormat.dwSize          = sizeof(DSBUFFERFORMAT);
    dSndBuffDesc.dsbfFormat.fdwBufferFormat = 0; 

    dSndBuffDesc.dsbfFormat.pwfx            = &wfmt;
    dSndBuffDesc.dsbfFormat.cbwfx           = iFmtSize ;

    if (    pIDS->CreateSoundBuffer(&dSndBuffDesc) != DS_OK
         || dSndBuffDesc.pIDSBuffer == NULL)
    {
        AfxMessageBox("Couldn't create IDirectSoundBuffer object.");
        goto abort;
    }
    else
        pIDSoundBuffer = dSndBuffDesc.pIDSBuffer;

    if (pIDSoundBuffer->Lock(0, dwSize, (LPLPVOID) &lpData, (LPDWORD) &dwSize2, NULL, NULL, NULL, 0) != DS_OK)
    {
        AfxMessageBox("Sound Buffer Lock Failed.");
        goto abort;
    }
    else
        bLock = TRUE;

    if (dwSize2 != dwSize)
        TRACE("Sizes didn't match on lock.\n");

    MMIOINFO mmioInfo;

    if (mmioGetInfo(hmmio, &mmioInfo, 0) != 0)
        {
        TRACE("Unexplained failure to get info on file.\n");
        goto abort;
        }

    //
    // Read the waveform data subchunk
    //
    if(mmioRead(hmmio, (HPSTR) lpData, dwDataBlock) != (int) dwDataBlock)
    {
        TRACE("Failed to read data chunk.\n");
        goto abort;
    }

    if (((int) dwDataBlock) != dwSize)
    {

        if (wfmt.wFormatTag & WAVE_FORMAT_8BIT)
            byte = 0x80;
        else
            byte = 0x00;

        memset( lpData + dwDataBlock, byte, dwSize - dwDataBlock);
    }

    bLock = FALSE;
    if (pIDSoundBuffer->Unlock((LPVOID) lpData, NULL) != DS_OK)
    {
        AfxMessageBox("Sound Buffer Unlock Failed.");
        goto abort;
    }


    pWav = new CWAV;

    if (!pWav)
        goto abort;

    if ((pWav->m_pwfmt = (WAVEFORMATEX *) malloc(sizeof(WAVEFORMATEX))) == NULL)
        goto abort;

    pWav->m_pwfmt->cbSize = 0;
    memcpy( pWav->m_pwfmt, &wfmt, iFmtSize );
    pWav->m_pIDSound = pIDS;
    pWav->m_pIDSound->AddRef();

    pWav->m_pIDSoundBuffer = pIDSoundBuffer;
    pWav->m_dwOffsetUsed   = 0;
    pWav->m_dwBufferSize   = dwSize;
    pWav->m_bZero          = byte;
    pWav->m_dwDataStart    = mmioInfo.lDiskOffset;
    pWav->m_dwWavSize      = dwDataSize;


    if (bExtraData)
    {
        pWav->m_hmmio = hmmio;
        pWav->m_dwExtraData = dwDataSize - dwSize;
    }
    else
        mmioClose(hmmio, 0);

    return(pWav);

abort:
    if (pIDSoundBuffer)
    {
        if (bLock)
            pIDSoundBuffer->Unlock(lpData, NULL);

        if (pIDSoundBuffer->Release() != 0)
        {
            AfxMessageBox("Sound Buffer unexpected release count: not 0.");
        }
    }

    if (pWav)
    {
        delete pWav;
    }

    if (hmmio)
        mmioClose(hmmio, 0);

    return(NULL);

}

BOOL CWAV::GetInvalidSize(LPDWORD pSize)
{
    DWORD       dwSize;
    BOOL        bOverRun = FALSE;
    DWORD       dwPlay;
    DWORD       dwWrite;
    DWORD       dwStatus;

    if (!pSize)
        return(FALSE);

    m_pIDSoundBuffer->GetCurrentPosition(&dwPlay, &dwWrite, &dwStatus);

    if (dwStatus & DSB_STATUSF_STOPPED)
    {
        //
        // We are stopped.
        //
        m_dwOffsetUsed = 0;  // And I really don't know what to do,
                             // Since dsound resets dwPlay and dwWrite to 0.
        dwSize = m_dwBufferSize;
    }
    else
    {
        //
        // We are currently playing.
        //
        if (   ((dwWrite < dwPlay) && (m_dwOffsetUsed < dwPlay))
            || ((m_dwOffsetUsed < dwPlay) && (dwPlay < dwWrite)))
        {
            dwSize = dwPlay - m_dwOffsetUsed;
        }
        else if ((dwPlay < dwWrite) && (dwWrite < m_dwOffsetUsed))
        {
            dwSize = m_dwBufferSize - m_dwOffsetUsed + dwPlay;
        }
        else
        {
            bOverRun = TRUE;            // Big problems, ignored for now.
            m_dwOffsetUsed = 0;
            dwSize = m_dwBufferSize;
        }
    }

    *pSize = dwSize;
    return(!bOverRun);
}

//
// this -- destination  (Primary)
// pSrc -- src          (Sound)
//




DWORD CWAV::Copy(CWAV *pSrc, DWORD dwBuffer)
{
    DSBWAVEBLT  dsbWavBlt;
    HRESULT     hr;
    LPVOID      lpData;
    BOOL        bLock;
    DWORD       dwSize;
    BOOL        bOverRun = FALSE;
    DWORD       dwSizeReload ;
    DWORD       dwSize2;

    if (dwBuffer > m_dwBufferSize)
        return(0);

    dwBuffer = min(dwBuffer, BUFFER_32K);
    memset( &dsbWavBlt, 0x00, sizeof(DSBWAVEBLT));

    dsbWavBlt.dwSize          = sizeof(DSBWAVEBLT);
    dsbWavBlt.fdwWaveBlt      = DSB_BLTF_COPY;
    dsbWavBlt.dwCount         = 1;

    dsbWavBlt.adswbb[0].pdsb             = pSrc->m_pIDSoundBuffer;
    dsbWavBlt.adswbb[0].fdwWaveBltBuffer = DSB_BLTF_COPY;


    bOverRun = ! GetInvalidSize(&dwSize);

    if (bOverRun)
    {
        m_pIDSoundBuffer->Stop();
    }

    dwSize = min(dwSize, dwBuffer);
    dwSize = min(pSrc->m_dwBufferSize - pSrc->m_dwOffsetUsed, dwSize);
    if (dwSize)
    {
        dsbWavBlt.dwPosition                 = m_dwOffsetUsed;
        dsbWavBlt.adswbb[0].dwPosition       = pSrc->m_dwOffsetUsed;
        dsbWavBlt.adswbb[0].cbCopyLength     = dwSize;
        dsbWavBlt.cbCopyLength               = dwSize;
        hr = m_pIDSoundBuffer->WavBlt( &dsbWavBlt);
    }

    if (bOverRun)
    {
        Play();
    }

    m_dwOffsetUsed       += dwSize;
    m_dwOffsetUsed        = m_dwOffsetUsed % m_dwBufferSize;
    pSrc->m_dwOffsetUsed += dwSize;
    pSrc->m_dwOffsetUsed  = pSrc->m_dwOffsetUsed % pSrc->m_dwBufferSize;

    if (pSrc->m_dwOffsetUsed == 0)
    {
        if (pSrc->m_dwExtraData)
            dwSizeReload  = min( pSrc->m_dwBufferSize, pSrc->m_dwExtraData);
        else
            dwSizeReload  = pSrc->m_dwBufferSize;

        if (pSrc->m_pIDSoundBuffer->Lock(0, dwSizeReload , (LPLPVOID) &lpData,
                                         (LPDWORD) &dwSize2, NULL, NULL,
                                         NULL, 0) != DS_OK)
        {
            TRACE("Lock Failed.\n");
            goto abort;
        }

    if (dwSize2 != dwSizeReload )
        TRACE("Sizes didn't match on lock.\n");

        if (pSrc->m_dwExtraData)
        {
            //
            // Read the waveform data subchunk
            //
            if(mmioRead(pSrc->m_hmmio, (HPSTR) lpData, dwSizeReload ) != (int) dwSizeReload )
            {
                TRACE("Failed to read data chunk.\n");
                goto abort;
            }

            pSrc->m_dwExtraData  -= dwSizeReload ;
        }
        else
            memset( lpData, pSrc->m_bZero, dwSizeReload );

        if (dwSizeReload  != pSrc->m_dwBufferSize)
            memset( (LPVOID) ((LPBYTE)lpData + dwSizeReload ), pSrc->m_bZero,
                    pSrc->m_dwBufferSize - dwSizeReload );

        pSrc->m_dwOffsetUsed  = 0;

        bLock = FALSE;
        if (pSrc->m_pIDSoundBuffer->Unlock((LPVOID) lpData, NULL) != DS_OK)
        {
            TRACE("Sound Buffer Unlock Failed.");
            goto abort;
        }

    if (pSrc->m_dwExtraData == 0 && pSrc->m_bWrapIt && pSrc->m_dwDataStart)
        {
        mmioSeek(pSrc->m_hmmio, pSrc->m_dwDataStart, SEEK_SET);
        pSrc->m_dwExtraData = pSrc->m_dwWavSize;
        }
    }

    return(dwSize);        
abort:
    return(0);

}

CWAV* CWAV::NewWav(IDirectSound *pIDS, WAVEFORMATEX *pwfmt, DWORD fFlags, DWORD dwSize)
{
    CWAV                 *pWav            = NULL;
    HPSTR                 lpData;         // pointer to memory for "data" chunk
    IDirectSoundBuffer   *pIDSoundBuffer  = NULL;
    BOOL                  bLock           = FALSE;
    BYTE byte;
    BOOL                  bExtraData;
    DWORD       dwSize2;

    if ((fFlags & DSB_DESCRIPTIONF_PRIMARY) && (dwSize > BUFFER_SIZE))
        dwSize = BUFFER_SIZE;       // Hardware limitation on buffer size;

    bExtraData = FALSE;

    dwSize +=   BUFFER_4K - 1;
    dwSize &= ~(BUFFER_4K - 1);         // DMA buffers multiple of 4K

    DSBUFFERDESC dSndBuffDesc;
    memset(&dSndBuffDesc, 0x00, sizeof(dSndBuffDesc));
    dSndBuffDesc.dwSize                     = sizeof(dSndBuffDesc);
    dSndBuffDesc.fdwBufferDesc              = fFlags;

    dSndBuffDesc.cbBufferSize               = dwSize;
    dSndBuffDesc.dwPriority                 = 0;
    dSndBuffDesc.dsbfFormat.dwSize          = sizeof(DSBUFFERFORMAT);
    dSndBuffDesc.dsbfFormat.fdwBufferFormat = 0; 

    dSndBuffDesc.dsbfFormat.pwfx            = pwfmt;
    dSndBuffDesc.dsbfFormat.cbwfx           = sizeof(WAVEFORMATEX);

    if (    pIDS->CreateSoundBuffer(&dSndBuffDesc) != DS_OK
         || dSndBuffDesc.pIDSBuffer == NULL)
    {
        AfxMessageBox("Couldn't create IDirectSoundBuffer object.");
        goto abort;
    }
    else
        pIDSoundBuffer = dSndBuffDesc.pIDSBuffer;

    if (pIDSoundBuffer->Lock(0, dwSize, (LPLPVOID) &lpData,
                                     &dwSize2, NULL, NULL, NULL, 0) != DS_OK)
    {
        AfxMessageBox("Sound Buffer Lock Failed.");
        goto abort;
    }
    else
        bLock = TRUE;


    if (dwSize2 != dwSize)
        TRACE("Sizes didn't match on lock.\n");

    if (pwfmt->wFormatTag & WAVE_FORMAT_8BIT)
        byte = 0x80;
    else
        byte = 0x00;

    memset( lpData , byte, dwSize);

    bLock = FALSE;

    if (pIDSoundBuffer->Unlock((LPVOID) lpData, NULL) != DS_OK)
    {
        AfxMessageBox("Sound Buffer Unlock Failed.");
        goto abort;
    }


    pWav = new CWAV;

    if (!pWav)
        goto abort;

    if ((pWav->m_pwfmt = (WAVEFORMATEX *) malloc(sizeof(WAVEFORMATEX))) == NULL)
        goto abort;

    pWav->m_pwfmt->cbSize = 0;
    memcpy( pWav->m_pwfmt, pwfmt, sizeof(WAVEFORMATEX));
    pWav->m_pIDSound = pIDS;
    pWav->m_pIDSound->AddRef();

    pWav->m_pIDSoundBuffer = pIDSoundBuffer;
    pWav->m_dwOffsetUsed   = 0;
    pWav->m_dwBufferSize   = dwSize;
    pWav->m_bZero          = byte;
    pWav->m_dwDataStart    = 0;

    return(pWav);

abort:
    if (pIDSoundBuffer)
    {
        if (bLock)
            pIDSoundBuffer->Unlock(lpData, NULL);

        if (pIDSoundBuffer->Release() != 0)
        {
            AfxMessageBox("Sound Buffer unexpected release count: not 0.");
        }
    }

    if (pWav)
    {
        delete pWav;
    }

    return(NULL);

}

//
// Since we can't stop the primary buffer on a dime, null out remaining sounds.
//
HRESULT CWAV::CleanStop()
{
    DWORD       dwPlay;
    DWORD       dwWrite;
    DWORD       dwStatus;
    BOOL        bLock = FALSE;
    LPBYTE      lpData;
    HRESULT     hr;
    HRESULT     hrStop;
    DWORD       dwSize2;


    if (!m_pIDSoundBuffer)
    return DSERR_GENERIC;

    hrStop = Stop();

    if ((hr = m_pIDSoundBuffer->Lock(0, m_dwBufferSize, (LPLPVOID) &lpData,
                                   &dwSize2, NULL, NULL, NULL, 0)) != DS_OK)
        return(hr);
    else
        bLock = TRUE;

    if (dwSize2 != m_dwBufferSize)
        TRACE("Sizes didn't match on lock.\n");

    if ((hr = m_pIDSoundBuffer->GetCurrentPosition(&dwPlay,
                                                   &dwWrite,
                                                   &dwStatus)) != DS_OK)
    {
        TRACE("Get Current Position failed in CleanStop");
        goto abort;
    }

//     memset( (LPVOID) (lpData + dwWrite), m_bZero, m_dwBufferSize - dwWrite);
    memset( (LPVOID) (lpData), m_bZero, m_dwBufferSize);
abort:
    if (bLock)
        m_pIDSoundBuffer->Unlock((LPVOID) lpData, NULL);


    return( hrStop);
    
}


