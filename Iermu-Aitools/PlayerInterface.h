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

	void *m_hplayer = NULL; //������
	HWND m_hStatic = 0;
	HWND m_hDialog = 0;
	char m_url[MAX_PATH];  //���ŵ�ַ
	PLAYER_INIT_PARAMS	m_Params;  //���Ų���	
};

