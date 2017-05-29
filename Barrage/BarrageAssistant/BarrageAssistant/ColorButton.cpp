// ColorButton.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorButton.h"
#include <uxtheme.h>

#pragma comment(lib,"UxTheme.lib")
// CColorButton

IMPLEMENT_DYNAMIC(CColorButton, CButton)

CColorButton::CColorButton()
{
	m_CurColor=0x000000;
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CColorButton::OnBnClicked)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MORECOLOR, OnMoreColor)
	ON_WM_ENABLE()
END_MESSAGE_MAP()



// CColorButton 消息处理程序



void CColorButton::OnBnClicked()
{
	if(IsWindow(m_ColorWnd.m_hWnd)){
		m_ColorWnd.PostMessage(WM_CLOSE);
	}else{
		m_ColorWnd.Show(this,m_CurColor);
	}
}

BOOL CColorButton::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL bRet=CButton::PreCreateWindow(cs);
	ColorButtonInit();
	return bRet;
}

void CColorButton::PreSubclassWindow()
{
	CButton::PreSubclassWindow();
	ColorButtonInit();
}
void CColorButton::ColorButtonInit()
{
	m_bTracking=false;
	m_bOver=m_bDown=m_bDisable=false;
	m_bDisable=IsWindowEnabled()?FALSE:TRUE;
	ModifyStyle(NULL,BS_OWNERDRAW);
}
void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	DrawButton(lpDrawItemStruct->hDC);
}

void CColorButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		m_bOver = TRUE;
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 50;
		m_bTracking = (bool)_TrackMouseEvent(&tme);
	}

	CButton::OnMouseMove(nFlags, point);
}

void CColorButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDown=TRUE;

	CButton::OnLButtonDown(nFlags, point);
}

void CColorButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDown=FALSE;
	CButton::OnLButtonUp(nFlags, point);
}
LRESULT CColorButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bOver = FALSE;
	m_bTracking = FALSE;
	m_bDown=FALSE;
	InvalidateRect(NULL, FALSE);
	return 0;
}

