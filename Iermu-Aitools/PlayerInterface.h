#pragma once
extern "C" {
#include "fanplayer.h"
}

class CPlayerInterface
{
public:
	CPlayerInterface();
	~CPlayerInterface();
	void init(HWND parent);
	void startPlay(const char* playurl);
	int ffplayWithUrl(const char* playurl);
	int ffplayStop();

	void *m_hplayer = NULL; //播放器
	HWND m_hStatic = 0;
	HWND m_hDialog = 0;
	char m_url[MAX_PATH];  //播放地址
	PLAYER_INIT_PARAMS	m_Params;  //播放参数	
};

