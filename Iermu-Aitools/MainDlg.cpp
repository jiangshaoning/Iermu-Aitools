// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Iphlpapi.h>
#include <shlobj.h>
#include "MainDlg.h"
#include "HTTPConfig.h"
#include "HTTPServer.h"
#include "json.h"

#define TOKEN		"1.j095DeA6kUL9oqZrcdmm.2592000.1538204705.4a802f17e619293fdaa6cd49615a5724"
using namespace std;
#pragma comment(lib,"Iphlpapi.lib")
#define RELEASEPLAYER(player) \
if (player) \
{ \
	player_close(player); \
	player = NULL; \
}

#ifdef DWMBLUR	//win7ë��������
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif

HTTPServer theServer;
HTTPConfig theConf;

class CBVAdapterFix : public SAdapterBase
{
protected:

	SArray<UserInfo>    m_userInfo;
public:
	CBVAdapterFix(SArray<UserInfo>  &ui)
	{
		m_userInfo.RemoveAll();
		for (unsigned int i = 0; i < ui.GetCount(); i++)
		{
			m_userInfo.Add(ui.GetAt(i));
		}
	}
	virtual int getCount()
	{
		return m_userInfo.GetCount();
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
		SWindow *pName = pItem->FindChildByID(1);
		pName->SetWindowText(m_userInfo.GetAt(position).strIp);
		SWindow *pAccount = pItem->FindChildByID(2);
		pAccount->SetWindowText(m_userInfo.GetAt(position).strName);
	}

	SStringT getItemDesc(int position)
	{
		return m_userInfo.GetAt(position).strIp;
	}


	UserInfo getItem(int position)
	{
		SASSERT(position >= 0 && position < (int)m_userInfo.GetCount());
		return m_userInfo[position];
	}
};

class ListAdapterFix : public SAdapterBase
{
protected:
	CMainDlg			*m_dlgMain;
	SArray<UserInfo>    m_userInfo;
public:
	ListAdapterFix(SArray<UserInfo>  &ui, CMainDlg *dlg)
	{
		m_dlgMain = dlg;
		for (unsigned int i = 0; i < ui.GetCount(); i++)
		{
			m_userInfo.Add(ui.GetAt(i));
		}
	}
	virtual int getCount()
	{
		return m_userInfo.GetCount();
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
		SWindow *pName = pItem->FindChildByName2<SWindow>(L"item_local_ip");
		pName->SetWindowText(m_userInfo.GetAt(position).strIp);

		pItem->SetUserData(position);
		pItem->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&ListAdapterFix::OnItemClick, this));
	}

	SStringT getItemDesc(int position)
	{
		return m_userInfo.GetAt(position).strIp;
	}

	bool OnItemClick(EventArgs *pEvt)
	{
		SWindow *pItem = sobj_cast<SWindow>(pEvt->sender);
		int pos = pItem->GetUserData();
		SStringT ip = getItemDesc(pos);

		m_dlgMain->OnMenuSelLocalIP(ip);
		return true;
	}
};
	
CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
	m_set_wnd = MAIN_WND;
	m_add_wnd = STEP_ONE;
}

CMainDlg::~CMainDlg()
{
	m_player.ffplayStop();
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7ë��������
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	initServerConfig();
	SetMsgHandled(FALSE);
	return 0;
}

void CMainDlg::InitSetEvent()
{
	STabCtrl *pTabmain = FindChildByName2<STabCtrl>(L"tab_main");
	pTabmain->GetEventSet()->subscribeEvent(EventTabSelChanged::EventID, Subscriber(&CMainDlg::OnListenTabSelChangePage, this));

	SComboView *pComboView = FindChildByName2<SComboView>(L"cbv_account");
	pComboView->GetEventSet()->subscribeEvent(EventCBDropdown::EventID, Subscriber(&CMainDlg::OnListenIPDropdownBox, this));

	SCheckBox *check_nas_anonymous = FindChildByName2<SCheckBox>(L"check_nas_anonymous");
	check_nas_anonymous->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&CMainDlg::OnListenAnonymousCheckBox, this));

	SCheckBox *check_switch_nas = FindChildByName2<SCheckBox>(L"check_switch_nas");
	check_switch_nas->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&CMainDlg::OnListenNASSwitchCheckBox, this));

	SCheckBox *check_switch_rec = FindChildByName2<SCheckBox>(L"check_switch_rec");
	check_switch_rec->GetEventSet()->subscribeEvent(EventCmd::EventID, Subscriber(&CMainDlg::OnListenRECSwitchCheckBox, this));

	SComboBox *cbx_step3_net = FindChildByName2<SComboBox>(L"cbx_step3_net");
	cbx_step3_net->GetEventSet()->subscribeEvent(EventCBSelChange::EventID, Subscriber(&CMainDlg::OnListenNetSelChangeBox, this));

}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	m_menushow = FALSE;
	InitServer();
	//SetLocalIPView();
	InitSetEvent();
	m_curSel = -1;
	return 0;
}
//TODO:��Ϣӳ��
void CMainDlg::OnClose()
{
	OnStoptheServer();
	m_player.ffplayStop();
	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::GoToCameraInfoPage()
{
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"irm_main");
	if (pTab)
	{
		SetLocalIPView();
		UpdateCameraInfo();
		pTab->SetCurSel(_T("camrainfo_page"));
		SetDisplayProgress(L"camera_ip_win", L"camera_ip_progress");
		FindChildByName2<SWindow>(L"text_formate_progress")->SetVisible(FALSE, TRUE);
	}
}

void CMainDlg::GoToCameraListPage()
{
	SetDisplayProgress(L"cameralist_win", L"refresh_progress");

	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"irm_main");
	if (pTab)
	{
		SMCListView * pMcListView = FindChildByName2<SMCListView>("cameralist_win");
		if (pMcListView)
		{
			CAdapterIP *m_pAdapter = new CAdapterIP(this);
			pMcListView->SetAdapter(m_pAdapter);
			m_pAdapter->SetTags(m_iplist);
			m_pAdapter->Release();
		}
		
		FindChildByName2<SWindow>("txt_lanip")->SetWindowTextW(S_CA2T(m_localIp.c_str()));
		pTab->SetCurSel(_T("camralist_page"));
		SetDisplayProgress(L"local_ip_win", L"local_ip_progress");
	}
}

int CMainDlg::GetLocalIPInfo(SArray<UserInfo> &Info)
{
	//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//�õ��ṹ���С,����GetAdaptersInfo����
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//��¼��������
	int netCardNum = 0;
	//��¼ÿ�������ϵ�IP��ַ����
	int IPnumPerNetCard = 0;

	UserInfo ui;

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//����������ص���ERROR_BUFFER_OVERFLOW
		//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
		//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
		//�ͷ�ԭ�����ڴ�ռ�
		delete pIpAdapterInfo;
		//���������ڴ�ռ������洢����������Ϣ
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		Info.RemoveAll();
		//���������Ϣ
		//�����ж�����,���ͨ��ѭ��ȥ�ж�
		while (pIpAdapterInfo)
		{
			//cout << "����������" << ++netCardNum << endl;
			//cout << "����������" << pIpAdapterInfo->Description << endl;
			ui.strName = S_CA2T(pIpAdapterInfo->Description);
			ui.strAdapterName = S_CA2T(pIpAdapterInfo->AdapterName);
			//cout << "����IP��ַ���£�" << endl;

			//���������ж�IP,���ͨ��ѭ��ȥ�ж�
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{
				//cout << "�������ϵ�IP������" << ++IPnumPerNetCard << endl;
				//cout << "IP ��ַ��" << pIpAddrString->IpAddress.String << endl;			
				if (strcmp(pIpAdapterInfo->GatewayList.IpAddress.String, "0.0.0.0") == 0)
					break;
				ui.dhcpEnabled = pIpAdapterInfo->DhcpEnabled;
				ui.strIp = S_CA2T(pIpAddrString->IpAddress.String);
				ui.strMask = pIpAddrString->IpMask.String;
				ui.strGateway = pIpAdapterInfo->GatewayList.IpAddress.String;
				ui.strDns = pIpAdapterInfo->DhcpServer.IpAddress.String;
				Info.Add(ui);
				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}

	//�ͷ��ڴ�ռ�
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}
	return 0;
}

void CMainDlg::SetLocalIPView(void)
{
	//GetLocalIPInfo(m_userInfo);
	//SComboView *pComboView = FindChildByName2<SComboView>(L"cbv_account");
	//if (pComboView)
	//{
	//	SListView *pLstView = pComboView->GetListView();
	//	CBVAdapterFix *pAdapter = new CBVAdapterFix(m_userInfo);
	//	pLstView->SetAdapter(pAdapter);
	//	pAdapter->Release();
	//	pComboView->SetCurSel(-1);
	//}

	SListView *pLstLocalIP = FindChildByName2<SListView>("list_local_ip");
	if (pLstLocalIP)
	{
		ListAdapterFix *pAdapter = new ListAdapterFix(m_userInfo, this);
		pLstLocalIP->SetAdapter(pAdapter);
		pAdapter->Release();
	}
}

