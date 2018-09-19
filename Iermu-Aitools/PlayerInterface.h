#pragma once
extern "C" {
#include "ffplayer.h"
}

class CPlayerInterface
{
private:
	PLAYER_INIT_PARAMS	m_Params;   //���Ų���	

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
	void OnRecordVideo();			//¼��

	void *m_hplayer = NULL;			//������
	HWND m_hStatic = 0;
	HWND m_hDialog = 0;
	char m_url[MAX_PATH];			//���ŵ�ַ
	TCHAR m_strTxt[MAX_PATH];
	BOOL m_bIsRecording;			//�Ƿ�����¼��
};	

