#pragma once


// CLinkStatic

class CLinkStatic : public CStatic
{
	DECLARE_DYNAMIC(CLinkStatic)

public:
	CLinkStatic();
	virtual ~CLinkStatic();
public:
	void SetLink(LPCTSTR lpszLinkURL,LPCTSTR lpszToolTip=NULL);
private:
	CFont m_Font;
	COLORREF m_bkColor;
	COLORREF m_TextColor;
	HCURSOR	m_hCursor;
	CString m_strLinkURL;
	CToolTipCtrl m_ToolTip;
private:
	void OpenLink();
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