BOOL CMainDlg::BrowseDir(SStringT &path, HWND hwndOwner, SStringT title)
{
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = hwndOwner;//������  
	bInfo.lpszTitle = title;
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI/*����һ���༭�� �û������ֶ���д·�� �Ի�����Ե�����С֮���..*/ |
		BIF_UAHINT/*��TIPS��ʾ*/ | BIF_NONEWFOLDERBUTTON /*�����½��ļ��а�ť*/;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL)//������ȷ����ť  
	{
		SHGetPathFromIDList(lpDlist, szPathName);
		path.Format(_T("%s"), szPathName);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CMainDlg::initServerConfig()
{
	// ��ʼ������ģ��,�����ļ��� EXE Ŀ¼�µ� conf.xml
	TCHAR szFilePath[520] = { 0 };

	if (0 == GetModuleFileName(NULL, szFilePath, 512))
	{
		assert(0);
	}
	else
	{
		TCHAR* pEnd = _tcsrchr(szFilePath, _T('\\'));
		if (pEnd != NULL)
		{
			_tcscpy(pEnd, _T("\\conf.xml"));
			m_ConfFileName = TtoA(szFilePath);
		}
	}
	if (m_ConfFileName.empty()) m_ConfFileName = "conf.xml";
	if (!theConf.load(m_ConfFileName))
	{
		MessageBox(NULL, _T("�޷���ȡ/���������ļ� conf.xml"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return -1;
	}
	return 0;
}

void CMainDlg::SetServer()
{
	SEdit *edit_picturepath = FindChildByName2<SEdit>(L"edit_picture_path");
	SEdit *edit_serverport = FindChildByName2<SEdit>(L"edit_server_port");
	SComboBox *check_autoserver = FindChildByName2<SComboBox>(L"cbx_autorun");
	SComboBox *check_dirbrowse = FindChildByName2<SComboBox>(L"cbx_dirbrowse");

	if (edit_picturepath)
	{
		string path = S_CT2A(edit_picturepath->GetWindowTextW());
		theConf.setTmpRoot(path);
		theConf.setDocRoot(path);
	}
	if (edit_serverport)
	{
		string port = S_CT2A(edit_serverport->GetWindowTextW());
		theConf.setPort(atoi(port.c_str()));
	}
	theConf.setAutoRun(check_autoserver->GetCurSel());//�Զ�����
	theConf.setDirVisible(check_dirbrowse->GetCurSel());//�������Ŀ¼
	theConf.save(m_ConfFileName);
	MessageBox(NULL, _T("�������"), _T("��ʾ"), MB_OK);
}

void CMainDlg::SetPicturePath()
{
	SStringT path;
	SEdit *edit_picture_path = FindChildByName2<SEdit>(L"edit_picture_path");
	BOOL ret = BrowseDir(path, m_hWnd, _T("��ѡ��һ���ļ���"));

	if (ret)
	{
		edit_picture_path->SetWindowTextW(path);
	}
}

int CMainDlg::OnStarttheServer()
{
	SWindow *server_status = FindChildByName2<SWindow>(L"server_status");
	int ret = theServer.run(&theConf, NULL);
	switch (ret)
	{
		case SE_RUNING:
			ret = 1;
			server_status->SetWindowTextW(_T("ͼƬ������������"));
			MessageBox(NULL, _T("ͼƬ�������Ѿ�����, �����ظ�������"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			break;
		case SE_SUCCESS:
			ret = 1;
			server_status->SetWindowTextW(_T("����������"));
			break;
		default:
			ret = -1;
			server_status->SetWindowTextW(_T("������ֹͣ"));
			MessageBox(NULL, _T("ͼƬ����������ʧ�ܣ���������Ƿ�ռ�û����Ѿ�����"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			break;
	}
	return ret;
}

void CMainDlg::OnStoptheServer()
{
	if (theServer.runing())
	{
		SWindow *server_status = FindChildByName2<SWindow>(L"server_status");
		server_status->SetWindowTextW(_T("������ֹͣ"));
		theServer.stop();
	}
}

void CMainDlg::InitServer()
{
	char port[32] = { 0 };
	FindChildByName2<SEdit>(L"edit_picture_path")->SetWindowTextW(S_CA2T(theConf.tmpRoot().c_str()));
	FindChildByName2<SEdit>(L"edit_server_port")->SetWindowTextW(S_CA2T(itoa(theConf.port(), port, 10)));
	FindChildByName2<SComboBox>(L"cbx_dirbrowse")->SetCurSel((int)theConf.dirVisible());

	if (theConf.autoRun())
	{
		FindChildByName2<SComboBox>(L"cbx_autorun")->SetCurSel(1);
		OnStarttheServer();
	}

}

void CMainDlg::GetTime(DateTime &dt)
{
	SYSTEMTIME st;
	SStringT strDateTime;

	GetLocalTime(&st);

	dt.date = st.wYear << 16 | ((BYTE)st.wMonth) << 8 | ((BYTE)st.wDay);
	dt.time = st.wHour << 16 | ((BYTE)st.wMinute) << 8 | ((BYTE)st.wSecond);
	SLOGFMTE("%04d��%02d��%02d��%02dʱ%02d��%02d��", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

void CMainDlg::OnMenuShowLocalIP()
{
	SListView *pLstLocalIP = FindChildByName2<SListView>("list_local_ip");
	pLstLocalIP->SetVisible(TRUE, TRUE);

	FindChildByName2<SWindow>("menu_btn_selip")->SetVisible(FALSE, TRUE);
	FindChildByName2<SWindow>("menu_btn_hideip")->SetVisible(TRUE, TRUE);
}

void CMainDlg::OnMenuHideLocalIP()
{
	SListView *pLstLocalIP = FindChildByName2<SListView>("list_local_ip");
	pLstLocalIP->SetVisible(FALSE, TRUE);

	FindChildByName2<SWindow>("menu_btn_hideip")->SetVisible(FALSE, TRUE);
	FindChildByName2<SWindow>("menu_btn_selip")->SetVisible(TRUE, TRUE);
}

void CMainDlg::OnMenuSelLocalIP(SStringT ip)
{
	OnMenuHideLocalIP();
	FindChildByName2<SEdit>("edit_general_server")->SetWindowTextW(ip);
}

void CMainDlg::OnMenuLoadLocalPort()
{
	FindChildByName2<SEdit>(L"edit_general_port")->SetWindowTextW(SStringT().Format(_T("%d"), theConf.port()));
}

bool CMainDlg::OnFormatSDcard()
{
	FindChildByName2<SWindow>(L"text_formate_progress")->SetVisible(TRUE, TRUE);
	return SendCMD(OPT_FORMAT_SDCARD, (REQUEST_TYPE)0, "", "");
}

void CMainDlg::GetCameraInfo()
{
	//jiangshaoning
	//STabCtrl *pTab = FindChildByName2<STabCtrl>(L"irm_main");
	//if (pTab)
	//{
	//	m_switchlist_tab = false;
	//	pTab->SetCurSel(_T("camrainfo_page"));
	//	return;
	//}

	if (CheckIp(0, L"edit_cameraIp", _T("�����������IP"), m_cameraIp))
	{
		if (SendCMD(OPT_GETCAMERA_INFO, (REQUEST_TYPE)0, "", ""))
			SetDisplayProgress(L"camera_ip_progress", L"camera_ip_win");
	}
}

void CMainDlg::SetDisplayBackBtn(int page, BOOL isShow, BACKTOWND wnd)
{
	switch (page)
	{
	case 0:
		m_set_wnd = wnd;
		break;
	case 2:
		m_add_wnd = wnd;
		break;
	}

	FindChildByName2<SWindow>(L"btn_camerainfo_back")->SetVisible(isShow, TRUE);
	FindChildByName2<SWindow>(L"btn_camerainfo_pre")->SetVisible(isShow, TRUE);
}

void CMainDlg::BackToTabpage()
{
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"irm_main");
	int selpage = FindChildByName2<STabCtrl>(L"tab_main")->GetCurSel();
	if (pTab)
	{
		if (selpage == 0)
		{
			switch (m_set_wnd)
			{
			case MAIN_WND:
				SetDisplayBackBtn(selpage, FALSE, MAIN_WND);
				pTab->SetCurSel(_T("tab_page"));
				break;
			case LIST_WND:
				SetDisplayBackBtn(selpage, FALSE, MAIN_WND);
				pTab->SetCurSel(_T("tab_page"));
				break;
			case LIST_INFO:
				SetDisplayBackBtn(selpage, TRUE, LIST_WND);
				pTab->SetCurSel(_T("camralist_page"));
				break;
			case INFO_WND:
				SetDisplayBackBtn(selpage, FALSE, MAIN_WND);
				pTab->SetCurSel(_T("tab_page"));
				break;
			}
		}

		if (selpage == 2)
		{
			switch (m_add_wnd)
			{
			case STEP_ZERO:
				OnStepChange(0, 0, L"");
				break;
			case STEP_ONE:
				OnStepChange(1, 1, L"");
				break;
			case STEP_TWO:
				OnStepChange(2, 1, L"");
				break;
			case STEP_THREE:
				OnStepChange(3, 2, L"");
				break;
			case STEP_FOUR:
				OnStepChange(4, 3, L"");
				break;
			}
		}
	}
}

//�б�ѡ�е��������Ϣ����
void CMainDlg::SetCameraInfoPage(int pos)
{
	m_curSel = pos;
	SetDisplayBackBtn(0, TRUE, LIST_INFO);
	memset(&m_cinfo, 0, sizeof(AICameraInfo));
	memcpy(&m_cinfo, &m_cinfolist.GetAt(pos), sizeof(AICameraInfo));
	SetCameraIP(m_cinfo.cad.cameraip);
	GoToCameraInfoPage();
}

bool CMainDlg::OnListenTabSelChangePage(EventArgs *pEvtBase)
{
	STabCtrl *pTabmain = sobj_cast<STabCtrl>(pEvtBase->sender);
	BOOL show1 = (m_set_wnd == MAIN_WND) ? FALSE : TRUE;
	BOOL show2 = (m_add_wnd == STEP_ONE) ? FALSE : TRUE;
	if (pTabmain)
	{
		switch (pTabmain->GetCurSel())
		{
		case 0:
			SetDisplayBackBtn(0, show1, m_set_wnd);
			break;
		case 1:
			SetDisplayBackBtn(0, FALSE, m_set_wnd);
			break;
		case 2:
			SetDisplayBackBtn(2, show2, m_add_wnd);
			break;
		}
	}
	return true;
}

bool CMainDlg::OnListenIPDropdownBox(EventArgs *pEvtBase)
{
	GetLocalIPInfo(m_userInfo);
	SComboView *pComboView = sobj_cast<SComboView>(pEvtBase->sender);
	if (pComboView)
	{
		SListView *pLstView = pComboView->GetListView();
		CBVAdapterFix* pAdapter = new CBVAdapterFix(m_userInfo);
		pLstView->SetAdapter(pAdapter);				
		pAdapter->Release();
		pComboView->SetCurSel(-1);
	}
	return true;
}

//����checkbox����
bool CMainDlg::OnListenAnonymousCheckBox(EventArgs *pEvtBase)
{
	SCheckBox *sCbx = sobj_cast<SCheckBox>(pEvtBase->sender);
	SEdit *edit_nas_user = FindChildByName2<SEdit>(L"edit_nas_user");
	SEdit *edit_nas_passwd = FindChildByName2<SEdit>(L"edit_nas_passwd");
	if (sCbx->IsChecked())
	{
		edit_nas_user->SetWindowTextW(L"");
		edit_nas_user->SetAttribute(L"readonly", L"1", FALSE);
		edit_nas_passwd->SetWindowTextW(L"");
		edit_nas_passwd->SetAttribute(L"readonly", L"1", FALSE);
	}
	else
	{
		edit_nas_user->SetAttribute(L"readonly", L"0", FALSE);
		edit_nas_passwd->SetAttribute(L"readonly", L"0", FALSE);
	}

	return true;
}

//NAS����checkbox����
bool CMainDlg::OnListenNASSwitchCheckBox(EventArgs *pEvtBase)
{
	SCheckBox *sCbx = sobj_cast<SCheckBox>(pEvtBase->sender);
	if (sCbx->IsChecked())
	{
		SCheckBox *check_switch_rec = FindChildByName2<SCheckBox>(L"check_switch_rec");
		check_switch_rec->SetCheck(false);
	}
	return true;
}

bool CMainDlg::OnListenRECSwitchCheckBox(EventArgs *pEvtBase)
{
	SCheckBox *sCbx = sobj_cast<SCheckBox>(pEvtBase->sender);
	if (sCbx->IsChecked())
	{
		SCheckBox *check_switch_nas = FindChildByName2<SCheckBox>(L"check_switch_nas");
		check_switch_nas->SetCheck(false);
	}
	return true;
}

bool CMainDlg::OnListenNetSelChangeBox(EventArgs *pEvtBase)
{
	SComboBox *sCbx = sobj_cast<SComboBox>(pEvtBase->sender);
	if (sCbx->GetCurSel())
		FindChildByName2<SWindow>(L"wnd_step3_ip")->SetVisible(TRUE, TRUE);
	else
		FindChildByName2<SWindow>(L"wnd_step3_ip")->SetVisible(FALSE, TRUE);

	return true;
}

bool CMainDlg::SendCMD(SOCKETOPTION opt, REQUEST_TYPE type, string url, string data)
{
	//�����̷߳�������
	memset(&m_data, 0, sizeof(SocketData));
	m_data.opt = opt;
	m_data.type = type;
	m_data.url = url;
	m_data.data = data;
	LPVOID param = (LPVOID)&m_data;

	return OnStartSocketThread(param);
}

void CMainDlg::UpdateCameraInfo()
{
	int i;
	wchar_t radio[12] = { 0 };
	char port[32] = { 0 };

	FindChildByName2<SEdit>(L"canmera_playurl")->SetWindowTextW(S_CA2T(m_cinfo.cad.url));

	//����ץ�Ŀ���
	FindChildByName2<SComboBox>(L"cbx_switch_face")->SetCurSel(m_cinfo.ffs.face_rec & 1);

	//�Ƿ�ֻץ����
	FindChildByName2<SComboBox>(L"cbx_facial")->SetCurSel(m_cinfo.fp.facial & 1);
	
	//��С�ߴ���
	SEdit *edit_pixel_width = FindChildByName2<SEdit>(L"edit_pixel_width");
	(m_cinfo.fp.width > 0 && m_cinfo.fp.width <0xffff) ? edit_pixel_width->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.width)) :
		edit_pixel_width->SetWindowTextW(SStringT().Format(_T("")));

	//��С�ߴ�߶�
	SEdit *edit_pixel_height = FindChildByName2<SEdit>(L"edit_pixel_height");
	(m_cinfo.fp.height > 0 && m_cinfo.fp.height <0xffff) ? edit_pixel_height->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.height)) :
		edit_pixel_height->SetWindowTextW(SStringT().Format(_T("")));	

	//���Ŷ�
	SEdit *edit_reliability = FindChildByName2<SEdit>(L"edit_reliability");
	(m_cinfo.fp.reliability > 0 && m_cinfo.fp.reliability <0xffff) ? edit_reliability->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.reliability)) :
		edit_reliability->SetWindowTextW(SStringT().Format(_T("")));

	//����ʱ��
	SEdit *edit_residence_time = FindChildByName2<SEdit>(L"edit_residence_time");
	(m_cinfo.fp.res_time > 0 && m_cinfo.fp.res_time <0xffff) ? edit_residence_time->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.res_time)) :
		edit_residence_time->SetWindowTextW(SStringT().Format(_T("")));

	//�ϴ�ͼƬ
	for (i = 0; i < 4; i++)
	{
		wsprintf(radio, L"radio_%d", i+1);
		m_cinfo.fp.face_img == i ? FindChildByName2<SRadioBox>(radio)->SetCheck(true) : FindChildByName2<SRadioBox>(radio)->SetCheck(false);
	}

	//�ϴ�����
	SEdit *eidt_faces_second = FindChildByName2<SEdit>(L"eidt_faces_second");
	(m_cinfo.fp.faces > 0 && m_cinfo.fp.faces <0xff) ? eidt_faces_second->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.faces)) :
		eidt_faces_second->SetWindowTextW(SStringT().Format(_T("")));

	//���滭��
	FindChildByName2<SCheckBox>(L"check_1")->SetCheck(m_cinfo.fp.box & 1);
	FindChildByName2<SCheckBox>(L"check_2")->SetCheck(m_cinfo.fp.box >> 3 & 1);
	FindChildByName2<SCheckBox>(L"check_3")->SetCheck(m_cinfo.fp.box >> 4 & 1);
	FindChildByName2<SCheckBox>(L"check_4")->SetCheck(m_cinfo.fp.box >> 6 & 1);

	//��������
	FindChildByName2<SComboBox>(L"cbx_mark_face")->SetCurSel(m_cinfo.ffs.face_cod & 1);


	//ͼƬ������
	//SWindow *pWndRgn = FindChildByName(L"wnd_rgn");
	//if (pWndRgn)
	//{
	//	CRect rc = pWndRgn->GetWindowRect();
	//	rc.MoveToXY(0, 0);
	//	HRGN hRgn = ::CreateEllipticRgnIndirect(&rc);

	//	CAutoRefPtr<IRegion> pRgn;
	//	GETRENDERFACTORY->CreateRegion(&pRgn);
	//	pRgn->SetRgn(hRgn);
	//	pWndRgn->SetWindowRgn(pRgn, TRUE);

	//	DeleteObject(hRgn);
	//}

	for (i = 0; i < 5; i++)
	{
		wsprintf(radio, L"radio_%d", i + 5);
		m_cinfo.af.comparison == i ? FindChildByName2<SRadioBox>(radio)->SetCheck(true) : FindChildByName2<SRadioBox>(radio)->SetCheck(false);
	}
	//������������ճ�ʼ��
	SEdit *edit_api_key = FindChildByName2<SEdit>(L"edit_api_key");
	edit_api_key->SetWindowTextW(L"");
	SEdit *edit_secret_key = FindChildByName2<SEdit>(L"edit_secret_key");
	edit_secret_key->SetWindowTextW(L"");
	SEdit *edit_bd_libname = FindChildByName2<SEdit>(L"edit_bd_libname");
	edit_bd_libname->SetWindowTextW(L"");
	SEdit *edit_sw_server = FindChildByName2<SEdit>(L"edit_sw_server");
	edit_sw_server->SetWindowTextW(L"");
	SEdit *edit_sw_port = FindChildByName2<SEdit>(L"edit_sw_port");
	edit_sw_port->SetWindowTextW(L"");
	SEdit *edit_sw_libname = FindChildByName2<SEdit>(L"edit_sw_libname");
	edit_sw_libname->SetWindowTextW(L"");
	SComboBox *cbx_sw_lib = FindChildByName2<SComboBox>(L"cbx_sw_lib");
	cbx_sw_lib->SetCurSel(0);
	SEdit *edit_general_server = FindChildByName2<SEdit>(L"edit_general_server");
	edit_general_server->SetWindowTextW(L"");
	SEdit *edit_general_port = FindChildByName2<SEdit>(L"edit_general_port");
	edit_general_port->SetWindowTextW(L"");
	SCheckBox *check_general_https = FindChildByName2<SCheckBox>(L"check_general_https");
	check_general_https->SetCheck(FALSE);
	SEdit *edit_iermu_server = FindChildByName2<SEdit>(L"edit_iermu_server");
	edit_iermu_server->SetWindowTextW(L"");
	SEdit *edit_iermu_port = FindChildByName2<SEdit>(L"edit_iermu_port");
	edit_iermu_port->SetWindowTextW(L"");

	//�ٶȱȶԷ�����
	if (m_cinfo.af.comparison == 0)
	{
		edit_api_key->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.fp.api_key)));
		edit_secret_key->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.fp.secret_key)));
		edit_bd_libname->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.ipp.sw_id)));
	}

	//�����ȶԷ�����
	if (m_cinfo.af.comparison == 1)
	{
		edit_sw_server->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.ipp.cp_domain)));

		(m_cinfo.ipp.cp_port > 0 && m_cinfo.ipp.cp_port < 0xffff) ? edit_sw_port->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.ipp.cp_port)) :
			edit_sw_port->SetWindowTextW(SStringT().Format(_T("80")));

		edit_sw_libname->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.ipp.sw_id)));
		cbx_sw_lib->SetCurSel(m_cinfo.ipp.lib_type & 1);
	}

	//ͼƬ�ϴ�������ͨ�ð�
	if (m_cinfo.af.comparison == 3)
	{
		edit_general_server->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.ipp.domain)));
		(m_cinfo.ipp.port > 0 && m_cinfo.ipp.port <0xffff) ? edit_general_port->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.ipp.port)) :
			edit_general_port->SetWindowTextW(SStringT().Format(_T("80")));

		check_general_https->SetCheck((m_cinfo.ipp.lib_type >> 7) & 1);
	}

	//ͼƬ�ϴ�������Iermu��
	if (m_cinfo.af.comparison == 4)
	{
		edit_iermu_server->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.ipp.domain)));

		(m_cinfo.ipp.port > 0 && m_cinfo.ipp.port < 0xffff) ? edit_iermu_port->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.ipp.port)) :
			edit_iermu_port->SetWindowTextW(SStringT().Format(_T("80")));
	}

	//NAS����
	FindChildByName2<SCheckBox>(L"check_switch_nas")->SetCheck((m_cinfo.np.status & 1) && (!(m_cinfo.rec.rec & 1)));

	SEdit *edit_nas_ip = FindChildByName2<SEdit>(L"edit_nas_ip");
	if (m_cinfo.np.ip[0] == 0 && m_cinfo.np.ip[1] == 0 && m_cinfo.np.ip[2] == 0 && m_cinfo.np.ip[3] == 0)
		edit_nas_ip->SetWindowTextW(SStringT().Format(_T("")));
	else
		edit_nas_ip->SetWindowTextW(SStringT().Format(_T("%d.%d.%d.%d"), m_cinfo.np.ip[0], m_cinfo.np.ip[1], m_cinfo.np.ip[2], m_cinfo.np.ip[3]));
	
	if (strcmp((char*)m_cinfo.np.user, "guest") == 0 && strcmp((char*)m_cinfo.np.passwd, "") == 0)
	{
		FindChildByName2<SEdit>(L"edit_nas_user")->SetWindowTextW(SStringT().Format(_T("")));
		FindChildByName2<SEdit>(L"edit_nas_passwd")->SetWindowTextW(SStringT().Format(_T("")));
		FindChildByName2<SCheckBox>(L"check_nas_anonymous")->SetCheck(true);
	}
	else
	{ 
		FindChildByName2<SEdit>(L"edit_nas_user")->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.np.user)));
		FindChildByName2<SEdit>(L"edit_nas_passwd")->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.np.passwd)));
		FindChildByName2<SCheckBox>(L"check_nas_anonymous")->SetCheck(false);
	}
	FindChildByName2<SEdit>(L"edit_nas_path")->SetWindowTextW(SStringT().Format(_T("%s"), S_CA2T((char *)m_cinfo.np.path, CP_UTF8)));

	SEdit *edit_nas_space = FindChildByName2<SEdit>(L"edit_nas_space");
	(m_cinfo.np.space > 0 && m_cinfo.np.space < 0x790fffffff) ? edit_nas_space->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.np.space)) :
		edit_nas_space->SetWindowTextW(SStringT().Format(_T("")));
	
	//SD��
	FindChildByName2<SCheckBox>(L"check_switch_rec")->SetCheck((!(m_cinfo.np.status & 1)) && (!(m_cinfo.rec.rec & 1)));


	//��������
	FindChildByName2<SComboBox>(L"cbx_type_rec")->SetCurSel((m_cinfo.rec.rec>>7) & 1);
	FindChildByName2<SComboBox>(L"cbx_switch_saveface")->SetCurSel(m_cinfo.af.lan >> 5 & 1);
	SEdit *edit_jpgmem = FindChildByName2<SEdit>(L"edit_jpgmem");
	if (m_cinfo.af.jpgmem > 0 && m_cinfo.af.jpgmem < 0xffff)
	{
		(m_cinfo.af.jpgmem >> 15 > 0) ? edit_jpgmem->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.af.jpgmem & 0x7FFF)) : edit_jpgmem->SetWindowTextW(SStringT().Format(_T("%.03f"), (double)(m_cinfo.af.jpgmem & 0x7FFF) / 1000));
	}
	else
	{
		edit_jpgmem->SetWindowTextW(L"");
	}

	//����
	//��������
	FindChildByName2<SComboBox>(L"cbx_lan")->SetCurSel(m_cinfo.af.lan >> 7 & 1);

	//������
	FindChildByName2<SComboBox>(L"cbx_network")->SetCurSel(m_cinfo.af.lan >> 6 & 1);
}

