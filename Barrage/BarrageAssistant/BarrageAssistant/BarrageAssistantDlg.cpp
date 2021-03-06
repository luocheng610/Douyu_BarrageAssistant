
// BarrageAssistantDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BarrageAssistant.h"
#include "BarrageAssistantDlg.h"
#include <kb_lib/text/transcode.h>
#include <kb_lib//text/format_utility.h>
#include "linkstatic.h"

using namespace kb::text;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void* g_MainFrm = NULL;
CString g_BarrageCfgPath;
CString sCachePath;
CString g_PlayingPath;
static int g_nDoneFlag = 0;
int g_nCurrentSongIndex = 0;

BOOL g_bPlay = FALSE;
BOOL g_bPaused = FALSE;
BOOL g_bStop = FALSE;
BOOL g_bThreadPlay = FALSE;
extern HWND g_OsdhWhd;
CString g_sInfo[4];
CURL *g_Updatecurl;
CURL *g_Songcurl;

static int g_nTextindexl = 0;
std::vector<CString> g_NickName;
std::vector<CString> g_Content;
std::vector<CString> g_MusicListInfo;

CString g_sInit = L"\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n";

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CBCGPDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	CLinkStatic m_StDouyu;
	CLinkStatic m_StZhihu;
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CBCGPDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_DOUYU, m_StDouyu);
	DDX_Control(pDX, IDC_ST_ZHIHU, m_StZhihu);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle(TRUE, TRUE);
	m_StDouyu.SetLink(_T("http://www.douyutv.com/sjingdaxia"), _T("http://www.douyutv.com/sjingdaxia"));
	m_StZhihu.SetLink(_T("http://www.zhihu.com/people/shenjingdaxia"), _T("http://www.zhihu.com/people/shenjingdaxia"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// CBarrageAssistantDlg 对话框
CBarrageAssistantDlg::CBarrageAssistantDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CBarrageAssistantDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	EnableVisualManagerStyle(TRUE, TRUE);
	CBCGPVisualManager2013::SetAccentColor((CBCGPVisualManager2013::AccentColor)5);

	m_MainCurl = NULL;
	m_BarrageCurl = NULL;
	m_MainServerThreadHandle = NULL;
	m_MainServerThreadStatus = Thread_Stop;
	m_BarrageServerThreadHandle = NULL;
	m_BarrageServerThreadStatus = Thread_Stop;
	m_UpdateRoomInfoThreadHandle = NULL;
	m_UpdateRoomInfoThreadStatus = Thread_Stop;
	m_bExit = true;
	m_hExitEvent = NULL;
	isNotify = FALSE;
	g_MainFrm = this;
	m_TimeFlag = FALSE;
	m_bFishBall = FALSE;
	m_bBannedToPost = FALSE;
	m_bGift = FALSE;
	m_bGuest = FALSE;
	m_bPraise = FALSE;
	m_bBarrageAssistant = FALSE;
	m_bHeart = FALSE;
	m_b666 = FALSE;
	m_bFbDbHit = FALSE;
	

}

CBarrageAssistantDlg::~CBarrageAssistantDlg()
{
	MainServerStop();
	Shell_NotifyIcon(NIM_DELETE,&m_Notify);
	::TerminateProcess(GetCurrentProcess(), 0);
}

void CBarrageAssistantDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FONTNAME, m_cb_FontFace);
	DDX_Control(pDX, IDC_COMBO_FONTSIZE, m_cb_FontSize);
	DDX_Control(pDX, IDC_COMBO_TEXTGRADIENTMODE, m_cb_TextGradientMode);
	DDX_Control(pDX, IDC_COMBO_TEXTHIGHLINEGRADIENTMODE, m_cb_HighlightGradientMode);
	DDX_Control(pDX, IDC_BT_COLOR1, m_ColorBtn1);
	DDX_Control(pDX, IDC_BT_COLOR2, m_ColorBtn2);
	DDX_Control(pDX, IDC_BT_BORDER, m_ColorBtBorder);
	DDX_Control(pDX, IDC_BT_HIGHLINE1, m_ColorBtnHighLine1);
	DDX_Control(pDX, IDC_BT_HIGHLINE2, m_ColorBtnHighLine2);
	DDX_Control(pDX, IDC_BT_HIGHLINEBORDER, m_ColorBtnHighLineBorder);
	DDX_Control(pDX, IDC_COMBO_LOGIN, m_cb_LoginType);
	DDX_Control(pDX, IDC_EDIT_ROOMID, m_EditID);
	DDX_Control(pDX, IDC_COMBO_ALIGNMENT, m_cb_AlignmentMethod);
	DDX_Control(pDX, IDC_COMBO_BATYPE, m_cb_BaType);
}

BEGIN_MESSAGE_MAP(CBarrageAssistantDlg, CBCGPDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BT_CONNECT_DOUYU, &CBarrageAssistantDlg::OnBnClickedBtConnectDouyu)
	ON_BN_CLICKED(IDC_BT_FONTSET, &CBarrageAssistantDlg::OnBnClickedBtFontset)
	ON_BN_CLICKED(IDC_BT_COLORSET, &CBarrageAssistantDlg::OnBnClickedBtColorset)
	ON_COMMAND(ID_MENU_RESTORE, &CBarrageAssistantDlg::OnMenuRestore)
	ON_COMMAND(ID_MENU_EXIT, &CBarrageAssistantDlg::OnMenuExit)
	ON_MESSAGE(WM_NOTIFYICON,OnShowTrayTipMsg)
	ON_WM_TIMER()
	ON_COMMAND(ID_ABOUTBARRAGE, &CBarrageAssistantDlg::OnAboutbarrage)
	ON_BN_CLICKED(IDC_BT_DISCONNECT_DOUYU, &CBarrageAssistantDlg::OnBnClickedBtDisconnectDouyu)
	ON_CBN_SELCHANGE(IDC_COMBO_LOGIN, &CBarrageAssistantDlg::OnCbnSelchangeComboLogin)
	ON_BN_CLICKED(IDC_BT_ALIGNMENTSET, &CBarrageAssistantDlg::OnBnClickedBtAlignmentset)
	ON_BN_CLICKED(IDC_CHECK_TIMEFLAG, &CBarrageAssistantDlg::OnBnClickedCheckTimeflag)
	ON_BN_CLICKED(IDC_CHECK_OSDLOCK, &CBarrageAssistantDlg::OnBnClickedCheckOsdlock)
	ON_BN_CLICKED(IDC_CHECK_FISH_BALL, &CBarrageAssistantDlg::OnBnClickedCheckFishBall)
	ON_BN_CLICKED(IDC_CHECK_BANNED, &CBarrageAssistantDlg::OnBnClickedCheckBanned)
	ON_BN_CLICKED(IDC_CHECK_GIFT, &CBarrageAssistantDlg::OnBnClickedCheckGift)
	ON_BN_CLICKED(IDC_CHECK_GUEST, &CBarrageAssistantDlg::OnBnClickedCheckGuest)
	ON_WM_INITMENUPOPUP()
	ON_BN_CLICKED(IDC_CHECK_GDN, &CBarrageAssistantDlg::OnBnClickedCheckGdn)
	ON_BN_CLICKED(IDC_CHECK_BA, &CBarrageAssistantDlg::OnBnClickedCheckBa)
	ON_BN_CLICKED(IDC_CHECK_HEART, &CBarrageAssistantDlg::OnBnClickedCheckHeart)
	ON_BN_CLICKED(IDC_CHECK_666, &CBarrageAssistantDlg::OnBnClickedCheck666)
	ON_MESSAGE(MSG_MOVEPANEL, MsgAutoMoveLocalRect)
	ON_BN_CLICKED(IDC_CHECK_FISH_DBHIT, &CBarrageAssistantDlg::OnBnClickedCheckFishDbhit)
END_MESSAGE_MAP()


// CBarrageAssistantDlg 消息处理程序

BOOL CBarrageAssistantDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CString sTempInit;
	sTempInit.Format(L"%s%s", g_sInit, L"房间未连接(鼠标放在当前,可拖动OSD，返回软件界面锁定/解锁)");
	//OSD(主弹幕UI)
	m_TextOSD.Create(L"text", 500, 800);
	m_TextOSD.UpdateTextStr(sTempInit, 0);

	//鱼丸连击 100起x
	m_TextFbDbHitOSD.Create(L"FishBall", 500, 30);
	m_TextFbDbHitOSD.UpdateTextStr(L"", 0);
	//心连击	520起x
	m_TextHeartOSD.Create(L"Heart", 500, 30);
	m_TextHeartOSD.UpdateTextStr(L"", 0);
	//赞
	m_TextPraiseOSD.Create(L"Praise", 500, 30);
	m_TextPraiseOSD.UpdateTextStr(L"", 0);
	//666
	m_TextSoGoodOSD.Create(L"66666", 500, 30);
	m_TextSoGoodOSD.UpdateTextStr(L"", 0);
	//单个100鱼丸
	m_TextSingleHitOSD.Create(L"SingleFB", 500, 30);
	m_TextSingleHitOSD.UpdateTextStr(L"", 0);
	//RoomIdInfo
	m_TextRoomInfoOSD.Create(L"RoomInfo", 200, 90);
	//来宾与禁言
	m_TextGuestOSD.Create(L"Guest", 500, 30);
	m_TextGuestOSD.UpdateTextStr(L"", 0);
	CString sTemp;
	CString sFans, sWeight, sOnline;

	sFans.Format(L"关注:%s\r\n", L"--");
	//sWeight.Format(L"体重:%s\r\n", L"--");
	sOnline.Format(L"人数:%s\r\n", L"--");
	sTemp.Append(sFans);
	//sTemp.Append(sWeight);
	sTemp.Append(sOnline);

	m_TextRoomInfoOSD.UpdateTextStr(sTemp, 0);

	g_OsdhWhd = this->m_hWnd;
	//////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 15; i++)
	{
		//CString tmp;
		//tmp.Format(L"%d", i);
		m_TextPanelOSD[i].Create(L" ", 150, 35);
		m_TextPanelOSD[i].UpdateTextStr(L"", 0);
	}
	for (int i = 15; i < 30; i++)
	{
		//CString tmp;
		//tmp.Format(L"%d", i);
		m_TextPanelOSD[i].Create(L" ", 400, 35);
		m_TextPanelOSD[i].UpdateTextStr(L"", 0);
	}
	//////////////////////////////////////////////////////////////////////////

	MoveOsd();

	SetTimer(0, 5000, NULL);
	SetTimer(1, 5000, NULL);
	SetTimer(2, 6000, NULL);
	SetTimer(3, 7000, NULL);
	SetTimer(4, 8000, NULL);
	SetTimer(5, 4000, NULL);
	SetTimer(6, 3000, NULL);
	SetTimer(7, 20000, NULL);

	m_cb_LoginType.AddString(L"用户ID");
	m_cb_LoginType.AddString(L"房间ID");
	m_cb_LoginType.SetCurSel(0);
	//OSD_字体&颜色
	m_cb_FontFace.AddString(_T("黑体"));
	m_cb_FontFace.AddString(_T("楷体"));
	m_cb_FontFace.AddString(_T("微软雅黑"));
	m_cb_FontFace.SetCurSel(0);
	m_cb_FontSize.AddString(_T("8"));
	m_cb_FontSize.AddString(_T("10"));
	m_cb_FontSize.AddString(_T("15"));
	m_cb_FontSize.AddString(_T("20"));
	m_cb_FontSize.AddString(_T("25"));
	m_cb_FontSize.AddString(_T("30"));
	m_cb_FontSize.AddString(_T("35"));
// 	m_cb_FontSize.AddString(_T("40"));
// 	m_cb_FontSize.AddString(_T("45"));
// 	m_cb_FontSize.AddString(_T("50"));
// 	m_cb_FontSize.AddString(_T("55"));
// 	m_cb_FontSize.AddString(_T("60"));
	m_cb_FontSize.SetCurSel(2);
	m_cb_TextGradientMode.AddString(_T("无渐变"));
	m_cb_TextGradientMode.AddString(_T("两色渐变"));
	m_cb_TextGradientMode.AddString(_T("三色渐变"));
	m_cb_TextGradientMode.SetCurSel(1);
	m_cb_HighlightGradientMode.AddString(_T("无渐变"));
	m_cb_HighlightGradientMode.AddString(_T("两色渐变"));
	m_cb_HighlightGradientMode.AddString(_T("三色渐变"));
	m_cb_HighlightGradientMode.SetCurSel(1);

	m_cb_BaType.AddString(L"普通弹幕");
	m_cb_BaType.AddString(L"昵称弹幕");
	m_cb_BaType.AddString(L"单次鱼丸");
	m_cb_BaType.AddString(L"连击鱼丸");
	m_cb_BaType.AddString(L"连击点赞");
	m_cb_BaType.AddString(L"连击很溜");
	m_cb_BaType.AddString(L"连击爱心");
	m_cb_BaType.SetCurSel(0);


	m_cb_AlignmentMethod.AddString(L"居中");
	m_cb_AlignmentMethod.AddString(L"靠左");
	m_cb_AlignmentMethod.AddString(L"靠右");
	m_cb_AlignmentMethod.SetCurSel(1);


	m_ColorBtn1.SetColor(0x0000FF);
	m_ColorBtn2.SetColor(0x00FFFF);
	m_ColorBtBorder.SetColor(0x000000);
	m_ColorBtnHighLine1.SetColor(0xFF8800);
	m_ColorBtnHighLine2.SetColor(0xFFFF00);
	m_ColorBtnHighLineBorder.SetColor(0x000000);

	//Create TrayIcon
	if (!isNotify)
	{
		m_Notify.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
		m_Notify.hWnd = this->m_hWnd;
		m_Notify.uID = IDR_BARRAGE_MENU;
		m_Notify.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO;
		m_Notify.uCallbackMessage = WM_NOTIFYICON;//自定义的消息名称
		m_Notify.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		wcscpy_s(m_Notify.szTip,_T("BarrageAssistan"));//信息提示条为"计划任务提醒"
		wcscpy_s(m_Notify.szInfoTitle,_T(""));
		wcscpy_s(m_Notify.szInfo,_T(""));

		m_Notify.dwInfoFlags = NIIF_INFO;
		m_Notify.uTimeout = 5000;		
		Shell_NotifyIcon(NIM_ADD,&m_Notify);//在托盘区添加图标

	}

	GetDlgItem(IDC_CHECK_TIMEFLAG)->ShowWindow(SW_SHOW);
	UpdateFontSet();
	GetDlgItem(IDC_BT_DISCONNECT_DOUYU)->EnableWindow(FALSE);
	EnableDebugPriv();
	GetDlgItem(IDC_ST_ROOMID)->SetWindowText(L"--");
	//GetDlgItem(IDC_ST_WEIGHT)->SetWindowText(L"--");
	GetDlgItem(IDC_ST_ONLINENUMS)->SetWindowText(L"--");

	GetDlgItem(IDC_ST_WEIGHT)->ShowWindow(SW_HIDE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBarrageAssistantDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	if (nID == SC_MINIMIZE)		//最小化到托盘
	{
		CWnd *TmpWnd;
		TmpWnd = FromHandle(this->GetSafeHwnd());
		TmpWnd->ShowWindow(SW_HIDE);
	}
	else
	{
		CBCGPDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBarrageAssistantDlg::OnAboutbarrage()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CBarrageAssistantDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBCGPDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBarrageAssistantDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CBarrageAssistantDlg::GetTimeStr()
{
	SYSTEMTIME tm;
	CString m_time,tempstr;
	GetLocalTime(&tm);
	//tempstr.Format(L"_%d年",tm.wYear);
	//m_time=tempstr;
	//tempstr.Format(L"%d月",tm.wMonth);
	//m_time+=tempstr;
	//tempstr.Format(L"%d日",tm.wDay);
	//m_time+=tempstr;
	tempstr.Format(L"%d:",tm.wHour);
	m_time+=tempstr;
	tempstr.Format(L"%d:",tm.wMinute);
	m_time+=tempstr;
	tempstr.Format(L"%d",tm.wSecond);
	m_time+=tempstr;
// 	tempstr.Format(L"%d毫秒",tm.wMilliseconds);
// 	m_time+=tempstr;

	return m_time;
}

void CBarrageAssistantDlg::OnCbnSelchangeComboLogin()
{
	CString sTmpLoginType;
	m_cb_LoginType.GetWindowText(sTmpLoginType);

	if (sTmpLoginType == L"用户ID")
	{
		DWORD   dwStyle = m_EditID.GetStyle();
		dwStyle &= ~ES_NUMBER;
		SetWindowLong(m_EditID.m_hWnd, GWL_STYLE, dwStyle);
		m_EditID.Clear();
	}
	if (sTmpLoginType == L"房间ID")
	{
		DWORD   dwStyle = m_EditID.GetStyle();
		dwStyle |= ES_NUMBER;
		SetWindowLong(m_EditID.m_hWnd, GWL_STYLE, dwStyle);
		m_EditID.Clear();
	}
}
void CBarrageAssistantDlg::OnBnClickedBtConnectDouyu()
{
	CString sTmpLoginType;
	CString sTmpStr;
	
	m_cb_LoginType.GetWindowText(sTmpLoginType);
	GetDlgItem(IDC_EDIT_ROOMID)->GetWindowText(sTmpStr);

	if (sTmpLoginType == L"用户ID")
	{
		GetRoomIdAndName((wstring)sTmpStr, m_roomId, m_roomName);
	}
	if (sTmpLoginType == L"房间ID")
	{
		m_roomId = _wtoi(sTmpStr);
	}

	MainServerStart();
	UpdateRoomInfoStart();

	//初始化除普通弹幕OSD以外都处于锁定状态
// 	::SetWindowLong(m_TextFbDbHitOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
// 	::SetWindowLong(m_TextHeartOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
// 	::SetWindowLong(m_TextPraiseOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
// 	::SetWindowLong(m_TextSoGoodOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
// 	::SetWindowLong(m_TextSingleHitOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
// 	m_TextFbDbHitOSD.m_bLock = TRUE;
// 	m_TextHeartOSD.m_bLock = TRUE;
// 	m_TextPraiseOSD.m_bLock = TRUE;
// 	m_TextSoGoodOSD.m_bLock = TRUE;
// 	m_TextSingleHitOSD.m_bLock = TRUE;

	GetDlgItem(IDC_EDIT_ROOMID)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_CONNECT_DOUYU)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_LOGIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BT_DISCONNECT_DOUYU)->EnableWindow(TRUE);

	WritePrivateProfileStringW(L"BARRAGESSISTANT", L"LoginType", sTmpLoginType, g_BarrageCfgPath);
	WritePrivateProfileStringW(L"BARRAGESSISTANT", L"UserID", sTmpStr, g_BarrageCfgPath);

	g_NickName.clear();
	g_Content.clear();
}

