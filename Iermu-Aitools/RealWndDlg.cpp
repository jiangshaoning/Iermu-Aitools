#include "StdAfx.h"
#include "RealWndDlg.h"


CRealWndDlg::CRealWndDlg():SHostDialog(_T("LAYOUT:XML_REALWND"))								
{
	m_menu_open.LoadMenu(_T("LAYOUT:menu_open"));
	m_ctrl_down = FALSE;
}

CRealWndDlg::~CRealWndDlg(void)
{
}
void CRealWndDlg::On_Menu_Open()
{
	CRect rc_menu;
	SWindow * pBtn = FindChildByID(401);
	if (pBtn)
	{
		pBtn->GetClientRect(&rc_menu);
		ClientToScreen(&rc_menu);
		m_menu_open.TrackPopupMenu(0, rc_menu.left, rc_menu.bottom, m_hWnd);
	}
}
void CRealWndDlg::On_Open_Files()
{
	::SendMessageW(SApplication::getSingleton().GetMainWnd(), MS_REALWND, 0, (LPARAM)(int)1);
}
void CRealWndDlg::OnCommand(UINT uNotifyCode, int nID, HWND wndCtl)
{

	if (uNotifyCode == 0)
	{
		if (nID == 2000)
		{
			::SendMessageW(SApplication::getSingleton().GetMainWnd(), MS_REALWND, 0, (LPARAM)(int)2);
		}		
	}
}
// ¼üÅÌÏûÏ¢
void CRealWndDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_ESCAPE:
		m_dlgplayer->FullScreen(FALSE);
		break;
	case VK_CONTROL:
		m_ctrl_down = TRUE;
		break;
	case 'R'://Ctrl + R
		if (m_ctrl_down)
		{
			m_dlgplayer->OnRecord();
		}
		break;
	case 'S'://Ctrl + S
		if (m_ctrl_down)
		{
			m_dlgplayer->OnSnapshot();
		}
		break;
	}
}

void CRealWndDlg::OnLbuttonDBLCLK(UINT nFlags, CPoint pt)
{	
	//Ë«»÷È«ÆÁ/ÍË³ö
	m_dlgplayer->m_bFullScreenMode ? m_dlgplayer->FullScreen(FALSE) : m_dlgplayer->FullScreen(TRUE);
	
	SetMsgHandled(false);
}

void CRealWndDlg::OnLbuttonup(UINT nFlags, CPoint pt)
{

	//if (GetWindowRect().PtInRect(pt))//µ¥»÷²¥·Å/ÔİÍ£
	//	::SendMessageW(SApplication::getSingleton().GetMainWnd(), MS_REALWND, 0, (LPARAM)(int)14);
	SetMsgHandled(false);

}

LRESULT CRealWndDlg::openVideo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_dlgplayer = (PlayerDlg*)lParam;
	return 0;
}

LRESULT CRealWndDlg::closeVideo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RELEASEPLAYER(m_dlgplayer->m_hplayer);
	
	return 0;
}

LRESULT CRealWndDlg::playVideo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	player_play(m_dlgplayer->m_hplayer);
	return 0;
}

