#include "stdafx.h"
#include "Resource.h"
#include "PlayerInterface.h"
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
	m_Params.vdev_render_type = VDEV_RENDER_TYPE_D3D;
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

// “关于”框的消息处理程序。
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	int param = 0;
	CPlayerInterface *pThisInstance = (CPlayerInterface*)::GetWindowLong(hDlg, GWL_USERDATA);
	switch (message)
	{
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

		case MSG_FFPLAYER:
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
						param = -0;  player_setparam(pThisInstance->m_hplayer, PARAM_AUDIO_VOLUME, &param);
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
		}
	
		return (INT_PTR)FALSE;
}