void CBarrageAssistantDlg::OnBnClickedBtDisconnectDouyu()
{
	MainServerStop();
	UpdateRoomInfoStop();
	GetDlgItem(IDC_EDIT_ROOMID)->EnableWindow(TRUE);
	GetDlgItem(IDC_BT_CONNECT_DOUYU)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_LOGIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BT_DISCONNECT_DOUYU)->EnableWindow(FALSE);
	CString sTemp;
	sTemp.Format(L"%s%s", g_sInit, L"已断开连接...\r\n(鼠标放在当前,可拖动OSD，返回软件界面锁定/解锁)");
	m_TextOSD.UpdateTextStr(sTemp, 0);
	CWnd * TmpWnd;
	TmpWnd = FromHandle(m_TextOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	g_NickName.clear();
	g_Content.clear();

	if (m_TextOSD.m_hWnd != NULL)
	{
		for (int i = 0; i < 30; i++)
		{
			m_TextPanelOSD[i].UpdateTextStr(L"", 0);
		}

	}
}


std::string CBarrageAssistantDlg::wstring2string( const std::wstring wstr )
{
	std::string result;  
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);  
	char* buffer = new char[len + 1];  
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);  
	buffer[len] = '\0';  
	//删除缓冲区并返回值  
	result.append(buffer);  
	delete[] buffer;  
	return result;  
}

const std::wstring CBarrageAssistantDlg::string2wstring( std::string str )
{
	wstring result;  
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);  
	TCHAR* buffer = new TCHAR[len + 1];  
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);  
	buffer[len] = '\0';             //添加字符串结尾  
	//删除缓冲区并返回值  
	result.append(buffer);  
	delete[] buffer;  
	return result;  
}

char* CBarrageAssistantDlg::CString2Char(CString szItem, char* pOutChars)
{
	int len = WideCharToMultiByte(CP_ACP,0,szItem,szItem.GetLength(),NULL,0,NULL,NULL);

	char * pOutStr = pOutChars; 
	if(pOutStr == NULL)
	{
		return NULL;
	}
	WideCharToMultiByte(CP_ACP,0,szItem,szItem.GetLength(),pOutStr,len,NULL,NULL);  
	pOutStr[len] = '\0';

	return pOutStr;
}

CString CBarrageAssistantDlg::CharToCString(const char *str)
{
	int charLen = strlen(str); //计算pChar所指向的字符串大小，以字节为单位，一个汉字占两个字节  
	int len = MultiByteToWideChar(CP_ACP, 0, str, charLen, NULL, 0); //计算多字节字符的大小，按字符计算  
	wchar_t *pWChar = new wchar_t[len + 1]; //为宽字节字符数申请空间，  
	MultiByteToWideChar(CP_ACP, 0, str, charLen, pWChar, len); //多字节编码转换成宽字节编码  
	pWChar[len] = '\0';  

	//将wchar_t数组转换为CString  
	CString Outstr;  
	Outstr.Append(pWChar);  

	//delete[] str;  
	delete[] pWChar;  

	return Outstr;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	strcat((char *)stream, (char *)ptr);
	return size*nmemb;
}

static std::string _getNickName(const std::string &src)
{
	std::string  tmp = src;

	int pos = tmp.find("nn@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string name = tmp.substr(pos + strlen("nn@="), pos2 - pos - strlen("nn@="));
			return name;
		}
	}

	return "";
}

static std::string _getContent(const std::string &src)
{
	std::string  tmp = src;

	int pos = tmp.find("txt@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string name = tmp.substr(pos + strlen("txt@="), pos2 - pos - strlen("txt@="));
			return name;
		}
	}

	return "";
}

static std::string _getConnectNums(const std::string &src, int &CntNums)
{
	std::string  tmp = src;

	int pos = tmp.find("Srg@A=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string name = tmp.substr(pos + strlen("Srg@A="), pos2 - pos - strlen("Srg@A="));
			CntNums = atoi(name.c_str());
		}
	}

	return "";
}

static void _getDisableTextInfo(const std::string &src, std::string &adminName, std::string &userName)
{
	std::string  tmp = src;

	int pos = tmp.find("snic@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("snic@="), pos2 - pos - strlen("snic@="));
			t.append(1, '\0');
			adminName = t/*((CBarrageAssistantDlg*)g_MainFrm)->string2wstring(t)*/;
		}
	}

	/////////////////////////////////////////////

	tmp = src;

	pos = tmp.find("dnic@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("dnic@="), pos2 - pos - strlen("dnic@="));
			userName = t/*((CBarrageAssistantDlg*)g_MainFrm)->string2wstring(t)*/;
		}
	}

}

//type@=bc_buy_deserve/lev@=3/rid@=238858/gid@=0/cnt@=1/sui@=id@A=16323429@Sname@A=auto_K2AquLzRTi@Snick@A=wangzequn77@Srg@A=4@Spg@A=1@Srt@A=1432743931@Sbg@A=0@Sweight@A=0@Sstrength@A=331657@Scps_id@A=0@Sps@A=1@Sver@A=20150623@Sm_deserve_lev@A=3@Scq_cnt@A=93@Sbest_dlev@A=3@S/
static void _getChouQinInfo(const std::string &src, std::string &sender, int &level)
{
	std::string  tmp = src;

	int pos = tmp.find("Snick@A=");//Snick@A=
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("@S", pos);//@S
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("Snick@A="), pos2 - pos - strlen("Snick@A="));
			t.append(1, '\0');
			sender = t/*((CBarrageAssistantDlg*)g_MainFrm)->string2wstring(t)*/;
		}
	}

	/////////////////////////////////////////////

	tmp = src;

	pos = tmp.find("lev@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("lev@="), pos2 - pos - strlen("lev@="));
			level = atoi(t.c_str());
		}
	}
}

//type@=donateres/rid@=3258/gid@=0/ms@=100/sb@=4900/src_strength@=1900/dst_weight@=31076500/r@=0/sui@=id@A=4890288@Srg@A=1@Snick@A=鏀鹃de绾搁扯@S/
static void _getFishballInfo(const std::string &src, std::string &sender, int &number)
{
	std::string  tmp = src;

	int pos = tmp.find("Snick@A=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("@S", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("Snick@A="), pos2 - pos - strlen("Snick@A="));
			t.append(1, '\0');
			sender = t/*((CBarrageAssistantDlg*)g_MainFrm)->string2wstring(t)*/;
		}
	}

	/////////////////////////////////////////////

	tmp = src;

	pos = tmp.find("ms@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("ms@="), pos2 - pos - strlen("ms@="));
			number = atoi(t.c_str());
		}
	}
}

//type@=userenter/rid@=3258/gid@=0/userinfo@=id@A=716089@Sname@A=qq_1Mytyo9O@Snick@A=瀹濊摑涓€绫冲叚@Srg@A=1@Spg@A=1@Srt@A=1400429055@Sbg@A=0@Sweight@A=0@Sstrength@A=119200@Scps_id@A=0@Sps@A=1@Sver@A=20150331@Sm_deserve_lev@A=0@Scq_cnt@A=0@Sbest_dlev@A=0@S/
static void _getUserEnterRoomInfo(const std::string &src, std::string &user)
{
	std::string  tmp = src;

	int pos = tmp.find("nn@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("nn@="), pos2 - pos - strlen("nn@="));
			t.append(1, '\0');
			user = t/*((CBarrageAssistantDlg*)g_MainFrm)->string2wstring(t)*/;
		}
	}

}



static void _getGIFT(const std::string &src, std::string &users, int &flag, int &hits, int &gfcnt)
{
	std::string  tmp = src;

	int pos = tmp.find("nn@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string name = tmp.substr(pos + strlen("nn@="), pos2 - pos - strlen("nn@="));
			name.append(1, '\0');
			users = name;
		}
	}

	tmp = src;

	pos = tmp.find("gfid@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("gfid@="), pos2 - pos - strlen("gfid@="));
			flag = atoi(t.c_str());
		}
	}

	tmp = src;

	pos = tmp.find("hits@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("hits@="), pos2 - pos - strlen("hits@="));
			hits = atoi(t.c_str());
		}
	}

	tmp = src;

	pos = tmp.find("gfcnt@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string t = tmp.substr(pos + strlen("gfcnt@="), pos2 - pos - strlen("gfcnt@="));
			gfcnt = atoi(t.c_str());
		}
	}
}

static void _getSpbc(const std::string &src, std::string &suser, std::string &luser, std::string &gname, std::string &gcnum)
{
	std::string  tmp = src;

	int pos = tmp.find("sn@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string name = tmp.substr(pos + strlen("sn@="), pos2 - pos - strlen("sn@="));
			name.append(1, '\0');
			suser = name;
		}
	}

	tmp = src;

	pos = tmp.find("dn@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string name = tmp.substr(pos + strlen("dn@="), pos2 - pos - strlen("dn@="));
			name.append(1, '\0');
			luser = name;
		}
	}

	tmp = src;

	pos = tmp.find("gn@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string name = tmp.substr(pos + strlen("gn@="), pos2 - pos - strlen("gn@="));
			name.append(1, '\0');
			gname = name;
		}
	}

	tmp = src;

	pos = tmp.find("gc@=");
	if (pos != std::string::npos)
	{
		int pos2 = tmp.find("/", pos);
		if (pos2 != std::string::npos)
		{
			std::string name = tmp.substr(pos + strlen("gc@="), pos2 - pos - strlen("gc@="));
			name.append(1, '\0');
			gcnum = name;
		}
	}

}

void CBarrageAssistantDlg::MainServerStart()
{
	SetTimer(0, 5000, NULL);
	m_MainServerThreadStatus = Thread_Play;
	m_bExit = false;
	if (m_MainServerThreadHandle == NULL)
	{
		m_MainServerThreadHandle = ::CreateThread(NULL, 0, MainServerThread, this, 0, NULL);
		if (m_MainServerThreadHandle == NULL)
		{
			m_bExit = true;
			return;
		}
		SetThreadPriority(m_MainServerThreadHandle, NORMAL_PRIORITY_CLASS);
	}

	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

void CBarrageAssistantDlg::BarrageServerStart()
{
	m_BarrageServerThreadStatus = Thread_Play;

	if (m_BarrageServerThreadHandle == NULL)
	{
		m_BarrageServerThreadHandle = ::CreateThread(NULL, 0, BarrageServerThread, this, 0, NULL);
		if (m_BarrageServerThreadHandle == NULL)
		{
			return;
		}
		SetThreadPriority(m_BarrageServerThreadHandle, REALTIME_PRIORITY_CLASS);
	}
}

int CBarrageAssistantDlg::MainServerStop()
{
	m_bExit = true;
	SetEvent(m_hExitEvent);
	m_MainServerThreadStatus = Thread_Stop;
	m_BarrageServerThreadStatus = Thread_Stop;
	while (m_MainServerThreadHandle)
	{
		DoEvent();
		Sleep(10);
	}

	while (m_BarrageServerThreadHandle)
	{
		DoEvent();
		Sleep(10);
	}

	if (m_MainCurl)
	{
		curl_easy_cleanup(m_MainCurl);
		m_MainCurl = NULL;
	}

	if (m_BarrageCurl)
	{
		curl_easy_cleanup(m_BarrageCurl);
		m_BarrageCurl = NULL;
	}

	return 0;
}



void CBarrageAssistantDlg::UpdateRoomInfoStart()
{
	g_Updatecurl = curl_easy_init();
	m_UpdateRoomInfoThreadStatus = Thread_Play;

	if (m_UpdateRoomInfoThreadHandle == NULL)
	{
		m_UpdateRoomInfoThreadHandle = ::CreateThread(NULL, 0, UpdateRoomInfoThread, this, 0, NULL);
		if (m_UpdateRoomInfoThreadHandle == NULL)
		{
			return;
		}
		SetThreadPriority(m_UpdateRoomInfoThreadHandle, REALTIME_PRIORITY_CLASS);
	}
}

void CBarrageAssistantDlg::UpdateRoomInfoStop()
{
	m_UpdateRoomInfoThreadStatus = Thread_Stop;
	while (m_UpdateRoomInfoThreadHandle)
	{
		DoEvent();
		Sleep(10);
	}
	curl_easy_cleanup(g_Updatecurl);

	CString sTemp;
	CString sFans, sWeight, sOnline;
	sFans.Format(L"关注:%s\r\n", L"--");
	//sWeight.Format(L"体重:%s\r\n", L"--");
	sOnline.Format(L"人数:%s", L"--");
	sTemp.Append(sFans);
	//sTemp.Append(sWeight);
	sTemp.Append(sOnline);
	m_TextRoomInfoOSD.UpdateTextStr(sTemp, 0);
 	GetDlgItem(IDC_ST_ROOMID)->SetWindowText(L"--");
 	//GetDlgItem(IDC_ST_WEIGHT)->SetWindowText(L"--");
 	GetDlgItem(IDC_ST_ONLINENUMS)->SetWindowText(L"--");
}


DWORD WINAPI CBarrageAssistantDlg::MainServerThread( LPVOID lpParam )
{
	CBarrageAssistantDlg* ThreadDouyu = (CBarrageAssistantDlg*)lpParam;
	while (1)
	{
		if (ThreadDouyu->m_MainServerThreadStatus == Thread_Stop)
		{
			break;
		}
		if (ThreadDouyu->m_MainServerThreadStatus == Thread_paused)
		{
			Sleep(1);
			continue;
		}

		ThreadDouyu->MainServerWorkProc();
	}

	CloseHandle(ThreadDouyu->m_MainServerThreadHandle);
	ThreadDouyu->m_MainServerThreadHandle = NULL;

	return 0;
}

DWORD WINAPI CBarrageAssistantDlg::BarrageServerThread( LPVOID lpParam )
{
	CBarrageAssistantDlg* ThreadBarrage = (CBarrageAssistantDlg*)lpParam;
	while (1)
	{
		if (ThreadBarrage->m_BarrageServerThreadStatus == Thread_Stop)
		{
			break;
		}
		if (ThreadBarrage->m_BarrageServerThreadStatus == Thread_paused)
		{
			Sleep(1);
			continue;
		}

		ThreadBarrage->BarrageServerWorkProc();
	}

	CloseHandle(ThreadBarrage->m_BarrageServerThreadHandle);
	ThreadBarrage->m_BarrageServerThreadHandle = NULL;

	return 0;
}

DWORD WINAPI CBarrageAssistantDlg::UpdateRoomInfoThread(LPVOID lpParam)
{
	CBarrageAssistantDlg* ThreadInfo = (CBarrageAssistantDlg*)lpParam;
	while (1)
	{
		if (ThreadInfo->m_UpdateRoomInfoThreadStatus == Thread_Stop)
		{
			break;
		}
		if (ThreadInfo->m_UpdateRoomInfoThreadStatus == Thread_paused)
		{
			Sleep(1);
			continue;
		}

		ThreadInfo->UpdateRoomInfoWorkProc();
		Sleep(1000);
	}

	CloseHandle(ThreadInfo->m_UpdateRoomInfoThreadHandle);
	ThreadInfo->m_UpdateRoomInfoThreadHandle = NULL;

	return 0;
}

void CBarrageAssistantDlg::UpdateRoomInfoWorkProc()
{
	//CURL *curl;
	CURLcode res;
	//g_Updatecurl = curl_easy_init();
	//curl = curl_easy_init();//对curl进行初始化

	char url[1024];
	//http://capi.douyucdn.cn/api/v1/searchNew/%d/1?limit=1&offset=0
	sprintf(url, "http://open.douyucdn.cn/api/RoomApi/room/%d", m_roomId);

	static const int size = 15 * 1024;
	static char str[size];

	strcpy(str, "");

	curl_easy_setopt(g_Updatecurl, CURLOPT_URL, url); //设置下载地址
	curl_easy_setopt(g_Updatecurl, CURLOPT_WRITEFUNCTION, write_data);//设置写数据的函数
	curl_easy_setopt(g_Updatecurl, CURLOPT_WRITEDATA, str);//设置写数据的变量

	res = curl_easy_perform(g_Updatecurl);
	str[size - 1] = 0;
	if (CURLE_OK != res)
	{
		curl_easy_cleanup(g_Updatecurl);
		return;
	}

	std::string sUserId;
	std::string sweight;
	unsigned int nOnlineNums = 0;


	std::string tmp = str;

	int pos, pos2;
	int first = 0;

	//获取当前主播ID号
	//"nickname":"thebs" "fans":"629929"
	pos = tmp.find("\"fans_num\":\"", first);
	if (pos == std::string::npos)
	{
		//break;
	}
	pos2 = tmp.find("\"", pos + strlen("\"fans_num\":\"") + 1);
	if (pos2 == std::string::npos)
	{
		//break;
	}
	sUserId = tmp.substr(pos + strlen("\"fans_num\":\""), pos2 - pos - strlen("\"fans_num\":\""));

	//获取当前在线人数
	//"online":151950
	pos = tmp.find("\"online\":", first);
	if (pos == std::string::npos)
	{
		//break;
	}
	pos2 = tmp.find(":", pos + strlen("\"online\"") + 1);
	if (pos2 == std::string::npos)
	{
		//break;
	}
	std::string tempNums = tmp.substr(pos + strlen("\"online\":"), pos2 - pos - strlen("\"online\":"));
	nOnlineNums = _atoi64(tempNums.c_str());

	//获取当前主播体重
	//"owner_weight":"296.42t"
	pos = tmp.find("\"owner_weight\":\"", first);
	if (pos == std::string::npos)
	{
		//break;
	}
	pos2 = tmp.find("\"", pos + strlen("\"owner_weight\":\"") + 1);
	if (pos2 == std::string::npos)
	{
		//break;
	}

	sweight = tmp.substr(pos + strlen("\"owner_weight\":\""), pos2 - pos - strlen("\"owner_weight\":\""));

	std::wstring wsUserName;
	std::wstring wsweight;

	transcode::UTF8_to_Unicode(sUserId.c_str(), sUserId.length(), wsUserName);
	transcode::UTF8_to_Unicode(sweight.c_str(), sweight.length(), wsweight);
	CString sNums;
	sNums.Format(L"%ld", nOnlineNums);

	CString sTemp;
	CString sFans, sWeight, sOnline;
	sFans.Format(L"关注:%s\r\n", wsUserName.c_str());
	//sWeight.Format(L"体重:%s\r\n", wsweight.c_str());
	sOnline.Format(L"人数:%s", sNums);
	sTemp.Append(sFans);
	//sTemp.Append(sWeight);
	sTemp.Append(sOnline);
	m_TextRoomInfoOSD.UpdateTextStr(sTemp, 0);

	GetDlgItem(IDC_ST_ROOMID)->SetWindowText(wsUserName.c_str());
	//GetDlgItem(IDC_ST_WEIGHT)->SetWindowText(wsweight.c_str());
	GetDlgItem(IDC_ST_ONLINENUMS)->SetWindowText(sNums);


	//curl_easy_cleanup(g_Updatecurl);
}

void CBarrageAssistantDlg::MainServerWorkProc()
{
	std::string ip;
	unsigned int port;
	std::vector<std::string> ips;
	std::vector<unsigned int> ports;

	CString sMsg;
	int cnt;
	int cur = 0;

	char info[1024];
	std::wstring wt;
	RetryToGetSvrList:

	if (m_bExit)
	{
		return;
	}


	BarrageServerStart();

	while (!m_bExit)
	{
		SendKeepLiveToMainServer();

		if (WaitForSingleObject(m_hExitEvent, 5000) == WAIT_OBJECT_0)
		{
			break;
		}
	}
}

void CBarrageAssistantDlg::BarrageServerWorkProc()
{
	CString sMsg;

RetryConnect:
	if (m_bExit)
	{
		return;
	}
	if (ConnectToBarrageServer() == true)
	{
		CString sTmp;
		sTmp = L"弹幕服务器连接成功,尝试登录中...\n";
		sMsg.Append(sTmp);
		m_TextOSD.UpdateTextStr(sMsg, 0);
		GetDlgItem(IDC_EDIT_ROOMID)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_CONNECT_DOUYU)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_LOGIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_DISCONNECT_DOUYU)->EnableWindow(TRUE);
	}
	else
	{
		CString sTmp;
		sTmp = L"弹幕服务器连接失败,重试中...\n";
		sMsg.Append(sTmp);
		m_TextOSD.UpdateTextStr(sMsg, 0);
		Sleep(200);
		GetDlgItem(IDC_EDIT_ROOMID)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_CONNECT_DOUYU)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_LOGIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_DISCONNECT_DOUYU)->EnableWindow(FALSE);
		SetTimer(0, 5000, NULL);
		goto RetryConnect;
	}

