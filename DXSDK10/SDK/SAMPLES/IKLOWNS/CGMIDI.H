/*===========================================================================*\
|
|  File:        cgmidi.h
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
#ifndef _CGMIDI_H
#define _CGMIDI_H

BOOL playMusic(LPSTR lpszMIDIFileName, BOOL fAutoStart=TRUE);
void pauseMusic();
void resumeMusic();
void restartMusic();
void closeMusic();

#endif
