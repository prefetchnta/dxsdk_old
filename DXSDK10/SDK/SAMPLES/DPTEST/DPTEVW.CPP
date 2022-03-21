/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   dptevw.cpp
 *  Content:    implementation of the CDPTestView class
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"

#include "dptedoc.h"
#include "dptevw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDPTestView

IMPLEMENT_DYNCREATE(CDPTestView, CView)

BEGIN_MESSAGE_MAP(CDPTestView, CView)
    //{{AFX_MSG_MAP(CDPTestView)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPTestView construction/destruction

CDPTestView::CDPTestView()
{
    // TODO: add construction code here

}

CDPTestView::~CDPTestView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDPTestView drawing

void CDPTestView::OnDraw(CDC* pDC)
{
    CDPTestDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDPTestView diagnostics

#ifdef _DEBUG
void CDPTestView::AssertValid() const
{
    CView::AssertValid();
}

void CDPTestView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CDPTestDoc* CDPTestView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDPTestDoc)));
    return (CDPTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDPTestView message handlers

