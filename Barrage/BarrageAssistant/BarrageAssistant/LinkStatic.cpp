// LinkStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "LinkStatic.h"


// CLinkStatic

IMPLEMENT_DYNAMIC(CLinkStatic, CStatic)

CLinkStatic::CLinkStatic()
{
	m_bkColor=GetSysColor(COLOR_BTNFACE);
	m_TextColor=0xFF0000;
	m_hCursor=::LoadCursor(NULL,IDC_HAND);

	HFONT hFont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;
	ZeroMemory(&lf,sizeof(LOGFONT));
	GetObjectW(hFont,sizeof(LOGFONT),&lf);
	lf.lfUnderline=1;
	m_Font.CreateFontIndirect(&lf);
}

CLinkStatic::~CLinkStatic()
{
}


BEGIN_MESSAGE_MAP(CLinkStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



void CLinkStatic::SetLink(LPCTSTR lpszLinkURL,LPCTSTR lpszToolTip)
{
	m_strLinkURL=CString(lpszLinkURL);
	if(lpszToolTip){
		if(!IsWindow(m_ToolTip.m_hWnd)){
			m_ToolTip.Create(this,TTS_ALWAYSTIP|TTS_NOPREFIX);
			EnableToolTips(TRUE);
		}
		m_ToolTip.AddTool(this,lpszToolTip);
	}else{
		if(IsWindow(m_ToolTip.m_hWnd)){
			m_ToolTip.DestroyWindow();
		}
	}

}

BOOL CLinkStatic::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bRet=CStatic::PreCreateWindow(cs);
	if(bRet){
		ModifyStyle(NULL,SS_NOTIFY);
	}
	return CStatic::PreCreateWindow(cs);
}

void CLinkStatic::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
	if(IsWindow(m_hWnd)){
		ModifyStyle(NULL,SS_NOTIFY);
	}
}

BOOL CLinkStatic::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}

void CLinkStatic::OnPaint()
{
	CRect rcClient;
	GetClientRect(rcClient);
	CString strText;
	GetWindowText(strText);
	CPaintDC dc(this); // device context for painting
	CDC memDC;
	CBitmap memBitmap;
	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());
	CBitmap* pOldBitmap=memDC.SelectObject(&memBitmap);
	CFont* pOldFont=memDC.SelectObject(&m_Font);
	memDC.FillSolidRect(rcClient,m_bkColor);
	memDC.SetBkMode(TRANSPARENT);
	memDC.SetTextColor(m_TextColor);
	memDC.DrawText(strText,-1,rcClient,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&memDC,0,0,SRCCOPY);
	pOldBitmap=memDC.SelectObject(pOldBitmap);
	pOldFont=memDC.SelectObject(pOldFont);
	memDC.DeleteDC();
	memBitmap.DeleteObject();
}

BOOL CLinkStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	::SetCursor(m_hCursor);   
	return TRUE;

	//return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CLinkStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CStatic::OnLButtonDown(nFlags, point);
}

void CLinkStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStatic::OnLButtonUp(nFlags, point);
	OpenLink();
}
void CLinkStatic::OpenLink()
{
	CString strLinkURL(_T(""));
	if(m_strLinkURL!=_T(""))
	{
		strLinkURL=m_strLinkURL;
	}else{
		CString strText;
		GetWindowText(strText);
		strLinkURL=strText;
		strText.MakeLower();
		if(strText.Left(7)!="http://"){
			strLinkURL=_T("");
		}

	}
	if(strLinkURL!=_T(""))
	{
		ShellExecute(NULL,_T("Open"),strLinkURL,NULL, NULL, SW_SHOW);
	}

}
BOOL CLinkStatic::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		{
			if(IsWindow(m_ToolTip.m_hWnd)){
				m_ToolTip.RelayEvent(pMsg); 
			}
			break;
		}
	}

	return CStatic::PreTranslateMessage(pMsg);
}