RetryLogin:
	if (m_bExit)
	{
		return;
	}
	if (LoginToBarrageServer() == true)
	{
		CString sTmp;
		//sTmp = L"弹幕服务器登录成功,准备接收弹幕...\n";
		//sMsg.Append(sTmp);
		if (((CButton*)GetDlgItem(IDC_CHECK_OSDLOCK))->GetCheck())
		{
			sTmp.Format(L"%s%s", g_sInit, L"弹幕服务器登录成功,准备接收弹幕...\r\n(鼠标放在当前,可拖动OSD，返回软件界面锁定/解锁)\r\n[当前已锁定]");
			m_TextOSD.UpdateTextStr(sTmp, 0);
		}
		else
		{
			sTmp.Format(L"%s%s", g_sInit, L"弹幕服务器登录成功,准备接收弹幕...\r\n(鼠标放在当前,可拖动OSD，返回软件界面锁定/解锁)\r\n[当前已解锁]");
			m_TextOSD.UpdateTextStr(sTmp, 0);
		}

		GetDlgItem(IDC_EDIT_ROOMID)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_CONNECT_DOUYU)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_LOGIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_DISCONNECT_DOUYU)->EnableWindow(TRUE);
	}
	else
	{
		CString sTmp;
		sTmp = L"弹幕服务器登录失败,重试中...\n";
		sMsg.Append(sTmp);
		m_TextOSD.UpdateTextStr(sMsg, 0);
		Sleep(200);
		GetDlgItem(IDC_EDIT_ROOMID)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_CONNECT_DOUYU)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_LOGIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_DISCONNECT_DOUYU)->EnableWindow(FALSE);
		SetTimer(0, 5000, NULL);
		goto RetryLogin;
	}

	DWORD lastSendKeepLiveTime = 0;
	while (!m_bExit)
	{
		DWORD curTime = GetTickCount();
		if (curTime - lastSendKeepLiveTime >= 5000)
		{
			lastSendKeepLiveTime = curTime;
			SendKeepLiveToBarrageServer();

			//SendChatMsg();
		}

		if (!CheckForBarrageMessage())
		{
			break;
		}

		if (WaitForSingleObject(m_hExitEvent, 100) == WAIT_OBJECT_0)
		{
			break;
		}
	}
}

bool CBarrageAssistantDlg::GetRoomIdAndName( const std::wstring roomAlias, unsigned int &roomId, std::wstring &roomName )
{
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();//对curl进行初始化
	if (!curl)
	{
		return false;
	}

	char url[1024];
	std::string aAlias = wstring2string(roomAlias)/*(roomAlias.begin(), roomAlias.end())*/;
	sprintf(url, "http://www.douyu.com/%s", aAlias.c_str());

	int size = 1024 * 1024 * 20;
	char *buf = new char[size];

	buf[0] = 0;

	curl_easy_setopt(curl, CURLOPT_URL, url); //设置下载地址
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//设置写数据的函数
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, buf);//设置写数据的变量

	res = curl_easy_perform(curl);//执行下载
	buf[size - 1] = 0;
	/*
	{
		FILE *fp = fopen("d:\\1.txt", "a+");
		fprintf(fp, "%s", buf);
		fclose(fp);
	}
	*/

	if (CURLE_OK != res)
	{
		curl_easy_cleanup(curl);
		delete[] buf;
		return false;
	}

	std::string tmp = buf;

	int pos = tmp.find("{\"room_id\":");
	if (pos == std::string::npos)
	{
		curl_easy_cleanup(curl);
		delete[] buf;
		return false;
	}

	int pos2 = tmp.find(",", pos + 1);
	if (pos2 == std::string::npos)
	{
		curl_easy_cleanup(curl);
		delete[] buf;
		return false;
	}

	std::string strRoomId = tmp.substr(pos + strlen("{\"room_id\":"), pos2 - pos - strlen("{\"room_id\":"));

	roomId = atoi(strRoomId.c_str());
	
	//----------------------

	pos = tmp.find("\"room_name\":\"");
	if (pos == std::string::npos)
	{
		curl_easy_cleanup(curl);
		delete[] buf;
		return false;
	}

	pos2 = tmp.find("\"", pos + 1);
	if (pos2 == std::string::npos)
	{
		curl_easy_cleanup(curl);
		delete[] buf;
		return false;
	}

	std::string strRoomName = tmp.substr(pos + strlen("\"room_name\":\""), pos2 - pos - strlen("\"room_name\":\""));
	roomName = string2wstring(strRoomName);

	curl_easy_cleanup(curl);
	delete[] buf;
	return true;
}

bool CBarrageAssistantDlg::GetMainServerInfoList( std::vector<std::string> &ips, std::vector<unsigned int> &ports )
{
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();//对curl进行初始化

	char url[1024];

	//http://api.douyutv.com/api/client/room/72860?cdn=ws

	//sprintf(url, "http://api.douyutv.com/api/client/room/%d?cdn=ws", m_roomId);
	sprintf(url, "http://www.douyutv.com/api/client/room/%d", m_roomId);

	static const int size = 15 * 1024;
	static char str[size];

	strcpy(str, "");

	curl_easy_setopt(curl, CURLOPT_URL, url); //设置下载地址
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//设置写数据的函数
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);//设置写数据的变量

	res = curl_easy_perform(curl);//执行下载
	str[size - 1] = 0;
	if (CURLE_OK != res)
	{
		curl_easy_cleanup(curl);
		return false;
	}

	//"servers":[{"ip":"182.18.47.18","port":"8055"},{"ip":"182.18.47.51","port":"8038"},{"ip":"182.18.47.10","port":"8011"},{"ip":"182.18.47.17","port":"8043"},{"ip":"182.18.47.51","port":"8019"},{"ip":"182.18.47.51","port":"8015"},{"ip":"182.18.47.18","port":"8051"},{"ip":"182.18.47.51","port":"8014"},{"ip":"182.18.47.18","port":"8053"},{"ip":"182.18.47.10","port":"8008"}]

	//	ip = "182.18.47.10";
	//	port = 8009;

	std::string ip;
	unsigned int port;

	std::string tmp = str;

	int pos, pos2;
	int first = 0;

	while (1)
	{
		pos = tmp.find("{\"ip\":\"", first);
		if (pos == std::string::npos)
		{
			break;
		}
		pos2 = tmp.find("\"", pos + strlen("{\"ip\":\"") + 1);
		if (pos2 == std::string::npos)
		{
			break;
		}
		ip = tmp.substr(pos + strlen("{\"ip\":\""), pos2 - pos - strlen("{\"ip\":\""));

		pos= tmp.find("\"port\":\"", pos2+1);
		if (pos == std::string::npos)
		{
			break;
		}
		pos2 = tmp.find("\"", pos + strlen("\"port\":\"") + 1);
		if (pos2 == std::string::npos)
		{
			break;
		}
		std::string strport = tmp.substr(pos + strlen("\"port\":\""), pos2 - pos - strlen("\"port\":\""));

		port = atoi(strport.c_str());

		ips.push_back(ip);
		ports.push_back(port);

#ifdef _DEBUG
		char info[1024];
		sprintf(info, "got one svr - ip=%s, port=%d\r\n", ip.c_str(), port);
		OutputDebugStringA(info);
#endif
		first = pos2+1;
	}
#ifdef _DEBUG
	char info[1024];
	sprintf(info, "-------got total svr cnt: %d\r\n", ips.size());
	OutputDebugStringA(info);
#endif
	curl_easy_cleanup(curl);
	return ips.size()>0?true:false;
}

bool CBarrageAssistantDlg::ConnectToMainServer( const std::string &ip, unsigned int port )
{
	CURLcode res;

	m_MainCurl = curl_easy_init();
	if (!m_MainCurl)
	{
		return false;
	}

	curl_easy_setopt(m_MainCurl, CURLOPT_URL, ip.c_str());
	curl_easy_setopt(m_MainCurl, CURLOPT_PORT, port);

	curl_easy_setopt(m_MainCurl, CURLOPT_CONNECT_ONLY, 1L);
	res = curl_easy_perform(m_MainCurl);
	if (CURLE_OK != res)
	{
		curl_easy_cleanup(m_MainCurl);
		m_MainCurl = NULL;
		return false;
	}
	return true;
}

bool CBarrageAssistantDlg::LoginToMainServer()
{
	if (!m_MainCurl)
	{
		return false;
	}

	CURLcode res;
	size_t iolen = 0;
	char *buf = new char[2048];
	char tmp[1024];

	/*
	type@=loginreq/roomid@=%d/gateid@=0/username@=/nickname@=/password@=/roompass@=d41d8cd98f00b204e9800998ecf8427e/clientip@=/ct@=0/
	*/

	char *content = buf + sizeof(DWORD) * 3;
	/*
	sprintf(tmp, "type@=loginreq/roomid@=%d/gateid@=0/username@=/nickname@=/password@=/roompass@=d41d8cd98f00b204e9800998ecf8427e/clientip@=/ct@=0/",
	//sprintf(tmp, "type@=loginreq/roomid@=%d/gateid@=0/username@=/nickname@=/password@=/roompass@=/clientip@=/ct@=0/",
		m_roomId);
*/
	//old:type@=loginreq/username@=/password@=/roompass@=/roomid@=%d/devid@=460303304A21771233BF64E3BC1E5461/rt@=%d/vk@=015c822ad706857355d3e6db18c35cbd/ver@=20150331/
	sprintf(tmp, "type@=loginreq/username@=/password@=/roompass@=/roomid@=%d/devid@=460303304A21771233BF64E3BC1E5461/rt@=%d/vk@=015c822ad706857355d3e6db18c35cbd/ver@=20150929/",
		m_roomId, GetTickCount());
	strcpy(content, tmp);
	
	*((DWORD*)buf) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 1) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 2) = 689;

	res = curl_easy_send(m_MainCurl, buf, strlen(tmp) + sizeof(DWORD) * 3 + 1, &iolen);
	if (CURLE_OK != res)
	{
		delete[] buf;
		return false;
	}
	delete[] buf;
	return true;
}

bool CBarrageAssistantDlg::GetBarrageUserName( std::string &userName )
{
	if (!m_MainCurl)
	{
		return false;
	}

	CURLcode res;
	size_t iolen = 0;

	char recv_buf[1024];

	DWORD dwBegin = GetTickCount();

	while (1)
	{
		res = curl_easy_recv(m_MainCurl, recv_buf, 1024, &iolen);
		if (CURLE_OK == res)
		{
			char *real_content = recv_buf + sizeof(DWORD) * 3;

			//type@=loginres/userid@=1345548295/roomgroup@=0/pg@=0/sessionid@=1244321275/username@=visitor45178295/nickname@=visitor45178295/is_signined@=0/signin_count@=0/s@=1428809883/live_stat@=1/npv@=0/
			if (strstr(real_content, "type@=loginres") != NULL)
			{
				std::string  tmp = real_content;

				int pos = tmp.find("username@=");
				if (pos != std::string::npos)
				{
					int pos2 = tmp.find("/", pos);
					if (pos2 != std::string::npos)
					{
						userName = tmp.substr(pos + strlen("username@="), pos2 - pos - strlen("username@="));
						return true;
					}
				}
			}
			else if (strstr(real_content, "type@=error") != NULL)
			{
				int d = 0;
				++d;
				return false;
			}
			else
			{
				int  a = 0;
				a++;

			}
		}
		else if (CURLE_AGAIN != res)
		{
			break;
		}

		if (GetTickCount() - dwBegin > 3000)
		{
			break;
		}
	}

	return false;
}

bool CBarrageAssistantDlg::GetBarrageGid( unsigned int &gid )
{
	if (!m_MainCurl)
	{
		return false;
	}

	CURLcode res;
	size_t iolen;

	char recv_buf[1024];

	while (1)
	{
		res = curl_easy_recv(m_MainCurl, recv_buf, 1024, &iolen);
		if (CURLE_OK == res)
		{
			char *real_content = recv_buf + sizeof(DWORD) * 3;

			while (1)
			{
				if (strstr(real_content, "type@=setmsggroup") != NULL)
				{ //type@=setmsggroup/rid@=68172/gid@=71/
					std::string  tmp = real_content;

					int pos = tmp.find("gid@=");
					if (pos != std::string::npos)
					{
						int pos2 = tmp.find("/", pos);
						if (pos2 != std::string::npos)
						{
							std::string name = tmp.substr(pos + strlen("gid@="), pos2 - pos - strlen("gid@="));
							gid = atoi(name.c_str());
							return true;
						}
					}

				}

				if (strlen(real_content) + 1 >= iolen)
				{
					break;
				}
				real_content = real_content + sizeof(DWORD) * 3 + strlen(real_content) + 1;
			}
		}
		else if (res != CURLE_AGAIN)
		{
			break;
		}
	}

	return false;
}

bool CBarrageAssistantDlg::ConnectToBarrageServer()
{
	CURLcode res;

	if (m_BarrageCurl)
	{
		curl_easy_cleanup(m_BarrageCurl);
	}

	m_BarrageCurl = curl_easy_init();
	if (!m_BarrageCurl)
	{
		return false;
	}

	curl_easy_setopt(m_BarrageCurl, CURLOPT_URL, "openapi-danmu.douyu.com");
	curl_easy_setopt(m_BarrageCurl, CURLOPT_PORT, 8601);

	curl_easy_setopt(m_BarrageCurl, CURLOPT_CONNECT_ONLY, 1L);
	res = curl_easy_perform(m_BarrageCurl);
	if (CURLE_OK != res)
	{
		curl_easy_cleanup(m_BarrageCurl);
		m_BarrageCurl = NULL;
		return false;
	}
	return true;
}

bool CBarrageAssistantDlg::LoginToBarrageServer()
{
	if (!m_BarrageCurl)
	{
		return false;
	}

	CURLcode res;
	size_t iolen;

	char *buf = new char[2048];
	char tmp[1024];


	char *content = buf + sizeof(DWORD) * 3;
	sprintf(tmp, "type@=loginreq/roomid@=%d/"/*"type@=loginreq/username@=%s/password@=1234567890123456/roomid@=%d/"*/,
		/*m_userName.c_str(), */m_roomId);
	strcpy(content, tmp);

	*((DWORD*)buf) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 1) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 2) = 689;

	res = curl_easy_send(m_BarrageCurl, buf, strlen(tmp) + sizeof(DWORD) * 3 + 1, &iolen);
	if (CURLE_OK != res)
	{
		delete[] buf;
		return false;
	}

	char recv_buf[1024];
	while (true)
	{
		res = curl_easy_recv(m_BarrageCurl, recv_buf, 1024, &iolen);
		if (CURLE_OK == res)
		{
			char *real_content = recv_buf + sizeof(DWORD) * 3;

			if (strstr(real_content, "loginres") != NULL)
			{
				//send "joingroup"
				char *content = buf + sizeof(DWORD) * 3;

				//2.3.4 入组消息
				//该消息用于完成加入房间分组，完整的数据部分应包含的字段如下：
				//type@=joingroup/rid@=59872/aid@=yourapplicaitonID/token@=4c8421535f9639d8c1ad35d1fa421f36/time=1574850339/auth=xxxxxxxxxxxx/
				
				sprintf(tmp, "type@=joingroup/rid@=%d/gid@=%d/", m_roomId, -9999);
				strcpy(content, tmp);

				*((DWORD*)buf) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
				*((DWORD*)buf + 1) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
				*((DWORD*)buf + 2) = 689;

				res = curl_easy_send(m_BarrageCurl, buf, strlen(tmp) + sizeof(DWORD) * 3 + 1, &iolen);
				return res == CURLE_OK ? true : false;
			}
		}
		else if (CURLE_AGAIN != res)
		{
			break;
		}
	}

	delete[] buf;
	return false;
}

bool CBarrageAssistantDlg::SendKeepLiveToBarrageServer()
{
 	if (!m_BarrageCurl)
	{
		return false;
	}

	size_t iolen = 0;
	char *buf = new char[2048];
	char tmp[1024];

	/*
	type@=keeplive/tick@=92748474/
	*/

	char *content = buf + sizeof(DWORD) * 3;
	sprintf(tmp, "type@=keeplive/tick@=%d/", GetTickCount());
	strcpy(content, tmp);

	*((DWORD*)buf) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 1) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 2) = 689;

	if (CURLE_OK != curl_easy_send(m_BarrageCurl, buf, strlen(tmp) + sizeof(DWORD) * 3 + 1, &iolen))
	{
		delete[] buf;
		return false;
	}

	delete[] buf;
	return true;
}

