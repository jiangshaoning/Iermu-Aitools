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

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void SetPlayUrl(char *url, int voiceType);
	//LRESULT playVideo(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_MAP_END()

		//HostWnd真实窗口消息处理
		BEGIN_MSG_MAP_EX(PlayerDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		//MESSAGE_HANDLER_EX(MSG_FANPLAYER, playVideo)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
private:
	BOOL			m_bLayoutInited;
	void*			m_hplayer;
	char			m_playUrl[60];		//播放路径
	int				m_voiceType;		//是否有声音
	RECT			m_rtClient;
};
