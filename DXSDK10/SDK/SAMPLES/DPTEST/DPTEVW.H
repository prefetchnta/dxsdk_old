/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   dptevw
 *  Content:    interface of the CDPTestView class
 *              
 ***************************************************************************/

class CDPTestView : public CView
{
protected: // create from serialization only
    CDPTestView();
    DECLARE_DYNCREATE(CDPTestView)

// Attributes
public:
    CDPTestDoc* GetDocument();

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDPTestView)
    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    protected:
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CDPTestView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    //{{AFX_MSG(CDPTestView)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in dptevw.cpp
inline CDPTestDoc* CDPTestView::GetDocument()
   { return (CDPTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

