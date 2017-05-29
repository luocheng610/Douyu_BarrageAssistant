
// BarrageAssistantDlg.h : 头文件
//

#pragma once
#include "TextAeroWindow.h"
#include "curl.h"
#include "easy.h"
#include <vector>
#include "afxwin.h"
#include "colorbutton.h"
#include "LinkStatic.h"
#include <sys/stat.h>
#include <atlstr.h>
#include <windows.h>
using namespace std;

enum _ThreadType
{
	Thread_Play = 0,
	Thread_paused = 1,
	Thread_Stop = 2
};

#define WM_NOTIFYICON   WM_USER + 9005
#define WM_HOOKDLL		WM_USER + 2013

// CBarrageAssistantDlg 对话框
class CBarrageAssistantDlg : public CBCGPDialog
{
// 构造
public:
	CBarrageAssistantDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CBarrageAssistantDlg();
// 对话框数据
	enum { IDD = IDD_BARRAGEASSISTANT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//线程与工作
	static DWORD WINAPI MainServerThread(LPVOID lpParam);
	HANDLE m_MainServerThreadHandle;
	int m_MainServerThreadStatus;

	static DWORD WINAPI BarrageServerThread(LPVOID lpParam);
	HANDLE m_BarrageServerThreadHandle;
	int  m_BarrageServerThreadStatus;

	static DWORD WINAPI UpdateRoomInfoThread(LPVOID lpParam);
	HANDLE m_UpdateRoomInfoThreadHandle;
	int  m_UpdateRoomInfoThreadStatus;

	void MainServerStart();
	void BarrageServerStart();
	int  MainServerStop();
	void MainServerWorkProc();
	void BarrageServerWorkProc();

	void UpdateRoomInfoStart();
	void UpdateRoomInfoStop();
	void UpdateRoomInfoWorkProc();

	void EnableDebugPriv();
	void MoveOsd();
	void SetWindOnTop();
	void UpdateFontSet();

	bool GetRoomIdAndName(const std::wstring roomAlias, unsigned int &roomId, std::wstring &roomName);
	bool GetMainServerInfoList(std::vector<std::string> &ips, std::vector<unsigned int> &ports);
	bool ConnectToMainServer(const std::string &ip, unsigned int port);
	bool LoginToMainServer();
	bool GetBarrageUserName(std::string &userName);
	bool GetBarrageGid(unsigned int &gid);
	bool ConnectToBarrageServer();
	bool LoginToBarrageServer();
	bool SendKeepLiveToBarrageServer();
	void SendKeepLiveToMainServer();
	bool CheckForBarrageMessage();
	bool SendChatMsg();
	
	//douyu
	CURL *m_MainCurl;
	CURL *m_BarrageCurl;
	bool m_bExit;
	HANDLE m_hExitEvent;
	std::wstring m_roomName;
	std::string m_userName;
	unsigned int m_gid;
	unsigned int m_roomId;

	//OSD
	CTextAeroWindow m_TextOSD;
	CTextAeroWindow m_TextFbDbHitOSD;	//鱼丸连击, 100x ？
	CTextAeroWindow m_TextHeartOSD;		//心连击，520x ？
	CTextAeroWindow m_TextPraiseOSD;	//点赞连击，x ？
	CTextAeroWindow m_TextSoGoodOSD;	//666连击，x ？
	CTextAeroWindow m_TextSingleHitOSD;	//单个鱼丸100，x ？
	CTextAeroWindow m_TextRoomInfoOSD;	//房间信息
	CTextAeroWindow m_TextGuestOSD;		//来宾与禁言
	CTextAeroWindow m_TextPanelOSD[30];

	CBCGPComboBox m_cb_FontFace;
	CBCGPComboBox m_cb_FontSize;
	CBCGPComboBox m_cb_TextGradientMode;
	CBCGPComboBox m_cb_HighlightGradientMode;
	CBCGPComboBox m_cb_BaType;
	CBCGPComboBox m_cb_AlignmentMethod;
	CBCGPComboBox m_cb_LoginType;

	CColorButton m_ColorBtn1;
	CColorButton m_ColorBtn2;
	CColorButton m_ColorBtBorder;
	CColorButton m_ColorBtnHighLine1;
	CColorButton m_ColorBtnHighLine2;
	CColorButton m_ColorBtnHighLineBorder;

	BOOL m_TimeFlag;
	BOOL m_bFishBall;
	BOOL m_bBannedToPost;
	BOOL m_bGift;
	BOOL m_bGuest;
	BOOL m_bPraise;
	BOOL m_bBarrageAssistant;
	BOOL m_bHeart;
	BOOL m_b666;
	BOOL m_bFbDbHit;
	BOOL isNotify;
	std::vector<CString> m_sBarrage;
	
	CBCGPEdit m_EditID;

	const char *CacheFile;
	char* CString2Char(CString szItem, char* pOutChars);
	std::string wstring2string(const std::wstring wstr);
	const std::wstring string2wstring(std::string str);  
	CString GetTimeStr();
	CString CharToCString(const char *str);
	CString UrlEncode(CString strUnicode);
	NOTIFYICONDATA	m_Notify;
	
	LRESULT MsgAutoMoveLocalRect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowTrayTipMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnMenuRestore();
	afx_msg void OnMenuExit();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnAboutbarrage();
	afx_msg void OnBnClickedBtConnectDouyu();
	afx_msg void OnBnClickedBtDisconnectDouyu();
	afx_msg void OnCbnSelchangeComboLogin();
	afx_msg void OnBnClickedBtAlignmentset();
	afx_msg void OnBnClickedCheckTimeflag();
	afx_msg void OnBnClickedCheckOsdlock();
	afx_msg void OnBnClickedCheckFishBall();
	afx_msg void OnBnClickedCheckBanned();
	afx_msg void OnBnClickedCheckGift();
	afx_msg void OnBnClickedCheckGuest();
	afx_msg void OnBnClickedCheckGdn();
	afx_msg void OnBnClickedCheckBa();
	afx_msg void OnBnClickedCheckHeart();
	afx_msg void OnBnClickedCheck666();
	afx_msg void OnBnClickedCheckFishDbhit();
	afx_msg void OnBnClickedBtFontset();
	afx_msg void OnBnClickedBtColorset();
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);  

	virtual BOOL PreTranslateMessage(MSG* pMsg);


};