void CBarrageAssistantDlg::SendKeepLiveToMainServer()
{
	if (!m_MainCurl)
	{
		return;
	}

	size_t iolen = 0;
	char *buf = new char[2048];
	char tmp[1024];

	/*
	type@=keeplive/tick@=92748474/
	*/

	char *content = buf + sizeof(DWORD) * 3;
	sprintf(tmp, "type@=keeplive/tick@=%d/", GetTickCount());
	strcpy(content, tmp);

	*((DWORD*)buf) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 1) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 2) = 689;

	curl_easy_send(m_MainCurl, buf, strlen(tmp) + sizeof(DWORD) * 3 + 1, &iolen);

	delete[] buf;
}

bool CBarrageAssistantDlg::SendChatMsg()
{
	if (!m_BarrageCurl)
	{
		return false;
	}

	size_t iolen = 0;
	char *buf = new char[2048];
	char tmp[1024];

	/*
	type@=chatmessage/rescode@=0/sender@=11115724/content@=浠欓€?snick@=濂逛箙浼存垜蹇?cd@=1/maxl@=30/chatmsgid@=05cf0c7e3e404ea4f5ad1e0000000000/col@=0/ct@=0/gid@=11/rid@=17732/sui@=id@A=11115724@Snick@A=濂逛箙浼存垜蹇傽Srg@A=1@Spg@A=1@Sstrength@A=26120@Sver@A=20150917@Sm_des...

	type@=chatmessage/receiver@=0/content@=450fsfs/scope@=/
	*/

	char *content = buf + sizeof(DWORD) * 3;
	sprintf(tmp, "type@=chatmessage/receiver@=0/content@=%s/scope@=/", "abcdefg");
	strcpy(content, tmp);

	*((DWORD*)buf) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 1) = strlen(tmp) + sizeof(DWORD) * 2 + 1;
	*((DWORD*)buf + 2) = 689;

	if (CURLE_OK != curl_easy_send(m_BarrageCurl, buf, strlen(tmp) + sizeof(DWORD) * 3 + 1, &iolen))
	{
		delete[] buf;
		return false;
	}

	delete[] buf;
	return true;
}

bool CBarrageAssistantDlg::CheckForBarrageMessage()
{
	if (!m_BarrageCurl)
	{
		return false;
	}

	size_t iolen = 0;
	char recv_buf[2048*8];

	CURLcode res = curl_easy_recv(m_BarrageCurl, recv_buf, 2048*8, &iolen);
	if (CURLE_OK == res)
	{
		unsigned int packSize = *((unsigned int*)recv_buf);
		unsigned int remainSize = iolen;
		char *curPos = recv_buf;

		while (remainSize >= packSize + 4 && !m_bExit)
		{
			//got one completed packet
			std::string onePacket;
			onePacket.append(curPos+3*sizeof(unsigned int), packSize-2*sizeof(unsigned int));

			//-------------------------------------------
			if (strstr(onePacket.c_str(), "type@=chatmsg") != NULL)
			{

				SetWindOnTop();
				SetTimer(7, 20000, NULL);
				int nConncetNums = 0;
				std::string nickName = _getNickName(onePacket);
				std::string content = _getContent(onePacket);

				nickName.append(1, '\0');
				content.append(1, '\0');

				std::wstring wNickname;
				std::wstring wContent;

				transcode::UTF8_to_Unicode(nickName.c_str(), nickName.length(), wNickname);
				transcode::UTF8_to_Unicode(content.c_str(), content.length(), wContent);

				
				CString sMusicTitle;
				sMusicTitle.Format(L"%s", wContent.c_str());



				if (m_bBarrageAssistant == TRUE)
				{
					CString sMsgId;
					CString sMsgInfo;
				
					sMsgId.Format(L"├%s┤:", wNickname.c_str());

					if (m_TimeFlag == TRUE)
					{
						sMsgInfo.Format(L"[%s]%s", GetTimeStr(), wContent.c_str());
					}
					else
					{
						sMsgInfo.Format(L"%s", wContent.c_str());

					}

					if (g_NickName.size() == 15)
					{
						for (int i = 1; i < 15; ++i)
						{
							g_NickName[i - 1] = g_NickName[i];
							g_Content[i - 1] = g_Content[i];
						}
						
						g_NickName[15 - 1] = sMsgId;
						g_Content[15 - 1] = sMsgInfo;

						for (int i = 0; i < g_NickName.size(); i++)
						{
							m_TextPanelOSD[14 - i].UpdateTextStr(g_NickName[14 - i], 0);
							m_TextPanelOSD[29 - i].UpdateTextStr(g_Content[14 - i], 0);
						}
					}
					else
					{
						g_NickName.push_back(sMsgId);
						g_Content.push_back(sMsgInfo);
						
						int nCount = g_NickName.size();

						for (int i = 0; i < g_NickName.size(); i++)
						{
							nCount--;
							m_TextPanelOSD[14 - i].UpdateTextStr(g_NickName[nCount], 0);
							m_TextPanelOSD[29 - i].UpdateTextStr(g_Content[nCount], 0);
						}

					}

				}
				

			}
			else if (strstr(onePacket.c_str(), "type@=donateres") != NULL)
			{
				SetWindOnTop();
				SetTimer(5, 4000, NULL);

				if (m_bFishBall == TRUE)
				{
					//type@=donateres/rid@=238858/gid@=0/ms@=100/sb@=1334/src_strength@=1000/dst_weight@=28639691/r@=0/sui@=id@A=11223151@Srg@A=1@Snick@A=@Scur_lev@A=0@Scq_cnt@A=0@Sbest_dlev@A=0@S/
					std::string sender;
					int number=0;
					_getFishballInfo(onePacket, sender, number);

					std::wstring username;
					transcode::UTF8_to_Unicode(sender.c_str(), sender.length(), username);

					CString sMsg;
					if (m_TimeFlag == TRUE)
					{
						sMsg.Format(L"『%s』(%s): -> 送了主播[%d]个鱼丸\r\n", username.c_str(), GetTimeStr(), number);
					}
					else
					{
						sMsg.Format(L"『%s』-> 送了主播[%d]个鱼丸\r\n", username.c_str(), number);

					}
					
					m_TextFbDbHitOSD.UpdateTextStr(sMsg, 0);

					//由于乱码(转成UNICODE)，调试查看，
					std::wstring TempMsg;
					transcode::UTF8_to_Unicode(onePacket.c_str(), onePacket.length(), TempMsg);
					OutputDebugStringW(TempMsg.c_str());
					OutputDebugStringW(L"\r\n");

					//wchar_t tmp[1024];
					//wsprintfW(tmp, L"%s xx %d 个鱼丸", sender.c_str(), number);
				}
			}
			else if (strstr(onePacket.c_str(), "type@=dgb") != NULL)
			{
				SetWindOnTop();

				int ndgnNums = 0;
				int nflag = 0;
				int ngfcnt = 0;
				std::string sDgnuser;
				_getGIFT(onePacket, sDgnuser, nflag, ndgnNums, ngfcnt);

				std::wstring swDgn;
				transcode::UTF8_to_Unicode(sDgnuser.c_str(), sDgnuser.length(), swDgn);

				

				//连续100x个鱼丸
				if (m_bFbDbHit == TRUE)
				{
					SetTimer(1, 5000, NULL);
					if (nflag == 50 || nflag == 191)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[0].Format(L"『%s』(%s): -> 送了主播 100x[%d]连击 个鱼丸\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[0].Format(L"『%s』-> 送了主播 100x[%d]连击 个鱼丸\r\n", swDgn.c_str(), ndgnNums);

						}

						m_TextFbDbHitOSD.UpdateTextStr(g_sInfo[0], 0);
					}
					
					
				}

				//连续x 6666666
				if (m_b666 == TRUE)
				{
					SetTimer(2, 6000, NULL);
					if (nflag == 52 || nflag == 194)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[1].Format(L"『%s』(%s): -> 送出【⒍⒍⒍】x %d连击\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[1].Format(L"『%s』-> 送出【⒍⒍⒍】x %d连击\r\n", swDgn.c_str(), ndgnNums);

						}
		
						m_TextSoGoodOSD.UpdateTextStr(g_sInfo[1], 0);
					}
				}

				//连续x 心
				if (m_bHeart == TRUE)
				{
					SetTimer(3, 7000, NULL);

					if (nflag == 53)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[2].Format(L"『%s』(%s): -> 送出【❤】x %d连击\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[2].Format(L"『%s』-> 送出【❤】x %d连击\r\n", swDgn.c_str(), ndgnNums);

						}


						m_TextHeartOSD.UpdateTextStr(g_sInfo[2], 0);
					}

				}

				//连续x 赞
				if (m_bPraise)
				{
					SetTimer(4, 8000, NULL);

					if (nflag == 57 || nflag == 192)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[3].Format(L"『%s』(%s): -> 点【赞】x %d连击\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[3].Format(L"『%s』-> 点【赞】x %d连击\r\n", swDgn.c_str(), ndgnNums);

						}

						m_TextPraiseOSD.UpdateTextStr(g_sInfo[3], 0);
					}
					else if (nflag == 193)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[3].Format(L"『%s』(%s): -> 送出【弱鸡】x %d连击\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[3].Format(L"『%s』-> 送出【弱鸡】x %d连击\r\n", swDgn.c_str(), ndgnNums);

						}

						m_TextPraiseOSD.UpdateTextStr(g_sInfo[3], 0);
					}
					else if (nflag == 520)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[3].Format(L"『%s』(%s): -> 送出【稳】x %d连击\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[3].Format(L"『%s』-> 送出【稳】x %d连击\r\n", swDgn.c_str(), ndgnNums);

						}

						m_TextPraiseOSD.UpdateTextStr(g_sInfo[3], 0);
					}
					else if (nflag == 713)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[3].Format(L"『%s』(%s): -> 送出【辣眼睛】x %d连击\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[3].Format(L"『%s』-> 送出【辣眼睛】x %d连击\r\n", swDgn.c_str(), ndgnNums);

						}

						m_TextPraiseOSD.UpdateTextStr(g_sInfo[3], 0);
					}
					else if (nflag == 826)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[3].Format(L"『%s』(%s): -> 送出【嘉年华蛋糕】x %d连击\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[3].Format(L"『%s』-> 送出【嘉年华蛋糕】x %d连击\r\n", swDgn.c_str(), ndgnNums);

						}

						m_TextPraiseOSD.UpdateTextStr(g_sInfo[3], 0);
					}
					else if (nflag == 516)
					{
						if (m_TimeFlag == TRUE)
						{
							g_sInfo[3].Format(L"『%s』(%s): -> 送出【呵呵】x %d连击\r\n", swDgn.c_str(), GetTimeStr(), ndgnNums);
						}
						else
						{
							g_sInfo[3].Format(L"『%s』-> 送出【呵呵】x %d连击\r\n", swDgn.c_str(), ndgnNums);

						}

						m_TextPraiseOSD.UpdateTextStr(g_sInfo[3], 0);
					}


				}


				//由于乱码(转成UNICODE)，调试查看，
 				std::wstring TempMsg;
 				transcode::UTF8_to_Unicode(onePacket.c_str(), onePacket.length(), TempMsg);
 				OutputDebugStringW(TempMsg.c_str());
 				OutputDebugStringW(L"\r\n");
			}
			else if (strstr(onePacket.c_str(), "type@=newblackres") != NULL)
			{
				SetWindOnTop();
				SetTimer(6, 3000, NULL);

				if (m_bBannedToPost == TRUE)
				{
					std::string admin, user;
					_getDisableTextInfo(onePacket, admin, user);

					std::wstring adminr, username;
					transcode::UTF8_to_Unicode(admin.c_str(), admin.length(), adminr);
					transcode::UTF8_to_Unicode(user.c_str(), user.length(), username);


					CString sMsg;
					if (m_TimeFlag == TRUE)
					{
						sMsg.Format(L"系统提示(%s):『%s』被管理员『%s』禁言\r\n", GetTimeStr(), username.c_str(), adminr.c_str());
					}
					else
					{
						sMsg.Format(L"系统提示:『%s』被管理员『%s』禁言\r\n", username.c_str(), adminr.c_str());

					}


					m_TextGuestOSD.UpdateTextStr(sMsg, 0);

					//由于乱码(转成UNICODE)，调试查看，
					std::wstring TempMsg;
					transcode::UTF8_to_Unicode(onePacket.c_str(), onePacket.length(), TempMsg);
					OutputDebugStringW(TempMsg.c_str());
					OutputDebugStringW(L"\r\n");
				}

			}
			else if (strstr(onePacket.c_str(), "type@=uenter") != NULL)
			{
				SetWindOnTop();
				SetTimer(6, 3000, NULL);

				if (m_bGuest == TRUE)
				{
					std::string user;
					_getUserEnterRoomInfo(onePacket, user);

					std::wstring username;
					transcode::UTF8_to_Unicode(user.c_str(), user.length(), username);

					CString sMsg;
					//CString sInfo;
					if (m_TimeFlag == TRUE)
					{
						sMsg.Format(L"系统提示(%s):欢迎『%s』来到直播间\r\n", GetTimeStr(), username.c_str());
					}
					else
					{
						sMsg.Format(L"系统提示:欢迎『%s』来到直播间\r\n", username.c_str());

					}


					m_TextGuestOSD.UpdateTextStr(sMsg, 0);

					//由于乱码(转成UNICODE)，调试查看，
					std::wstring TempMsg;
					transcode::UTF8_to_Unicode(onePacket.c_str(), onePacket.length(), TempMsg);
					OutputDebugStringW(TempMsg.c_str());
					OutputDebugStringW(L"\r\n");

					//wchar_t tmp[1024];
					//wsprintfW(tmp, L"欢迎 %s 来到本直播间", user.c_str());
				}
				
			

			}
			else if (strstr(onePacket.c_str(), "type@=spbc") != NULL)
			{
				SetWindOnTop();
				
				std::string suser;
				std::string luser;
				std::string gname;
				std::string gcnum;
				_getSpbc(onePacket, suser, luser, gname, gcnum);

				std::wstring susername;
				std::wstring lusername;
				std::wstring gnname;
				std::wstring gcnums;
				transcode::UTF8_to_Unicode(suser.c_str(), luser.length(), susername);
				transcode::UTF8_to_Unicode(luser.c_str(), luser.length(), lusername);
				transcode::UTF8_to_Unicode(gname.c_str(), gname.length(), gnname);
				transcode::UTF8_to_Unicode(gcnum.c_str(), gcnum.length(), gcnums);
				
				//支持所有主播(火箭)

				////thebs(陈一发版本)
				//if (lusername.c_str() == L"thebs")
				//{
					SetTimer(8, 40000, NULL);
					CString sMsg;

					sMsg.Format(L"前方高能:[%s]发射了%s枚[%s]-->(%s)", susername.c_str(), gcnums.c_str(), gnname.c_str(), lusername.c_str());

					m_TextSingleHitOSD.UpdateTextStr(sMsg, 0);
				//}


				std::wstring TempMsg;
				transcode::UTF8_to_Unicode(onePacket.c_str(), onePacket.length(), TempMsg);
				OutputDebugStringW(TempMsg.c_str());
				OutputDebugStringW(L"\r\n");

			}
			else if (strstr(onePacket.c_str(), "type@=bc_buy_deserve") != NULL)
			{
				SetWindOnTop();
				SetTimer(6, 3000, NULL);
				//SetTimer(0, 30000, NULL);

				if (m_bGift == TRUE)
				{
					//type@=bc_buy_deserve/lev@=3/rid@=56040/gid@=8/cnt@=1/sui@=id@A=893138@Sname@A=auto_M2tKU7LcOn@Snick@A=1020304808@Srg@A=1@Spg@A=1@Srt@A=1402149479@Sbg@A=0@Sweight@A=0@Sstrength@A=89700@Scps_id@A=0@Sps@A=1@Sver@A=20150331@Sm_deserve_lev@A=0@Scq_cnt@A=0@Sbest_dlev@A=0@S/
					std::string sender;
					int level = 0; //1-初级；2-中级；3-高级
					_getChouQinInfo(onePacket, sender, level);

					std::wstring username;
					transcode::UTF8_to_Unicode(sender.c_str(), sender.length(), username);

					std::wstring strLevel;
					if (level == 1)
					{
						strLevel = L"初级";
					}
					else if (level == 2)
					{
						strLevel = L"中级";
					}
					else if (level == 3)
					{
						strLevel = L"高级";
					}

					CString sMsg;
					//CString sInfo;
					if (m_TimeFlag == TRUE)
					{
						sMsg.Format(L"『%s』(%s)赠送主播『%s』酬勤\r\n", GetTimeStr(), username.c_str(), strLevel.c_str());
					}
					else
					{
						sMsg.Format(L"『%s』赠送主播『%s』酬勤\r\n", username.c_str(), strLevel.c_str());

					}

					m_TextGuestOSD.UpdateTextStr(sMsg, 0);

					//由于乱码(转成UNICODE)，调试查看，
					std::wstring TempMsg;
					transcode::UTF8_to_Unicode(onePacket.c_str(), onePacket.length(), TempMsg);
					OutputDebugStringW(TempMsg.c_str());
					OutputDebugStringW(L"\r\n");
					//OutputDebugStringA(onePacket.c_str());
					//OutputDebugStringA("\r\n");

					//系统提示：xxxx 赠送了高级酬勤
					// 				wchar_t tmp[1024];
					// 				wsprintfW(tmp, L"系统提示： %s 赠送了 %s 酬勤", sender.c_str(), strLevel.c_str());
				}
				
				

			}
			else if (strstr(onePacket.c_str(), "type@=keeplive") != NULL)
			{

			}
			else
			{
				//由于乱码(转成UNICODE)，调试查看，
				std::wstring TempMsg;
				transcode::UTF8_to_Unicode(onePacket.c_str(), onePacket.length(), TempMsg);

				OutputDebugStringW(TempMsg.c_str());
				OutputDebugStringW(L"\r\n");
			}
			//-------------------------------------------

			remainSize -= (packSize + 4);
			if (0 == remainSize)
			{
				break;
			}
			curPos += (packSize + 4);
			packSize = *((unsigned int*)curPos);
		}

	}
	else if (CURLE_AGAIN != res)
	{
		return false;
	}

	return true;
}

