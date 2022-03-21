/*===========================================================================*\
|
|  File:        cggraph.cpp
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

#include "cggraph.h"

/*---------------------------------------------------------------------------*\
|
|       CGameGraphic()
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/

CGameGraphic::CGameGraphic(
        int curz
    ) : mIsValid(FALSE),
        mXParallax(1),
        mYParallax(1),
        mpNext( NULL )
{
    mRect.top = mRect.bottom = mRect.left = mRect.right = 0;

    // set parallax factor
    mXParallax = curz >> 4;
//  mXParallax = curz / 8 +1;
    mYParallax = curz >> 4;
//  mYParallax = curz / 8 +1;
}   

CGameGraphic::~CGameGraphic()
{
}   

/*---------------------------------------------------------------------------*\
|
|       class CGameZSet
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameZSet::CGameZSet(
    ) : CGameGraphic(),
        mpGraphicList( NULL )
{
}

CGameZSet::~CGameZSet()
{
}

void
CGameZSet::Add(CGameGraphic* pGraphic)
{
    if (pGraphic->GetMaxZ() > mMaxZ)
    {
        // graphic is out of our range -- insert it normally
        CGameGraphic::Add(pGraphic);
        return;
    }

    // else it's in our range, so put in our private list
    if (mpGraphicList)
    {
        CGameGraphic* pNode = mpGraphicList;
        int lookZ = pGraphic->GetMinZ();

        while (pNode->GetNext() && (pNode->GetMinZ() < lookZ))
        {
            pNode = pNode->GetNext();
        }

        pNode->Add(pGraphic);
    }
    else
    {
        mpGraphicList = pGraphic;
    }
}

