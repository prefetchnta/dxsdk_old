/*===========================================================================*\
|
|  File:        cgtimer.h
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

class CGameTimer {

public:
    volatile int paused;
    int rate;
    HANDLE  thread;
    HANDLE  Event;
    volatile int    Time;

    CGameTimer (void);
    ~CGameTimer (void);

    void    SetRate(int hertz);
    int     GetRate(void);
    void    Pause(void);
    void    Resume(void);
};