//字体设置
void CBarrageAssistantDlg::OnBnClickedBtFontset()
{
	CString strFontFace;
	CString strFontSize;
	CString strFontBold, strFontTalic, strFontUnderLine;
	INT nFontStyle=0;
	m_cb_FontFace.GetLBText(m_cb_FontFace.GetCurSel(),strFontFace);
	m_cb_FontSize.GetLBText(m_cb_FontSize.GetCurSel(),strFontSize);
	int nFontSize=_wtoi((LPTSTR)(LPCTSTR)strFontSize);
	if(((CButton*)GetDlgItem(IDC_CHECK_FONTBOLD))->GetCheck())
	{
		nFontStyle |= FontStyleBold;
		strFontBold = L"1";
	}
	else
	{
		strFontBold = L"0";
	}
	if(((CButton*)GetDlgItem(IDC_CHECK_FONTTILT))->GetCheck())
	{
		nFontStyle |= FontStyleItalic;
		strFontTalic = L"1";
	}
	else
	{
		strFontTalic = L"0";
	}
	if(((CButton*)GetDlgItem(IDC_CHECK_FONTUNDERLINE))->GetCheck())
	{
		nFontStyle |= FontStyleUnderline;
		strFontUnderLine = L"1";
	}
	else
	{
		strFontUnderLine = L"0";
	}
	//-----------------
	//设置文本字体
	m_TextOSD.SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace,nFontSize,nFontStyle);
	//重绘文本
	m_TextOSD.Draw();

	for (int i = 0; i < 30; i++)
	{
		//-----------------
		//设置文本字体
		m_TextPanelOSD[i].SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace,nFontSize,nFontStyle);
		//重绘文本
		m_TextPanelOSD[i].Draw();
	}

	//-----------------
	//设置文本字体
	m_TextFbDbHitOSD.SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace,nFontSize,nFontStyle);
	//重绘文本
	m_TextFbDbHitOSD.Draw();
	//-----------------
	//设置文本字体
	m_TextHeartOSD.SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace,nFontSize,nFontStyle);
	//重绘文本
	m_TextHeartOSD.Draw();
	//-----------------
	//设置文本字体
	m_TextPraiseOSD.SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace,nFontSize,nFontStyle);
	//重绘文本
	m_TextPraiseOSD.Draw();
	//-----------------
	//设置文本字体
	m_TextSoGoodOSD.SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace,nFontSize,nFontStyle);
	//重绘文本
	m_TextSoGoodOSD.Draw();
	//-----------------
	//设置文本字体
	m_TextSingleHitOSD.SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace,nFontSize,nFontStyle);
	//重绘文本
	m_TextSingleHitOSD.Draw();

	//设置文本字体
	m_TextGuestOSD.SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace,nFontSize,nFontStyle);
	//重绘文本
	m_TextGuestOSD.Draw();

	//保存到本地配置文件
	WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontFace", strFontFace, g_BarrageCfgPath);
	WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontSize", strFontSize, g_BarrageCfgPath);
	WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontBold", strFontBold, g_BarrageCfgPath);
	WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontTalic", strFontTalic, g_BarrageCfgPath);
	WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontUnderLine", strFontUnderLine, g_BarrageCfgPath);



}

//字体颜色设置
void CBarrageAssistantDlg::OnBnClickedBtColorset()
{
	CString sTempBaType;
	m_cb_BaType.GetWindowText(sTempBaType);

	if (sTempBaType == L"普通弹幕")
	{
		COLORREF crText1[2];
		COLORREF crText2[2];
		COLORREF crBorder[2];
		int nCurSel_TextGradientMode, HighlightGradientMode;
		Gdiplus::Color TextColor1[2];
		Gdiplus::Color TextColor2[2];
		Gdiplus::Color BorderColor[2];
		GradientMode TextGradientMode[2];

		//-------------------------------------
		crText1[0] = m_ColorBtn1.GetColor();
		crText2[0] = m_ColorBtn2.GetColor();
		crBorder[0] = m_ColorBtBorder.GetColor();
		nCurSel_TextGradientMode = m_cb_TextGradientMode.GetCurSel();
		TextColor1[0].SetFromCOLORREF(crText1[0]);
		TextColor2[0].SetFromCOLORREF(crText2[0]);
		BorderColor[0].SetFromCOLORREF(crBorder[0]);
		switch (nCurSel_TextGradientMode)
		{
		case 0:
			TextGradientMode[0] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[0] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[0] = GradientMode_Three;
			break;
		}
		for (int i = 15; i < 30; i++)
		{
			//设置文本颜色
			m_TextPanelOSD[i].SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
			//设置文本边框
			m_TextPanelOSD[i].SetTextStrBorder(BorderColor[0]);
			//-------------------------------------
		}

		crText1[1] = m_ColorBtnHighLine1.GetColor();
		crText2[1] = m_ColorBtnHighLine2.GetColor();
		crBorder[1] = m_ColorBtnHighLineBorder.GetColor();
		HighlightGradientMode = m_cb_HighlightGradientMode.GetCurSel();
		TextColor1[1].SetFromCOLORREF(crText1[1]);
		TextColor2[1].SetFromCOLORREF(crText2[1]);
		BorderColor[1].SetFromCOLORREF(crBorder[1]);
		switch (HighlightGradientMode)
		{
		case 0:
			TextGradientMode[1] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[1] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[1] = GradientMode_Three;
			break;
		}

		for (int i = 15; i < 30; i++)
		{
			//设置高亮文本颜色
			m_TextPanelOSD[i].SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
			//设置高亮文本边框
			m_TextPanelOSD[i].SetHighlightBorder(BorderColor[1]);
			//-------------------------------------
			//设置后要重新绘制文笔
			m_TextPanelOSD[i].Draw();
		}



		//保存到本地配置文件
		CString sFontColor1, sFontColor2, sFontColorBorder, sTextGradientMode;
		CString sFontColorHighLine1, sFontColorHighLine2, sFontColorHighLineBorder, sHighlightGradientMode;
		sFontColor1.Format(L"%ld", crText1[0]); sFontColor2.Format(L"%ld", crText2[0]);
		sFontColorBorder.Format(L"%ld", crBorder[0]); sTextGradientMode.Format(L"%d", TextGradientMode[0]);
		sFontColorHighLine1.Format(L"%ld", crText1[1]); sFontColorHighLine2.Format(L"%ld", crText2[1]);
		sFontColorHighLineBorder.Format(L"%ld", crBorder[1]); sHighlightGradientMode.Format(L"%d", TextGradientMode[1]);

		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontColor1", sFontColor1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontColor2", sFontColor2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontColorBorder", sFontColorBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"TextGradientMode", sTextGradientMode, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontColorHighLine1", sFontColorHighLine1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontColorHighLine2", sFontColorHighLine2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FontColorHighLineBorder", sFontColorHighLineBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HighlightGradientMode", sHighlightGradientMode, g_BarrageCfgPath);
	}

	if (sTempBaType == L"昵称弹幕")
	{
		COLORREF crText1[2];
		COLORREF crText2[2];
		COLORREF crBorder[2];
		int nCurSel_TextGradientMode, HighlightGradientMode;
		Gdiplus::Color TextColor1[2];
		Gdiplus::Color TextColor2[2];
		Gdiplus::Color BorderColor[2];
		GradientMode TextGradientMode[2];

		//-------------------------------------
		crText1[0] = m_ColorBtn1.GetColor();
		crText2[0] = m_ColorBtn2.GetColor();
		crBorder[0] = m_ColorBtBorder.GetColor();
		nCurSel_TextGradientMode = m_cb_TextGradientMode.GetCurSel();
		TextColor1[0].SetFromCOLORREF(crText1[0]);
		TextColor2[0].SetFromCOLORREF(crText2[0]);
		BorderColor[0].SetFromCOLORREF(crBorder[0]);
		switch (nCurSel_TextGradientMode)
		{
		case 0:
			TextGradientMode[0] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[0] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[0] = GradientMode_Three;
			break;
		}
		for (int i = 0; i < 15; i++)
		{
			//设置文本颜色
			m_TextPanelOSD[i].SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
			//设置文本边框
			m_TextPanelOSD[i].SetTextStrBorder(BorderColor[0]);
			//-------------------------------------
		}

		crText1[1] = m_ColorBtnHighLine1.GetColor();
		crText2[1] = m_ColorBtnHighLine2.GetColor();
		crBorder[1] = m_ColorBtnHighLineBorder.GetColor();
		HighlightGradientMode = m_cb_HighlightGradientMode.GetCurSel();
		TextColor1[1].SetFromCOLORREF(crText1[1]);
		TextColor2[1].SetFromCOLORREF(crText2[1]);
		BorderColor[1].SetFromCOLORREF(crBorder[1]);
		switch (HighlightGradientMode)
		{
		case 0:
			TextGradientMode[1] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[1] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[1] = GradientMode_Three;
			break;
		}

		for (int i = 0; i < 15; i++)
		{
			//设置高亮文本颜色
			m_TextPanelOSD[i].SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
			//设置高亮文本边框
			m_TextPanelOSD[i].SetHighlightBorder(BorderColor[1]);
			//-------------------------------------
			//设置后要重新绘制文笔
			m_TextPanelOSD[i].Draw();
		}



		//保存到本地配置文件
		CString sFontColor1, sFontColor2, sFontColorBorder, sTextGradientMode;
		CString sFontColorHighLine1, sFontColorHighLine2, sFontColorHighLineBorder, sHighlightGradientMode;
		sFontColor1.Format(L"%ld", crText1[0]); sFontColor2.Format(L"%ld", crText2[0]);
		sFontColorBorder.Format(L"%ld", crBorder[0]); sTextGradientMode.Format(L"%d", TextGradientMode[0]);
		sFontColorHighLine1.Format(L"%ld", crText1[1]); sFontColorHighLine2.Format(L"%ld", crText2[1]);
		sFontColorHighLineBorder.Format(L"%ld", crBorder[1]); sHighlightGradientMode.Format(L"%d", TextGradientMode[1]);

		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColor1", sFontColor1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColor2", sFontColor2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColorBorder", sFontColorBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"NickTextGradientMode", sTextGradientMode, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColorHighLine1", sFontColorHighLine1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColorHighLine2", sFontColorHighLine2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColorHighLineBorder", sFontColorHighLineBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"NickHighlightGradientMode", sHighlightGradientMode, g_BarrageCfgPath);
	}

	if (sTempBaType == L"单次鱼丸")
	{
		COLORREF crText1[2];
		COLORREF crText2[2];
		COLORREF crBorder[2];
		int nCurSel_TextGradientMode, HighlightGradientMode;
		Gdiplus::Color TextColor1[2];
		Gdiplus::Color TextColor2[2];
		Gdiplus::Color BorderColor[2];
		GradientMode TextGradientMode[2];

		//-------------------------------------
		crText1[0] = m_ColorBtn1.GetColor();
		crText2[0] = m_ColorBtn2.GetColor();
		crBorder[0] = m_ColorBtBorder.GetColor();
		nCurSel_TextGradientMode = m_cb_TextGradientMode.GetCurSel();
		TextColor1[0].SetFromCOLORREF(crText1[0]);
		TextColor2[0].SetFromCOLORREF(crText2[0]);
		BorderColor[0].SetFromCOLORREF(crBorder[0]);
		switch (nCurSel_TextGradientMode)
		{
		case 0:
			TextGradientMode[0] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[0] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[0] = GradientMode_Three;
			break;
		}
		//设置文本颜色
		m_TextSingleHitOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
		//设置文本边框
		m_TextSingleHitOSD.SetTextStrBorder(BorderColor[0]);
		//-------------------------------------
		crText1[1] = m_ColorBtnHighLine1.GetColor();
		crText2[1] = m_ColorBtnHighLine2.GetColor();
		crBorder[1] = m_ColorBtnHighLineBorder.GetColor();
		HighlightGradientMode = m_cb_HighlightGradientMode.GetCurSel();
		TextColor1[1].SetFromCOLORREF(crText1[1]);
		TextColor2[1].SetFromCOLORREF(crText2[1]);
		BorderColor[1].SetFromCOLORREF(crBorder[1]);
		switch (HighlightGradientMode)
		{
		case 0:
			TextGradientMode[1] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[1] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[1] = GradientMode_Three;
			break;
		}
		//设置高亮文本颜色
		m_TextSingleHitOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
		//设置高亮文本边框
		m_TextSingleHitOSD.SetHighlightBorder(BorderColor[1]);
		//-------------------------------------
		//设置后要重新绘制文笔
		m_TextSingleHitOSD.Draw();


		//保存到本地配置文件
		CString sFontColor1, sFontColor2, sFontColorBorder, sTextGradientMode;
		CString sFontColorHighLine1, sFontColorHighLine2, sFontColorHighLineBorder, sHighlightGradientMode;
		sFontColor1.Format(L"%ld", crText1[0]); sFontColor2.Format(L"%ld", crText2[0]);
		sFontColorBorder.Format(L"%ld", crBorder[0]); sTextGradientMode.Format(L"%d", TextGradientMode[0]);
		sFontColorHighLine1.Format(L"%ld", crText1[1]); sFontColorHighLine2.Format(L"%ld", crText2[1]);
		sFontColorHighLineBorder.Format(L"%ld", crBorder[1]); sHighlightGradientMode.Format(L"%d", TextGradientMode[1]);

		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColor1", sFontColor1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColor2", sFontColor2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColorBorder", sFontColorBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SgleTextGradientMode", sTextGradientMode, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColorHighLine1", sFontColorHighLine1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColorHighLine2", sFontColorHighLine2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColorHighLineBorder", sFontColorHighLineBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SgleHighlightGradientMode", sHighlightGradientMode, g_BarrageCfgPath);
	}
	
	if (sTempBaType == L"连击鱼丸")
	{
		COLORREF crText1[2];
		COLORREF crText2[2];
		COLORREF crBorder[2];
		int nCurSel_TextGradientMode, HighlightGradientMode;
		Gdiplus::Color TextColor1[2];
		Gdiplus::Color TextColor2[2];
		Gdiplus::Color BorderColor[2];
		GradientMode TextGradientMode[2];

		//-------------------------------------
		crText1[0] = m_ColorBtn1.GetColor();
		crText2[0] = m_ColorBtn2.GetColor();
		crBorder[0] = m_ColorBtBorder.GetColor();
		nCurSel_TextGradientMode = m_cb_TextGradientMode.GetCurSel();
		TextColor1[0].SetFromCOLORREF(crText1[0]);
		TextColor2[0].SetFromCOLORREF(crText2[0]);
		BorderColor[0].SetFromCOLORREF(crBorder[0]);
		switch (nCurSel_TextGradientMode)
		{
		case 0:
			TextGradientMode[0] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[0] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[0] = GradientMode_Three;
			break;
		}
		//设置文本颜色
		m_TextFbDbHitOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
		//设置文本边框
		m_TextFbDbHitOSD.SetTextStrBorder(BorderColor[0]);
		//-------------------------------------
		crText1[1] = m_ColorBtnHighLine1.GetColor();
		crText2[1] = m_ColorBtnHighLine2.GetColor();
		crBorder[1] = m_ColorBtnHighLineBorder.GetColor();
		HighlightGradientMode = m_cb_HighlightGradientMode.GetCurSel();
		TextColor1[1].SetFromCOLORREF(crText1[1]);
		TextColor2[1].SetFromCOLORREF(crText2[1]);
		BorderColor[1].SetFromCOLORREF(crBorder[1]);
		switch (HighlightGradientMode)
		{
		case 0:
			TextGradientMode[1] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[1] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[1] = GradientMode_Three;
			break;
		}
		//设置高亮文本颜色
		m_TextFbDbHitOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
		//设置高亮文本边框
		m_TextFbDbHitOSD.SetHighlightBorder(BorderColor[1]);
		//-------------------------------------
		//设置后要重新绘制文笔
		m_TextFbDbHitOSD.Draw();


		//保存到本地配置文件
		CString sFontColor1, sFontColor2, sFontColorBorder, sTextGradientMode;
		CString sFontColorHighLine1, sFontColorHighLine2, sFontColorHighLineBorder, sHighlightGradientMode;
		sFontColor1.Format(L"%ld", crText1[0]); sFontColor2.Format(L"%ld", crText2[0]);
		sFontColorBorder.Format(L"%ld", crBorder[0]); sTextGradientMode.Format(L"%d", TextGradientMode[0]);
		sFontColorHighLine1.Format(L"%ld", crText1[1]); sFontColorHighLine2.Format(L"%ld", crText2[1]);
		sFontColorHighLineBorder.Format(L"%ld", crBorder[1]); sHighlightGradientMode.Format(L"%d", TextGradientMode[1]);

		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColor1", sFontColor1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColor2", sFontColor2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColorBorder", sFontColorBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitTextGradientMode", sTextGradientMode, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColorHighLine1", sFontColorHighLine1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColorHighLine2", sFontColorHighLine2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitontColorHighLineBorder", sFontColorHighLineBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitHighlightGradientMode", sHighlightGradientMode, g_BarrageCfgPath);
	}

	if (sTempBaType == L"连击点赞")
	{
		COLORREF crText1[2];
		COLORREF crText2[2];
		COLORREF crBorder[2];
		int nCurSel_TextGradientMode, HighlightGradientMode;
		Gdiplus::Color TextColor1[2];
		Gdiplus::Color TextColor2[2];
		Gdiplus::Color BorderColor[2];
		GradientMode TextGradientMode[2];

		//-------------------------------------
		crText1[0] = m_ColorBtn1.GetColor();
		crText2[0] = m_ColorBtn2.GetColor();
		crBorder[0] = m_ColorBtBorder.GetColor();
		nCurSel_TextGradientMode = m_cb_TextGradientMode.GetCurSel();
		TextColor1[0].SetFromCOLORREF(crText1[0]);
		TextColor2[0].SetFromCOLORREF(crText2[0]);
		BorderColor[0].SetFromCOLORREF(crBorder[0]);
		switch (nCurSel_TextGradientMode)
		{
		case 0:
			TextGradientMode[0] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[0] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[0] = GradientMode_Three;
			break;
		}
		//设置文本颜色
		m_TextPraiseOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
		//设置文本边框
		m_TextPraiseOSD.SetTextStrBorder(BorderColor[0]);
		//-------------------------------------
		crText1[1] = m_ColorBtnHighLine1.GetColor();
		crText2[1] = m_ColorBtnHighLine2.GetColor();
		crBorder[1] = m_ColorBtnHighLineBorder.GetColor();
		HighlightGradientMode = m_cb_HighlightGradientMode.GetCurSel();
		TextColor1[1].SetFromCOLORREF(crText1[1]);
		TextColor2[1].SetFromCOLORREF(crText2[1]);
		BorderColor[1].SetFromCOLORREF(crBorder[1]);
		switch (HighlightGradientMode)
		{
		case 0:
			TextGradientMode[1] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[1] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[1] = GradientMode_Three;
			break;
		}
		//设置高亮文本颜色
		m_TextPraiseOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
		//设置高亮文本边框
		m_TextPraiseOSD.SetHighlightBorder(BorderColor[1]);
		//-------------------------------------
		//设置后要重新绘制文笔
		m_TextPraiseOSD.Draw();


		//保存到本地配置文件
		CString sFontColor1, sFontColor2, sFontColorBorder, sTextGradientMode;
		CString sFontColorHighLine1, sFontColorHighLine2, sFontColorHighLineBorder, sHighlightGradientMode;
		sFontColor1.Format(L"%ld", crText1[0]); sFontColor2.Format(L"%ld", crText2[0]);
		sFontColorBorder.Format(L"%ld", crBorder[0]); sTextGradientMode.Format(L"%d", TextGradientMode[0]);
		sFontColorHighLine1.Format(L"%ld", crText1[1]); sFontColorHighLine2.Format(L"%ld", crText2[1]);
		sFontColorHighLineBorder.Format(L"%ld", crBorder[1]); sHighlightGradientMode.Format(L"%d", TextGradientMode[1]);

		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColor1", sFontColor1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColor2", sFontColor2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColorBorder", sFontColorBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"PraTextGradientMode", sTextGradientMode, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColorHighLine1", sFontColorHighLine1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColorHighLine2", sFontColorHighLine2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"PraSgitontColorHighLineBorder", sFontColorHighLineBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"PraSgleHighlightGradientMode", sHighlightGradientMode, g_BarrageCfgPath);
	}

	if (sTempBaType == L"连击很溜")
	{
		COLORREF crText1[2];
		COLORREF crText2[2];
		COLORREF crBorder[2];
		int nCurSel_TextGradientMode, HighlightGradientMode;
		Gdiplus::Color TextColor1[2];
		Gdiplus::Color TextColor2[2];
		Gdiplus::Color BorderColor[2];
		GradientMode TextGradientMode[2];

		//-------------------------------------
		crText1[0] = m_ColorBtn1.GetColor();
		crText2[0] = m_ColorBtn2.GetColor();
		crBorder[0] = m_ColorBtBorder.GetColor();
		nCurSel_TextGradientMode = m_cb_TextGradientMode.GetCurSel();
		TextColor1[0].SetFromCOLORREF(crText1[0]);
		TextColor2[0].SetFromCOLORREF(crText2[0]);
		BorderColor[0].SetFromCOLORREF(crBorder[0]);
		switch (nCurSel_TextGradientMode)
		{
		case 0:
			TextGradientMode[0] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[0] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[0] = GradientMode_Three;
			break;
		}
		//设置文本颜色
		m_TextSoGoodOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
		//设置文本边框
		m_TextSoGoodOSD.SetTextStrBorder(BorderColor[0]);
		//-------------------------------------
		crText1[1] = m_ColorBtnHighLine1.GetColor();
		crText2[1] = m_ColorBtnHighLine2.GetColor();
		crBorder[1] = m_ColorBtnHighLineBorder.GetColor();
		HighlightGradientMode = m_cb_HighlightGradientMode.GetCurSel();
		TextColor1[1].SetFromCOLORREF(crText1[1]);
		TextColor2[1].SetFromCOLORREF(crText2[1]);
		BorderColor[1].SetFromCOLORREF(crBorder[1]);
		switch (HighlightGradientMode)
		{
		case 0:
			TextGradientMode[1] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[1] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[1] = GradientMode_Three;
			break;
		}
		//设置高亮文本颜色
		m_TextSoGoodOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
		//设置高亮文本边框
		m_TextSoGoodOSD.SetHighlightBorder(BorderColor[1]);
		//-------------------------------------
		//设置后要重新绘制文笔
		m_TextSoGoodOSD.Draw();


		//保存到本地配置文件
		CString sFontColor1, sFontColor2, sFontColorBorder, sTextGradientMode;
		CString sFontColorHighLine1, sFontColorHighLine2, sFontColorHighLineBorder, sHighlightGradientMode;
		sFontColor1.Format(L"%ld", crText1[0]); sFontColor2.Format(L"%ld", crText2[0]);
		sFontColorBorder.Format(L"%ld", crBorder[0]); sTextGradientMode.Format(L"%d", TextGradientMode[0]);
		sFontColorHighLine1.Format(L"%ld", crText1[1]); sFontColorHighLine2.Format(L"%ld", crText2[1]);
		sFontColorHighLineBorder.Format(L"%ld", crBorder[1]); sHighlightGradientMode.Format(L"%d", TextGradientMode[1]);

		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColor1", sFontColor1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColor2", sFontColor2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColorBorder", sFontColorBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SogdTextGradientMode", sTextGradientMode, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColorHighLine1", sFontColorHighLine1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColorHighLine2", sFontColorHighLine2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SogditontColorHighLineBorder", sFontColorHighLineBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"SogdleHighlightGradientMode", sHighlightGradientMode, g_BarrageCfgPath);
	}

	if (sTempBaType == L"连击爱心")
	{
		COLORREF crText1[2];
		COLORREF crText2[2];
		COLORREF crBorder[2];
		int nCurSel_TextGradientMode, HighlightGradientMode;
		Gdiplus::Color TextColor1[2];
		Gdiplus::Color TextColor2[2];
		Gdiplus::Color BorderColor[2];
		GradientMode TextGradientMode[2];

		//-------------------------------------
		crText1[0] = m_ColorBtn1.GetColor();
		crText2[0] = m_ColorBtn2.GetColor();
		crBorder[0] = m_ColorBtBorder.GetColor();
		nCurSel_TextGradientMode = m_cb_TextGradientMode.GetCurSel();
		TextColor1[0].SetFromCOLORREF(crText1[0]);
		TextColor2[0].SetFromCOLORREF(crText2[0]);
		BorderColor[0].SetFromCOLORREF(crBorder[0]);
		switch (nCurSel_TextGradientMode)
		{
		case 0:
			TextGradientMode[0] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[0] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[0] = GradientMode_Three;
			break;
		}
		//设置文本颜色
		m_TextHeartOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
		//设置文本边框
		m_TextHeartOSD.SetTextStrBorder(BorderColor[0]);
		//-------------------------------------
		crText1[1] = m_ColorBtnHighLine1.GetColor();
		crText2[1] = m_ColorBtnHighLine2.GetColor();
		crBorder[1] = m_ColorBtnHighLineBorder.GetColor();
		HighlightGradientMode = m_cb_HighlightGradientMode.GetCurSel();
		TextColor1[1].SetFromCOLORREF(crText1[1]);
		TextColor2[1].SetFromCOLORREF(crText2[1]);
		BorderColor[1].SetFromCOLORREF(crBorder[1]);
		switch (HighlightGradientMode)
		{
		case 0:
			TextGradientMode[1] = GradientMode_None;
			break;
		case 1:
			TextGradientMode[1] = GradientMode_Two;
			break;
		case 2:
			TextGradientMode[1] = GradientMode_Three;
			break;
		}
		//设置高亮文本颜色
		m_TextHeartOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
		//设置高亮文本边框
		m_TextHeartOSD.SetHighlightBorder(BorderColor[1]);
		//-------------------------------------
		//设置后要重新绘制文笔
		m_TextHeartOSD.Draw();


		//保存到本地配置文件
		CString sFontColor1, sFontColor2, sFontColorBorder, sTextGradientMode;
		CString sFontColorHighLine1, sFontColorHighLine2, sFontColorHighLineBorder, sHighlightGradientMode;
		sFontColor1.Format(L"%ld", crText1[0]); sFontColor2.Format(L"%ld", crText2[0]);
		sFontColorBorder.Format(L"%ld", crBorder[0]); sTextGradientMode.Format(L"%d", TextGradientMode[0]);
		sFontColorHighLine1.Format(L"%ld", crText1[1]); sFontColorHighLine2.Format(L"%ld", crText2[1]);
		sFontColorHighLineBorder.Format(L"%ld", crBorder[1]); sHighlightGradientMode.Format(L"%d", TextGradientMode[1]);

		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColor1", sFontColor1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColor2", sFontColor2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColorBorder", sFontColorBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HeartTextGradientMode", sTextGradientMode, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColorHighLine1", sFontColorHighLine1, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColorHighLine2", sFontColorHighLine2, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColorHighLineBorder", sFontColorHighLineBorder, g_BarrageCfgPath);
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"HeartHighlightGradientMode", sHighlightGradientMode, g_BarrageCfgPath);
	}
}

