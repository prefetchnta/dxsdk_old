/*===========================================================================*\
|
|  File:        cgupdate.h
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

#ifndef CGUPDATE_H
#define CGUPDATE_H

#include <linklist.h>

// NOTE: all update rectangles are inclusive; both coordinates in the
// RECT are included in the update
class CGameUpdateList
{
public:
    CGameUpdateList();
    virtual ~CGameUpdateList();

    virtual void AddRect( RECT Rect );      // add to update list
    virtual BOOL Intersect( RECT Rect );        // find out if rectangle intersects us...
    virtual void Clear() {                      // remove all rectangles
        mRect.top = mRect.bottom = mRect.left = mRect.right = 0;
        mDirty = FALSE;
    };                      
    virtual LPRECT GetDirtyRect()
    {
        return &mRect;
    }

protected:
    RECT    mRect;
    BOOL    mDirty;
};

#endif // CGUPDATE_H