bool CMainDlg::SetCameraGeneral(void)
{
	int facesval = 0, widthval = 0, heightval = 0, relival = 0, timeval = 0;

	//��С�ߴ���
	SStringT tpixel_width = FindChildByName2<SEdit>(L"edit_pixel_width")->GetWindowTextW();
	if (tpixel_width.GetLength() > 0)
	{
		if (!((swscanf(tpixel_width, L"%d", &widthval) == 1) && (widthval >= 0 && widthval <= 0xffff)))
		{
			MessageBox(NULL, _T("��С�ߴ��������ʽ����ȷ"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	//��С�ߴ�߶�
	SStringT tpixel_height = FindChildByName2<SEdit>(L"edit_pixel_height")->GetWindowTextW();
	if (tpixel_height.GetLength() > 0)
	{
		if (!((swscanf(tpixel_height, L"%d", &heightval) == 1) && (heightval >= 0 && heightval <= 0xffff)))
		{
			MessageBox(NULL, _T("��С�ߴ�߶������ʽ����ȷ"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	//���Ŷ�
	SStringT treliability = FindChildByName2<SEdit>(L"edit_reliability")->GetWindowTextW();
	if (treliability.GetLength() > 0)
	{
		if (!((swscanf(treliability, L"%d", &relival) == 1) && (relival >= 0 && relival <= 0xffff)))
		{
			MessageBox(NULL, _T("���Ŷ������ʽ����ȷ"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	//����ʱ��
	SStringT tresidence_time = FindChildByName2<SEdit>(L"edit_residence_time")->GetWindowTextW();
	if (tresidence_time.GetLength() > 0)
	{
		if (!((swscanf(tresidence_time, L"%d", &timeval) == 1) && (timeval >= 0 && timeval <= 0xffff)))
		{
			MessageBox(NULL, _T("����ʱ�������ʽ����ȷ"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	//�ϴ�����
	SStringT tfaces_second = FindChildByName2<SEdit>(L"eidt_faces_second")->GetWindowTextW();
	if (tfaces_second.GetLength() > 0)
	{
		if (!((swscanf(tfaces_second, L"%d", &facesval) == 1) && (facesval >= 0 && facesval <= 255)))
		{
			MessageBox(NULL, _T("ÿ����������������ʽ����ȷ"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	//����ץ�Ŀ���
	m_cinfo.ffs.face_rec = FindChildByName2<SComboBox>(L"cbx_switch_face")->GetCurSel() & 1;

	//�Ƿ�ֻץ����
	m_cinfo.fp.facial = FindChildByName2<SComboBox>(L"cbx_facial")->GetCurSel() & 1;

	//��С�ߴ���
	m_cinfo.fp.width = widthval;

	//��С�ߴ�߶�
	m_cinfo.fp.height = heightval;

	//���Ŷ�
	m_cinfo.fp.reliability = relival;

	//����ʱ��
	m_cinfo.fp.res_time = timeval;

	//�ϴ�ͼƬ
	m_cinfo.fp.face_img = FindChildByName(L"grp_face_img")->GetSelectedChildInGroup()->GetID();

	//�ϴ�����
	m_cinfo.fp.faces = facesval;

	//���滭��
	SCheckBox *check_1 = FindChildByName2<SCheckBox>(L"check_1");
	SCheckBox *check_2 = FindChildByName2<SCheckBox>(L"check_2");
	SCheckBox *check_3 = FindChildByName2<SCheckBox>(L"check_3");
	SCheckBox *check_4 = FindChildByName2<SCheckBox>(L"check_4");
	m_cinfo.fp.box = (check_1->IsChecked() ? 1 : 0) | (check_2->IsChecked() ? (1 << 3) : 0) |
					(check_3->IsChecked() ? (1 << 4) : 0) | (check_4->IsChecked() ? (1 << 6) : 0);

	//��������
	m_cinfo.ffs.face_cod = FindChildByName2<SComboBox>(L"cbx_mark_face")->GetCurSel() & 1;

	return SendCMD(OPT_SETCAMERA_GENERAL, (REQUEST_TYPE)0, "", "");
}

bool CMainDlg::SetCameraServer()
{
	int cpport = 0, port = 0 ;

	//ͼƬ������
	m_cinfo.af.comparison = FindChildByName(L"grp_face_server")->GetSelectedChildInGroup()->GetID();

	//�ٶȱȶԷ�����
	if (m_cinfo.af.comparison == 0)
	{
		SStringT tapi_key = FindChildByName2<SEdit>(L"edit_api_key")->GetWindowTextW();
		if (tapi_key.GetLength() > 32)
		{
			MessageBox(NULL, _T("API_KEY�����ʽ����ȷ"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}

		SStringT tsecret_key = FindChildByName2<SEdit>(L"edit_secret_key")->GetWindowTextW();
		if (tsecret_key.GetLength() > 32)
		{
			MessageBox(NULL, _T("SECRET_KEY�����ʽ����ȷ"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}

		//�ٶȿ���
		SStringT tbd_libname = FindChildByName2<SEdit>(L"edit_bd_libname")->GetWindowTextW();
		if (tbd_libname.GetLength() > 119)
		{
			MessageBox(NULL, _T("�������ó���119�ַ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
		string api_key = S_CT2A(tapi_key);
		memset(m_cinfo.fp.api_key, 0, sizeof(m_cinfo.fp.api_key));
		if (api_key.length() > 0)
			memcpy(m_cinfo.fp.api_key, api_key.c_str(), api_key.length());

		string secret_key = S_CT2A(tsecret_key);
		memset(m_cinfo.fp.secret_key, 0, sizeof(m_cinfo.fp.secret_key));
		if (secret_key.length() > 0)
			memcpy(m_cinfo.fp.secret_key, secret_key.c_str(), secret_key.length());

		string bd_libname = S_CT2A(tbd_libname);
		memset(m_cinfo.ipp.sw_id, 0, sizeof(m_cinfo.ipp.sw_id));
		if (bd_libname.length() > 0)
			memcpy(m_cinfo.ipp.sw_id, bd_libname.c_str(), bd_libname.length());
	}

	//�����ȶԷ�����
	if (m_cinfo.af.comparison == 1)
	{ 
		SStringT tsw_server = FindChildByName2<SEdit>(L"edit_sw_server")->GetWindowTextW();
		if (tsw_server.GetLength() > 60)
		{
			MessageBox(NULL, _T("�ȶ�������IP���Ȳ��ó���60�ַ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	
		SStringT sw_port = FindChildByName2<SEdit>(L"edit_sw_port")->GetWindowTextW();
		if (sw_port.GetLength() > 0)
		{
			if (!((swscanf(sw_port, L"%d", &cpport) == 1) && (cpport >= 0 && cpport <= 65535)))
			{
				MessageBox(NULL, _T("�ȶԶ˿ں�ֵ����ȷ����������ȷ�Ķ˿ں�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
		}

		//��������
		SStringT tsw_libname = FindChildByName2<SEdit>(L"edit_sw_libname")->GetWindowTextW();
		if (tsw_libname.GetLength() > 119)
		{
			MessageBox(NULL, _T("�������ó���119�ַ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}

		string sw_server = S_CT2A(tsw_server);
		memset(m_cinfo.ipp.cp_domain, 0, sizeof(m_cinfo.ipp.cp_domain));
		if (sw_server.length() > 0)
			memcpy(m_cinfo.ipp.cp_domain, sw_server.c_str(), sw_server.length());

		m_cinfo.ipp.cp_port = cpport;

		string sw_libname = S_CT2A(tsw_libname);
		memset(m_cinfo.ipp.sw_id, 0, sizeof(m_cinfo.ipp.sw_id));
		if (sw_libname.length() > 0)
			memcpy(m_cinfo.ipp.sw_id, sw_libname.c_str(), sw_libname.length());

		FindChildByName2<SComboBox>(L"cbx_sw_lib")->GetCurSel() ? (m_cinfo.ipp.lib_type |= 1) : (m_cinfo.ipp.lib_type &= 0);
	}


	//ͼƬ�ϴ�������ͨ�ð�
	if (m_cinfo.af.comparison == 3)
	{
		SStringT tgeneral_server = FindChildByName2<SEdit>(L"edit_general_server")->GetWindowTextW();
		if (tgeneral_server.GetLength() > 60)
		{
			MessageBox(NULL, _T("�ϴ�������IP���Ȳ��ó���60�ַ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}

		SStringT tgeneral_port = FindChildByName2<SEdit>(L"edit_general_port")->GetWindowTextW();
		if (tgeneral_port.GetLength() > 0)
		{
			if (!((swscanf(tgeneral_port, L"%d", &port) == 1) && (port >= 0 && port <= 65535)))
			{
				MessageBox(NULL, _T("�ϴ��˿ں�ֵ����ȷ����������ȷ�Ķ˿ں�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
		}

		string general_server = S_CT2A(tgeneral_server);
		memset(m_cinfo.ipp.domain, 0, sizeof(m_cinfo.ipp.domain));
		if (general_server.length() > 0)
			memcpy(m_cinfo.ipp.domain, general_server.c_str(), general_server.length());

		m_cinfo.ipp.port = port;

		SCheckBox *check_general_https = FindChildByName2<SCheckBox>(L"check_general_https");
		check_general_https->IsChecked() ? (m_cinfo.ipp.lib_type |= (1 << 7)) : (m_cinfo.ipp.lib_type &= 0);
	}

	//ͼƬ�ϴ�������Iermu��
	if (m_cinfo.af.comparison == 4)
	{
		SStringT tiermu_server = FindChildByName2<SEdit>(L"edit_iermu_server")->GetWindowTextW();
		if (tiermu_server.GetLength() > 60)
		{
			MessageBox(NULL, _T("�ϴ�������IP���Ȳ��ó���60�ַ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}

		SStringT tiermu_port = FindChildByName2<SEdit>(L"edit_iermu_port")->GetWindowTextW();
		if (tiermu_port.GetLength() > 0)
		{
			if (!((swscanf(tiermu_port, L"%d", &port) == 1) && (port >= 0 && port <= 65535)))
			{
				MessageBox(NULL, _T("�ϴ��˿ں�ֵ����ȷ����������ȷ�Ķ˿ں�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
		}

		string iermu_server = S_CT2A(tiermu_server);
		memset(m_cinfo.ipp.domain, 0, sizeof(m_cinfo.ipp.domain));
		if (iermu_server.length() > 0)
			memcpy(m_cinfo.ipp.domain, iermu_server.c_str(), iermu_server.length());
		m_cinfo.ipp.port = port;
	}

	return SendCMD(OPT_SETCAMERA_SERVER, (REQUEST_TYPE)0, "", "");
}

bool CMainDlg::SetCameraStore()
{
	int a = 0, b = 0, c = 0, d = 0, space = 0;
	unsigned int jpgmem = 2, decimal = 0;
	
	//NAS IP
	SStringT tnas_ip = FindChildByName2<SEdit>(L"edit_nas_ip")->GetWindowTextW();
	if (tnas_ip.GetLength() > 0)
	{
		if (!((swscanf(tnas_ip, L"%d.%d.%d.%d", &a, &b, &c, &d) == 4) && (a >= 0 && a <= 255)
			&& (b >= 0 && b <= 255)
			&& (c >= 0 && c <= 255)
			&& (d >= 0 && d <= 255)))
		{
			MessageBox(NULL, _T("ip��ʽ����ȷ����������ȷ��ipv4"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	}
	//NAS�û���
	SStringT tnas_user = FindChildByName2<SEdit>(L"edit_nas_user")->GetWindowTextW();
	if (tnas_user.GetLength() > 30)
	{
		MessageBox(NULL, _T("NAS�û������Ȳ��ó���30�ַ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return false;
	}
	//NAS����
	SStringT tnas_passwd = FindChildByName2<SEdit>(L"edit_nas_passwd")->GetWindowTextW();
	if (tnas_passwd.GetLength() > 30)
	{
		MessageBox(NULL, _T("NAS���볤�Ȳ��ó���30�ַ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return false;
	}
	//NAS·��
	SStringT tnas_path = FindChildByName2<SEdit>(L"edit_nas_path")->GetWindowTextW();
	if (tnas_path.GetLength() > NASPATH_LEN)
	{
		MessageBox(NULL, _T("NAS·�����Ȳ��ó���50�ַ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return false;
	}
	//NAS�洢��С
	SStringT tnas_space = FindChildByName2<SEdit>(L"edit_nas_space")->GetWindowTextW();
	if (tnas_space.GetLength() > 0)
	{
		if (!((swscanf(tnas_space, L"%d", &space) == 1) && (space >= 0 && space <= 0x7fffffff)))
		{
			MessageBox(NULL, _T("��������ȷ��NAS����ռ��С"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	}
	//����ͼƬ�洢��С
	SStringT tjpgmem = FindChildByName2<SEdit>(L"edit_jpgmem")->GetWindowTextW();
	if (tjpgmem.GetLength() > 0)
	{
		if (!((swscanf(tjpgmem, L"%d.%d", &jpgmem, &decimal) == 1 || swscanf(tjpgmem, L"%d.%d", &jpgmem, &decimal) == 2) && (jpgmem >= 0 && jpgmem <= 0xffff)))
		{
			MessageBox(NULL, _T("����ͼƬ�洢��С��ʽ����ȷ"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	string sjpgmem = S_CT2A(tjpgmem);
	double djpgmem = atof(sjpgmem.c_str());
	if (djpgmem > -0.0000001 && djpgmem < 0.000001)
		jpgmem = 2;
	else if (djpgmem >= 1)
		jpgmem = (unsigned int)djpgmem;
	else
		jpgmem = 0;

	//NAS ����
	m_cinfo.np.status = FindChildByName2<SCheckBox>(L"check_switch_nas")->IsChecked() & 1;
	m_cinfo.np.status = m_cinfo.np.status | (1 << 7); //SAMBA

	if (jpgmem >= space && m_cinfo.np.status & 1)
	{
		MessageBox(NULL, _T("NAS����ռ��С�������2G���Ҵ�������ͼƬ�洢��С"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return false;
	}

	//NAS IP
	m_cinfo.np.ip[0] = a;
	m_cinfo.np.ip[1] = b;
	m_cinfo.np.ip[2] = c;
	m_cinfo.np.ip[3] = d;

	//NAS����
	BOOL anonymous = FindChildByName2<SCheckBox>(L"check_nas_anonymous")->IsChecked();

	//NAS�û���
	string nas_user = S_CT2A(tnas_user);
	memset(m_cinfo.np.user, 0, sizeof(m_cinfo.np.user));
	if (anonymous)
		strcpy((char *)m_cinfo.np.user, "guest");
	else
	{
		if (nas_user.length() > 0)
			memcpy(m_cinfo.np.user, nas_user.c_str(), nas_user.length());
	}

	//NAS����
	string nas_passwd = S_CT2A(tnas_passwd);
	memset(m_cinfo.np.passwd, 0, sizeof(m_cinfo.np.passwd));
	if (nas_passwd.length() > 0 && !anonymous)
		memcpy(m_cinfo.np.passwd, nas_passwd.c_str(), nas_passwd.length());

	//NAS·��
	string nas_path = S_CT2A(tnas_path, CP_UTF8);
	memset(m_cinfo.np.path, 0, sizeof(m_cinfo.np.path));
	if (nas_path.length() > 0)
		memcpy(m_cinfo.np.path, nas_path.c_str(), nas_path.length());

	//NAS�洢��С
	m_cinfo.np.space = space;


	//SD��
	UINT8 rec = FindChildByName2<SCheckBox>(L"check_switch_rec")->IsChecked();


	//��������
	UINT8 rectype = FindChildByName2<SComboBox>(L"cbx_type_rec")->GetCurSel();
	m_cinfo.rec.rec = (!(m_cinfo.np.status | rec)) | (rectype << 7);

	//����ͼƬ���ش���
	UINT8 cbx_switch_saveface = FindChildByName2<SComboBox>(L"cbx_switch_saveface")->GetCurSel() & 1;
	cbx_switch_saveface ? (m_cinfo.af.lan |= (cbx_switch_saveface << 5)) : (m_cinfo.af.lan &= (cbx_switch_saveface << 5));

	//����ͼƬ�洢��С
	if (jpgmem >= 1)
		m_cinfo.af.jpgmem = jpgmem | (1 << 15);
	else
		m_cinfo.af.jpgmem = (int)(djpgmem * 1000);


	return SendCMD(OPT_SETCAMERA_STORE, (REQUEST_TYPE)0, "", "");
}

bool CMainDlg::SetCameraOther()
{
	//��������
	UINT8 cbx_lan = FindChildByName2<SComboBox>(L"cbx_lan")->GetCurSel() & 1;
	//������
	UINT8 cbx_network = FindChildByName2<SComboBox>(L"cbx_network")->GetCurSel() & 1;

	cbx_lan ? (m_cinfo.af.lan |= (cbx_lan << 7)) : (m_cinfo.af.lan &= (cbx_lan << 7));
	cbx_network ? (m_cinfo.af.lan |= (cbx_network << 6)) : (m_cinfo.af.lan &= (cbx_network << 6));

	return SendCMD(OPT_SETCAMERA_OTHER, (REQUEST_TYPE)0, "", "");
}


bool CMainDlg::CheckIp(int type, LPCWSTR pszName, LPCWSTR tipName, string &ip)
{
	int a, b, c, d;
	SStringT cameraIp;
	if (type)
	{
		SComboView *pComboView = FindChildByName2<SComboView>(L"cbv_account");
		if (!pComboView)  return false;
		cameraIp = pComboView->GetWindowTextW();
	}		
	else
	{
		SEdit *edit_cameraIp = FindChildByName2<SEdit>(pszName);
		if (!edit_cameraIp)  return false;
		cameraIp = edit_cameraIp->GetWindowTextW();

	}
	if (!cameraIp.GetLength() && tipName != NULL)
	{
		MessageBox(NULL, tipName, _T("��ʾ"), MB_OK | MB_ICONERROR);
		return false;
	}
	if (!((swscanf(cameraIp, L"%d.%d.%d.%d", &a, &b, &c, &d) == 4) && (a >= 0 && a <= 255)
		&& (b >= 0 && b <= 255)
		&& (c >= 0 && c <= 255)
		&& (d >= 0 && d <= 255)))
	{
		MessageBox(NULL, _T("ip��ʽ����ȷ����������ȷ��ipv4"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return false;
	}
	ip = S_CT2A(cameraIp);

	return true;
}

void CMainDlg::SetCameraIP(char *ip)
{
	m_cameraIp = ip;
}

bool CMainDlg::SetCameraTime()
{
	return SendCMD(OPT_SETTIME_SYNC, (REQUEST_TYPE)0, "", "");
}

// Dpflag  flase�� ����  true: ��ʾ  
void CMainDlg::SetDisplayProgress(const wchar_t * showName, const wchar_t * hideName)
{
	SWindow *WinShow = FindChildByName(showName);
	SWindow *WinHide = FindChildByName(hideName);
	if (WinShow)
	{
		WinShow->SetVisible(TRUE, TRUE);
	}

	if (WinHide)
	{
		WinHide->SetVisible(FALSE, TRUE);
	}
}

void CMainDlg::RefreshCameraList()
{
	SetDisplayProgress(L"refresh_progress", L"cameralist_win");
	GetCameraList();
}

void CMainDlg::OnOpenPlay(int voiceType)
{
	m_dlgPlayer.Create(NULL);
	m_dlgPlayer.SetPlayUrl(m_cinfo.cad.url, voiceType);
	m_dlgPlayer.SendMessage(WM_INITDIALOG);
	m_dlgPlayer.CenterWindow(m_dlgPlayer.m_hWnd);
	m_dlgPlayer.ShowWindow(SW_NORMAL);
}

void CMainDlg::OnPlay()
{
	if (strlen(m_cinfo.cad.url) > 0 && m_cinfo.cad.url != NULL)
	{
		OnOpenPlay(0);
		//m_player.ffplayWithUrl(m_cinfo.cad.url);
	}	
	else
	{
		MessageBox(NULL, _T("��ȡ����������Ӳ��ɹ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
	}
}

void CMainDlg::OnPlayNoVoice()
{
	if (strlen(m_cinfo.cad.url) > 0 && m_cinfo.cad.url != NULL)
	{
		OnOpenPlay(1);
		//m_player.ffplayWithUrlNoVoice(m_cinfo.cad.url);
	}
	else
	{
		MessageBox(NULL, _T("��ȡ����������Ӳ��ɹ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
	}
}

void CMainDlg::GetCameraList()
{
	if (CheckIp(1, L"edit_localIp", _T("�����뱾��IP"), m_localIp))
	{
		if (SendCMD(OPT_GETCAMERA_LIST, (REQUEST_TYPE)0, "", ""))
			SetDisplayProgress(L"local_ip_progress", L"local_ip_win");
	}
}

bool CMainDlg::GetNASError(WinSocketClient &client, SStringT &code)
{
	int timeout = 0;
	NASParameter nasp = { 0 };
	bool ret = false;
	code = L"";

	while (timeout<30000)
	{
		if (!(ret = client.GetNAS(m_cameraIp.c_str(), nasp)))
		{
			code = L"�������";
			break;
		}
			
		if ((nasp.status >> 3) & 1)
		{
			timeout += 200;
			Sleep(200);
		}
		else
		{
			if ((nasp.status >> 4) & 1)
			{
				code = L"���óɹ�";
				break;
			}
			else
			{
				ret = false;
				switch (nasp.user[31])
				{
					case 0:
						ret = true;
						break;
					case 1:
						code = L"�ļ�ϵͳ���󣬲��ܻ�ȡ�ļ�ϵͳ��С!";
						break;
					case 2:
						code = L"NAS�ռ䲻��!";
						break;
					case 3:
						code = L"NAS���ÿռ����̫С!";
						break;
					case 4:
						code = L"�޷�����NAS������!";
						break;
					case 5:
						code = L"����Ŀ¼ʧ��!";
						break;
					case 6:
						code = L"����ʧ�ܻ��û������벻��ȷ!";
						break;
					case 12:
						code = L"����ʧ�ܣ�������æ!";
						break;
					default:
						code = L"δ֪����";
						break;
				}
				break;
			}
		}
	}

	return ret;
}

void CMainDlg::ConfigureNative()
{
	size_t i;
	int dhcp = 0;
	HKEY hk;
	long ret;
	wchar_t keyname[200] = { 0 }, buf[256] = { 0 };
	char cmd[256] = { 0 };
	SStringT name = L"";
	string alias;
	DWORD size;
	DWORD m_attr = REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_MULTI_SZ | REG_NONE | REG_SZ;

	GetLocalIPInfo(m_userInfo);
	for (i = 0; i < m_userInfo.GetCount(); i++)
	{
		if (!m_userInfo.GetAt(i).dhcpEnabled)
		{
			name = m_userInfo.GetAt(i).strAdapterName;
			break;
		}
	}
	if (!name.GetLength())
	{
		dhcp = 1;
		i = 0;
	}
	name = m_userInfo.GetAt(i).strAdapterName;

	SStringT tkeyname = SStringT().Format(_T("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection"), name);
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, tkeyname, NULL, KEY_ALL_ACCESS, &hk))
	{
		ret = RegQueryValueEx(hk, L"Name", NULL, &m_attr, (BYTE*)buf, &size);
	}
	RegCloseKey(hk);

	if (!ret)
		alias = S_CW2A((wchar_t *)buf);
	if (dhcp)
	{
		string ip = S_CT2A(m_userInfo.GetAt(i).strIp);
		sprintf(cmd, "netsh interface ip set address name=\"%s\" source=static address=%s mask=%s gateway=%s gwmetric=0", alias.c_str(), ip.c_str(), m_userInfo.GetAt(i).strMask.c_str(), m_userInfo.GetAt(i).strGateway.c_str());
		WinExec(cmd, SW_HIDE);
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "netsh interface ip set dns name=\"%s\" source = static address=%s register=PRIMARY", alias.c_str(), m_userInfo.GetAt(i).strDns.c_str());
		WinExec(cmd, SW_HIDE);
		memset(cmd, 0, sizeof(cmd));
	}

	sprintf(cmd, "netsh interface ip add address name=\"%s\" address=192.168.1.123 mask=255.255.255.0", alias.c_str());
	WinExec(cmd, SW_HIDE);
	//memset(cmd, 0, sizeof(cmd));
	//sprintf(cmd, "netsh interface ip add dns name=\"%s\" address=192.168.1.1", alias.c_str());
	//WinExec(cmd, SW_HIDE);
}

void CMainDlg::OnStepChange(int pre, int back, SStringT tip)
{
	switch (pre)
	{
	case 0:
		FindChildByName2<SWindow>(L"step0_operation")->SetVisible(FALSE, TRUE);
		break;
	case 1:
		FindChildByName2<SWindow>(L"step1_operation")->SetVisible(FALSE, TRUE);
		FindChildByName2<SWindow>(L"current_step1")->SetVisible(FALSE, TRUE);
		FindChildByName2<SWindow>(L"step1")->SetVisible(TRUE, TRUE);
		break;
	case 2:
		FindChildByName2<SWindow>(L"step2_operation")->SetVisible(FALSE, TRUE);
		FindChildByName2<SWindow>(L"current_step2")->SetVisible(FALSE, TRUE);
		FindChildByName2<SWindow>(L"step2")->SetVisible(TRUE, TRUE);
		break;
	case 3:
		FindChildByName2<SWindow>(L"step3_operation")->SetVisible(FALSE, TRUE);
		FindChildByName2<SWindow>(L"current_step3")->SetVisible(FALSE, TRUE);
		FindChildByName2<SWindow>(L"step3")->SetVisible(TRUE, TRUE);
		break;
	case 4:
		FindChildByName2<SWindow>(L"step4_operation")->SetVisible(FALSE, TRUE);
		FindChildByName2<SWindow>(L"current_step4")->SetVisible(FALSE, TRUE);
		FindChildByName2<SWindow>(L"step4")->SetVisible(TRUE, TRUE);
		break;
		
	}

	switch (back)
	{
	case 0:
		FindChildByName2<SWindow>(L"txt_tip")->SetWindowTextW(tip);
		FindChildByName2<SWindow>(L"step0_operation")->SetVisible(TRUE, TRUE);
		SetDisplayBackBtn(2, FALSE, STEP_ZERO);
		break;
	case 1:
		FindChildByName2<SWindow>(L"step1_operation")->SetVisible(TRUE, TRUE);
		FindChildByName2<SWindow>(L"current_step1")->SetVisible(TRUE, TRUE);
		FindChildByName2<SWindow>(L"step1")->SetVisible(FALSE, TRUE);
		SetDisplayBackBtn(2, FALSE, STEP_ONE);
		break;
	case 2:
		FindChildByName2<SWindow>(L"step2_operation")->SetVisible(TRUE, TRUE);
		FindChildByName2<SWindow>(L"current_step2")->SetVisible(TRUE, TRUE);
		FindChildByName2<SWindow>(L"step2")->SetVisible(FALSE, TRUE);
		SetDisplayBackBtn(2, TRUE, STEP_TWO);
		break;
	case 3:
		FindChildByName2<SWindow>(L"step3_operation")->SetVisible(TRUE, TRUE);
		FindChildByName2<SWindow>(L"current_step3")->SetVisible(TRUE, TRUE);
		FindChildByName2<SWindow>(L"step3")->SetVisible(FALSE, TRUE);
		SetDisplayBackBtn(2, TRUE, STEP_THREE);
		break;
	case 4:
		FindChildByName2<SWindow>(L"step4_operation")->SetVisible(TRUE, TRUE);
		FindChildByName2<SWindow>(L"current_step4")->SetVisible(TRUE, TRUE);
		FindChildByName2<SWindow>(L"step4")->SetVisible(FALSE, TRUE);
		SetDisplayBackBtn(2, TRUE, STEP_FOUR);
		break;
	}
}

void CMainDlg::OnStepOne()
{
	SStringT  usrName = FindChildByName2<SEdit>(L"edit_step1_username")->GetWindowTextW();
	SStringT  usrpasswd = FindChildByName2<SEdit>(L"edit_step1_passwd")->GetWindowTextW();

	if (!usrName.GetLength())
	{
		MessageBox(NULL, _T("�������˺�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return;
	}
	if (!usrpasswd.GetLength())
	{
		MessageBox(NULL, _T("����������"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return;
	}

	string usr = S_CT2A(usrName);
	string passwd = S_CT2A(usrpasswd);
	string data = "client_id=j095DeA6kUL9oqZrcdmm&grant_type=password&lang=zh-Hans";
	data.append("&mobile=").append(usr).append("&password=").append(passwd);

	//�����̷߳�������
	if (SendCMD(OPT_LOGIN, POST, AUTHORIZATION_URL, data))
		OnStepChange(1, 0, L"��¼��");
}

void CMainDlg::OnCancelTwo()
{
	SStringT  device = FindChildByName2<SEdit>(L"edit_step2_device")->GetWindowTextW();
	if (!device.GetLength())
	{
		MessageBox(NULL, _T("�������豸��"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return;
	}
	m_deviceId = S_CT2A(device);
	string data = "deviceid=";
	data.append(m_deviceId).append("&method=drop&access_token=").append(m_token);
	//�����̷߳�������
	if (SendCMD(OPT_CANCELLATION, POST, GETDEVICEINFO_URL, data))
		OnStepChange(2, 0, L"����ע�������");
}

void CMainDlg::OnStepTwo()
{
	SStringT  device = FindChildByName2<SEdit>(L"edit_step2_device")->GetWindowTextW();
	if (!device.GetLength())
	{
		MessageBox(NULL, _T("�������豸��"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return;
	}
	int sel = FindChildByName2<SComboBox>(L"cbx_step2_cloud")->GetCurSel();
	int connect_type = sel ? 1:2;
	m_deviceId = S_CT2A(device);
	string data = "deviceid=";
	data.append(m_deviceId).append("&device_type=1&desc=%e6%88%91%e7%9a%84%e6%91%84%e5%83%8f%e6%9c%ba&connect_type=").append(to_string(connect_type)).append("&method=register&access_token=").append(m_token);

	//�����̷߳�������
	if(SendCMD(OPT_REGISTRE, POST, GETDEVICEINFO_URL, data))
		OnStepChange(2, 0, L"����ע�������");
}

void CMainDlg::OnJumpTwo()
{
	SStringT  device = FindChildByName2<SEdit>(L"edit_step2_device")->GetWindowTextW();
	if (!device.GetLength())
	{
		MessageBox(NULL, _T("�������豸��"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return;
	}
	m_deviceId = S_CT2A(device);
	OnStepChange(2, 3, L"");
}

void CMainDlg::OnStepThree()
{
	int a=0, b=0, c=0, d=0;
	int e=0, f=0, g=0, h=0;
	int i=0, j=0, k=0, l=0;
	int m=0, n=0, p=0, q=0;
	if (FindChildByName2<SComboBox>(L"cbx_step3_net")->GetCurSel())
	{
		SStringT tstep3_ip = FindChildByName2<SEdit>(L"edit_step3_ip")->GetWindowTextW();
		if (tstep3_ip.GetLength() > 0)
		{
			if (!((swscanf(tstep3_ip, L"%d.%d.%d.%d", &a, &b, &c, &d) == 4) && (a >= 0 && a <= 255)
				&& (b >= 0 && b <= 255)
				&& (c >= 0 && c <= 255)
				&& (d >= 0 && d <= 255)))
			{
				MessageBox(NULL, _T("ip��ʽ����ȷ����������ȷ��ipv4"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return;
			}
		}
		else
		{
			MessageBox(NULL, _T("��������Ҫ���õ������IP"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return;
		}

		SStringT tstep3_mask = FindChildByName2<SEdit>(L"edit_step3_mask")->GetWindowTextW();
		if (tstep3_mask.GetLength() > 0)
		{
			if (!((swscanf(tstep3_mask, L"%d.%d.%d.%d", &e, &f, &g, &h) == 4) && (e >= 0 && e <= 255)
				&& (f >= 0 && f <= 255)
				&& (g >= 0 && g <= 255)
				&& (h >= 0 && h <= 255)))
			{
				MessageBox(NULL, _T("���������ʽ����ȷ����������ȷ����������"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return;
			}
		}
		else
		{
			MessageBox(NULL, _T("��������Ҫ���õ��������������"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return;
		}

		SStringT tstep3_gateway = FindChildByName2<SEdit>(L"edit_step3_gateway")->GetWindowTextW();
		if (tstep3_gateway.GetLength() > 0)
		{
			if (!((swscanf(tstep3_gateway, L"%d.%d.%d.%d", &i, &j, &k, &l) == 4) && (i >= 0 && i <= 255)
				&& (j >= 0 && j <= 255)
				&& (k >= 0 && k <= 255)
				&& (l >= 0 && l <= 255)))
			{
				MessageBox(NULL, _T("���ظ�ʽ����ȷ����������ȷ������"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return;
			}
		}
		else
		{
			MessageBox(NULL, _T("��������Ҫ���õ����������"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return;
		}

		SStringT tstep3_dns = FindChildByName2<SEdit>(L"edit_step3_dns")->GetWindowTextW();
		if (tstep3_dns.GetLength() > 0)
		{
			if (!((swscanf(tstep3_dns, L"%d.%d.%d.%d", &m, &n, &p, &q) == 4) && (m >= 0 && m <= 255)
				&& (n >= 0 && n <= 255)
				&& (p >= 0 && p <= 255)
				&& (q >= 0 && q <= 255)))
			{
				MessageBox(NULL, _T("dns��ʽ����ȷ����������ȷ��dns"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return;
			}
		}
		else
		{
			MessageBox(NULL, _T("��������Ҫ���õ������dns"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			return;
		}

		memset(&m_cameraNet, 0, sizeof(CameraNet));

		m_cameraNet.hostIP[0] = a;
		m_cameraNet.hostIP[1] = b;
		m_cameraNet.hostIP[2] = c;
		m_cameraNet.hostIP[3] = d;

		m_cameraNet.ipmask[0] = e;
		m_cameraNet.ipmask[1] = f;
		m_cameraNet.ipmask[2] = g;
		m_cameraNet.ipmask[3] = h;

		m_cameraNet.gateway[0] = i;
		m_cameraNet.gateway[1] = j;
		m_cameraNet.gateway[2] = k;
		m_cameraNet.gateway[3] = l;

		m_cameraNet.dnsip1[0] = m;
		m_cameraNet.dnsip1[1] = n;
		m_cameraNet.dnsip1[2] = p;
		m_cameraNet.dnsip1[3] = q;

		m_cameraNet.dnsip2[0] = 8;
		m_cameraNet.dnsip2[1] = 8;
		m_cameraNet.dnsip2[2] = 8;
		m_cameraNet.dnsip2[3] = 8;

		ConfigureNative();
		m_cameraNet.dhcp = 0;
		
	}
	else
	{
		m_cameraNet.dhcp = 1;
	}

	OnStepChange(3, 0, L"�ȴ����������");
	string data = "deviceid=";
	data.append(m_deviceId).append("&method=meta&access_token=").append(m_token);
	SendCMD(OPT_GETONLINE, POST, GETDEVICEINFO_URL, data);
}

void CMainDlg::OnStepFour()
{
	OnStepChange(4, 2, L"");
}

UINT CMainDlg::Run(LPVOID data)
{
	MainSocketThread *pEvt = new MainSocketThread(this);
	SocketData *param = (SocketData *)data;
	int timeout = 0;

	pEvt->opt = param->opt;
	switch (pEvt->opt)
	{
		case OPT_GETCAMERA_LIST:
		{
			char ip[30] = { 0 };
			char url[128] = { 0 };
			UpnpTool Supnp;
			WinSocketClient client;
			AICameraInfo AIinfo;
			m_cinfolist.RemoveAll();

			pEvt->retOK = Supnp.upnpDiscover(3000, m_localIp, m_iplist);
			if (!pEvt->retOK)
				break;

			for (unsigned int i = 0; i < m_iplist.GetCount(); i++)
			{
				memset(&AIinfo, 0, sizeof(AICameraInfo));

				memcpy(&AIinfo.cad, &m_iplist.GetAt(i), sizeof(CameraAddr));
				client.GetFaceParameter(m_iplist.GetAt(i).cameraip, AIinfo.fp);
				client.GetHostIPAddr(m_iplist.GetAt(i).cameraip, AIinfo.ipp);
				client.GetAIFunction(m_iplist.GetAt(i).cameraip, AIinfo.af);
				client.GetFaceFuncSetting(m_iplist.GetAt(i).cameraip, AIinfo.ffs);
				client.GetNAS(m_iplist.GetAt(i).cameraip, AIinfo.np);
				client.GetRec(m_iplist.GetAt(i).cameraip, AIinfo.rec);
				m_cinfolist.Add(AIinfo);
			}
			break;
		}
		case OPT_GETCAMERA_INFO:
		{
			string id;
			WinSocketClient client;			

			memset(&m_cinfo, 0, sizeof(AICameraInfo));
			if (!(pEvt->retOK = client.GetDeviceID(m_cameraIp.c_str(), m_cinfo.cad)))
				break;
			if (!(pEvt->retOK = client.GetFaceParameter(m_cameraIp.c_str(), m_cinfo.fp)))
				break;
			if (!(pEvt->retOK = client.GetHostIPAddr(m_cameraIp.c_str(), m_cinfo.ipp)))
				break;
			if (!(pEvt->retOK = client.GetAIFunction(m_cameraIp.c_str(), m_cinfo.af)))
				break;
			if (!(pEvt->retOK = client.GetFaceFuncSetting(m_cameraIp.c_str(), m_cinfo.ffs)))
				break;
			if (!(pEvt->retOK = client.GetNAS(m_cameraIp.c_str(), m_cinfo.np)))
				break;
			pEvt->retOK = client.GetRec(m_cameraIp.c_str(), m_cinfo.rec);
			break;
		}

		case OPT_SETCAMERA_GENERAL:
		{
			WinSocketClient client;
			if (!(pEvt->retOK = client.SetFaceParameter(m_cameraIp.c_str(), m_cinfo.fp, false)))
				break;
			if (!(pEvt->retOK = client.SetFaceFuncSetting(m_cameraIp.c_str(), m_cinfo.ffs, true)))
				break;
			break;
		}

		case OPT_SETCAMERA_SERVER:
		{
			WinSocketClient client;
			if (!(pEvt->retOK = client.SetAIFunction(m_cameraIp.c_str(), m_cinfo.af, false)))
				break;
			if (!(pEvt->retOK = client.SetFaceParameter(m_cameraIp.c_str(), m_cinfo.fp, false)))
				break;
			if (!(pEvt->retOK = client.SetHostIPAddr(m_cameraIp.c_str(), m_cinfo.ipp, true)))
				break;
			break;
		}

		case OPT_SETCAMERA_STORE:
		{
			WinSocketClient client;
			bool ret;

			if (!(pEvt->retOK = client.SetAIFunction(m_cameraIp.c_str(), m_cinfo.af, false)))
				break;
			if (!(pEvt->retOK = client.SetRec(m_cameraIp.c_str(), m_cinfo.rec, true)))
				break;

			//����NAS
			ret = client.SetNAS(m_cameraIp.c_str(), m_cinfo.np, false);

			if (m_cinfo.np.status & 1)
			{
				if (!ret)
				{
					pEvt->retOK = ret;
					break;
				}
				if (!(pEvt->retOK = GetNASError(client, pEvt->nData)))
					break;

				//���������
				if (!(pEvt->retOK = client.RestartDevice(m_cameraIp.c_str())))
					pEvt->nData = L"���������ʧ��!";
				pEvt->nData = L"�������������...";

			}
			break;
		}

		case OPT_SETCAMERA_OTHER:
		{
			WinSocketClient client;
			pEvt->retOK = client.SetAIFunction(m_cameraIp.c_str(), m_cinfo.af, true);
			break;
		}

		case OPT_FORMAT_SDCARD:
		{
			WinSocketClient client;
			pEvt->progress = 0;
			if (!(pEvt->retOK = client.SetFormateSDCard(m_cameraIp.c_str())))
				break;
				
			while ((pEvt->retOK = client.GetFormateSDCard(m_cameraIp.c_str(), pEvt->progress)) && pEvt->progress < 1000)
			{
				SNotifyCenter::getSingleton().FireEventAsync(pEvt);
			}
			break;
		}

		case OPT_SETTIME_SYNC:
		{
			WinSocketClient client;
			DateTime dt;
			GetTime(dt);
			pEvt->retOK = client.SetCameraTime(m_cinfo.cad.cameraip, dt, true);
			break;
		}

		case OPT_LOGIN:
		{
			CHttpConnect WinClient;

			pEvt->hData = WinClient.Request(param->url, param->type, param->data);
			pEvt->retOK = WinClient.GetStatusIsOK();
			break;
		}

		case OPT_REGISTRE:
		case OPT_CANCELLATION:
		{
			CHttpConnect WinClient;

			pEvt->hData = WinClient.Request(param->url, param->type, param->data);
			pEvt->retOK = WinClient.GetStatusIsOK();
			break;
		}
		case OPT_GETONLINE:
		{
			int timeout = 0;
			Json::Reader reader;
			Json::Value jsonobj;
			WinSocketClient client;
			CHttpConnect WinClient;
			if (!m_cameraNet.dhcp)
			{
				CameraNet net;
				if (!(pEvt->retOK = client.GetCameraNET(CAMERA_DEFAULTIP, net)))
					break;

				m_cameraNet.cmdPort = net.cmdPort;
				m_cameraNet.dataPort = net.dataPort;
				m_cameraNet.httpPort = net.httpPort;
				m_cameraNet.talkPort = net.talkPort;
				m_cameraNet.autoConnect = net.autoConnect;
				m_cameraNet.reserved = net.reserved;
				m_cameraNet.centerIp0 = net.centerIp0;
				m_cameraNet.centerIp1 = net.centerIp1;
				if (!(pEvt->retOK = client.SetCameraNET(CAMERA_DEFAULTIP, m_cameraNet, true)))
					break;
			}

			pEvt->waitOK = false;
			while (timeout < 30000 && pEvt->waitOK==false)
			{
				pEvt->hData = WinClient.Request(param->url, param->type, param->data);
				if (!reader.parse(pEvt->hData, jsonobj)) 
					break;
				int status = atoi(jsonobj["status"].asString().c_str());
				if ((status> 0) && ((status & 4) == 4)) 
					pEvt->waitOK = true;

				timeout += 500;
				Sleep(500);
			}
			break;
		}
	}
	Sleep(500);
	SNotifyCenter::getSingleton().FireEventAsync(pEvt);
	pEvt->Release();
	return 0;
}

bool CMainDlg::OnStartSocketThread(LPVOID data)
{
	if (IsRunning())
	{
		MessageBox(NULL, _T("����̫Ƶ�������Ժ����ԣ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return false;
	}
	SNotifyCenter::getSingleton().addEvent(EVENTID(MainSocketThread));

	BeginThread(data);
	return true;
}


bool CMainDlg::OnMainSocketThread(EventArgs *e)
{
	MainSocketThread *pEvt = sobj_cast<MainSocketThread>(e);
	Json::Reader reader;
	Json::Value jsonobj;
	switch (pEvt->opt)
	{
		case OPT_GETCAMERA_LIST:
			if (!pEvt->retOK || !m_cinfolist.GetCount())
			{
				SetDisplayProgress(L"cameralist_win", L"refresh_progress");
				SetDisplayProgress(L"local_ip_win", L"local_ip_progress");
				MessageBox(NULL, _T("û�л�ȡ�����������豸��\n��ȷ������������AI�����"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
			GoToCameraListPage();
			SetDisplayBackBtn(0, TRUE, LIST_WND);
			break;

		case OPT_GETCAMERA_INFO:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				SetDisplayProgress(L"camera_ip_win", L"camera_ip_progress");
				MessageBox(NULL, _T("��ȡ����ʧ�ܣ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
			m_curSel = -1;
			GoToCameraInfoPage();
			SetDisplayBackBtn(0, TRUE, INFO_WND);
			break;
		case OPT_SETCAMERA_GENERAL:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("����ͨ�ò���ʧ�ܣ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				//m_cinfolist.RemoveAt(m_curSel);
				if (m_curSel>=0)
					m_cinfolist.SetAt(m_curSel, m_cinfo);
				MessageBox(NULL, _T("����ͨ�ò����ɹ���"), _T("��ʾ"), MB_OK);
			}

			break;
		case OPT_SETCAMERA_SERVER:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("���÷������ʧ�ܣ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				if (m_curSel >= 0)
					m_cinfolist.SetAt(m_curSel, m_cinfo);
				MessageBox(NULL, _T("���÷�������ɹ���"), _T("��ʾ"), MB_OK);
			}

			break;
		case OPT_SETCAMERA_STORE:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, SStringT().Format(_T("%s ���ô������ʧ�ܣ�"), pEvt->nData), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				if (m_curSel >= 0)
					m_cinfolist.SetAt(m_curSel, m_cinfo);
				MessageBox(NULL, SStringT().Format(_T("���ô�������ɹ���%s"), pEvt->nData), _T("��ʾ"), MB_OK);
			}

			break;
		case OPT_SETCAMERA_OTHER:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("������������ʧ�ܣ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				if (m_curSel >= 0)
					m_cinfolist.SetAt(m_curSel, m_cinfo);
				MessageBox(NULL, _T("�������������ɹ���"), _T("��ʾ"), MB_OK);
			}

			break;
		case OPT_FORMAT_SDCARD:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("SD����ʽ��ʧ�ܣ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				SWindow *text_formate_progress = FindChildByName2<SWindow>(L"text_formate_progress");
				if (pEvt->progress >= 0 && pEvt->progress < 1000)
				{
					text_formate_progress->SetWindowTextW(SStringT().Format(_T("%d.%d%%"), pEvt->progress/10, pEvt->progress%100));
					SLOGFMTE("#################SD����ʽ������=====: %d\n", pEvt->progress);
					break;
				}
				text_formate_progress->SetWindowTextW(SStringT().Format(_T("%d.%d%%"), pEvt->progress / 10, pEvt->progress % 100));
				MessageBox(NULL, _T("SD����ʽ���ɹ���"), _T("��ʾ"), MB_OK);
			}
			break;
		case OPT_SETTIME_SYNC:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("����ʱ��ʧ�ܣ�\n���������ã�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				MessageBox(NULL, _T("����ʱ��ɹ���"), _T("��ʾ"), MB_OK);
			}
			break;
		case OPT_LOGIN:
			if (pEvt->retOK)
			{
				if (!reader.parse(pEvt->hData, jsonobj))
				{
					OnStepChange(0, 1, L"");
					MessageBox(NULL, _T("�������������"), _T("�������"), MB_OK | MB_ICONERROR);
					return false;
				}
				m_token = jsonobj["access_token"].asString();
				if (!m_token.empty())
					OnStepChange(0, 2, L"");
				else
				{
					OnStepChange(0, 1, L"");
					MessageBox(NULL, _T("��ȡ��token��Ч"), _T("�������"), MB_OK | MB_ICONERROR);
				}			
			}
			else
			{
				OnStepChange(0, 1, L"");
				MessageBox(NULL, _T("��������ȷ�İ���Ŀ�˺�����!"), _T("�˺Ż��������"), MB_OK | MB_ICONERROR);
			}			
			break;
		case OPT_REGISTRE:
			if (!reader.parse(pEvt->hData, jsonobj))
			{
				OnStepChange(0, 2, L"");
				MessageBox(NULL, _T("�������������"), _T("�������"), MB_OK | MB_ICONERROR);
				return false;
			}
			if (pEvt->retOK)
			{
				OnStepChange(0, 3, L"");
			}
			else
			{
				SStringT error_msg;
				int error_code = atoi(jsonobj["error_code"].asString().c_str());
				switch (error_code)
				{
				case 110:
					error_msg = _T("token��Ч����ڣ������µ�¼");
					break;
				case 31023:
					error_msg = _T("���ʵĲ�������");
					break;
				case 31021:
					error_msg = _T("��������");
					break;
				case 300100:
					error_msg = _T("ע��ƽ̨����");
					break;
				case 31350:
					error_msg = _T("�豸�Ѿ�ע���");
					break;
				case 400306:
					error_msg = _T("�����û�������");
					break;
				default:
					error_msg = _T("����豸����");
					break;
				}
				OnStepChange(0, 2, L"");
				MessageBox(NULL, error_msg, _T("ע��ʧ��"), MB_OK | MB_ICONERROR);
			}
			break;
		case OPT_CANCELLATION:
			if (!reader.parse(pEvt->hData, jsonobj))
			{
				OnStepChange(0, 2, L"");
				MessageBox(NULL, _T("�������������"), _T("�������"), MB_OK | MB_ICONERROR);
				return false;
			}
			if (pEvt->retOK)
			{
				OnStepChange(0, 2, L"");
			}
			else
			{
				SStringT error_msg;
				int error_code = atoi(jsonobj["error_code"].asString().c_str());
				switch (error_code)
				{
				case 110:
					error_msg = _T("token��Ч����ڣ������µ�¼");
					break;
				case 31023:
					error_msg = _T("���ʵĲ�������");
					break;
				case 31021:
					error_msg = _T("��������");
					break;
				case 31353:
					error_msg = _T("�豸������");
					break;
				case 31354:
					error_msg = _T("��ǰ�û�û�и��豸��Ȩ��");
					break;
				case 31399:
					error_msg = _T("ע���豸ʧ��");
					break;
				default:
					error_msg = _T("ע���豸ʧ��");
					break;
				}
				OnStepChange(0, 2, L"");
				MessageBox(NULL, error_msg, _T("ע��ʧ��"), MB_OK | MB_ICONERROR);
			}
			break;
		case OPT_GETONLINE:
			if (pEvt->waitOK)
			{
				if (!reader.parse(pEvt->hData, jsonobj))
					break;

				SStringT deviceId = S_CA2T(jsonobj["deviceid"].asString().c_str());
				FindChildByName2<SWindow>(L"step4_deviceid")->SetWindowTextW(deviceId);
				OnStepChange(0, 4, L"");
			}
			else
			{
				if (pEvt->retOK)
				{
					OnStepChange(0, 4, L"");
					MessageBox(NULL, _T("δ�ܻ�ȡ���������״̬������������"), _T("��ʱ"), MB_OK | MB_ICONERROR);
				}
				else
				{
					OnStepChange(0, 3, L"");
					MessageBox(NULL, _T("��������߳�ʱʧ��"), _T("����ʧ��"), MB_OK | MB_ICONERROR);
				}
			}
			break;
		default:
			MessageBox(NULL, _T("����ʧ�ܣ�\n���������ã�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
			break;
	}

	return true;
}