void CBarrageAssistantDlg::OnBnClickedBtAlignmentset()
{
	CString sTemp;
	m_cb_AlignmentMethod.GetWindowText(sTemp);

	if (sTemp == L"居中")
	{
		m_TextOSD.SetAlignmentMethod(StringAlignmentCenter);
		for (int i = 0; i < 30; i++)
		{
			m_TextPanelOSD[i].SetAlignmentMethod(StringAlignmentCenter);
		}
	}
	if (sTemp == L"靠左")
	{
		m_TextOSD.SetAlignmentMethod(StringAlignmentNear);
		for (int i = 0; i < 30; i++)
		{
			m_TextPanelOSD[i].SetAlignmentMethod(StringAlignmentNear);
		}
	}
	if (sTemp == L"靠右")
	{
		m_TextOSD.SetAlignmentMethod(StringAlignmentFar);
		for (int i = 0; i < 30; i++)
		{
			m_TextPanelOSD[i].SetAlignmentMethod(StringAlignmentFar);
		}
	}

	WritePrivateProfileStringW(L"BARRAGESSISTANT", L"AlignmentMethod", sTemp, g_BarrageCfgPath);
}

void CBarrageAssistantDlg::OnBnClickedCheckTimeflag()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_TIMEFLAG))->GetCheck())
	{
		m_TimeFlag = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Time", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_TimeFlag = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Time", L"0", g_BarrageCfgPath);
	}

	
}

void CBarrageAssistantDlg::OnBnClickedCheckOsdlock()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_OSDLOCK))->GetCheck())
	{
		::SetWindowLong(m_TextOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		::SetWindowLong(m_TextFbDbHitOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		::SetWindowLong(m_TextHeartOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		::SetWindowLong(m_TextPraiseOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		::SetWindowLong(m_TextSoGoodOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		::SetWindowLong(m_TextSingleHitOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		::SetWindowLong(m_TextGuestOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		::SetWindowLong(m_TextRoomInfoOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		
		for (int i = 0; i < 30; i++)
		{
			::SetWindowLong(m_TextPanelOSD[i].GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
			m_TextPanelOSD[i].m_bLock = TRUE;

		}
		m_TextOSD.m_bLock = TRUE;
		m_TextFbDbHitOSD.m_bLock = TRUE;
		m_TextHeartOSD.m_bLock = TRUE;
		m_TextPraiseOSD.m_bLock = TRUE;
		m_TextSoGoodOSD.m_bLock = TRUE;
		m_TextSingleHitOSD.m_bLock = TRUE;
		m_TextGuestOSD.m_bLock = TRUE;
		m_TextRoomInfoOSD.m_bLock = TRUE;

		SetTimer(0, 5000, NULL);
		if (m_bExit == false)
		{

			CString sTemp;
			sTemp.Format(L"%s%s", g_sInit, L"(房间已连接)鼠标放在当前,可拖动OSD，返回软件界可设(锁定/解锁)\r\n[当前已锁定]");
			m_TextOSD.UpdateTextStr(sTemp, 0);
		}
		else
		{
			CString sTemp;
			sTemp.Format(L"%s%s", g_sInit, L"(房间未连接)鼠标放在当前,可拖动OSD，返回软件界可设(锁定/解锁)\r\n[当前已锁定]");
			m_TextOSD.UpdateTextStr(sTemp, 0);
		}

	}
	else
	{
		::SetWindowLong(m_TextOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
 		::SetWindowLong(m_TextFbDbHitOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
 		::SetWindowLong(m_TextHeartOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
 		::SetWindowLong(m_TextPraiseOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
 		::SetWindowLong(m_TextSoGoodOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
 		::SetWindowLong(m_TextSingleHitOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
		::SetWindowLong(m_TextGuestOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
		::SetWindowLong(m_TextRoomInfoOSD.GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);

		for (int i = 0; i < 30; i++)
		{
			::SetWindowLong(m_TextPanelOSD[i].GetSafeHwnd() , GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
			m_TextPanelOSD[i].m_bLock = FALSE;
		}

		m_TextOSD.m_bLock = FALSE;
 		m_TextFbDbHitOSD.m_bLock = FALSE;
 		m_TextHeartOSD.m_bLock = FALSE;
 		m_TextPraiseOSD.m_bLock = FALSE;
 		m_TextSoGoodOSD.m_bLock = FALSE;
 		m_TextSingleHitOSD.m_bLock = FALSE;
		m_TextGuestOSD.m_bLock = FALSE;
		m_TextRoomInfoOSD.m_bLock = FALSE;

		SetTimer(0, 5000, NULL);
		if (m_bExit == false)
		{

			CString sTemp;
			sTemp.Format(L"%s%s", g_sInit, L"(房间已连接)鼠标放在当前,可拖动OSD，返回软件界可设(锁定/解锁)\r\n[当前已解锁]");
			m_TextOSD.UpdateTextStr(sTemp, 0);
		}
		else
		{
			CString sTemp;
			sTemp.Format(L"%s%s", g_sInit, L"(房间未连接)鼠标放在当前,可拖动OSD，返回软件界可设(锁定/解锁)\r\n[当前已解锁]");
			m_TextOSD.UpdateTextStr(sTemp, 0);
		}
	}
}


//显示鱼丸/单个鱼丸，如xx送了100个..
void CBarrageAssistantDlg::OnBnClickedCheckFishBall()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_FISH_BALL))->GetCheck())
	{
		m_bFishBall = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FishBall", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_bFishBall = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"FishBall", L"0", g_BarrageCfgPath);
	}
}

//显示禁言
void CBarrageAssistantDlg::OnBnClickedCheckBanned()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_BANNED))->GetCheck())
	{
		m_bBannedToPost = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"BannedToPost", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_bBannedToPost = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"BannedToPost", L"0", g_BarrageCfgPath);
	}
}

//酬勤
void CBarrageAssistantDlg::OnBnClickedCheckGift()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_GIFT))->GetCheck())
	{
		m_bGift = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Gift", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_bGift = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Gift", L"0", g_BarrageCfgPath);
	}
}

//来宾
void CBarrageAssistantDlg::OnBnClickedCheckGuest()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_GUEST))->GetCheck())
	{
		m_bGuest = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Guest", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_bGuest = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Guest", L"0", g_BarrageCfgPath);
	}
}

//赞
void CBarrageAssistantDlg::OnBnClickedCheckGdn()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_GDN))->GetCheck())
	{
		m_bPraise = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Praise", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_bPraise = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Praise", L"0", g_BarrageCfgPath);
	}
}

//弹幕
void CBarrageAssistantDlg::OnBnClickedCheckBa()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_BA))->GetCheck())
	{
		m_bBarrageAssistant = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"BA", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_bBarrageAssistant = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"BA", L"0", g_BarrageCfgPath);
	}
}

//心
void CBarrageAssistantDlg::OnBnClickedCheckHeart()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_HEART))->GetCheck())
	{
		m_bHeart = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Heart", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_bHeart = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"Heart", L"0", g_BarrageCfgPath);
	}
}

//666
void CBarrageAssistantDlg::OnBnClickedCheck666()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_666))->GetCheck())
	{
		m_b666 = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"666", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_b666 = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"666", L"0", g_BarrageCfgPath);
	}
}

//连击鱼丸，如100x ?个鱼丸
void CBarrageAssistantDlg::OnBnClickedCheckFishDbhit()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_FISH_DBHIT))->GetCheck())
	{
		m_bFbDbHit = TRUE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbHit", L"1", g_BarrageCfgPath);
	}
	else
	{
		m_bFbDbHit = FALSE;
		WritePrivateProfileStringW(L"BARRAGESSISTANT", L"DbHit", L"0", g_BarrageCfgPath);
	}
}

void CBarrageAssistantDlg::OnMenuRestore()
{
	CWnd *TmpWnd;
	TmpWnd = FromHandle(this->GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
}

void CBarrageAssistantDlg::OnMenuExit()
{
	OnCancel();
}

LRESULT CBarrageAssistantDlg::OnShowTrayTipMsg(WPARAM wParam,LPARAM lParam)
{
	if(wParam != IDR_BARRAGE_MENU)
	{
		return 1;
	}

	switch(lParam)
	{
	case WM_RBUTTONUP:			//右键起来时弹出快捷菜单，这里只有一个"关闭"
		{
			//LPPOINT lpoint=new tagPOINT;
			//::GetCursorPos(lpoint);			//得到鼠标位置
			//CMenu menu;
			//menu.CreatePopupMenu();				//声明一个弹出式菜单			
			//menu.AppendMenu(MF_STRING,WM_DESTROY,_T("关闭"));   //增加菜单项"关闭"，点击则发送消息WM_DESTROY给主窗口（已隐藏），将程序结束。			
			//menu.TrackPopupMenu(TPM_LEFTALIGN,lpoint->x,lpoint->y,this);//确定弹出式菜单的位置			
			//SetForegroundWindow();
			//HMENU hmenu=menu.Detach();   //资源回收
			//menu.DestroyMenu();
			//delete lpoint;
			CMenu menu, *pSubMenu;

			if (!menu.LoadMenu(m_Notify.uID)) return 0;
			if (!(pSubMenu = menu.GetSubMenu(0))) return 0;

			// Make first menu item the default (bold font)
			::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);

			//Display and track the popup menu
			CPoint pos;
			GetCursorPos(&pos);
			::SetForegroundWindow(m_Notify.hWnd);  
			::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y, 0, m_Notify.hWnd, NULL);

			//pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this, NULL);
			menu.DestroyMenu();

		}break;
	case WM_LBUTTONDBLCLK:    //双击左键的处理
		{
			this->ShowWindow(SW_SHOW);    //简单的显示主窗口完事儿
		}break;
	}
	return 0;
}

//屏蔽esc&enter
BOOL CBarrageAssistantDlg::PreTranslateMessage(MSG* pMsg)
{
	//屏蔽ESC
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN )     
	{

		return TRUE;
	}
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)    
	{
		return TRUE;  
	}



	return CBCGPDialog::PreTranslateMessage(pMsg);
}

