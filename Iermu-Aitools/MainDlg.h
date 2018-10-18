// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "threadObject.h"
#include "WinSocketClient.h"
#include "UpnpTool.h"
#include "AdapterIP.h"
#include "PlayerDlg.h"
#include "PlayerInterface.h"

struct UserInfo {
	SStringT strName;			//������
	SStringT strIp;				//IP��ַ
	SStringT strAdapterName;	//���������ַ
	string	 strMask;			//����
	string	 strGateway;		//����
	string   strDns;			//dns
	int		 dhcpEnabled;		//�Ƿ��Զ���ȡip
};


typedef enum
{
	MAIN_WND,
	LIST_WND,
	LIST_INFO,
	INFO_WND,
	STEP_ZERO,
	STEP_ONE,
	STEP_TWO,
	STEP_THREE,
	STEP_FOUR,
}BACKTOWND;

class CMainDlg : public SHostWnd
				, public CThreadObject	//�̶߳���
				, public TAutoEventMapReg<CMainDlg>	//֪ͨ�����Զ�ע��
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);
	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	void SetCameraIP(char *ip);
	bool SetCameraTime();
	void RefreshCameraList();
	void GetCameraList();
	void GetCameraInfo();
	void OnMenuSelLocalIP(SStringT ip);
	void SetDisplayProgress(const wchar_t * showName, const wchar_t * hideName);
	void SetCameraInfoPage(int pos);

	CPlayerInterface m_player;
private:
	int  initServerConfig();
	int  OnStarttheServer();
	void OnStoptheServer();
	void InitServer();
	void InitSetEvent();
	BOOL BrowseDir(SStringT &path, HWND hwndOwner, SStringT title);
	void SetPicturePath();
	void SetServer();
	void OnOpenPlay(int voiceType);
	void OnPlay();
	void OnPlayNoVoice();
	void UpdateCameraInfo();
	void GetTime(DateTime &dt);
	bool CheckIp(int type, LPCWSTR pszName, LPCWSTR tipName, string &ip);
	bool SendCMD(SOCKETOPTION opt, REQUEST_TYPE type, string url, string data);
	bool GetNASError(WinSocketClient &client, SStringT &code);
	void OnMenuShowLocalIP();
	void OnMenuHideLocalIP();
	void OnMenuLoadLocalPort();
	bool OnFormatSDcard();
	bool OnListenTabSelChangePage(EventArgs *pEvtBase);
	bool OnListenIPDropdownBox(EventArgs *pEvtBase);
	bool OnListenAnonymousCheckBox(EventArgs *pEvtBase);
	bool OnListenNASSwitchCheckBox(EventArgs *pEvtBase);
	bool OnListenRECSwitchCheckBox(EventArgs *pEvtBase);
	bool OnListenNetSelChangeBox(EventArgs *pEvtBase);
	void GoToCameraInfoPage();
	void GoToCameraListPage();
	bool SetCameraGeneral();
	bool SetCameraServer();
	bool SetCameraStore();
	bool SetCameraOther();
	bool OnStartSocketThread(LPVOID data);
	bool OnMainSocketThread(EventArgs *e);
	int  GetLocalIPInfo(SArray<UserInfo> &Info);
	void SetLocalIPView();
	void SetDisplayBackBtn(int page, BOOL isShow, BACKTOWND wnd);
	void BackToTabpage();
	void OnStepChange(int pre, int back, SStringT tip);
	void ConfigureNative();
	void OnStepOne();
	void OnCancelTwo();
	void OnStepTwo();
	void OnJumpTwo();
	void OnStepThree();
	void OnStepFour();
	virtual UINT Run(LPVOID data);
	virtual int GetID() const { return SENDER_MAINDLG_ID; }
