#include "stdafx.h"
#include "Resource.h"
#include "PlayerInterface.h"

#define TIMER_ID_HIDE_TEXT	3

#define RELEASEPLAYER(player) \
if (player) \
{ \
	player_close(player); \
	player = NULL; \
}


LRESULT CALLBACK WinMainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM LParam);
INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

CPlayerInterface::CPlayerInterface(): m_hplayer(NULL)
{

}


CPlayerInterface::~CPlayerInterface()
{
	RELEASEPLAYER(m_hplayer);
}

void CPlayerInterface::init(HWND parent)
{
	RELEASEPLAYER(m_hplayer);
	if (m_hDialog)
	{
		DestroyWindow(m_hDialog);
		m_hDialog = 0;
	}

	m_hDialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PLAYER_DIALOG), parent, (DLGPROC)DlgProc);
	if (m_hDialog)
	{
		::SetWindowLong(m_hDialog, GWL_USERDATA, (LONG)this);
		//显示对话框  
		ShowWindow(m_hDialog, SW_NORMAL);
	}

	memset(&m_Params, 0, sizeof(m_Params));
	m_Params.adev_render_type = ADEV_RENDER_TYPE_WAVEOUT;
	m_Params.vdev_render_type = VDEV_RENDER_TYPE_GDI;
	m_Params.audio_stream_cur = -1;
	m_Params.low_delay = 1;
	m_Params.init_timeout = 10000;

}

void CPlayerInterface::startPlay(const char* playurl)
{
	memset(m_url, 0, MAX_PATH);
	strcpy(m_url, playurl);
	m_hplayer = player_open(m_url, m_hDialog, &m_Params);
}

int CPlayerInterface::ffplayStop()
{
	DestroyWindow(m_hDialog);

	RELEASEPLAYER(m_hplayer);

	m_hDialog = 0;
	return 0;
}

int CPlayerInterface::ffplayWithUrl(const char* playurl)
{
	ffplayStop();
	init(NULL);
	startPlay(playurl);
	return 0;
}

void CPlayerInterface::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_ID_HIDE_TEXT:
		KillTimer(m_hDialog, TIMER_ID_HIDE_TEXT);
		player_textout(m_hplayer, 0, 0, 0, NULL);
		m_strTxt[0] = '\0';
		break;

	default:
		CPlayerInterface::OnTimer(nIDEvent);
		break;
	}
}

void CPlayerInterface::PlayerShowText(int time)
{
	player_textout(m_hplayer, 20, 20, RGB(0, 255, 0), m_strTxt);
	SetTimer(m_hDialog, TIMER_ID_HIDE_TEXT, time, NULL);
}

void CPlayerInterface::OnRecordVideo()
{
	player_record(m_hplayer, m_bIsRecording ? NULL : "record.mp4");
	m_bIsRecording = !m_bIsRecording;
	_stprintf(m_strTxt, TEXT("recording %s"), m_bIsRecording ? TEXT("started") : TEXT("stoped"));
	PlayerShowText(2000);
}


// “关于”框的消息处理程序。
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	PLAYER_INIT_PARAMS params = {0};
	CPlayerInterface *pThisInstance = (CPlayerInterface*)::GetWindowLong(hDlg, GWL_USERDATA);
	SLOGFMTE("DlgProc 消息：%d   %d    %d\n", message, wParam, lParam);
	switch (message) {
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
		{
			// 如果执行了关闭  
			// 销毁对话框，将收到WM_DESTROY消息  
			if (pThisInstance)
			{
				pThisInstance->ffplayStop();
			}
			else
			{
				DestroyWindow(hDlg);
			};
			//PostQuitMessage(0);
		}
		if (wParam == ID_TAKE_SNAPSHOT)
		{
			pThisInstance->OnRecordVideo();
		}
		break;
	case WM_SIZE:
	{
		LONG width = LOWORD(lParam); // width of client area
		LONG height = HIWORD(lParam); // height of client area

		if (pThisInstance)
		{
			player_setrect(pThisInstance->m_hplayer, 0, 0, 0, width, height);
		}
	}
		break;

	case MSG_FANPLAYER:
		if (pThisInstance)
		{
			switch (wParam)
			{
				case MSG_PLAY_COMPLETED:
					//SetEvent(pThisInstance->m_hEvent);
					//pThisInstance->ffplaySingleStop();
					//pThisInstance->ffplaySingleWithUrl(pThisInstance->m_url);

					//RELEASEPLAYER(pThisInstance->m_hplayer);
					//SetEvent(pThisInstance->m_hEvent);					
					break;
				case MSG_OPEN_DONE:
					player_play(pThisInstance->m_hplayer);
					break;
				case MSG_OPEN_FAILED:
					//RELEASEPLAYER(pThisInstance->m_hplayer);
					//SetEvent(pThisInstance->m_hEvent);
					char url[MAX_PATH] = { 0 };
					strcpy(url, pThisInstance->m_url);
					pThisInstance->ffplayStop();
					pThisInstance->ffplayWithUrl(url);
					break;
			}
		}
		break;
	case WM_KEYDOWN:
	case WM_CHAR:
		switch (wParam){
		case VK_UP:
			if (pThisInstance)
			{
				pThisInstance->OnRecordVideo();
			}
			break;
		}
		break;

	}
	
	return (INT_PTR)FALSE;
}