//30秒无任何信息，隐藏各OSD
void CBarrageAssistantDlg::OnTimer(UINT_PTR nIDEvent)
{
 	if (nIDEvent == 0)
 	{
 		if (m_TextOSD.m_hWnd != NULL)
 		{
 			m_TextOSD.UpdateTextStr(L"", 0);
 		}
 	}
 
	if (nIDEvent == 1)
	{
		if (m_TextOSD.m_hWnd != NULL)
		{
			m_TextFbDbHitOSD.UpdateTextStr(L"", 0);
		}
	}


	if (nIDEvent == 2)
	{
		if (m_TextOSD.m_hWnd != NULL)
		{
			m_TextSoGoodOSD.UpdateTextStr(L"", 0);
		}
	}


	if (nIDEvent == 3)
	{
		if (m_TextOSD.m_hWnd != NULL)
		{
			m_TextHeartOSD.UpdateTextStr(L"", 0);
		}
	}


	if (nIDEvent == 4)
	{
		if (m_TextOSD.m_hWnd != NULL)
		{
			m_TextPraiseOSD.UpdateTextStr(L"", 0);
		}
	}

	if (nIDEvent == 5)
	{
		if (m_TextOSD.m_hWnd != NULL)
		{
			m_TextSingleHitOSD.UpdateTextStr(L"", 0);
		}
	}
	if (nIDEvent == 6)
	{
		if (m_TextOSD.m_hWnd != NULL)
		{
			m_TextGuestOSD.UpdateTextStr(L"", 0);
		}
	}
	
	if (nIDEvent == 7)
	{
		if (m_TextOSD.m_hWnd != NULL)
		{
			for (int i = 0; i < 30; i++)
			{
				m_TextPanelOSD[i].UpdateTextStr(L"", 0);
			}
			
		}
	}

	if (nIDEvent == 8)
	{
		if (m_TextOSD.m_hWnd != NULL)
		{

			m_TextSingleHitOSD.UpdateTextStr(L"", 0);


		}
	}
	CBCGPDialog::OnTimer(nIDEvent);
}

void CBarrageAssistantDlg::UpdateFontSet()
{
	CString strFontFace;
	CString strFontSize;
	CString strFontBold, strFontTalic, strFontUnderLine;
	INT nFontStyle = 0;

	COLORREF crText1[2];
	COLORREF crText2[2];
	COLORREF crBorder[2];
	Gdiplus::Color TextColor1[2];
	Gdiplus::Color TextColor2[2];
	Gdiplus::Color BorderColor[2];
	GradientMode TextGradientMode[2];

	CString sFontColor1, sFontColor2, sFontColorBorder, sTextGradientMode;
	CString sFontColorHighLine1, sFontColorHighLine2, sFontColorHighLineBorder, sHighlightGradientMode;
	CString sLoginType, sUserID, sAlignmentMethod, sTime, sFishBall, sBannedToPost, sGift, sGuest, sPraise, sBA, sHeart, s666, sDbHit;

	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontFace", L"黑体", strFontFace.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontSize", L"35", strFontSize.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontBold", L"0", strFontBold.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontTalic", L"0", strFontTalic.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontUnderLine", L"0", strFontUnderLine.GetBuffer(255), 255, g_BarrageCfgPath);

	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontColor1", L"255", sFontColor1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontColor2", L"65535", sFontColor2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontColorBorder", L"0", sFontColorBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"TextGradientMode", L"1", sTextGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontColorHighLine1", L"16746496", sFontColorHighLine1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontColorHighLine2", L"16776960", sFontColorHighLine2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FontColorHighLineBorder", L"0", sFontColorHighLineBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HighlightGradientMode", L"1", sHighlightGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);

	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"LoginType", L"用户ID", sLoginType.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"UserID", L"", sUserID.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"AlignmentMethod", L"靠左", sAlignmentMethod.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"Time", L"0", sTime.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"FishBall", L"0", sFishBall.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"BannedToPost", L"0", sBannedToPost.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"Gift", L"0", sGift.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"Guest", L"0", sGuest.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"Praise", L"0", sPraise.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"BA", L"0", sBA.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"Heart", L"0", sHeart.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"666", L"0", s666.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbHit", L"0", sDbHit.GetBuffer(255), 255, g_BarrageCfgPath);


	crText1[0] = _wtoi64(sFontColor1);
	crText1[1] = _wtoi64(sFontColorHighLine1);
	crText2[0] = _wtoi64(sFontColor2);
	crText2[1] = _wtoi64(sFontColorHighLine2);
	crBorder[0] = _wtoi64(sFontColorBorder);
	crBorder[1] = _wtoi64(sFontColorHighLineBorder);
	TextColor1[0].SetFromCOLORREF(crText1[0]);
	TextColor2[0].SetFromCOLORREF(crText2[0]);
	BorderColor[0].SetFromCOLORREF(crBorder[0]);
	TextColor1[1].SetFromCOLORREF(crText1[1]);
	TextColor2[1].SetFromCOLORREF(crText2[1]);
	BorderColor[1].SetFromCOLORREF(crBorder[1]);
	if (_wtoi(sTextGradientMode) == 0)
	{
		TextGradientMode[0] = GradientMode_None;
	}
	if (_wtoi(sTextGradientMode) == 1)
	{
		TextGradientMode[0] = GradientMode_Two;
	}
	if (_wtoi(sTextGradientMode) == 2)
	{
		TextGradientMode[0] = GradientMode_Three;
	}
	if (_wtoi(sHighlightGradientMode) == 0)
	{
		TextGradientMode[1] = GradientMode_None;
	}
	if (_wtoi(sHighlightGradientMode) == 1)
	{
		TextGradientMode[1] = GradientMode_Two;
	}
	if (_wtoi(sHighlightGradientMode) == 2)
	{
		TextGradientMode[1] = GradientMode_Three;
	}

	if (sLoginType == L"用户ID")
	{
		m_cb_LoginType.SetCurSel(0);
	}
	if (sLoginType == L"房间ID")
	{
		m_cb_LoginType.SetCurSel(1);
	}
	m_EditID.SetWindowText(sUserID);

	if (sAlignmentMethod == L"居中")
	{
		m_TextOSD.SetAlignmentMethod(StringAlignmentCenter);
		for (int i = 0; i < 30; i++)
		{
			m_TextPanelOSD[i].SetAlignmentMethod(StringAlignmentCenter);
		}
		m_cb_AlignmentMethod.SetCurSel(0);
	}
	if (sAlignmentMethod == L"靠左")
	{
		m_TextOSD.SetAlignmentMethod(StringAlignmentNear);
		for (int i = 0; i < 30; i++)
		{
			m_TextPanelOSD[i].SetAlignmentMethod(StringAlignmentCenter);
		}
		m_cb_AlignmentMethod.SetCurSel(1);
	}
	if (sAlignmentMethod == L"靠右")
	{
		m_TextOSD.SetAlignmentMethod(StringAlignmentFar);
		for (int i = 0; i < 30; i++)
		{
			m_TextPanelOSD[i].SetAlignmentMethod(StringAlignmentCenter);
		}
		m_cb_AlignmentMethod.SetCurSel(2);
	}

	if (sTime == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_TIMEFLAG))->SetCheck(0);
		m_TimeFlag = FALSE;
	}
	if (sTime == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_TIMEFLAG))->SetCheck(1);
		m_TimeFlag = TRUE;
	}

	if (sFishBall == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_FISH_BALL))->SetCheck(0);
		m_bFishBall = FALSE;
	}
	if (sFishBall == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_FISH_BALL))->SetCheck(1);
		m_bFishBall = TRUE;
	}
	if (sBannedToPost == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_BANNED))->SetCheck(0);
		m_bBannedToPost = FALSE;
	}
	if (sBannedToPost == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_BANNED))->SetCheck(1);
		m_bBannedToPost = TRUE;
	}
	if (sGift == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_GIFT))->SetCheck(0);
		m_bGift = FALSE;
	}
	if (sGift == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_GIFT))->SetCheck(1);
		m_bGift = TRUE;
	}
	if (sGuest == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_GUEST))->SetCheck(0);
		m_bGuest = FALSE;
	}
	if (sGuest == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_GUEST))->SetCheck(1);
		m_bGuest = TRUE;
	}

	if (sPraise == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_GDN))->SetCheck(0);
		m_bPraise = FALSE;
	}
	if (sPraise == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_GDN))->SetCheck(1);
		m_bPraise = TRUE;
	}

	if (sBA == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_BA))->SetCheck(0);
		m_bBarrageAssistant = FALSE;
	}
	if (sBA == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_BA))->SetCheck(1);
		m_bBarrageAssistant = TRUE;
	}

	if (sHeart == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_HEART))->SetCheck(0);
		m_bHeart = FALSE;
	}
	if (sHeart == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_HEART))->SetCheck(1);
		m_bHeart = TRUE;
	}

	if (s666 == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_666))->SetCheck(0);
		m_b666 = FALSE;
	}
	if (s666 == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_666))->SetCheck(1);
		m_b666 = TRUE;
	}

	if (sDbHit == L"0")
	{
		((CButton*)GetDlgItem(IDC_CHECK_FISH_DBHIT))->SetCheck(0);
		m_bFbDbHit = FALSE;
	}
	if (sDbHit == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_FISH_DBHIT))->SetCheck(1);
		m_bFbDbHit = TRUE;
	}


	for (int i = 0; i < m_cb_FontFace.GetCount(); i++)
	{
		CString sTmp;
		m_cb_FontFace.GetLBText(i, sTmp);
		if (sTmp == strFontFace)
		{
			m_cb_FontFace.SetCurSel(i);
		}
	}
	
	for (int i = 0; i < m_cb_FontSize.GetCount(); i++)
	{
		CString sTmp;
		m_cb_FontSize.GetLBText(i, sTmp);
		if (sTmp == strFontSize)
		{
			m_cb_FontSize.SetCurSel(i);
		}
	}

	if (strFontBold == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_FONTBOLD))->SetCheck(1);
		nFontStyle |= FontStyleBold;
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CHECK_FONTBOLD))->SetCheck(0);
	}
	if (strFontTalic == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_FONTTILT))->SetCheck(1);
		nFontStyle |= FontStyleItalic;
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CHECK_FONTTILT))->SetCheck(0);
	}
	if (strFontUnderLine == L"1")
	{
		((CButton*)GetDlgItem(IDC_CHECK_FONTUNDERLINE))->SetCheck(1);
		nFontStyle |= FontStyleUnderline;
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CHECK_FONTUNDERLINE))->SetCheck(0);
	}
	
	m_ColorBtn1.SetColor(crText1[0]);
	m_ColorBtn2.SetColor(crText2[0]);
	m_ColorBtBorder.SetColor(crBorder[0]);
	m_cb_TextGradientMode.SetCurSel(TextGradientMode[0]);
	m_ColorBtnHighLine1.SetColor(crText1[1]);
	m_ColorBtnHighLine2.SetColor(crText2[1]);
	m_ColorBtnHighLineBorder.SetColor(crBorder[1]);
	m_cb_HighlightGradientMode.SetCurSel(TextGradientMode[1]);


	for (int i = 15; i < 30; i++)
	{
		//-----------------
		//设置文本字体
		m_TextPanelOSD[i].SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace, _wtoi(strFontSize), nFontStyle);
		//设置文本颜色
		m_TextPanelOSD[i].SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
		//设置文本边框
		m_TextPanelOSD[i].SetTextStrBorder(BorderColor[0]);
		//-------------------------------------
		//设置高亮文本颜色
		m_TextPanelOSD[i].SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
		//设置高亮文本边框
		m_TextPanelOSD[i].SetHighlightBorder(BorderColor[1]);
		//-------------------------------------
		//重绘文本
		m_TextPanelOSD[i].Draw();
	}

	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColor1", L"255", sFontColor1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColor2", L"65535", sFontColor2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColorBorder", L"0", sFontColorBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"NickTextGradientMode", L"1", sTextGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColorHighLine1", L"16746496", sFontColorHighLine1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColorHighLine2", L"16776960", sFontColorHighLine2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"NickFontColorHighLineBorder", L"0", sFontColorHighLineBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"NickHighlightGradientMode", L"1", sHighlightGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);

	crText1[0] = _wtoi64(sFontColor1);
	crText1[1] = _wtoi64(sFontColorHighLine1);
	crText2[0] = _wtoi64(sFontColor2);
	crText2[1] = _wtoi64(sFontColorHighLine2);
	crBorder[0] = _wtoi64(sFontColorBorder);
	crBorder[1] = _wtoi64(sFontColorHighLineBorder);
	TextColor1[0].SetFromCOLORREF(crText1[0]);
	TextColor2[0].SetFromCOLORREF(crText2[0]);
	BorderColor[0].SetFromCOLORREF(crBorder[0]);
	TextColor1[1].SetFromCOLORREF(crText1[1]);
	TextColor2[1].SetFromCOLORREF(crText2[1]);
	BorderColor[1].SetFromCOLORREF(crBorder[1]);
	if (_wtoi(sTextGradientMode) == 0)
	{
		TextGradientMode[0] = GradientMode_None;
	}
	if (_wtoi(sTextGradientMode) == 1)
	{
		TextGradientMode[0] = GradientMode_Two;
	}
	if (_wtoi(sTextGradientMode) == 2)
	{
		TextGradientMode[0] = GradientMode_Three;
	}
	if (_wtoi(sHighlightGradientMode) == 0)
	{
		TextGradientMode[1] = GradientMode_None;
	}
	if (_wtoi(sHighlightGradientMode) == 1)
	{
		TextGradientMode[1] = GradientMode_Two;
	}
	if (_wtoi(sHighlightGradientMode) == 2)
	{
		TextGradientMode[1] = GradientMode_Three;
	}

	for (int i = 0; i < 15; i++)
	{
		//-----------------
		//设置文本字体
		m_TextPanelOSD[i].SetTextStrFont((LPTSTR)(LPCTSTR)strFontFace, _wtoi(strFontSize), nFontStyle);
		//设置文本颜色
		m_TextPanelOSD[i].SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
		//设置文本边框
		m_TextPanelOSD[i].SetTextStrBorder(BorderColor[0]);
		//-------------------------------------
		//设置高亮文本颜色
		m_TextPanelOSD[i].SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
		//设置高亮文本边框
		m_TextPanelOSD[i].SetHighlightBorder(BorderColor[1]);
		//-------------------------------------
		//重绘文本
		m_TextPanelOSD[i].Draw();
	}



	//////////////////////////////////////////////////////////////////////////
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColor1", L"255", sFontColor1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColor2", L"65535", sFontColor2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColorBorder", L"0", sFontColorBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SgleTextGradientMode", L"1", sTextGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColorHighLine1", L"16746496", sFontColorHighLine1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColorHighLine2", L"16776960", sFontColorHighLine2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SgleFontColorHighLineBorder", L"0", sFontColorHighLineBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SgleHighlightGradientMode", L"1", sHighlightGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);

	crText1[0] = _wtoi64(sFontColor1);
	crText1[1] = _wtoi64(sFontColorHighLine1);
	crText2[0] = _wtoi64(sFontColor2);
	crText2[1] = _wtoi64(sFontColorHighLine2);
	crBorder[0] = _wtoi64(sFontColorBorder);
	crBorder[1] = _wtoi64(sFontColorHighLineBorder);
	TextColor1[0].SetFromCOLORREF(crText1[0]);
	TextColor2[0].SetFromCOLORREF(crText2[0]);
	BorderColor[0].SetFromCOLORREF(crBorder[0]);
	TextColor1[1].SetFromCOLORREF(crText1[1]);
	TextColor2[1].SetFromCOLORREF(crText2[1]);
	BorderColor[1].SetFromCOLORREF(crBorder[1]);
	if (_wtoi(sTextGradientMode) == 0)
	{
		TextGradientMode[0] = GradientMode_None;
	}
	if (_wtoi(sTextGradientMode) == 1)
	{
		TextGradientMode[0] = GradientMode_Two;
	}
	if (_wtoi(sTextGradientMode) == 2)
	{
		TextGradientMode[0] = GradientMode_Three;
	}
	if (_wtoi(sHighlightGradientMode) == 0)
	{
		TextGradientMode[1] = GradientMode_None;
	}
	if (_wtoi(sHighlightGradientMode) == 1)
	{
		TextGradientMode[1] = GradientMode_Two;
	}
	if (_wtoi(sHighlightGradientMode) == 2)
	{
		TextGradientMode[1] = GradientMode_Three;
	}

	//-----------------
	//设置文本字体
	m_TextSingleHitOSD.SetTextStrFont((LPTSTR)(LPCTSTR)L"黑体", 15, FontStyleBold);
	//设置文本颜色
	m_TextSingleHitOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
	//设置文本边框
	m_TextSingleHitOSD.SetTextStrBorder(BorderColor[0]);
	//-------------------------------------
	//设置高亮文本颜色
	m_TextSingleHitOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
	//设置高亮文本边框
	m_TextSingleHitOSD.SetHighlightBorder(BorderColor[1]);
	//-------------------------------------
	//重绘文本
	m_TextSingleHitOSD.Draw();

	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColor1", L"255", sFontColor1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColor2", L"65535", sFontColor2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColorBorder", L"0", sFontColorBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitTextGradientMode", L"1", sTextGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColorHighLine1", L"16746496", sFontColorHighLine1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitFontColorHighLine2", L"16776960", sFontColorHighLine2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitontColorHighLineBorder", L"0", sFontColorHighLineBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"DbhitHighlightGradientMode", L"1", sHighlightGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);

	crText1[0] = _wtoi64(sFontColor1);
	crText1[1] = _wtoi64(sFontColorHighLine1);
	crText2[0] = _wtoi64(sFontColor2);
	crText2[1] = _wtoi64(sFontColorHighLine2);
	crBorder[0] = _wtoi64(sFontColorBorder);
	crBorder[1] = _wtoi64(sFontColorHighLineBorder);
	TextColor1[0].SetFromCOLORREF(crText1[0]);
	TextColor2[0].SetFromCOLORREF(crText2[0]);
	BorderColor[0].SetFromCOLORREF(crBorder[0]);
	TextColor1[1].SetFromCOLORREF(crText1[1]);
	TextColor2[1].SetFromCOLORREF(crText2[1]);
	BorderColor[1].SetFromCOLORREF(crBorder[1]);
	if (_wtoi(sTextGradientMode) == 0)
	{
		TextGradientMode[0] = GradientMode_None;
	}
	if (_wtoi(sTextGradientMode) == 1)
	{
		TextGradientMode[0] = GradientMode_Two;
	}
	if (_wtoi(sTextGradientMode) == 2)
	{
		TextGradientMode[0] = GradientMode_Three;
	}
	if (_wtoi(sHighlightGradientMode) == 0)
	{
		TextGradientMode[1] = GradientMode_None;
	}
	if (_wtoi(sHighlightGradientMode) == 1)
	{
		TextGradientMode[1] = GradientMode_Two;
	}
	if (_wtoi(sHighlightGradientMode) == 2)
	{
		TextGradientMode[1] = GradientMode_Three;
	}

	//-----------------
	//设置文本字体
	m_TextFbDbHitOSD.SetTextStrFont((LPTSTR)(LPCTSTR)L"黑体", 15, FontStyleBold);
	//设置文本颜色
	m_TextFbDbHitOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
	//设置文本边框
	m_TextFbDbHitOSD.SetTextStrBorder(BorderColor[0]);
	//-------------------------------------
	//设置高亮文本颜色
	m_TextFbDbHitOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
	//设置高亮文本边框
	m_TextFbDbHitOSD.SetHighlightBorder(BorderColor[1]);
	//-------------------------------------
	//重绘文本
	m_TextFbDbHitOSD.Draw();

	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColor1", L"255", sFontColor1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColor2", L"65535", sFontColor2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColorBorder", L"0", sFontColorBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"PraTextGradientMode", L"1", sTextGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColorHighLine1", L"16746496", sFontColorHighLine1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"PraFontColorHighLine2", L"16776960", sFontColorHighLine2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"PraSgitontColorHighLineBorder", L"0", sFontColorHighLineBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"PraSgleHighlightGradientMode", L"1", sHighlightGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);

	crText1[0] = _wtoi64(sFontColor1);
	crText1[1] = _wtoi64(sFontColorHighLine1);
	crText2[0] = _wtoi64(sFontColor2);
	crText2[1] = _wtoi64(sFontColorHighLine2);
	crBorder[0] = _wtoi64(sFontColorBorder);
	crBorder[1] = _wtoi64(sFontColorHighLineBorder);
	TextColor1[0].SetFromCOLORREF(crText1[0]);
	TextColor2[0].SetFromCOLORREF(crText2[0]);
	BorderColor[0].SetFromCOLORREF(crBorder[0]);
	TextColor1[1].SetFromCOLORREF(crText1[1]);
	TextColor2[1].SetFromCOLORREF(crText2[1]);
	BorderColor[1].SetFromCOLORREF(crBorder[1]);
	if (_wtoi(sTextGradientMode) == 0)
	{
		TextGradientMode[0] = GradientMode_None;
	}
	if (_wtoi(sTextGradientMode) == 1)
	{
		TextGradientMode[0] = GradientMode_Two;
	}
	if (_wtoi(sTextGradientMode) == 2)
	{
		TextGradientMode[0] = GradientMode_Three;
	}
	if (_wtoi(sHighlightGradientMode) == 0)
	{
		TextGradientMode[1] = GradientMode_None;
	}
	if (_wtoi(sHighlightGradientMode) == 1)
	{
		TextGradientMode[1] = GradientMode_Two;
	}
	if (_wtoi(sHighlightGradientMode) == 2)
	{
		TextGradientMode[1] = GradientMode_Three;
	}

	//-----------------
	//设置文本字体
	m_TextPraiseOSD.SetTextStrFont((LPTSTR)(LPCTSTR)L"黑体", 15, FontStyleBold);
	//设置文本颜色
	m_TextPraiseOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
	//设置文本边框
	m_TextPraiseOSD.SetTextStrBorder(BorderColor[0]);
	//-------------------------------------
	//设置高亮文本颜色
	m_TextPraiseOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
	//设置高亮文本边框
	m_TextPraiseOSD.SetHighlightBorder(BorderColor[1]);
	//-------------------------------------
	//重绘文本
	m_TextPraiseOSD.Draw();

	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColor1", L"255", sFontColor1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColor2", L"65535", sFontColor2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColorBorder", L"0", sFontColorBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SogdTextGradientMode", L"1", sTextGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColorHighLine1", L"16746496", sFontColorHighLine1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SogdFontColorHighLine2", L"16776960", sFontColorHighLine2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SogditontColorHighLineBorder", L"0", sFontColorHighLineBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"SogdleHighlightGradientMode", L"1", sHighlightGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);

	crText1[0] = _wtoi64(sFontColor1);
	crText1[1] = _wtoi64(sFontColorHighLine1);
	crText2[0] = _wtoi64(sFontColor2);
	crText2[1] = _wtoi64(sFontColorHighLine2);
	crBorder[0] = _wtoi64(sFontColorBorder);
	crBorder[1] = _wtoi64(sFontColorHighLineBorder);
	TextColor1[0].SetFromCOLORREF(crText1[0]);
	TextColor2[0].SetFromCOLORREF(crText2[0]);
	BorderColor[0].SetFromCOLORREF(crBorder[0]);
	TextColor1[1].SetFromCOLORREF(crText1[1]);
	TextColor2[1].SetFromCOLORREF(crText2[1]);
	BorderColor[1].SetFromCOLORREF(crBorder[1]);
	if (_wtoi(sTextGradientMode) == 0)
	{
		TextGradientMode[0] = GradientMode_None;
	}
	if (_wtoi(sTextGradientMode) == 1)
	{
		TextGradientMode[0] = GradientMode_Two;
	}
	if (_wtoi(sTextGradientMode) == 2)
	{
		TextGradientMode[0] = GradientMode_Three;
	}
	if (_wtoi(sHighlightGradientMode) == 0)
	{
		TextGradientMode[1] = GradientMode_None;
	}
	if (_wtoi(sHighlightGradientMode) == 1)
	{
		TextGradientMode[1] = GradientMode_Two;
	}
	if (_wtoi(sHighlightGradientMode) == 2)
	{
		TextGradientMode[1] = GradientMode_Three;
	}

	//-----------------
	//设置文本字体
	m_TextSoGoodOSD.SetTextStrFont((LPTSTR)(LPCTSTR)L"黑体", 15, FontStyleBold);
	//设置文本颜色
	m_TextSoGoodOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
	//设置文本边框
	m_TextSoGoodOSD.SetTextStrBorder(BorderColor[0]);
	//-------------------------------------
	//设置高亮文本颜色
	m_TextSoGoodOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
	//设置高亮文本边框
	m_TextSoGoodOSD.SetHighlightBorder(BorderColor[1]);
	//-------------------------------------
	//重绘文本
	m_TextSoGoodOSD.Draw();

	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColor1", L"255", sFontColor1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColor2", L"65535", sFontColor2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColorBorder", L"0", sFontColorBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HeartTextGradientMode", L"1", sTextGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColorHighLine1", L"16746496", sFontColorHighLine1.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColorHighLine2", L"16776960", sFontColorHighLine2.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HeartFontColorHighLineBorder", L"0", sFontColorHighLineBorder.GetBuffer(255), 255, g_BarrageCfgPath);
	GetPrivateProfileStringW(L"BARRAGESSISTANT", L"HeartHighlightGradientMode", L"1", sHighlightGradientMode.GetBuffer(255), 255, g_BarrageCfgPath);

	crText1[0] = _wtoi64(sFontColor1);
	crText1[1] = _wtoi64(sFontColorHighLine1);
	crText2[0] = _wtoi64(sFontColor2);
	crText2[1] = _wtoi64(sFontColorHighLine2);
	crBorder[0] = _wtoi64(sFontColorBorder);
	crBorder[1] = _wtoi64(sFontColorHighLineBorder);
	TextColor1[0].SetFromCOLORREF(crText1[0]);
	TextColor2[0].SetFromCOLORREF(crText2[0]);
	BorderColor[0].SetFromCOLORREF(crBorder[0]);
	TextColor1[1].SetFromCOLORREF(crText1[1]);
	TextColor2[1].SetFromCOLORREF(crText2[1]);
	BorderColor[1].SetFromCOLORREF(crBorder[1]);
	if (_wtoi(sTextGradientMode) == 0)
	{
		TextGradientMode[0] = GradientMode_None;
	}
	if (_wtoi(sTextGradientMode) == 1)
	{
		TextGradientMode[0] = GradientMode_Two;
	}
	if (_wtoi(sTextGradientMode) == 2)
	{
		TextGradientMode[0] = GradientMode_Three;
	}
	if (_wtoi(sHighlightGradientMode) == 0)
	{
		TextGradientMode[1] = GradientMode_None;
	}
	if (_wtoi(sHighlightGradientMode) == 1)
	{
		TextGradientMode[1] = GradientMode_Two;
	}
	if (_wtoi(sHighlightGradientMode) == 2)
	{
		TextGradientMode[1] = GradientMode_Three;
	}

	//-----------------
	//设置文本字体
	m_TextHeartOSD.SetTextStrFont((LPTSTR)(LPCTSTR)L"黑体", 15, FontStyleBold);
	//设置文本颜色
	m_TextHeartOSD.SetTextStrColor(TextColor1[0],TextColor2[0],TextGradientMode[0]);
	//设置文本边框
	m_TextHeartOSD.SetTextStrBorder(BorderColor[0]);
	//-------------------------------------
	//设置高亮文本颜色
	m_TextHeartOSD.SetHighlightColor(TextColor1[1],TextColor2[1],TextGradientMode[1]);
	//设置高亮文本边框
	m_TextHeartOSD.SetHighlightBorder(BorderColor[1]);
	//-------------------------------------
	//重绘文本
	m_TextHeartOSD.Draw();
	//////////////////////////////////////////////////////////////////////////
}

