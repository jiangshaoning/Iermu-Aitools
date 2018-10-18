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
	SStringT strName;			//网卡名
	SStringT strIp;				//IP地址
	SStringT strAdapterName;	//网卡物理地址
	string	 strMask;			//掩码
	string	 strGateway;		//网关
	string   strDns;			//dns
	int		 dhcpEnabled;		//是否自动获取ip
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
		//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"get_camera_list", GetCameraList)				//通过本机IP获取列表
		EVENT_NAME_COMMAND(L"get_camera_info", GetCameraInfo)				//通过摄像机IP获取信息	
		EVENT_NAME_COMMAND(L"btn_step1", OnStepOne)							//第一步登录
		EVENT_NAME_COMMAND(L"cancel_step2", OnCancelTwo)					//第二步注消
		EVENT_NAME_COMMAND(L"btn_step2", OnStepTwo)							//第二步注册
		EVENT_NAME_COMMAND(L"jump_step2", OnJumpTwo)						//第二步跳过
		EVENT_NAME_COMMAND(L"btn_step3", OnStepThree)						//第三步上线
		EVENT_NAME_COMMAND(L"btn_step4", OnStepFour)						//第四步继续
		EVENT_NAME_COMMAND(L"btn_camerainfo_back", BackToTabpage)			//返回按键
		EVENT_NAME_COMMAND(L"btn_camerainfo_ref", RefreshCameraList)		//刷新列表
		EVENT_NAME_COMMAND(L"btn_playLive_novoice", OnPlayNoVoice)			//低延迟直播
		EVENT_NAME_COMMAND(L"btn_playLive", OnPlay)							//播放直播
		EVENT_NAME_COMMAND(L"btn_timeSync", SetCameraTime)					//时间同步	
		EVENT_NAME_COMMAND(L"btn_camera_general", SetCameraGeneral)			//通用设置
		EVENT_NAME_COMMAND(L"btn_camera_server", SetCameraServer)			//服务设置
		EVENT_NAME_COMMAND(L"btn_camera_store", SetCameraStore)				//储存设置
		EVENT_NAME_COMMAND(L"btn_camera_other", SetCameraOther)				//其他设置
		EVENT_NAME_COMMAND(L"btn_picture_path", SetPicturePath)				//设置服务存储图片路径			
		EVENT_NAME_COMMAND(L"btn_set_server", SetServer)					//设置服务配置
		EVENT_NAME_COMMAND(L"start_server", OnStarttheServer)				//启动服务器
		EVENT_NAME_COMMAND(L"stop_server", OnStoptheServer)					//关闭服务器
		EVENT_NAME_COMMAND(L"menu_btn_selip", OnMenuShowLocalIP)			//点击选取本机IP
		EVENT_NAME_COMMAND(L"menu_btn_hideip", OnMenuHideLocalIP)			//点击隐藏IP菜单
		EVENT_NAME_COMMAND(L"menu_btn_selport", OnMenuLoadLocalPort)		//读取服务器端口号
		EVENT_NAME_COMMAND(L"btn_format_sdcard", OnFormatSDcard)			//格式化SD卡
		//EVENT_ID_HANDLER(R.id.cbx_as_server, EventCBSelChange::EventID, OnAsServerChange)
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
	string					m_token;
	SocketData				m_data;
	BOOL					m_menushow;			//选取本机IP的点击显示或隐藏
	SArray<UserInfo>		m_userInfo;			//本机ip列表信息
	BACKTOWND				m_set_wnd;			//设置页面表示当前窗口
	BACKTOWND				m_add_wnd;			//添加页面表示当前窗口
	AICameraInfo			m_cinfo;			//当前的摄像机人脸参数信息
	SArray<AICameraInfo>	m_cinfolist;		//摄像机的人脸参数信息列表
	SArray<CameraAddr>		m_iplist;			//摄像机列表id, ip, url
	string					m_ConfFileName;		//配置文件路径
	int						m_curSel;			//当前list选中行
	PlayerDlg				m_dlgPlayer;
	string					m_deviceId;			//注册设备id
	CameraNet				m_cameraNet;		//手动设置摄像机NET信息
};
