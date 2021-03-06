/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       shell.h
 *  Content:    DirectSound shell header
 *
 ***************************************************************************/
#ifndef __SHELL_INCLUDED__
#define __SHELL_INCLUDED__
// If this is defined, then an extra text string under the filename will show
// if the file is stopped or playing.
#define SHOWSTATUS

#define MAXCONTROLS             8

// Error codes.
#define ER_OUTOFMEMORY                  0xd100      // Out of memory.
#define ER_CANNOTALLOCATEDC             0xd101      // Cannot get a DC.
#define ER_CANNOTRELEASEDC              0xd102
#define ER_APIFAILED                    0xd103      // A general API failed.
#define ER_CANNOTCREATEWINDOW           0xd104      // Cannot create a window.

#define ER_BADROUTINEPARAM              0xd200      // Bad parameter for procedure call.
#define ER_TIMERALLOC                   0xd201      // Timer alloc error.


// For setting the ranges on the freq, vol, etc.
// dwFreq*FREQMUL+FREQADD = # to show on screen.

#define FREQMUL                 (100)                   
#define FREQADD                 (4000)

#define MINFREQ_TB              0
#define MAXFREQ_TB              512
#define MINPAN_TB               0
#define MAXPAN_TB               127
#define MIDPAN_TB               64
#define MINVOL_TB               0
#define MAXVOL_TB               127


// Id code deltas
#define idFreqTB                1
#define idPanTB                 2
#define idVolLTB                3
#define idVolRTB                4
#define idVolMTB                5
#define idLoopedBN              6
#define idPlayBN                7
#define idRemoveBN              8


// UI stuff.


#define DX_WIN_BORDER           30
#define DY_WIN_BORDER           30
#define DX_MINWINDOW            640
#define DY_MINWINDOW            500

#define DX_CONTROLSPACING       150
#define DY_CONTROLSPACING       0

#define DX_LINEEDGE             (DX_CONTROLSPACING - 20)
#define DY_LINEEDGE             (1)


#define COX_STARTCONTROL        (20)
#define COY_STARTCONTROL        (10)

#define DX_TEXTSPACING          5
#define DY_TEXTSPACING          10

#define DX_LOOPEDSPACING        0
#define DY_LOOPEDSPACING        2

#define DX_STATUSSPACING        0

#ifdef SHOWSTATUS
#define DY_STATUSSPACING        8
#else
#define DY_STATUSSPACING        0
#endif


#define DX_FREQSPACING          0
#define DY_FREQSPACING          10

#define DY_PANSPACING           10
#define DY_VOLSPACING           10

#define DX_FILENAME_TXT         120
#define DX_STATUS_TXT           120
#define DX_FREQ_TXT             100
#define DX_PAN_TXT              100
#define DX_VOL_TXT              100
#define DX_LOOPED_TXT           100

#define DX_FREQ_TB              130
#define DY_FREQ_TB              40
#define DX_PAN_TB               130
#define DY_PAN_TB               40
#define DX_VOL_TB               40
#define DY_VOL_TB               100
#define DX_VOLSPACING_TB        5

#define DX_VOLUMECHAR           15
#define DY_VOLSPACINGY          -5

#define DX_BUTTONSPACING        80
#define DY_BUTTONSPACING        10
#define DY_BEFOREFIRSTBUTTON    22  //31
#define DY_BETWEENBUTTONS       5

#define DX_FRAMEEDGE            10
#define DY_FRAMEEDGE            10
#define DX_FRAMEEDGEINNER       4           

#define DSBSIZE                 0x10000

// To check for stopping of sounds, a timer is set...use this for the rate.
#define TIMERPERIOD             100             // In milliseconds

// In Options/Output Type, there are strings to pick the format...Here are the number of them.
#define C_DROPDOWNPCMFORMATS    16

typedef struct _fileinfo
    {
    BYTE                *pbData;            // Pointer to actual data of file.
    UINT                cbSize;             // Size of data.
    WAVEFORMATEX        *pwfx;              // Pointer to waveformatex structure.

    DWORD               dwFreq;             // Frequency.
    DWORD               dwPan;              // Panning info.
    DWORD               dwVol;              // Total volume.
    BOOL                fLooped;            // Looped?

    BOOL                fPlaying;           // Is this one playing?

    int                 cox;                // Coordinates of where the structure is
    int                 coy;                // printed.

    // HWND's needed.
    HWND                hWndFileName_TXT;   // Filename text string.
    HWND                hWndFreq_TB;        // Trackbar handle.
    HWND                hWndFreq_TXT;       // Text string for freq.
    HWND                hWndPan_TB;         // Trackbar handle.
    HWND                hWndPan_TXT;        // Text string for pan.
    HWND                hWndVol_TXT;        // Text string for volume.
    HWND                hWndVolL_TB;        // Trackbar for volume left.
    HWND                hWndVolR_TB;        // Trackbar for volume right.
    HWND                hWndVolM_TB;        // Main volume.
    HWND                hWndLooped_BN;      // Looped
    HWND                hWndPlay_BN;        // Play
    HWND                hWndRemove_BN;      // Remove.
    
    HWND                hWndFileName_EDGE;  // The line under filename.
    HWND                hWndLooped_EDGE;    // The line under looped.
    HWND                hWndFreq_EDGE;      // The line under freq.
    HWND                hWndPan_EDGE;       // The line under pan.
    HWND                hWndVol_EDGE;       // The line under volume.
    HWND                hWndWhole_EDGE;     // The whole surrounding edge.
    HWND                hWndVolL_TXT;       // For the L
    HWND                hWndVolR_TXT;       // For the R

    #ifdef SHOWSTATUS
    HWND                hWndStatus_TXT;     // For status.
    HWND                hWndStatus_EDGE;    
    #endif

    LPDIRECTSOUNDBUFFER pDSB;               // Pointer to direct sound buffer.

    int                 nFileName;          // Index to filename, not including dir.
    char                szFileName[MAX_PATH];
    struct _fileinfo    *pNext;             // Pointer to next file.

    } FILEINFO;