protected:
		//soui��Ϣ
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"get_camera_list", GetCameraList)				//ͨ������IP��ȡ�б�
		EVENT_NAME_COMMAND(L"get_camera_info", GetCameraInfo)				//ͨ�������IP��ȡ��Ϣ	
		EVENT_NAME_COMMAND(L"btn_step1", OnStepOne)							//��һ����¼
		EVENT_NAME_COMMAND(L"cancel_step2", OnCancelTwo)					//�ڶ���ע��
		EVENT_NAME_COMMAND(L"btn_step2", OnStepTwo)							//�ڶ���ע��
		EVENT_NAME_COMMAND(L"jump_step2", OnJumpTwo)						//�ڶ�������
		EVENT_NAME_COMMAND(L"btn_step3", OnStepThree)						//����������
		EVENT_NAME_COMMAND(L"btn_step4", OnStepFour)						//���Ĳ�����
		EVENT_NAME_COMMAND(L"btn_camerainfo_back", BackToTabpage)			//���ذ���
		EVENT_NAME_COMMAND(L"btn_camerainfo_ref", RefreshCameraList)		//ˢ���б�
		EVENT_NAME_COMMAND(L"btn_playLive_novoice", OnPlayNoVoice)			//���ӳ�ֱ��
		EVENT_NAME_COMMAND(L"btn_playLive", OnPlay)							//����ֱ��
		EVENT_NAME_COMMAND(L"btn_timeSync", SetCameraTime)					//ʱ��ͬ��	
		EVENT_NAME_COMMAND(L"btn_camera_general", SetCameraGeneral)			//ͨ������
		EVENT_NAME_COMMAND(L"btn_camera_server", SetCameraServer)			//��������
		EVENT_NAME_COMMAND(L"btn_camera_store", SetCameraStore)				//��������
		EVENT_NAME_COMMAND(L"btn_camera_other", SetCameraOther)				//��������
		EVENT_NAME_COMMAND(L"btn_picture_path", SetPicturePath)				//���÷���洢ͼƬ·��			
		EVENT_NAME_COMMAND(L"btn_set_server", SetServer)					//���÷�������
		EVENT_NAME_COMMAND(L"start_server", OnStarttheServer)				//����������
		EVENT_NAME_COMMAND(L"stop_server", OnStoptheServer)					//�رշ�����
		EVENT_NAME_COMMAND(L"menu_btn_selip", OnMenuShowLocalIP)			//���ѡȡ����IP
		EVENT_NAME_COMMAND(L"menu_btn_hideip", OnMenuHideLocalIP)			//�������IP�˵�
		EVENT_NAME_COMMAND(L"menu_btn_selport", OnMenuLoadLocalPort)		//��ȡ�������˿ں�
		EVENT_NAME_COMMAND(L"btn_format_sdcard", OnFormatSDcard)			//��ʽ��SD��
		//EVENT_ID_HANDLER(R.id.cbx_as_server, EventCBSelChange::EventID, OnAsServerChange)
		EVENT_ID_HANDLER(SENDER_MAINDLG_ID, MainSocketThread::EventID, OnMainSocketThread)
	EVENT_MAP_END()
		
	//HostWnd��ʵ������Ϣ����
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
	string					m_token;
	SocketData				m_data;
	BOOL					m_menushow;			//ѡȡ����IP�ĵ����ʾ������
	SArray<UserInfo>		m_userInfo;			//����ip�б���Ϣ
	BACKTOWND				m_set_wnd;			//����ҳ���ʾ��ǰ����
	BACKTOWND				m_add_wnd;			//���ҳ���ʾ��ǰ����
	AICameraInfo			m_cinfo;			//��ǰ�����������������Ϣ
	SArray<AICameraInfo>	m_cinfolist;		//�����������������Ϣ�б�
	SArray<CameraAddr>		m_iplist;			//������б�id, ip, url
	string					m_ConfFileName;		//�����ļ�·��
	int						m_curSel;			//��ǰlistѡ����
	PlayerDlg				m_dlgPlayer;
	string					m_deviceId;			//ע���豸id
	CameraNet				m_cameraNet;		//�ֶ����������NET��Ϣ
};