LRESULT CColorButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_bOver = TRUE;
	InvalidateRect(NULL);
	return 0;
}
LRESULT CColorButton::OnMoreColor(WPARAM wParam, LPARAM lParam)
{
	CColorDialog dlg(m_CurColor,0,this);
	if(dlg.DoModal()==IDOK){
		SetColor(dlg.GetColor());
	}
	return 0;
}
void CColorButton::DrawButton()
{
	HDC hDC=::GetDC(m_hWnd);
	DrawButton(hDC);
	::ReleaseDC(m_hWnd,hDC);
}
void CColorButton::DrawButton(HDC hDestDC)
{
	CRect rc;
	GetClientRect(rc);
	int nWindth=rc.Width();
	int nHeight=rc.Height();
	HDC hDC=CreateCompatibleDC(hDestDC);//创建兼容DC,采用双缓冲画出
	HBITMAP hBitmap=CreateCompatibleBitmap(hDestDC,nWindth,nHeight);
	HBITMAP hOldBitmap=(HBITMAP)SelectObject(hDC,hBitmap);
	//画出整个控件背景-------------------------------------------------
	HBRUSH hbr=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	FillRect(hDC,&rc,hbr);
	DeleteObject(hbr);
	HTHEME hTheme=OpenThemeData(m_hWnd,L"Button");
	if(hTheme){
		if(m_bDisable){
			DrawThemeBackground (hTheme,hDC, BP_PUSHBUTTON, PBS_DISABLED,&rc,NULL);
		}else if(m_bDown){
			DrawThemeBackground (hTheme,hDC, BP_PUSHBUTTON, PBS_PRESSED,&rc,NULL);
		}else if(m_bOver){
			DrawThemeBackground (hTheme,hDC, BP_PUSHBUTTON, PBS_HOT,&rc,NULL);
		}else{
			DrawThemeBackground (hTheme,hDC, BP_PUSHBUTTON, PBS_NORMAL,&rc,NULL);
		}
		CloseThemeData (hTheme);
	}else{
		if(m_bDisable){
			DrawFrameControl (hDC,rc,DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE);
		}else if(m_bDown){
			DrawFrameControl (hDC,rc,DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED);
		}else if(m_bOver){
			DrawFrameControl (hDC,rc,DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_HOT);
		}else{
			DrawFrameControl (hDC,rc,DFC_BUTTON, DFCS_BUTTONPUSH);
		}
		
	}
	//画出颜色显示区-------------------------------------------------
	rc=CRect(4,4,nWindth-17,nHeight-4);
	hbr=CreateSolidBrush(0xFFFFFF);//颜色显示区的背景
	FillRect(hDC,&rc,hbr);
	DeleteObject(hbr);
	HPEN hPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOWFRAME));//颜色显示区的边框
	FrameRect(hDC,&rc,(HBRUSH)hPen);
	DeleteObject(hPen);
	rc.InflateRect(-2,-2);
	hbr=CreateSolidBrush(m_CurColor);//当前的颜色
	FillRect(hDC,&rc,hbr);
	DeleteObject(hbr);
	//画出下拉的小三角-------------------------------------------------
	int w=7;
	int h=4;
	int x=nWindth-w-7;
	int y=(nHeight-h)/2;
	for(int i=0;i<h;i++){
		MoveToEx(hDC,x,y,NULL);
		LineTo(hDC,x+w,y);
		x++;
		y++;
		w=w-2;
	}
	//复制到控件的DC上------------------------
	BitBlt(hDestDC,0,0,nWindth,nHeight,hDC,0,0,SRCCOPY);
	//删除资源,释放内存-----------------------
	SelectObject(hDC,hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hDC);

}
void CColorButton::OnEnable(BOOL bEnable)
{
	CButton::OnEnable(bEnable);
	m_bDisable=IsWindowEnabled()?FALSE:TRUE;
	DrawButton();
}
COLORREF CColorButton::GetColor( ) const
{
	return m_CurColor;
}
void CColorButton::SetColor(COLORREF newColor)
{
	if(m_CurColor==newColor)return;
	m_CurColor=newColor;
	if(IsWindow(m_hWnd)){
		DrawButton();
		CWnd* pParent=GetParent();
		if(pParent){
			pParent->PostMessage(WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(),BN_COLORCHANGE),(LPARAM)m_hWnd);
		}
	}
}


// CColorWnd

IMPLEMENT_DYNAMIC(CColorWnd, CWnd)

CColorWnd::CColorWnd()
{
	m_nCount=m_nItemX=m_nItemY=0;
	m_hCacheDC=NULL;
	m_hCacheBitmap=m_hOldBitmap=NULL;
	m_hOrderPen1=m_hOrderPen2=NULL;
	m_hBackBrush1=NULL;
	m_bTracking=false;
	m_nHotItem=m_nDownItem=-1;
}

CColorWnd::~CColorWnd()
{
}


BEGIN_MESSAGE_MAP(CColorWnd, CWnd)
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
END_MESSAGE_MAP()



// CColorWnd 消息处理程序

