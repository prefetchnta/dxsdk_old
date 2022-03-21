/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   dptedoc.cpp 
 *  Content:    implementation of the CDPTestDoc class
 *              
 ***************************************************************************/

#include "stdafx.h"
#include "dptest.h"

#include "dptedoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDPTestDoc

IMPLEMENT_DYNCREATE(CDPTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CDPTestDoc, CDocument)
    //{{AFX_MSG_MAP(CDPTestDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPTestDoc construction/destruction

CDPTestDoc::CDPTestDoc()
{
    // TODO: add one-time construction code here

}

CDPTestDoc::~CDPTestDoc()
{
}

BOOL CDPTestDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDPTestDoc serialization

void CDPTestDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}

/////////////////////////////////////////////////////////////////////////////
// CDPTestDoc diagnostics

#ifdef _DEBUG
void CDPTestDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CDPTestDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDPTestDoc commands

