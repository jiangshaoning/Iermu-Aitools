#pragma once
extern "C" {
#include "ffplayer.h"
}

class CPlayerInterface
{
private:
	PLAYER_INIT_PARAMS	m_Params;   //播放参数	

	void init(HWND parent);
	void OnTimer(UINT_PTR nIDEvent);
public:
	CPlayerInterface();
	~CPlayerInterface();
	
	void startPlay(const char* playurl);
	int ffplayWithUrl(const char* playurl);
	int ffplayWithUrlNoVoice(const char* playurl);
	int ffplayStop();
	void PlayerShowText(int time);
	void OnRecordVideo();			//录像

	void *m_hplayer = NULL;			//播放器
	HWND m_hStatic = 0;
	HWND m_hDialog = 0;
	char m_url[MAX_PATH];			//播放地址
	TCHAR m_strTxt[MAX_PATH];
	BOOL m_bIsRecording;			//是否正在录像
};	