BOOL CColorWnd::Show(CWnd* pParentWnd,COLORREF clrInit)
{
	m_pParentWnd=pParentWnd;
	m_CurColor=clrInit;
	int nWidth=126;
	int nHeight=121;
	CRect rc;
	SystemParametersInfo(SPI_GETWORKAREA,NULL,&rc,0);
	int x=(rc.Width()-nWidth)/2;
	int y=(rc.Height()-nHeight)/2;
	if(m_pParentWnd){
		m_pParentWnd->GetWindowRect(rc);
		x=rc.left;
		y=rc.bottom;
	}
	DWORD dwStyle=WS_POPUP|WS_DLGFRAME;
	if(!CreateEx(NULL,AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_PARENTDC),NULL,dwStyle,CRect(x,y,x+nWidth,y+nHeight),m_pParentWnd,NULL)){
		return FALSE;
	}
	GetWindowRect(&rc);
	int w=rc.Width();
	int h=rc.Height();
	GetClientRect(&rc);
	SetWindowPos(NULL,0,0,w-rc.Width()+nWidth,h-rc.Height()+nHeight,SWP_NOZORDER|SWP_NOMOVE);
	//---------------------------------
	m_ToolTip.Create(this);
	EnableToolTips(TRUE);
	//---------------------------------
	CreateItem(0x000000,_T("黑色"));
	CreateItem(0x800000,_T("藏青"));
	CreateItem(0x008000,_T("墨绿"));
	CreateItem(0x808000,_T("深青"));
	CreateItem(0x000080,_T("红褐"));
	CreateItem(0x800080,_T("紫红"));
	CreateItem(0x008080,_T("褐绿"));

	CreateItem(0xC0C0C0,_T("浅灰"));
	CreateItem(0x808080,_T("灰色"));
	CreateItem(0xFF0000,_T("蓝色"));
	CreateItem(0x00FF00,_T("绿色"));
	CreateItem(0xFFFF00,_T("艳青"));
	CreateItem(0x0000FF,_T("红色"));
	CreateItem(0xFF00FF,_T("品红"));

	CreateItem(0x00FFFF,_T("黄色"));
	CreateItem(0xFFFFFF,_T("白色"));
	CreateItem(0xFF8080,_T("蓝灰"));
	CreateItem(0xE03058,_T("藏蓝"));
	CreateItem(0x00E080,_T("嫩绿"));
	CreateItem(0x80E000,_T("青绿"));
	CreateItem(0x0060C0,_T("黄褐"));

	CreateItem(0xFFA8FF,_T("粉红"));
	CreateItem(0x00D8D8,_T("嫩黄"));
	CreateItem(0xECECEC,_T("银白"));
	CreateItem(0xFF0090,_T("紫色"));
	CreateItem(0xFF8800,_T("天蓝"));
	CreateItem(0x80A080,_T("灰绿"));
	CreateItem(0xC06000,_T("青蓝"));

	CreateItem(0x0080FF,_T("橙黄"));
	CreateItem(0x8050FF,_T("桃红"));
	CreateItem(0xC080FF,_T("芙红"));
	CreateItem(0x606060,_T("深灰"));
	//---------------------------------
	m_nItemX=1;
	m_nItemY=nHeight-24;
	COLORITEM item;
	item.bCheck=FALSE;
	item.color=-1;
	item.rect=CRect(m_nItemX,m_nItemY,m_nItemX+124,m_nItemY+21);
	m_ItemArray.Add(item);
	m_nCount++;
	//---------------------------------
	UpdateCache();
	ShowWindow(SW_SHOW);
	return TRUE;
}
void CColorWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);

	if(nState==WA_INACTIVE){
		::PostMessage(m_hWnd,WM_CLOSE,0,0);
	}
}
int CColorWnd::CreateItem(COLORREF color,CString strName)
{
	int nIndex=m_nCount;
	COLORITEM item;
	item.bCheck=color==m_CurColor;
	item.color=color;
	item.rect=CRect(m_nItemX,m_nItemY,m_nItemX+18,m_nItemY+18);
	m_ItemArray.Add(item);
	m_nCount++;
	m_nItemX+=18;
	if(m_nCount%7==0){
		m_nItemY+=18;
		m_nItemX=0;
	}
	//--------------
	m_ToolTip.AddTool(this,strName,&item.rect,100+nIndex);
	return nIndex;
}
void CColorWnd::UpdateCache()
{
	CRect rc;
	GetClientRect(rc);
	int nWindth=rc.Width();
	int nHeight=rc.Height();
	HDC hDC=::GetDC(m_hWnd);
	m_hCacheDC=CreateCompatibleDC(hDC);//创建兼容DC,采用双缓冲画出
	m_hCacheBitmap=CreateCompatibleBitmap(hDC,nWindth,nHeight);
	m_hOldBitmap=(HBITMAP)SelectObject(m_hCacheDC,m_hCacheBitmap);	
	m_hOldFont=(HFONT)SelectObject(m_hCacheDC,(HFONT)GetStockObject(DEFAULT_GUI_FONT));	
	SetBkMode(m_hCacheDC,TRANSPARENT);
	
	m_hOrderPen1=CreatePen(PS_SOLID,1,0xA0A0A0);
	m_hOrderPen2=CreatePen(PS_SOLID,1,0xFFFFFF);
	m_hBackBrush1=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	m_hBackBrush2=CreateSolidBrush(0xFFFFFF);
	//----------------------------
	FillRect(m_hCacheDC,&rc,m_hBackBrush1);
	int x=4;
	int y=nHeight-29;
	HPEN hOldPen=(HPEN)SelectObject(m_hCacheDC,m_hOrderPen1);
	MoveToEx(m_hCacheDC,x,y,NULL);
	LineTo(m_hCacheDC,nWindth-x,y);
	SelectObject(m_hCacheDC,hOldPen);
	y+=1;
	hOldPen=(HPEN)SelectObject(m_hCacheDC,m_hOrderPen2);
	MoveToEx(m_hCacheDC,x,y,NULL);
	LineTo(m_hCacheDC,nWindth-x,y);
	SelectObject(m_hCacheDC,hOldPen);
	//----------------------------	
	for(int i=0;i<m_nCount;i++){
		DrawItem(m_hCacheDC,i);
	}
	//----------------------------
	BitBlt(hDC,0,0,nWindth,nHeight,m_hCacheDC,0,0,SRCCOPY);
	::ReleaseDC(m_hWnd,hDC);
}
void CColorWnd::DrawItem(int nIndex)
{
	if(nIndex==-1)return;
	DrawItem(m_hCacheDC,nIndex);
	HDC hDC=::GetDC(m_hWnd);
	COLORITEM item=m_ItemArray.GetAt(nIndex);
	int nWindth=item.rect.right-item.rect.left;
	int nHeight=item.rect.bottom-item.rect.top;
	BitBlt(hDC,item.rect.left,item.rect.top,nWindth,nHeight,m_hCacheDC,item.rect.left,item.rect.top,SRCCOPY);
	::ReleaseDC(m_hWnd,hDC);
}
void CColorWnd::DrawItem(HDC hDC,int nIndex)
{
	COLORITEM item=m_ItemArray.GetAt(nIndex);
	HBRUSH hbr=m_hBackBrush1;
	if(item.color!=-1 && item.bCheck){
		hbr=m_hBackBrush2;
	}
	FillRect(m_hCacheDC,&item.rect,hbr);
	//----------------------------
	if(m_nDownItem==nIndex || item.bCheck){
		HPEN hOldPen=(HPEN)SelectObject(m_hCacheDC,m_hOrderPen1);
		MoveToEx(m_hCacheDC,item.rect.left,item.rect.top,NULL);
		LineTo(m_hCacheDC,item.rect.right-1,item.rect.top);
		MoveToEx(m_hCacheDC,item.rect.left,item.rect.top,NULL);
		LineTo(m_hCacheDC,item.rect.left,item.rect.bottom-1);
		SelectObject(m_hCacheDC,hOldPen);
		hOldPen=(HPEN)SelectObject(m_hCacheDC,m_hOrderPen2);
		MoveToEx(m_hCacheDC,item.rect.right-1,item.rect.top,NULL);
		LineTo(m_hCacheDC,item.rect.right-1,item.rect.bottom-1);
		MoveToEx(m_hCacheDC,item.rect.left,item.rect.bottom-1,NULL);
		LineTo(m_hCacheDC,item.rect.right-1,item.rect.bottom-1);
		SelectObject(m_hCacheDC,hOldPen);
	}else if(m_nHotItem==nIndex){
		HPEN hOldPen=(HPEN)SelectObject(m_hCacheDC,m_hOrderPen2);
		MoveToEx(m_hCacheDC,item.rect.left,item.rect.top,NULL);
		LineTo(m_hCacheDC,item.rect.right-1,item.rect.top);
		MoveToEx(m_hCacheDC,item.rect.left,item.rect.top,NULL);
		LineTo(m_hCacheDC,item.rect.left,item.rect.bottom-1);
		SelectObject(m_hCacheDC,hOldPen);
		hOldPen=(HPEN)SelectObject(m_hCacheDC,m_hOrderPen1);
		MoveToEx(m_hCacheDC,item.rect.right-1,item.rect.top,NULL);
		LineTo(m_hCacheDC,item.rect.right-1,item.rect.bottom-1);
		MoveToEx(m_hCacheDC,item.rect.left,item.rect.bottom-1,NULL);
		LineTo(m_hCacheDC,item.rect.right-1,item.rect.bottom-1);
		SelectObject(m_hCacheDC,hOldPen);
	}
	//----------------------------
	if(item.color!=-1){
		CRect rc(item.rect);
		rc.InflateRect(-3,-3);	
		hbr=CreateSolidBrush(item.color);
		FillRect(hDC,&rc,hbr);
		DeleteObject(hbr);
		FrameRect(hDC,&rc,(HBRUSH)m_hOrderPen1);
	}else{
		DrawText(hDC,_T("其他颜色..."),-1,&item.rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}
}
BOOL CColorWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	int nWindth=rc.Width();
	int nHeight=rc.Height();
	BitBlt(pDC->m_hDC,0,0,nWindth,nHeight,m_hCacheDC,0,0,SRCCOPY);
	return TRUE;
	return CWnd::OnEraseBkgnd(pDC);
}

void CColorWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(rc);
	int nWindth=rc.Width();
	int nHeight=rc.Height();
	BitBlt(dc.m_hDC,0,0,nWindth,nHeight,m_hCacheDC,0,0,SRCCOPY);
}

void CColorWnd::OnDestroy()
{
	CWnd::OnDestroy();
	//---------------
	m_ItemArray.RemoveAll();
	m_nCount=m_nItemX=m_nItemY=0;
	//---------------
	if(m_hCacheDC){
		SelectObject(m_hCacheDC,m_hOldFont);
		SelectObject(m_hCacheDC,m_hOldBitmap);
		DeleteObject(m_hCacheBitmap);
		DeleteDC(m_hCacheDC);
	}
	DeleteObject(m_hOrderPen1);
	DeleteObject(m_hOrderPen2);
	DeleteObject(m_hBackBrush1);
	DeleteObject(m_hBackBrush2);
	m_hCacheDC=NULL;
	m_hCacheBitmap=m_hOldBitmap=NULL;
	m_hOrderPen1=m_hOrderPen2=NULL;
	m_hBackBrush1=NULL;
	m_bTracking=false;
	m_nHotItem=m_nDownItem=-1;
}
void CColorWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 50;
		m_bTracking = (bool)_TrackMouseEvent(&tme);
	}
	CWnd::OnMouseMove(nFlags, point);
	//-------------------------------------
	int nIndex=GetItemIndex(point);
	if(nIndex!=m_nHotItem){
		int nOldIndex=m_nHotItem;
		m_nHotItem=nIndex;
		DrawItem(nOldIndex);
		DrawItem(nIndex);
	}
}

void CColorWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	//-------------------------------------
	int nIndex=GetItemIndex(point);
	if(nIndex!=m_nDownItem){
		int nOldIndex=m_nDownItem;
		m_nDownItem=nIndex;
		DrawItem(nOldIndex);
		DrawItem(nIndex);
	}
}

void CColorWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
	int nIndex=GetItemIndex(point);
	if(m_nDownItem!=-1){
		if(m_nDownItem==nIndex){
			COLORITEM item=m_ItemArray.GetAt(nIndex);
			if(item.color==-1){
				m_pParentWnd->PostMessage(WM_MORECOLOR);
			}else{
				((CColorButton*)m_pParentWnd)->SetColor(item.color);
			}
			::PostMessage(m_hWnd,WM_CLOSE,0,0);
		}
		//int nOldIndex=m_nDownItem;
		//m_nDownItem=-1;
		//DrawItem(nOldIndex);
	}
}
LRESULT CColorWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = false;
	if(m_nDownItem!=-1){
		int nOldIndex=m_nDownItem;
		m_nDownItem=-1;
		DrawItem(nOldIndex);
	}
	if(m_nHotItem!=-1){
		int nOldIndex=m_nHotItem;
		m_nHotItem=-1;
		DrawItem(nOldIndex);
	}
	return 0;
}

LRESULT CColorWnd::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
int CColorWnd::GetItemIndex(CPoint point)
{
	for (int i=0;i<m_nCount;i++){
		if(PtInRect(&m_ItemArray[i].rect,point)){
			return i;
			break;
		}
	}
	return -1;
}
BOOL CColorWnd::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg); 
	return CWnd::PreTranslateMessage(pMsg);
}


