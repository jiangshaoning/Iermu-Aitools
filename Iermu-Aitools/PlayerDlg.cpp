// MainDlg.cpp : implementation of the PlayerDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlayerDlg.h"	

PlayerDlg::PlayerDlg() : SHostWnd(_T("LAYOUT:XML_PLAYERWND"))
{
	m_bLayoutInited = FALSE;
	m_hplayer = NULL;
	m_voiceType = 0;
	memset(m_playUrl, 0, sizeof(m_playUrl));
}

PlayerDlg::~PlayerDlg()
{
}

int PlayerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);
	return 0;
}

BOOL PlayerDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	HWND PlayHwnd = NULL;
	RELEASEPLAYER(m_hplayer);
	SRealWnd  *p_RealWnd = FindChildByName2<SRealWnd>(L"ffplaywnd");
	if (p_RealWnd)
	{
		PLAYER_INIT_PARAMS	Params;
		PlayHwnd = p_RealWnd->GetRealHwnd();
		memset(&Params, 0, sizeof(Params));
		Params.adev_render_type = ADEV_RENDER_TYPE_WAVEOUT;
		Params.vdev_render_type = VDEV_RENDER_TYPE_GDI;
		Params.init_timeout = 10000;
		if (m_voiceType)
		{
			Params.audio_stream_cur = -1;
			Params.low_delay = 1;
		}
		m_hplayer = player_open((char *)m_playUrl, PlayHwnd, &Params);

		::SendMessageW(PlayHwnd, MS_OPENVIDEO_REALWND, 0, (LPARAM)(void *)m_hplayer);
	}
	return 0;
}
//TODO:ÏûÏ¢Ó³Éä
void PlayerDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}

void PlayerDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void PlayerDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void PlayerDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void PlayerDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;

	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if (!pBtnMax || !pBtnRestore) return;

	if (nType != SIZE_MINIMIZED) {
		m_rtClient = GetClientRect();
		player_setrect(m_hplayer, 0, 0, 0, size.cx, size.cy - 2);
		player_setrect(m_hplayer, 1, 0, 0, size.cx, size.cy - 2);
	}
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}

}

void PlayerDlg::SetPlayUrl(char *url, int voiceType)
{
	m_voiceType = voiceType;
	strcpy(m_playUrl, url);
}

//LRESULT PlayerDlg::playVideo(UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	player_play(m_hplayer);
//	return 0;
//}