static size_t DownloadCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)  
{  
	FILE* fp = NULL;  
	fopen_s(&fp, ((CBarrageAssistantDlg*)g_MainFrm)->CacheFile, "wb+");  
	size_t nWrite = fwrite(pBuffer, nSize, nMemByte, fp);  
	fclose(fp);  
	return nWrite;  
}  
static int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)  
{  
	if ( dltotal > -0.1 && dltotal < 0.1 )  
		return 0;  
	int nPos = (int) ( (dlnow/dltotal)*100 );
	g_nDoneFlag = nPos;
	 //::PostMessage(((CBarrageAssistantDlg*)g_MainFrm)->GetSafeHwnd(), WM_USER + 110, nPos, 0);  

	return 0;  
}


void CBarrageAssistantDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu; // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue; // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE); // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CBarrageAssistantDlg::MoveOsd()
{
	CRect MainUiRect;
	CRect FishBallRect;
	CRect HeartRect;
	CRect PraiseRect;
	CRect SoGoodRect;
	CRect SingleFbRect;
	CRect RoomInfoRect;
	CRect GuestRect;
	CRect TextPanelRect[30];
	m_TextOSD.GetWindowRect(&MainUiRect);
	m_TextFbDbHitOSD.GetWindowRect(&FishBallRect);
	m_TextHeartOSD.GetWindowRect(&HeartRect);
	m_TextPraiseOSD.GetWindowRect(&PraiseRect);
	m_TextSoGoodOSD.GetWindowRect(&SoGoodRect);
	m_TextSingleHitOSD.GetWindowRect(&SingleFbRect);
	m_TextRoomInfoOSD.GetWindowRect(&RoomInfoRect);
	m_TextGuestOSD.GetWindowRect(&GuestRect);

	for (int i = 0; i < 30; i++)
	{
		m_TextPanelOSD[i].GetWindowRect(&TextPanelRect[i]);
	}

	//房间信息
	RoomInfoRect.left = MainUiRect.left - RoomInfoRect.Width() + 70;
	RoomInfoRect.top = MainUiRect.top;
	m_TextRoomInfoOSD.SetWindowPos(NULL, RoomInfoRect.left, RoomInfoRect.top, 0, 0, SWP_NOSIZE);


	//鱼丸连击
	FishBallRect.left = MainUiRect.left;
	FishBallRect.top = MainUiRect.top;
	m_TextFbDbHitOSD.SetWindowPos(NULL, FishBallRect.left, FishBallRect.top, 0, 0, SWP_NOSIZE);
	m_TextFbDbHitOSD.GetWindowRect(&FishBallRect);

	//爱心连击
	HeartRect.left = MainUiRect.left;
	HeartRect.top = FishBallRect.bottom;
	m_TextHeartOSD.SetWindowPos(NULL, HeartRect.left, HeartRect.top, 0, 0, SWP_NOSIZE);
	m_TextHeartOSD.GetWindowRect(&HeartRect);

	//点赞连击
	PraiseRect.left = MainUiRect.left;
	PraiseRect.top = HeartRect.bottom;
	m_TextPraiseOSD.SetWindowPos(NULL, PraiseRect.left, PraiseRect.top, 0, 0, SWP_NOSIZE);
	m_TextPraiseOSD.GetWindowRect(&PraiseRect);

	//666连击
	SoGoodRect.left = MainUiRect.left;
	SoGoodRect.top = PraiseRect.bottom;
	m_TextSoGoodOSD.SetWindowPos(NULL, SoGoodRect.left, SoGoodRect.top, 0, 0, SWP_NOSIZE);
	m_TextSoGoodOSD.GetWindowRect(&SoGoodRect);

	//单次100鱼丸
	SingleFbRect.left = MainUiRect.left;
	SingleFbRect.top = SoGoodRect.bottom;
	m_TextSingleHitOSD.SetWindowPos(NULL, SingleFbRect.left, SingleFbRect.top, 0, 0, SWP_NOSIZE);
	m_TextSingleHitOSD.GetWindowRect(&SingleFbRect);

	//来宾与禁言
	GuestRect.left = MainUiRect.left;
	GuestRect.top = SingleFbRect.bottom;
	m_TextGuestOSD.SetWindowPos(NULL, GuestRect.left, GuestRect.top, 0, 0, SWP_NOSIZE);
	m_TextGuestOSD.GetWindowRect(&GuestRect);

	TextPanelRect[0].left = MainUiRect.left;
	TextPanelRect[0].top = GuestRect.bottom;
	m_TextPanelOSD[0].SetWindowPos(NULL, TextPanelRect[0].left, TextPanelRect[0].top, 0, 0, SWP_NOSIZE);
	m_TextPanelOSD[0].GetWindowRect(&TextPanelRect[0]);

	TextPanelRect[15].left = MainUiRect.left + 160;
	TextPanelRect[15].top = GuestRect.bottom;
	m_TextPanelOSD[15].SetWindowPos(NULL, TextPanelRect[15].left, TextPanelRect[15].top, 0, 0, SWP_NOSIZE);
	m_TextPanelOSD[15].GetWindowRect(&TextPanelRect[15]);
	
	//////////////////////////////////////////////////////////////////////////
	for (int i = 1; i < 15; i++)
	{
		TextPanelRect[i].left = MainUiRect.left;
		TextPanelRect[i].top = TextPanelRect[i - 1].bottom;
		m_TextPanelOSD[i].SetWindowPos(NULL, TextPanelRect[i].left, TextPanelRect[i].top, 0, 0, SWP_NOSIZE);
		m_TextPanelOSD[i].GetWindowRect(&TextPanelRect[i]);

	}
	for (int i = 16; i < 30; i++)
	{
		TextPanelRect[i].left = MainUiRect.left + 160;
		TextPanelRect[i].top = TextPanelRect[i - 1].bottom;
		m_TextPanelOSD[i].SetWindowPos(NULL, TextPanelRect[i].left, TextPanelRect[i].top, 0, 0, SWP_NOSIZE);
		m_TextPanelOSD[i].GetWindowRect(&TextPanelRect[i]);

	}
	//////////////////////////////////////////////////////////////////////////


}

LRESULT CBarrageAssistantDlg::MsgAutoMoveLocalRect(WPARAM wParam, LPARAM lParam)
{
	if (wParam == MSG_AUTOMOVE)
	{
		MoveOsd();
	}
	if (wParam == MSG_LOCKINFO)
	{
		if (((CButton*)GetDlgItem(IDC_CHECK_OSDLOCK))->GetCheck())
		{
			SetTimer(0, 5000, NULL);
			if (m_bExit == false)
			{

				CString sTemp;
				sTemp.Format(L"%s%s", g_sInit, L"(房间已连接)鼠标放在当前,可拖动OSD，返回软件界可设(锁定/解锁)\r\n[当前已锁定]");
				m_TextOSD.UpdateTextStr(sTemp, 0);
			}
			else
			{
				CString sTemp;
				sTemp.Format(L"%s%s", g_sInit, L"(房间未连接)鼠标放在当前,可拖动OSD，返回软件界可设(锁定/解锁)\r\n[当前已锁定]");
				m_TextOSD.UpdateTextStr(sTemp, 0);
			}
		}
		else
		{
			SetTimer(0, 5000, NULL);
			if (m_bExit == false)
			{

				CString sTemp;
				sTemp.Format(L"%s%s", g_sInit, L"(房间已连接)鼠标放在当前,可拖动OSD，返回软件界可设(锁定/解锁)\r\n[当前已解锁]");
				m_TextOSD.UpdateTextStr(sTemp, 0);
			}
			else
			{
				CString sTemp;
				sTemp.Format(L"%s%s", g_sInit, L"(房间未连接)鼠标放在当前,可拖动OSD，返回软件界可设(锁定/解锁)\r\n[当前已解锁]");
				m_TextOSD.UpdateTextStr(sTemp, 0);
			}
		}



	}

	return 0;
}

void CBarrageAssistantDlg::SetWindOnTop()
{
	CWnd *TmpWnd;

	TmpWnd = FromHandle(m_TextOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	TmpWnd = FromHandle(m_TextFbDbHitOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	TmpWnd = FromHandle(m_TextHeartOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	TmpWnd = FromHandle(m_TextPraiseOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	TmpWnd = FromHandle(m_TextSoGoodOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	TmpWnd = FromHandle(m_TextSingleHitOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	TmpWnd = FromHandle(m_TextRoomInfoOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	TmpWnd = FromHandle(m_TextGuestOSD.GetSafeHwnd());
	TmpWnd->ShowWindow(SW_SHOW);
	TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	for (int i = 0; i < 30; i++)
	{
		TmpWnd = FromHandle(m_TextPanelOSD[i].GetSafeHwnd());
		TmpWnd->ShowWindow(SW_SHOW);
		TmpWnd->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}


void CBarrageAssistantDlg::EnableDebugPriv()
{
	HANDLE hToken;  
	LUID sedebugnameValue;  
	TOKEN_PRIVILEGES tkp;  

	if (!OpenProcessToken(GetCurrentProcess(),  
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {  
			return;  
	}  
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) {  
		CloseHandle(hToken);  
		return;  
	}  
	tkp.PrivilegeCount = 1;  
	tkp.Privileges[0].Luid = sedebugnameValue;  
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {  
		CloseHandle(hToken);  
		return;  
	}
}


CString CBarrageAssistantDlg::UrlEncode(CString strUnicode)
{
	LPCWSTR unicode = T2CW(strUnicode);
	int len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, 0, 0, 0, 0);
	if (!len)
		return strUnicode;
	char *utf8 = new char[len + 1];
	char *utf8temp = utf8;
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, len + 1, 0, 0);
	utf8[len] = NULL;   
	CString strTemp, strEncodeData; 
	while (*utf8 != '\0') 
	{ 
		strTemp.Format(_T("%%%2x"), (BYTE)*utf8); 
		strEncodeData += strTemp; 
		++utf8; 
	} 

	delete []utf8temp;

	return CString(strEncodeData);
}