char            szAppName[]     = "Direct Sound Demo";
char            szMessage[]     = "Direct Sound Demo";

HANDLE          hInst;
HWND            hWndMain        = NULL;
DWORD           dwTimer         = 0;            // Timer handle.

/*  This is the main head of the linked list, but its only used for the
    pNext which will point to the first FILEINFO structure, or NULL if there
    are no files loaded  */
FILEINFO        FileInfoFirst;              // Start of linked list.

char            szFreq[]        = "Freq";
char            szPan[]         = "Pan";
char            szVolume[]      = "Volume";
char            szLooped[]      = "Looped";
char            szPlay[]        = "Play";
char            szStop[]        = "Stop";
char            szRemove[]      = "Remove";

#ifdef SHOWSTATUS
char            szPlaying[]     = "Playing";
char            szStopped[]     = "Stopped";
#endif


char *rgszTypes[C_DROPDOWNPCMFORMATS] = 
    {                                       // Index
    "8.000 kHz, 8-Bit, Mono",               // 0
    "8.000 kHz, 8-Bit, Stereo",             // 1
    "8.000 kHz, 16-Bit, Mono",              // 2
    "8.000 kHz, 16-Bit, Stereo",            // 3
    "11.025 kHz, 8-Bit, Mono",              // 4
    "11.025 kHz, 8-Bit, Stereo",            // 5
    "11.025 kHz, 16-Bit, Mono",             // 6
    "11.025 kHz, 16-Bit, Stereo",           // 7
    "22.050 kHz, 8-Bit, Mono",              // 8
    "22.050 kHz, 8-Bit, Stereo",            // 9
    "22.050 kHz, 16-Bit, Mono",             // 10
    "22,050 kHz, 16-Bit, Stereo",           // 11
    "44.100 kHz, 8-Bit, Mono",              // 12
    "44.100 kHz, 8-Bit, Stereo",            // 13
    "44.100 kHz, 16-Bit, Mono",             // 14
    "44.100 kHz, 16-Bit, Stereo"            // 15
    };                                      

BOOL                    rgfcoxAvail[MAXCONTROLS];

LPDIRECTSOUND           gpds            = NULL;


long FAR PASCAL WndProc(HWND, unsigned, WPARAM, LPARAM);
long FAR PASCAL DLGHelpAbout(HWND, UINT, WPARAM, LPARAM);
long FAR PASCAL DLGOutputBufferType(HWND, UINT, WPARAM, LPARAM);
long FAR PASCAL DLGCheckLatency(HWND, UINT, WPARAM, LPARAM);

BOOL ClassInit(HANDLE);
void PD_FileOpen(HWND);
BOOL OpenFileDialog(HWND, LPSTR, int *);
BOOL IsValidWave(LPSTR);

int CreateControl(HWND, FILEINFO *, DWORD, DWORD, DWORD);
void GetNextControlCoords(FILEINFO *, int *, int *);
int AddToList(FILEINFO *, FILEINFO *);
int FreeAllList(HWND, FILEINFO *);
int RemoveFromList(FILEINFO *, FILEINFO *);
int GetNumControls(FILEINFO *);
int StartDSound(HWND, FILEINFO *);
int StopDSound(HWND, FILEINFO *);
int StopAllDSounds(HWND, FILEINFO *);

BOOL UIMainWindowVSBHandler(HWND, WPARAM, LPARAM); 
BOOL UIMainWindowHSBHandler(HWND, WPARAM, LPARAM); 
BOOL UIMainWindowCMDHandler(HWND, WPARAM, LPARAM); 
BOOL UIMainWindowTimerHandler(HWND, WPARAM, LPARAM); 
void SetAllText(FILEINFO *);
void UpdateLRVolume(FILEINFO *);

void AppDestroy(HWND);
void AppInit(HWND);

int NewDirectSoundBuffer(FILEINFO *);
int ReleaseDirectSoundBuffer(FILEINFO *);
int ChangeOutputPan(FILEINFO *);
int ChangeOutputFreq(FILEINFO *);
int ChangeOutputVol(FILEINFO *);

int FormatToIndex(HWND, FILEINFO *);
int IndexToFormat(HWND, FILEINFO *, int);
#endif
