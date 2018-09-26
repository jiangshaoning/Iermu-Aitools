// MainDlg.cpp : implementation of the PlayerDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlayerDlg.h"	


#define TIMER_ID_HIDE_TEXT     3

PlayerDlg::PlayerDlg() : SHostWnd(_T("LAYOUT:XML_PLAYERWND"))
{
	m_bLayoutInited = FALSE;
	m_hplayer = NULL;
	m_voiceType = 0;
	m_bIsRecording = FALSE;
	m_ctrl_down = FALSE;
	m_LButtonDown = 0;
	m_bFullScreenMode = FALSE;
	m_captionHeight = 40;
	m_toolsHeight = 50;
	memset(m_playUrl, 0, sizeof(m_playUrl));
}

PlayerDlg::~PlayerDlg()
{
	RELEASEPLAYER(m_hplayer);
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

	m_VolumeSlider = FindChildByName2<SSliderBar>(L"volumeSlider");
	if (m_VolumeSlider)
	{
		int volume = m_VolumeSlider->GetValue();
		//m_VolumeSlider->SetRange(-182, 73);
		player_setparam(m_hplayer, PARAM_AUDIO_VOLUME, (void*)&volume);
	}
	return 0;
}
//TODO:消息映射
void PlayerDlg::OnClose()
{
	RELEASEPLAYER(m_hplayer);
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
		player_setrect(m_hplayer, 0, 2, 0, size.cx - 4, size.cy - m_captionHeight - m_toolsHeight);
		player_setrect(m_hplayer, 1, 2, 0, size.cx - 4, size.cy - m_captionHeight - m_toolsHeight);
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

//接受键盘输入
void PlayerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_ESCAPE:
		FullScreen(FALSE);
		break;
	case VK_CONTROL:
		m_ctrl_down = TRUE;
		break;
	case 'R'://Ctrl + R
		if (m_ctrl_down)
		{
			player_record(m_hplayer, m_bIsRecording ? NULL : "record.mp4");
			m_bIsRecording = !m_bIsRecording;
		}
		break;
	case 'S'://Ctrl + S
		if (m_ctrl_down)
		{
			player_snapshot(m_hplayer, "snapshot.jpg", 0, 0, 1000);
		}
		break;
	}
}

void PlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_ID_HIDE_TEXT:
		KillTimer(TIMER_ID_HIDE_TEXT);
		player_textout(m_hplayer, 0, 0, 0, NULL);
		m_strTxt[0] = '\0';
		break;
	}
}

void PlayerDlg::PlayerShowText(int time)
{
	player_textout(m_hplayer, 20, 20, RGB(0, 255, 0), m_strTxt);
	SetTimer(TIMER_ID_HIDE_TEXT, time);
}

void PlayerDlg::SetPlayUrl(char *url, int voiceType)
{
	m_voiceType = voiceType;
	strcpy(m_playUrl, url);
}

void PlayerDlg::OnRecord()
{
	player_record(m_hplayer, m_bIsRecording ? NULL : "record.mp4");
	m_bIsRecording = !m_bIsRecording;
	_stprintf(m_strTxt, TEXT("%s: record.mp4"), m_bIsRecording ? TEXT("开始录屏") : TEXT("结束录屏 保存到当前路径"));
	PlayerShowText(2000);
}

void PlayerDlg::OnSnapshot()
{
	player_snapshot(m_hplayer, "snapshot.jpg", 0, 0, 1000);
	_tcscpy(m_strTxt, TEXT("抓拍到当前路径：snapshot.jpg"));
	PlayerShowText(2000);
}

void PlayerDlg::OnVolume()
{
	int volume = -182;
	FindChildByName2<SWindow>(L"btn_volume")->SetVisible(FALSE, TRUE);
	FindChildByName2<SWindow>(L"btn_volume_zero")->SetVisible(TRUE, TRUE);
	player_setparam(m_hplayer, PARAM_AUDIO_VOLUME, (void*)&volume);
}

void PlayerDlg::OnVolumeZero()
{
	int volume = m_VolumeSlider->GetValue();
	FindChildByName2<SWindow>(L"btn_volume")->SetVisible(TRUE, TRUE);
	FindChildByName2<SWindow>(L"btn_volume_zero")->SetVisible(FALSE, TRUE);
	player_setparam(m_hplayer, PARAM_AUDIO_VOLUME, (void*)&volume);
}


void PlayerDlg::FullScreen(BOOL bFull)
{
	int iBorderX = GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXBORDER);
	int iBorderY = GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYBORDER);
	m_bFullScreenMode = bFull;

	if (bFull)
	{

		::GetWindowPlacement(m_hWnd, &m_OldWndPlacement);

		if (::IsZoomed(m_hWnd))
		{
			::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
		}

		::SetWindowPos(m_hWnd, HWND_TOP, -iBorderX, -iBorderY, GetSystemMetrics(SM_CXSCREEN) + 2 * iBorderX, GetSystemMetrics(SM_CYSCREEN) + 2 * iBorderY + m_captionHeight + m_toolsHeight, 0);
		m_captionHeight = 0;
		m_toolsHeight = 0;
		SCaption  *cap = FindChildByID2<SCaption>(6000);
		cap->SetVisible(FALSE, TRUE);
		cap = FindChildByID2<SCaption>(7000);
		cap->SetVisible(FALSE, TRUE);

	}
	else
	{
		m_captionHeight = 40;
		m_toolsHeight = 50;
		::SetWindowPlacement(m_hWnd, &m_OldWndPlacement);
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, m_captionHeight + m_toolsHeight, SWP_NOSIZE | SWP_NOMOVE);

		SCaption  *cap = FindChildByID2<SCaption>(6000);
		cap->SetVisible(TRUE, TRUE);
		cap = FindChildByID2<SCaption>(7000);
		cap->SetVisible(TRUE, TRUE);
	}
}

void PlayerDlg::OnScreenFull()
{
	if (!m_bFullScreenMode)
		FullScreen(true);
}

void PlayerDlg::OnLButtonUp(UINT nFlags, CPoint pt)//处理进度条鼠标事件 播放指定位置 此处没有用EventSliderPos事件
{
	m_LButtonDown = 0;
	SetMsgHandled(false);
}

//-182, 73
void PlayerDlg::OnMouseMove(UINT nFlags, CPoint pt)
{
	if (m_VolumeSlider->GetWindowRect().PtInRect(pt) && m_LButtonDown == 1 && !m_voiceType)
	{
		int volume = m_VolumeSlider->GetValue();
		switch (volume)
		{
		case -182:
			OnVolume();
			break;
		default:
			OnVolumeZero();
			break;
		}
	}
	SetMsgHandled(false);
}

void PlayerDlg::OnLButtonDown(UINT nFlags, CPoint pt)//处理进度条鼠标事件
{
	if (m_VolumeSlider->GetWindowRect().PtInRect(pt) && !m_voiceType)//处理音量
		m_LButtonDown = 1;
	SetMsgHandled(false);

}