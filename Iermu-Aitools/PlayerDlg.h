// MainDlg.h : interface of the PlayerDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ffplayer.h"

class PlayerDlg : public SHostWnd
{
public:
	PlayerDlg();
	~PlayerDlg();

	void*			m_hplayer;
	BOOL            m_bFullScreenMode;  // 是否在全屏模式

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//接受键盘输入
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnTimer(UINT_PTR nIDEvent);
	void PlayerShowText(int time);
	void SetPlayUrl(char *url, int voiceType);
	void GetCurTimeName(char* Ctime, wchar_t* Wtime, char* name, char* postfix);
	void OnRecord();
	void OnSnapshot();
	void OnVolume();
	void OnVolumeZero();
	void FullScreen(BOOL bFull);
	void OnScreenFull();
	void OnLButtonUp(UINT nFlags, CPoint pt);
	void OnLButtonDown(UINT nFlags, CPoint pt);
	void OnMouseMove(UINT nFlags, CPoint pt);
;protected:
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"btn_record", OnRecord)
		EVENT_NAME_COMMAND(L"btn_record_stop", OnRecord)
		EVENT_NAME_COMMAND(L"btn_snapshot", OnSnapshot)
		EVENT_NAME_COMMAND(L"btn_volume", OnVolume)
		EVENT_NAME_COMMAND(L"btn_volume_zero", OnVolumeZero)
		EVENT_NAME_COMMAND(L"btn_screen_full", OnScreenFull)
		EVENT_MAP_END()

		//HostWnd真实窗口消息处理
		BEGIN_MSG_MAP_EX(PlayerDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_KEYDOWN(OnKeyDown)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
private:
	BOOL			m_bLayoutInited;
	char			m_playUrl[60];		//播放路径
	int				m_voiceType;		//是否有声音
	RECT			m_rtClient;
	BOOL			m_ctrl_down;
	BOOL			m_bIsRecording;
	SSliderBar*		m_VolumeSlider;
	int				m_LButtonDown;
	WINDOWPLACEMENT m_OldWndPlacement;  // 保存窗口原来的位置
	int				m_captionHeight;	// 上面title高度
	int				m_toolsHeight;		// 下面tools高度
	TCHAR			m_strTxt[MAX_PATH];
};
