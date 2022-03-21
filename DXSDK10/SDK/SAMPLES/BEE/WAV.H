// wav.h : header file
//
// CWAV class
//

#ifndef __CWAV__
#define __CWAV__
#include <mmsystem.h>
#include "dsound.h"

#define BUFFER_SIZE  (1024 * 64)
#define BUFFER_4K    (1024 *  4)
#define BUFFER_32K    (1024 *  32)

class CWAV : public CObject
{
public:
    static CWAV *NewWav(IDirectSound *pIDS, PSTR pchFile,
                        DWORD fFlags, DWORD dwSize = BUFFER_SIZE);

    static CWAV *NewWav(IDirectSound *pIDS, WAVEFORMATEX *pwfmt,
                        DWORD fFlags, DWORD dwSize = BUFFER_SIZE);

    HRESULT Play(DWORD dwPlayFlag = 0) {return( m_pIDSoundBuffer->Play(0, dwPlayFlag));}
    HRESULT Stop()                     {return( m_pIDSoundBuffer->Stop());}
    HRESULT CleanStop();
    HRESULT SetLooping(DWORD dw)       {return( m_pIDSoundBuffer->SetLooping(dw));}
    BOOL    GetInvalidSize(LPDWORD lpdw);
    BOOL    bSetFileWrap(BOOL bb = TRUE)
        {
        BOOL bOld = m_bWrapIt;
        m_bWrapIt = bb;
        return(m_bWrapIt);
        }


    ~CWAV();

    WAVEFORMATEX* GetFmtAddress()        {return(m_pwfmt);}
    IDirectSoundBuffer* GetSoundBuffer()
        {
        if (m_pIDSoundBuffer)
            m_pIDSoundBuffer->AddRef();
        return(m_pIDSoundBuffer);
        }

    IDirectSoundBuffer* RefSoundBuffer()
        {
        return(m_pIDSoundBuffer);
        }

    BOOL bFinished()
        {
        if (m_dwExtraData == 0 && (m_dwOffsetUsed == 0) && (!m_bWrapIt))
            return(TRUE);
        return(FALSE);
        }
    DWORD Copy(CWAV *pSrc, DWORD dwBuffer = BUFFER_SIZE / 2);


protected:

private:
    IDirectSound       *m_pIDSound;
    IDirectSoundBuffer *m_pIDSoundBuffer;
    WAVEFORMATEX       *m_pwfmt;
    DWORD               m_dwExtraData;
    HMMIO               m_hmmio;
    DWORD               m_dwOffsetUsed;
    DWORD               m_dwBufferSize;
    DWORD               m_dwDataStart;
    DWORD               m_dwWavSize;
    BOOL                m_bWrapIt;


    BYTE                m_bZero;
    CWAV();
};


#endif // __CWAV__


