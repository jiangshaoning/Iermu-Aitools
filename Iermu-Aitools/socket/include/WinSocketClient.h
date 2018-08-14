#pragma once
#include <iostream>
#include <ws2tcpip.h> 

#pragma comment(lib, "ws2_32.lib") 
using namespace std;


#define CAMERA_USERNAME					"88888888"
#define CAMERA_USERPWD					"88888888"
#define EVT_SOCKET_BEGIN				(EVT_EXTERNAL_BEGIN + 30000)

#define DOMAIN_LEN						(61)
#define CPDOMAIN_LEN					(64)
#define LIBNAME_LEN						(110)
enum
{
	SENDER_MAINDLG_ID = 30000,
};

typedef enum
{
	OPT_UPLOADIP,
	OPT_GETCAMERA_INFO,
	OPT_SETAIFUNC_FROMCAMERAIP,
	OPT_SETFACE_FROMCAMERAIP,
	OPT_SETTIME_FROMLOCALIP,
	OPT_PLAY_FROMLOCALIP,
	OPT_SETTIME_FROMCAMERAIP,
	OPT_PLAY_FROMCAMERAIP

}SOCKETOPTION;


typedef struct
{
	UINT8 cp_domain[CPDOMAIN_LEN];
	UINT8 domain[DOMAIN_LEN];
	UINT16 port;
	UINT16 cp_port;
	UINT8 sw_id[LIBNAME_LEN];
	UINT8 lib_type;
}IPPort;

typedef struct
{
	SOCKETOPTION opt;
	string url;
	IPPort ipp;
	string data;
}SocketData;

typedef struct
{
	int date;
	int time;
}DateTime;

typedef struct
{
	const char* errMsg;
}HttpErrorResponse;

typedef struct __tagCommand
{
	char main;
	char child;
	unsigned short len;
}COMMAND, *LPCOMMAND;


typedef struct
{
	UINT8 face_rec;
	UINT8 face_cod;
	UINT8 face_cou;
	//UINT16 count1_x;
	//UINT16 count1_y;
	//UINT16 count2_x;
	//UINT16 count2_y;
	//UINT16 count3_x;
	//UINT16 count3_y;
}FaceFuncSetting;


typedef struct
{
	UINT8 api_key[32];
	UINT8 secret_key[32];
	UINT8 facial;
	UINT8 faces;
	UINT8 face_img;
	UINT16 box;
	UINT16 width;
	UINT16 height;
	UINT16 reliability;
	UINT16 res_time;
	UINT8 groupID[4];
}FaceParameter;


typedef struct
{
	UINT8 head[4];
	UINT8 lan;
	UINT8 comparison;
	UINT16 jpgmem;
}AIFunction;


typedef struct
{
	IPPort	ipp;
	FaceParameter fp;
	FaceFuncSetting ffs;
	AIFunction af;
	UINT8 rec;
}AICameraInfo;

//在MainDlg中使用SNotifyCenter的通知的异步事件 
class MainSocketThread : public TplEventArgs<MainSocketThread>
{
	SOUI_CLASS_NAME(MainSocketThread, L"on_main_socket_thread")
public:
	MainSocketThread(SObject *pSender) :TplEventArgs<MainSocketThread>(pSender){}
	enum{ EventID = EVT_SOCKET_BEGIN };

	SOCKETOPTION opt;
	bool  retOK;
	string nData;
};

class WinSocketClient
{
private:
	long long Convert(char* data);
	int ll2str(char *s, long long value);
	int ull2str(char *s, unsigned long long v);
	int cmsSend(SOCKET sockfd, char *buff, int len, int mode);
	int cmsRecv(SOCKET sockfd, char *buff, int len, int mode);
	bool SendBuffToHost(const char *ip, char *sendbuff, int sendlen, char *outbuff);
public:
	bool GetDeviceID(const char *ip, string &id);
	bool SetCameraTime(const char *ip, DateTime &dt);
	bool GetHostIPAddr(const char *ip, IPPort &ipp);
	bool SetHostIPAddr(const char *ip, IPPort &ipp);
	bool SetFaceParameter(const char *ip, FaceParameter &fpar);
	bool GetFaceParameter(const char *ip, FaceParameter &fpar);
	bool SetAIFunction(const char *ip, AIFunction &aifunc);
	bool GetAIFunction(const char *ip, AIFunction &aifunc);
	bool SetVmd(const char *ip, UINT8 &aifunc);
	bool GetVmd(const char *ip, UINT8 &aifunc);
	bool SetFaceFuncSetting(const char *ip, FaceFuncSetting &ffset);
	bool GetFaceFuncSetting(const char *ip, FaceFuncSetting &ffset);
	//bool GetPanorama(const char *ip, string &panoTemplate);
};