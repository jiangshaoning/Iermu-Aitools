// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "threadObject.h"
#include "WinSocketClient.h"
#include "UpnpTool.h"
#include "AdapterIP.h"
#include "PlayerInterface.h"

struct UserInfo {
	SStringT strName;
	SStringT strIp;
};

class CMainDlg : public SHostWnd
				, public CThreadObject	//线程对象
				, public TAutoEventMapReg<CMainDlg>	//通知中心自动注册
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);
	
	void OnBtnMsgBox();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

//	void UploadIPRequest(IPPort &dm);
	void SetCameraIP(string &ip);
	void SetCameraTime();
	void BackToSetLocalIp();
	void RefreshLocalIp();
	void OnPlayFromCameraIp();
	void OnPlayFromLocalIp();
	void SetTimeFromCameraIp();
	void SetTimeFromLocalIp();
	void SetFaceParameter();
	void SetFaceParameterFromLoalIP(UINT8&rec, FaceParameter &fpara, FaceFuncSetting &ffset, IPPort &dm);
	void SetAIFunctionFromLoalIP(AIFunction &aifunc);
	void SetAIFunction();
	void GetCameraInfo();

	CPlayerInterface m_player;
private:
	int  initServerConfig();
	int  OnStarttheServer();
	void  OnStoptheServer();
	void InitServer();
	BOOL BrowseDir(SStringT &path, HWND hwndOwner, SStringT title);
	void SetPicturePath();
	void SetServer();
	void OnPlay();
//	void UploadIP();
	void UpdateCameraInfo();
	void GetTime(DateTime &dt);
	bool CheckIp(int type, LPCWSTR pszName, LPCWSTR tipName, string &ip);
	bool CheckAIFunction();
	bool CheckFaceParameter();
	void PlayFromLocalPage();
	void OnStartSocketThread(LPVOID data);
	bool OnMainSocketThread(EventArgs *e);
	void SetRefreshDisplay(bool Dpflag);
	int  GetLocalIPInfo(SArray<UserInfo> &Info);
	void SetLocalIPView(void);
	virtual UINT Run(LPVOID data);
	virtual int GetID() const { return SENDER_MAINDLG_ID; }
protected:
		//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"get_camera_info", GetCameraInfo)
		EVENT_NAME_COMMAND(L"btn_back", BackToSetLocalIp)
		EVENT_NAME_COMMAND(L"btn_ref", RefreshLocalIp)
		EVENT_NAME_COMMAND(L"btn_play_fc", OnPlayFromCameraIp)
		EVENT_NAME_COMMAND(L"btn_setFace_parameter", SetFaceParameter)
		EVENT_NAME_COMMAND(L"btn_setAI_function", SetAIFunction)
		EVENT_NAME_COMMAND(L"btn_timeSet", SetTimeFromCameraIp)
		EVENT_NAME_COMMAND(L"btn_localIpSet", OnPlayFromLocalIp)
//		EVENT_NAME_COMMAND(L"btn_hostIp", UploadIP)
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"set_picturepath", SetPicturePath)
		EVENT_NAME_COMMAND(L"set_server", SetServer)
		EVENT_NAME_COMMAND(L"start_server", OnStarttheServer)
		EVENT_NAME_COMMAND(L"stop_server", OnStoptheServer)
		EVENT_ID_HANDLER(SENDER_MAINDLG_ID, MainSocketThread::EventID, OnMainSocketThread)
	EVENT_MAP_END()
		
	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	BOOL					m_bLayoutInited;
	string					m_localIp;
	string					m_cameraIp;
	string					m_cameraUrl;
	SocketData				m_data;
	UINT8					m_rec;
	FaceFuncSetting			m_ffset;  //人脸功能开启
	FaceParameter			m_fpara;  //人脸识别参数等
	IPPort					m_dm;     //图片上传配置
	AIFunction				m_aifunc;
	AICameraInfo			m_cinfo;
	SArray<UINT8>			m_reclist;
	SArray<FaceFuncSetting> m_ffsetlist;
	SArray<CameraAddr>		m_iplist;
	SArray<IPPort>			m_domainlist;
	SArray<FaceParameter>	m_fplist;
	SArray<AIFunction>		m_aflist;
	string					m_ConfFileName; //配置文件路径
};
