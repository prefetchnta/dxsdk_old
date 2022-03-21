/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   dptedoc.h
 *  Content:    interface of the CDPTestDoc class
 *              
 ***************************************************************************/

class CDPTestDoc : public CDocument
{
protected: // create from serialization only
    CDPTestDoc();
    DECLARE_DYNCREATE(CDPTestDoc)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDPTestDoc)
    public:
    virtual BOOL OnNewDocument();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CDPTestDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    //{{AFX_MSG(CDPTestDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

