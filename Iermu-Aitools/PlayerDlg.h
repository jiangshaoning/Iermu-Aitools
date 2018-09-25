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
	//���ܼ�������
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void SetPlayUrl(char *url, int voiceType);
protected:
	//soui��Ϣ
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_MAP_END()

		//HostWnd��ʵ������Ϣ����
		BEGIN_MSG_MAP_EX(PlayerDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_KEYDOWN(OnKeyDown)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
private:
	BOOL			m_bLayoutInited;
	void*			m_hplayer;
	char			m_playUrl[60];		//����·��
	int				m_voiceType;		//�Ƿ�������
	RECT			m_rtClient;
	BOOL			m_ctrl_down;
	BOOL			m_bIsRecording;
};