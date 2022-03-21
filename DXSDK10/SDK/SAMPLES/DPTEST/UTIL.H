/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   util.h
 *  Content:    header file for utilities needed in a few other files
 *              
 ***************************************************************************/

#include <stdarg.h>

extern void MsgPrintf(PCSTR pcsz, ...);
extern void Dprintf( PCSTR pcsz, ...);
extern char *GetDPErrorString( HRESULT error );


