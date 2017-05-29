// 接口说明
// Create				创建文本窗口 
// UpdateLyrics			更新文本(文本,高亮进度百分比)
// Draw					重画文本窗口
// SetLyricsColor		设置文本颜色
// SetLyricsBorder		设置文本边框
// SetHighlightColor	设置高亮文本颜色
// SetHighlightBorder	设置高亮文本边框
// SetLyricsShadow		设置文本阴影
// SetLyricsFont		设置文本字体
// ========================================================

#pragma once

#include <gdiplus.h>
#include "resource.h"
#pragma comment(lib,"GdiPlus.lib")
using namespace Gdiplus; 


//文本渐变模式
enum GradientMode
{
	GradientMode_None		=0, //无渐变
	GradientMode_Two		=1, //两色渐变
	GradientMode_Three	=2 //三色渐变
};

// CTextAeroWindow

class CTextAeroWindow : public CWnd
{
	DECLARE_DYNAMIC(CTextAeroWindow)

public:
	CTextAeroWindow();
	virtual ~CTextAeroWindow();
public:
	//创建窗口 
	BOOL Create();
	BOOL Create(LPCTSTR lpszClassName);
	BOOL Create(LPCTSTR lpszClassName,int nWidth,int nHeight);
public:
	//更新文本(高亮进度百分比)
	void UpdateTextStr(LPCSTR lpszText,float nHighlight);
	void UpdateTextStr(LPCWSTR lpszText,float nHighlight);
	//更新高亮进度(高亮进度百分比)
	void UpdateTextStr(float nHighlight);
	//重画文本窗口
	void Draw();
	//设置文本颜色
	void SetTextStrColor(Gdiplus::Color TextColor1);
	void SetTextStrColor(Gdiplus::Color TextColor1,Gdiplus::Color TextColor2,GradientMode TextGradientMode);
	//设置文本边框
	void SetTextStrBorder(Gdiplus::Color BorderColor,REAL BorderWidth = 1.0f);
	//设置高亮文本颜色
	void SetHighlightColor(Gdiplus::Color TextColor1);
	void SetHighlightColor(Gdiplus::Color TextColor1,Gdiplus::Color TextColor2,GradientMode TextGradientMode);
	//设置高亮文本边框
	void SetHighlightBorder(Gdiplus::Color BorderColor,REAL BorderWidth = 1.0f);
	//设置文本阴影
	void SetTextStrShadow(Gdiplus::Color ShadowColor,int nShadowOffset=2);
	//设置文本字体
	void SetTextStrFont(const WCHAR * familyName,REAL emSize,INT style=FontStyleRegular,Unit unit=UnitPixel);
	//设置对齐方式
	void SetAlignmentMethod(IN StringAlignment align);
	//获得文本宽度，高度
	SizeF GetTextBounds(const Gdiplus::Font* font,const StringFormat& strFormat,const CString& szText);
	BOOL m_bLock;
private:
	//绘制文本
	void DrawTextStr(Gdiplus::Graphics* pGraphics);
	//绘制高亮文本
	void DrawHighlightTextStr(Gdiplus::Graphics* pGraphics,Gdiplus::GraphicsPath* pPath,RectF& dstRect);
	//创建渐变画刷
	Gdiplus::Brush* CreateGradientBrush(GradientMode TextGradientMode,Gdiplus::Color& Color1,Gdiplus::Color& Color2,RectF& dstRect);
	//注册窗口类
	BOOL RegisterWndClass(LPCTSTR lpszClassName);
	
	//HACCEL   hAccel;
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	HDC m_hCacheDC;//缓存DC
	int m_nWidth;
	int m_nHeight;
	LPWSTR m_lpszTextStr;//Unicode格式的文本
	float m_nHighlight;//高亮文本的百分比 0--100
	Gdiplus::Color m_TextColor1;//普通文本颜色,ARGB颜色
	Gdiplus::Color m_TextColor2;//普通文本颜色,ARGB颜色
	GradientMode m_TextGradientMode;//普通文本渐变模式
	Gdiplus::Pen* m_pTextPen;//普通文本边框画笔
	Gdiplus::Color m_HighlightColor1;//高亮文本颜色,ARGB颜色
	Gdiplus::Color m_HighlightColor2;//高亮文本颜色,ARGB颜色
	GradientMode m_HighlightGradientMode;//高亮文本渐变模式
	Gdiplus::Pen*m_pHighlightPen;//高亮文本边框画笔
	Gdiplus::Brush* m_pShadowBrush;//阴影画刷,GDIPlus画刷 
	int m_nShadowOffset;//阴影偏移
	Gdiplus::Font* m_pFont;//GDIPlus字体
	int m_FontStyle;
	REAL m_FontSize;
	Gdiplus::FontFamily* m_pFontFamily;
	Gdiplus::StringFormat* m_pTextFormat;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLockOsd();
	afx_msg void OnUnlockOsd();
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateLockOsd(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUnlockOsd(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);  
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


