
#pragma once

#define WM_MORECOLOR	WM_APP+100
#define BN_COLORCHANGE	100
#define ON_BN_COLORCHANGE(id, memberFxn) \
	ON_CONTROL(BN_COLORCHANGE, id, memberFxn)

// CColorWnd

class CColorWnd : public CWnd
{
	DECLARE_DYNAMIC(CColorWnd)
	typedef struct tagCOLORITEM
	{
		COLORREF color;
		RECT rect;
		BOOL bCheck;
	}COLORITEM,*LPCOLORITEM;
public:
	CColorWnd();
	virtual ~CColorWnd();
private:
	COLORREF m_CurColor;
	CWnd* m_pParentWnd;
	CToolTipCtrl m_ToolTip;
	bool m_bTracking;
	int m_nCount;
	int m_nHotItem;
	int m_nDownItem;
	int m_nItemX;
	int m_nItemY;
	CArray <COLORITEM,COLORITEM&> m_ItemArray;
	HDC m_hCacheDC;
	HBITMAP m_hCacheBitmap;
	HBITMAP m_hOldBitmap;
	HPEN m_hOrderPen1;
	HPEN m_hOrderPen2;
	HBRUSH m_hBackBrush1;
	HBRUSH m_hBackBrush2;
	HFONT m_hOldFont;
private:
	int CreateItem(COLORREF color,CString strName);
	void UpdateCache();
	void DrawItem(HDC hDC,int nIndex);
	void DrawItem(int nIndex);
	int GetItemIndex(CPoint point);
public:
	BOOL Show(CWnd* pParentWnd,COLORREF clrInit);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


// CColorButton

class CColorButton : public CButton
{
	DECLARE_DYNAMIC(CColorButton)
private:
	CColorWnd m_ColorWnd;
	COLORREF m_CurColor;
	bool m_bOver;
	bool m_bDown;
	bool m_bDisable;
	bool m_bTracking;
public:
	CColorButton();
	virtual ~CColorButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMoreColor(WPARAM wParam, LPARAM lParam);
private:
	void ColorButtonInit();
	void DrawButton();
	void DrawButton(HDC hDestDC);
public:
	COLORREF GetColor( ) const;
	void SetColor(COLORREF newColor);
	afx_msg void OnEnable(BOOL bEnable);
};


