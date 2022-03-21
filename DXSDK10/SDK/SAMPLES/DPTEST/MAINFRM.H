/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   mainfrm.h
 *  Content:    interface of the CMainFrame class
 *              
 ***************************************************************************/

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CStatusBar  m_wndStatusBar;

// Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTestInitialise();
    afx_msg void OnTestNewentity();
    afx_msg void OnTestSend();
    afx_msg void OnTestShutdown();
    afx_msg void OnTestGetnames();
    afx_msg void OnTestOpenchannel();
    afx_msg void OnTestBeginreceive();
    afx_msg void OnTestAbort();
    afx_msg void OnTestInformplayerchange();
    afx_msg void OnTestCreateobject();
    afx_msg void OnTestDestroyplayer();
    afx_msg void OnTestDirectPlayEnumerate();
    afx_msg void OnTestRelease();
    afx_msg void OnGameidAlternateguid();
    afx_msg void OnUpdateGameidAlternateguid(CCmdUI* pCmdUI);
    afx_msg void OnGameidDefaultguid();
    afx_msg void OnUpdateGameidDefaultguid(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTestBeginreceive(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTestDestroyplayer(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTestGetnames(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTestInitialise(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTestNewentity(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTestRelease(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTestSend(CCmdUI* pCmdUI);
    afx_msg void OnUpdateTestShutdown(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

