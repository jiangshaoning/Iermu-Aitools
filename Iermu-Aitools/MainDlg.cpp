// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Iphlpapi.h>
#include <shlobj.h>
#include "MainDlg.h"
#include "HTTPConfig.h"
#include "HTTPServer.h"

using namespace std;
#pragma comment(lib,"Iphlpapi.lib")
#define RELEASEPLAYER(player) \
if (player) \
{ \
	player_close(player); \
	player = NULL; \
}

#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif

HTTPServer theServer;
HTTPConfig theConf;

class CTestAdapterFix : public SAdapterBase
{
protected:

	SArray<UserInfo>    m_userInfo;
public:
	CTestAdapterFix(SArray<UserInfo>  &ui)
	{
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
	
CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
}

CMainDlg::~CMainDlg()
{
	m_player.ffplayStop();
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	initServerConfig();
	SetMsgHandled(FALSE);
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	InitServer();
	SetLocalIPView();
	return 0;
}
//TODO:消息映射
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

void CMainDlg::PlayFromLocalPage()
{
	SetRefreshDisplay(false);
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"hide_main");
	if (pTab)
	{
		SListView *pListViewIP = FindChildByName2<SListView>("lv_ip_play");
		if (pListViewIP)
		{
			CAdapterIP *m_pAdapter = new CAdapterIP(this);
			pListViewIP->SetAdapter(m_pAdapter);
			m_pAdapter->SetTags(m_iplist);
			m_pAdapter->SetFPTags(m_fplist);
			m_pAdapter->SetDMTags(m_domainlist);
			m_pAdapter->SetAFTags(m_aflist);
			m_pAdapter->SetFFSTags(m_ffsetlist);
			m_pAdapter->SetRECTags(m_reclist);
			m_pAdapter->Release();
		}
		pTab->SetCurSel(_T("ip_playlist"));
	}
}

int CMainDlg::GetLocalIPInfo(SArray<UserInfo> &Info)
{
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	//记录网卡数量
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量
	int IPnumPerNetCard = 0;

	UserInfo ui;

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}
	if (ERROR_SUCCESS == nRel)
	{
		//输出网卡信息
		//可能有多网卡,因此通过循环去判断
		while (pIpAdapterInfo)
		{
			cout << "网卡数量：" << ++netCardNum << endl;
			cout << "网卡描述：" << pIpAdapterInfo->Description << endl;
			SStringA strName = pIpAdapterInfo->Description;
			ui.strName = S_CA2T(strName);

			cout << "网卡IP地址如下：" << endl;
			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
			do
			{
				cout << "该网卡上的IP数量：" << ++IPnumPerNetCard << endl;
				cout << "IP 地址：" << pIpAddrString->IpAddress.String << endl;
				SStringA strIp = pIpAddrString->IpAddress.String;
				ui.strIp = S_CA2T(strIp);
				Info.Add(ui);
				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}

	}
	//释放内存空间
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}

	return 0;
}

void CMainDlg::SetLocalIPView(void)
{
	SArray<UserInfo>    userInfo;
	GetLocalIPInfo(userInfo);
	SComboView *pComboView = FindChildByName2<SComboView>(L"cbx_account");
	if (pComboView)
	{
		SListView *pLstView = pComboView->GetListView();
		CTestAdapterFix *pAdapter = new CTestAdapterFix(userInfo);
		pLstView->SetAdapter(pAdapter);
		pAdapter->Release();
		pComboView->SetCurSel(0);
	}
}

BOOL CMainDlg::BrowseDir(SStringT &path, HWND hwndOwner, SStringT title)
{
	TCHAR szPathName[MAX_PATH];
	BROWSEINFO bInfo = { 0 };
	bInfo.hwndOwner = hwndOwner;//父窗口  
	bInfo.lpszTitle = title;
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI/*包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..*/ |
		BIF_UAHINT/*带TIPS提示*/ | BIF_NONEWFOLDERBUTTON /*不带新建文件夹按钮*/;
	LPITEMIDLIST lpDlist;
	lpDlist = SHBrowseForFolder(&bInfo);
	if (lpDlist != NULL)//单击了确定按钮  
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
	// 初始化配置模块,配置文件是 EXE 目录下的 conf.xml
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
		MessageBox(NULL, _T("无法读取/生成配置文件 conf.xml"), _T("提示"), MB_OK | MB_ICONERROR);
		return -1;
	}
}

void CMainDlg::SetServer()
{
	SEdit *edit_picturepath = FindChildByName2<SEdit>(L"edit_picturepath");
	SEdit *edit_serverport = FindChildByName2<SEdit>(L"edit_serverport");
	SCheckBox *check_autoserver = FindChildByName2<SCheckBox>(L"check_autoserver");
	SCheckBox *check_dirbrowse = FindChildByName2<SCheckBox>(L"check_dirbrowse");
	if (edit_picturepath)
	{
		SStringA Apath = S_CT2A(edit_picturepath->GetWindowTextW());
		string path = Apath;
		theConf.setTmpRoot(path);
		theConf.setDocRoot(path);
	}
	if (edit_serverport)
	{
		SStringA Aport = S_CT2A(edit_serverport->GetWindowTextW());
		string port = Aport;
		theConf.setPort(atoi(port.c_str()));
	}
	theConf.setAutoRun((bool)check_autoserver->IsChecked());//自动启动
	theConf.setDirVisible((bool)check_dirbrowse->IsChecked());//启动浏览目录
	theConf.save(m_ConfFileName);
	MessageBox(NULL, _T("设置完成"), _T("提示"), MB_OK);
}

void CMainDlg::SetPicturePath()
{
	SStringT path;
	SEdit *edit_picturepath = FindChildByName2<SEdit>(L"edit_picturepath");
	BOOL ret = BrowseDir(path, m_hWnd, _T("请选择一个文件夹"));

	if (ret)
	{
		edit_picturepath->SetWindowTextW(path);
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
			server_status->SetWindowTextW(_T("图片服务器已启动"));
			MessageBox(NULL, _T("图片服务器已经启动, 请勿重复启动！"), _T("提示"), MB_OK | MB_ICONERROR);
			break;
		case SE_SUCCESS:
			ret = 1;
			server_status->SetWindowTextW(_T("图片服务器已启动"));
			break;
		default:
			ret = -1;
			server_status->SetWindowTextW(_T("图片服务器已停止"));
			MessageBox(NULL, _T("图片服务器启动失败！请检查服务是否被占用或者已经启动"), _T("提示"), MB_OK | MB_ICONERROR);
			break;
	}
	return ret;
}

void CMainDlg::OnStoptheServer()
{
	if (theServer.runing())
	{
		SWindow *server_status = FindChildByName2<SWindow>(L"server_status");
		server_status->SetWindowTextW(_T("图片服务器已停止"));
		theServer.stop();
	}
}

void CMainDlg::InitServer()
{
	SEdit *edit_picturepath = FindChildByName2<SEdit>(L"edit_picturepath");
	SEdit *edit_serverport = FindChildByName2<SEdit>(L"edit_serverport");
	SCheckBox *check_autoserver = FindChildByName2<SCheckBox>(L"check_autoserver");
	SCheckBox *check_dirbrowse = FindChildByName2<SCheckBox>(L"check_dirbrowse");

	if (edit_picturepath)
	{
		SStringT Tpath = S_CA2T(theConf.tmpRoot().c_str());
		edit_picturepath->SetWindowTextW(Tpath);
	}
	if (edit_serverport)
	{
		char port[32] = { 0 };
		SStringT Tport = S_CA2T(itoa(theConf.port(), port, 10));
		edit_serverport->SetWindowTextW(Tport);
	}
	if (theConf.dirVisible())
	{
		check_dirbrowse->SetCheck(true);
	}

	if (theConf.autoRun())
	{
		check_autoserver->SetCheck(true);
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
	SLOGFMTE("%04d年%02d月%02d日%02d时%02d分%02d秒", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

void CMainDlg::GetCameraInfo()
{
	if (CheckIp(0, L"edit_cameraIp", _T("请输入摄像机IP"), m_cameraIp))
	{
		memset(&m_data, 0, sizeof(SocketData));
		m_data.opt = OPT_GETCAMERA_INFO;
		LPVOID param = (LPVOID)&m_data;

		OnStartSocketThread(param);
	}
}

void CMainDlg::UpdateCameraInfo()
{

	SCheckBox *check_1 = FindChildByName2<SCheckBox>(L"check_1");
	SCheckBox *check_2 = FindChildByName2<SCheckBox>(L"check_2");
	SCheckBox *check_3 = FindChildByName2<SCheckBox>(L"check_3");
	SCheckBox *check_4 = FindChildByName2<SCheckBox>(L"check_4");
	SCheckBox *check_5 = FindChildByName2<SCheckBox>(L"check_5");
	SCheckBox *check_6 = FindChildByName2<SCheckBox>(L"check_6");
	SCheckBox *check_7 = FindChildByName2<SCheckBox>(L"check_7");
	SRadioBox *radio_1 = FindChildByName2<SRadioBox>(L"radio_1");
	SRadioBox *radio_2 = FindChildByName2<SRadioBox>(L"radio_2");
	SRadioBox *radio_3 = FindChildByName2<SRadioBox>(L"radio_3");
	SRadioBox *radio_4 = FindChildByName2<SRadioBox>(L"radio_4");

	SEdit *edit_api_key = FindChildByName2<SEdit>(L"edit_api_key");
	SEdit *edit_secret_key = FindChildByName2<SEdit>(L"edit_secret_key");
	SEdit *edit_isnot_facial = FindChildByName2<SEdit>(L"edit_isnot_facial");
	SEdit *eidt_faces_second = FindChildByName2<SEdit>(L"eidt_faces_second");
	SEdit *edit_pixel_width = FindChildByName2<SEdit>(L"edit_pixel_width");
	SEdit *edit_pixel_height = FindChildByName2<SEdit>(L"edit_pixel_height");
	SEdit *edit_reliability = FindChildByName2<SEdit>(L"edit_reliability");
	SEdit *edit_residence_time = FindChildByName2<SEdit>(L"edit_residence_time");

	SEdit *edit_hostIp = FindChildByName2<SEdit>(L"edit_hostIp");
	SEdit *edit_hostPort = FindChildByName2<SEdit>(L"edit_hostPort");
	SEdit *edit_libname = FindChildByName2<SEdit>(L"edit_libname");
	SEdit *edit_cphostIp = FindChildByName2<SEdit>(L"edit_cphostIp");
	SEdit *edit_cphostPort = FindChildByName2<SEdit>(L"edit_cphostPort");
	SEdit *edit_jpgmem = FindChildByName2<SEdit>(L"edit_jpgmem");

	SCheckBox *check_lan = FindChildByName2<SCheckBox>(L"check_lan");
	SCheckBox *check_network = FindChildByName2<SCheckBox>(L"check_network");
	SCheckBox *check_savejpg = FindChildByName2<SCheckBox>(L"check_savejpg");
	SCheckBox *check_https = FindChildByName2<SCheckBox>(L"check_https");
	SComboBox * tCbx = FindChildByName2<SComboBox>(L"cbx_rec");
	SComboBox * pCbx = FindChildByName2<SComboBox>(L"cbx_ai");
	SComboBox * sCbx = FindChildByName2<SComboBox>(L"cbx_lib");
	//SComboBox * jCbx = FindChildByName2<SComboBox>(L"cbx_size");


	SStringA Aapi = (char *)m_cinfo.fp.api_key;
	SStringT Tapi = S_CA2T(Aapi);
	edit_api_key->SetWindowTextW(SStringT().Format(_T("%s"), Tapi));

	SStringA Asecr = (char *)m_cinfo.fp.secret_key;
	SStringT Tsecr = S_CA2T(Asecr);
	edit_secret_key->SetWindowTextW(SStringT().Format(_T("%s"), Tsecr));

	(m_cinfo.fp.facial >= 0 && m_cinfo.fp.facial <0xff) ? edit_isnot_facial->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.facial)) :
		edit_isnot_facial->SetWindowTextW(SStringT().Format(_T("")));

	(m_cinfo.fp.faces >= 0 && m_cinfo.fp.faces <0xff) ? eidt_faces_second->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.faces)) :
		eidt_faces_second->SetWindowTextW(SStringT().Format(_T("")));

	(m_cinfo.fp.width >= 0 && m_cinfo.fp.width <0xffff) ? edit_pixel_width->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.width)) :
		edit_pixel_width->SetWindowTextW(SStringT().Format(_T("")));

	(m_cinfo.fp.height >= 0 && m_cinfo.fp.height <0xffff)? edit_pixel_height->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.height)) :
		edit_pixel_height->SetWindowTextW(SStringT().Format(_T("")));

	(m_cinfo.fp.reliability >= 0 && m_cinfo.fp.reliability <0xffff) ? edit_reliability->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.reliability)) :
		edit_reliability->SetWindowTextW(SStringT().Format(_T("")));

	(m_cinfo.fp.res_time >= 0 && m_cinfo.fp.res_time <0xffff)  ? edit_residence_time->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.fp.res_time)) :
		edit_residence_time->SetWindowTextW(SStringT().Format(_T("")));


	SStringA Ahoip = (char *)m_cinfo.ipp.domain;
	SStringT Thoip = S_CA2T(Ahoip);
	edit_hostIp->SetWindowTextW(SStringT().Format(_T("%s"), Thoip));

	(m_cinfo.ipp.port >= 0 && m_cinfo.ipp.port <0xffff) ? edit_hostPort->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.ipp.port)) :
		edit_hostPort->SetWindowTextW(SStringT().Format(_T("80")));

	SStringA Acphoip = (char *)m_cinfo.ipp.cp_domain;
	SStringT Tcphoip = S_CA2T(Acphoip);
	edit_cphostIp->SetWindowTextW(SStringT().Format(_T("%s"), Tcphoip));

	(m_cinfo.ipp.cp_port >= 0 && m_cinfo.ipp.cp_port <0xffff) ? edit_cphostPort->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.ipp.cp_port)) :
		edit_cphostPort->SetWindowTextW(SStringT().Format(_T("80")));

	SStringA Aswid = (char *)m_cinfo.ipp.sw_id;
	SStringT Tswid = S_CA2T(Aswid);
	edit_libname->SetWindowTextW(SStringT().Format(_T("%s"), Tswid));


	if (m_cinfo.af.jpgmem >= 0 && m_cinfo.af.jpgmem < 0xffff)
	{
		(m_cinfo.af.jpgmem >> 15 > 0) ? edit_jpgmem->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.af.jpgmem & 0x7FFF)) : edit_jpgmem->SetWindowTextW(SStringT().Format(_T("%.03f"), (double)(m_cinfo.af.jpgmem & 0x7FFF)/1000));
	}
	else
	{
		edit_jpgmem->SetWindowTextW(SStringT().Format(_T("")));
	}

	//(m_cinfo.af.jpgmem >= 0 && m_cinfo.af.jpgmem <0xffff) ? edit_jpgmem->SetWindowTextW(SStringT().Format(_T("%d"), m_cinfo.af.jpgmem&0x7FFF)) :
	//	edit_jpgmem->SetWindowTextW(SStringT().Format(_T("")));
	//jCbx->SetCurSel(m_cinfo.af.jpgmem>>15);


	if (m_cinfo.fp.face_img == 0)
		radio_1->SetCheck(true);
	else
		radio_1->SetCheck(false);

	if (m_cinfo.fp.face_img == 1)
		radio_2->SetCheck(true);
	else
		radio_2->SetCheck(false);

	if (m_cinfo.fp.face_img == 2)
		radio_3->SetCheck(true);
	else
		radio_3->SetCheck(false);

	if (m_cinfo.fp.face_img == 3)
		radio_4->SetCheck(true);
	else
		radio_4->SetCheck(false);

	if (m_cinfo.fp.box & 1)
		check_1->SetCheck(true);
	else
		check_1->SetCheck(false);

	if (m_cinfo.fp.box >> 3 & 1)
		check_2->SetCheck(true);
	else
		check_2->SetCheck(false);

	if (m_cinfo.fp.box >> 4 & 1)
		check_3->SetCheck(true);
	else
		check_3->SetCheck(false);

	if (m_cinfo.fp.box >> 6 & 1)
		check_4->SetCheck(true);
	else
		check_4->SetCheck(false);

	if (m_cinfo.ffs.face_rec & 1)
		check_5->SetCheck(true);
	else
		check_5->SetCheck(false);

	if (m_cinfo.ffs.face_cod & 1)
		check_6->SetCheck(true);
	else
		check_6->SetCheck(false);

	if (m_cinfo.ffs.face_cou & 1)
		check_7->SetCheck(true);
	else
		check_7->SetCheck(false);


	if (m_cinfo.af.lan >> 7 & 1)
		check_lan->SetCheck(true);
	else
		check_lan->SetCheck(false);

	if (m_cinfo.af.lan >> 6 & 1)
		check_network->SetCheck(true);
	else
		check_network->SetCheck(false);

	if (m_cinfo.af.lan >> 5 & 1)
		check_savejpg->SetCheck(true);
	else
		check_savejpg->SetCheck(false);

	if (m_cinfo.ipp.lib_type >>7 &1)
		check_https->SetCheck(true);
	else
		check_https->SetCheck(false);

	pCbx->SetCurSel(m_cinfo.af.comparison);
	sCbx->SetCurSel(m_cinfo.ipp.lib_type & 1);

	switch (m_cinfo.rec)
	{
		case 0:
			tCbx->SetCurSel(0);
			break;
		case 0x80:
			tCbx->SetCurSel(1);
			break;
		case 1:
			tCbx->SetCurSel(2);
			break;
	}
}

//void CMainDlg::UploadIPRequest(IPPort &dm)
//{
//	memset(&m_data, 0, sizeof(SocketData));
//	m_data.opt = OPT_UPLOADIP;
//	memcpy(m_data.ipp.domain, dm.domain, DOMAIN_LEN);
//	memcpy(m_data.ipp.cp_domain, dm.cp_domain, CPDOMAIN_LEN);
//	memcpy(m_data.ipp.sw_id, dm.sw_id, LIBNAME_LEN);
//	//m_data.ipp.ip = (a << 24) | (b << 16) | (c << 8) | d;
//	m_data.ipp.lib_type = dm.lib_type;
//	m_data.ipp.port = dm.port;
//	m_data.ipp.cp_port = dm.cp_port;
//
//	LPVOID param = (LPVOID)&m_data;
//	OnStartSocketThread(param);
//}
//
//void CMainDlg::UploadIP()
//{
//	IPPort dm;
//	//int port = 0xffff, cpport = 0xffff;
//	//unsigned char default[2] = { 0xff, 0xff };
//	int port = 0, cpport = 0;
//
//	if (!CheckIp(0, L"edit_cameraIp", _T("请输入摄像机IP"), m_cameraIp))
//		return;
//
//	memset(&m_dm, 0, sizeof(IPPort));
//	SEdit *edit_hostIp = FindChildByName2<SEdit>(L"edit_hostIp");
//	SEdit *edit_hostPort = FindChildByName2<SEdit>(L"edit_hostPort");
//	SEdit *edit_libname = FindChildByName2<SEdit>(L"edit_libname");
//	SEdit *edit_cphostIp = FindChildByName2<SEdit>(L"edit_cphostIp");
//	SEdit *edit_cphostPort = FindChildByName2<SEdit>(L"edit_cphostPort");
//	SStringT  hostIp = edit_hostIp->GetWindowTextW();
//	SStringT  hostPort = edit_hostPort->GetWindowTextW();
//	SStringT  libname = edit_libname->GetWindowTextW();
//	SStringT  cphostIp = edit_cphostIp->GetWindowTextW();
//	SStringT  cphostPort = edit_cphostPort->GetWindowTextW();
//	SComboBox * sCbx = FindChildByName2<SComboBox>(L"cbx_lib");
//
//	//if (!((swscanf(hostIp, L"%d.%d.%d.%d", &a, &b, &c, &d) == 4) && (a >= 0 && a <= 255)
//	//	&& (b >= 0 && b <= 255)
//	//	&& (c >= 0 && c <= 255)
//	//	&& (d >= 0 && d <= 255)))
//	//{
//	//	MessageBox(NULL, _T("ip格式不正确，请输入正确的ipv4"), _T("提示"), MB_OK | MB_ICONERROR);
//	//	return;
//	//}
//
//	if (hostIp.GetLength() > 60)
//	{
//		MessageBox(NULL, _T("上传域名或IP长度不得超过60字符"), _T("提示"), MB_OK | MB_ICONERROR);
//		return;
//	}
//
//	if (cphostIp.GetLength() > 60)
//	{
//		MessageBox(NULL, _T("比对域名或IP长度不得超过60字符"), _T("提示"), MB_OK | MB_ICONERROR);
//		return;
//	}
//
//	if (libname.GetLength() > 119)
//	{
//		MessageBox(NULL, _T("库名不得超过119字符"), _T("提示"), MB_OK | MB_ICONERROR);
//		return;
//	}
//
//	if (hostPort.GetLength() > 0)
//	{
//		if (!((swscanf(hostPort, L"%d", &port) == 1) && (port >= 0 && port <= 65535)))
//		{
//			MessageBox(NULL, _T("上传端口号值不正确，请输入正确的端口号"), _T("提示"), MB_OK | MB_ICONERROR);
//			return;
//		}
//	}
//
//	if (cphostPort.GetLength() > 0)
//	{
//		if (!((swscanf(cphostPort, L"%d", &cpport) == 1) && (cpport >= 0 && cpport <= 65535)))
//		{
//			MessageBox(NULL, _T("比对端口号值不正确，请输入正确的端口号"), _T("提示"), MB_OK | MB_ICONERROR);
//			return;
//		}
//	}
//
//	m_dm.port = port;
//	m_dm.cp_port = cpport;
//	m_dm.lib_type = sCbx->GetCurSel();
//	SStringA hostipA = S_CT2A(hostIp);
//	hostipA.GetLength() > 0 
//		?memcpy(m_dm.domain, hostipA.GetBuffer(hostipA.GetLength()), hostipA.GetLength())
//		:memset(m_dm.domain, 0, sizeof(m_dm.domain));
//
//	SStringA cphostipA = S_CT2A(cphostIp);
//	cphostipA.GetLength() > 0 
//		?memcpy(m_dm.cp_domain, cphostipA.GetBuffer(cphostipA.GetLength()), cphostipA.GetLength())
//		: memset(m_dm.cp_domain, 0, sizeof(m_dm.cp_domain));
//
//	SStringA libnameA = S_CT2A(libname);
//	libnameA.GetLength() > 0 
//		? memcpy(m_dm.sw_id, libnameA.GetBuffer(libnameA.GetLength()), libnameA.GetLength())
//		: memset(m_dm.sw_id, 0, sizeof(m_dm.sw_id));
//
//	UploadIPRequest(m_dm);
//}

bool CMainDlg::CheckIp(int type, LPCWSTR pszName, LPCWSTR tipName, string &ip)
{
	int a, b, c, d;
	SStringT cameraIp;
	if (type)
	{
		SComboView *pComboView = FindChildByName2<SComboView>(L"cbx_account");
		if (!pComboView)  return false;
		cameraIp = pComboView->GetWindowTextW();
	}		
	else
	{
		SEdit *edit_cameraIp = FindChildByName2<SEdit>(pszName);
		if (!edit_cameraIp)  return false;
		cameraIp = edit_cameraIp->GetWindowTextW();

	}
	if (!cameraIp.GetLength())
	{
		MessageBox(NULL, tipName, _T("提示"), MB_OK | MB_ICONERROR);
		return false;
	}
	if (!((swscanf(cameraIp, L"%d.%d.%d.%d", &a, &b, &c, &d) == 4) && (a >= 0 && a <= 255)
		&& (b >= 0 && b <= 255)
		&& (c >= 0 && c <= 255)
		&& (d >= 0 && d <= 255)))
	{
		MessageBox(NULL, _T("ip格式不正确，请输入正确的ipv4"), _T("提示"), MB_OK | MB_ICONERROR);
		return false;
	}
	SStringA tip = S_CT2A(cameraIp);
	ip = tip;

	return true;
}

void CMainDlg::SetCameraIP(string &ip)
{
	m_cameraIp = ip;
}

void CMainDlg::SetCameraTime()
{
	memset(&m_data, 0, sizeof(SocketData));
	m_data.opt = OPT_SETTIME_FROMCAMERAIP;

	LPVOID param = (LPVOID)&m_data;

	OnStartSocketThread(param);
}

void CMainDlg::SetTimeFromCameraIp()
{
	if (CheckIp(0, L"edit_cameraIp", _T("请输入摄像机IP"), m_cameraIp))
	{
		memset(&m_data, 0, sizeof(SocketData));
		m_data.opt = OPT_SETTIME_FROMCAMERAIP;
		LPVOID param = (LPVOID)&m_data;

		OnStartSocketThread(param);
	}
}

void CMainDlg::SetFaceParameterFromLoalIP(UINT8	&rec, FaceParameter &fpara, FaceFuncSetting &ffset, IPPort &dm)
{
	memset(&m_fpara, 0, sizeof(FaceParameter));
	memcpy(m_fpara.api_key, fpara.api_key, 32);
	memcpy(m_fpara.secret_key, fpara.secret_key, 32);

	m_fpara.facial = fpara.facial;
	m_fpara.faces = fpara.faces;
	m_fpara.face_img = fpara.face_img;
	m_fpara.box = fpara.box;
	m_fpara.width = fpara.width;
	m_fpara.height = fpara.height;
	m_fpara.reliability = fpara.reliability;
	m_fpara.res_time = fpara.res_time;
	memcpy(m_fpara.groupID, fpara.groupID, sizeof(fpara.groupID));

	m_ffset.face_rec = ffset.face_rec;
	m_ffset.face_cod = ffset.face_cod;
	m_ffset.face_cou = ffset.face_cou;

	m_rec = rec;

	memcpy(m_dm.domain, dm.domain, DOMAIN_LEN);
	memcpy(m_dm.cp_domain, dm.cp_domain, CPDOMAIN_LEN);
	memcpy(m_dm.sw_id, dm.sw_id, LIBNAME_LEN);
	//m_data.ipp.ip = (a << 24) | (b << 16) | (c << 8) | d;
	m_dm.lib_type = dm.lib_type;
	m_dm.port = dm.port;
	m_dm.cp_port = dm.cp_port;

	memset(&m_data, 0, sizeof(SocketData));
	m_data.opt = OPT_SETFACE_FROMCAMERAIP;
	LPVOID param = (LPVOID)&m_data;

	OnStartSocketThread(param);
}

bool CMainDlg::CheckFaceParameter()
{
	//int facialval=0xff, facesval =0xff, widthval=0xffff, heightval=0xffff, relival=0xffff, timeval=0xfffff;
	//unsigned char default[2] = { 0xff, 0xff };
	int facialval = 0, facesval = 0, widthval = 0, heightval = 0, relival = 0, timeval = 0, port = 0, cpport = 0;

	SWindow * grp_face_img = FindChildByName(L"grp_face_img")->GetSelectedChildInGroup();
	SCheckBox *check_1 = FindChildByName2<SCheckBox>(L"check_1");
	SCheckBox *check_2 = FindChildByName2<SCheckBox>(L"check_2");
	SCheckBox *check_3 = FindChildByName2<SCheckBox>(L"check_3");
	SCheckBox *check_4 = FindChildByName2<SCheckBox>(L"check_4");
	SCheckBox *check_5 = FindChildByName2<SCheckBox>(L"check_5");
	SCheckBox *check_6 = FindChildByName2<SCheckBox>(L"check_6");
	SCheckBox *check_7 = FindChildByName2<SCheckBox>(L"check_7");

	SEdit *edit_api_key = FindChildByName2<SEdit>(L"edit_api_key");
	SEdit *edit_secret_key = FindChildByName2<SEdit>(L"edit_secret_key");
	SEdit *edit_isnot_facial = FindChildByName2<SEdit>(L"edit_isnot_facial");
	SEdit *eidt_faces_second = FindChildByName2<SEdit>(L"eidt_faces_second");
	SEdit *edit_pixel_width = FindChildByName2<SEdit>(L"edit_pixel_width");
	SEdit *edit_pixel_height = FindChildByName2<SEdit>(L"edit_pixel_height");
	SEdit *edit_reliability = FindChildByName2<SEdit>(L"edit_reliability");
	SEdit *edit_residence_time = FindChildByName2<SEdit>(L"edit_residence_time");
	//SEdit *edit_group_id = FindChildByName2<SEdit>(L"edit_group_id");

	SComboBox * tCbx = FindChildByName2<SComboBox>(L"cbx_rec");

	int face_img_id = grp_face_img->GetID();	
	SStringT tapi_key = edit_api_key->GetWindowTextW();
	SStringT tsecret_key = edit_secret_key->GetWindowTextW();
	SStringT tisnot_facial = edit_isnot_facial->GetWindowTextW();
	SStringT tfaces_second = eidt_faces_second->GetWindowTextW();
	SStringT tpixel_width = edit_pixel_width->GetWindowTextW();
	SStringT tpixel_height = edit_pixel_height->GetWindowTextW();
	SStringT treliability = edit_reliability->GetWindowTextW();
	SStringT tresidence_time = edit_residence_time->GetWindowTextW();
	//SStringT tgroup_id = edit_group_id->GetWindowTextW();

	//图片上传配置检查
	SEdit *edit_hostIp = FindChildByName2<SEdit>(L"edit_hostIp");
	SEdit *edit_hostPort = FindChildByName2<SEdit>(L"edit_hostPort");
	SEdit *edit_libname = FindChildByName2<SEdit>(L"edit_libname");
	SEdit *edit_cphostIp = FindChildByName2<SEdit>(L"edit_cphostIp");
	SEdit *edit_cphostPort = FindChildByName2<SEdit>(L"edit_cphostPort");
	SCheckBox *check_https = FindChildByName2<SCheckBox>(L"check_https");

	SStringT  hostIp = edit_hostIp->GetWindowTextW();
	SStringT  hostPort = edit_hostPort->GetWindowTextW();
	SStringT  libname = edit_libname->GetWindowTextW();
	SStringT  cphostIp = edit_cphostIp->GetWindowTextW();
	SStringT  cphostPort = edit_cphostPort->GetWindowTextW();
	SComboBox * sCbx = FindChildByName2<SComboBox>(L"cbx_lib");
	

	if (tapi_key.GetLength() > 32 )
	{
		MessageBox(NULL, _T("API_KEY输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
		return false;
	}

	if (tsecret_key.GetLength() > 32 )
	{
		MessageBox(NULL, _T("SECRET_KEY输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
		return false;
	}

	if (tisnot_facial.GetLength() > 0)
	{
		if (!((swscanf(tisnot_facial, L"%d", &facialval) == 1) && (facialval >= 0 && facialval <= 1)))
		{
			MessageBox(NULL, _T("是否只抓正脸输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	if (tfaces_second.GetLength() > 0)
	{
		if (!((swscanf(tfaces_second, L"%d", &facesval) == 1) && (facesval >= 0 && facesval <= 255)))
		{
			MessageBox(NULL, _T("每秒多少张人脸输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	if (tpixel_width.GetLength() > 0)
	{
		if (!((swscanf(tpixel_width, L"%d", &widthval) == 1) && (widthval >= 0 && widthval <= 0xffff)))
		{
			MessageBox(NULL, _T("最小尺寸宽度输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	if (tpixel_height.GetLength() > 0)
	{
		if (!((swscanf(tpixel_height, L"%d", &heightval) == 1) && (heightval >= 0 && heightval <= 0xffff)))
		{
			MessageBox(NULL, _T("最小尺寸高度输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	if (treliability.GetLength() > 0)
	{
		if (!((swscanf(treliability, L"%d", &relival) == 1) && (relival >= 0 && relival <= 0xffff)))
		{
			MessageBox(NULL, _T("可信度输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	if (tresidence_time.GetLength() > 0)
	{
		if (!((swscanf(tresidence_time, L"%d", &timeval) == 1) && (timeval >= 0 && timeval <= 0xffff)))
		{
			MessageBox(NULL, _T("滞留时间输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	//if (tgroup_id.GetLength() > 4 )
	//{
	//	MessageBox(NULL, _T("GROUPID输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
	//	return false;
	//}

	if (hostIp.GetLength() > 60)
	{
		MessageBox(NULL, _T("上传域名或IP长度不得超过60字符"), _T("提示"), MB_OK | MB_ICONERROR);
		return false;
	}

	if (cphostIp.GetLength() > 60)
	{
		MessageBox(NULL, _T("比对域名或IP长度不得超过60字符"), _T("提示"), MB_OK | MB_ICONERROR);
		return false;
	}

	if (libname.GetLength() > 119)
	{
		MessageBox(NULL, _T("库名不得超过119字符"), _T("提示"), MB_OK | MB_ICONERROR);
		return false;
	}

	if (hostPort.GetLength() > 0)
	{
		if (!((swscanf(hostPort, L"%d", &port) == 1) && (port >= 0 && port <= 65535)))
		{
			MessageBox(NULL, _T("上传端口号值不正确，请输入正确的端口号"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	if (cphostPort.GetLength() > 0)
	{
		if (!((swscanf(cphostPort, L"%d", &cpport) == 1) && (cpport >= 0 && cpport <= 65535)))
		{
			MessageBox(NULL, _T("比对端口号值不正确，请输入正确的端口号"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	SStringA tmpstr = S_CT2A(tapi_key);
	string api_key = tmpstr;
	memset(m_fpara.api_key, 0, sizeof(m_fpara.api_key));
	api_key.length() > 0 ? memcpy(m_fpara.api_key, api_key.c_str(), api_key.length()) : memset(m_fpara.api_key, 0, sizeof(m_fpara.api_key));
	

	tmpstr = S_CT2A(tsecret_key);
	string secret_key = tmpstr;
	memset(m_fpara.secret_key, 0, sizeof(m_fpara.secret_key));
	secret_key.length() > 0 ? memcpy(m_fpara.secret_key, secret_key.c_str(), secret_key.length()) : memset(m_fpara.secret_key, 0, sizeof(m_fpara.secret_key));

	switch (face_img_id)
	{
		case 1000:
			m_fpara.face_img = 0;
			break;
		case 1001:
			m_fpara.face_img = 1;
			break;
		case 1002:
			m_fpara.face_img = 2;
			break;
		case 1003:
			m_fpara.face_img = 3;
			break;
	}

	m_fpara.box = (check_1->IsChecked() ? 1 : 0) | (check_2->IsChecked() ? (1 << 3) : 0) |
				(check_3->IsChecked() ? (1 << 4) : 0) | (check_4->IsChecked() ? (1 << 6) : 0);

	m_ffset.face_rec = (check_5->IsChecked() ? 1 : 0);
	m_ffset.face_cod = (check_6->IsChecked() ? 1 : 0);
	m_ffset.face_cou = (check_7->IsChecked() ? 1 : 0);

	tCbx->GetCurSel();
	switch (tCbx->GetCurSel())
	{
		case 0:
			m_rec = 0;
			break;
		case 1:
			m_rec = 0x80;
			break;
		case 2:
			m_rec = 1;
			break;
	}

	m_fpara.facial = facialval;
	m_fpara.faces = facesval;
	m_fpara.width = widthval;
	m_fpara.height = heightval;
	m_fpara.reliability = relival;
	m_fpara.res_time = timeval;

	//tmpstr = S_CT2A(tgroup_id);
	//string group_id = tmpstr;
	//memset(m_fpara.groupID, 0, sizeof(m_fpara.groupID));
	//group_id.length() > 0 ? memcpy(m_fpara.groupID, group_id.c_str(), group_id.length()) : memcpy(m_fpara.groupID, default, sizeof(default));
	memset(m_fpara.groupID, 0, sizeof(m_fpara.groupID));

	memset(&m_dm, 0, sizeof(IPPort));
	m_dm.port = port;
	m_dm.cp_port = cpport;
	m_dm.lib_type = sCbx->GetCurSel() | (check_https->IsChecked() ? (1 << 7) : 0);
	SStringA hostipA = S_CT2A(hostIp);
	hostipA.GetLength() > 0
		? memcpy(m_dm.domain, hostipA.GetBuffer(hostipA.GetLength()), hostipA.GetLength())
		: memset(m_dm.domain, 0, sizeof(m_dm.domain));

	SStringA cphostipA = S_CT2A(cphostIp);
	cphostipA.GetLength() > 0
		? memcpy(m_dm.cp_domain, cphostipA.GetBuffer(cphostipA.GetLength()), cphostipA.GetLength())
		: memset(m_dm.cp_domain, 0, sizeof(m_dm.cp_domain));

	SStringA libnameA = S_CT2A(libname);
	libnameA.GetLength() > 0
		? memcpy(m_dm.sw_id, libnameA.GetBuffer(libnameA.GetLength()), libnameA.GetLength())
		: memset(m_dm.sw_id, 0, sizeof(m_dm.sw_id));

	return true;
}

void CMainDlg::SetFaceParameter()
{
	if (!CheckIp(0, L"edit_cameraIp", _T("请输入摄像机IP"), m_cameraIp))
		return;

	if(CheckFaceParameter())
	{
		memset(&m_data, 0, sizeof(SocketData));
		m_data.opt = OPT_SETFACE_FROMCAMERAIP;
		LPVOID param = (LPVOID)&m_data;

		OnStartSocketThread(param);
	}
}

void CMainDlg::SetAIFunctionFromLoalIP(AIFunction &aifunc)
{
	memset(&m_aifunc, 0, sizeof(AIFunction));
	memcpy(m_aifunc.head, aifunc.head, 4);

	m_aifunc.lan = aifunc.lan;
	m_aifunc.comparison = aifunc.comparison;
	m_aifunc.jpgmem = aifunc.jpgmem;

	memset(&m_data, 0, sizeof(SocketData));
	m_data.opt = OPT_SETAIFUNC_FROMCAMERAIP;
	LPVOID param = (LPVOID)&m_data;

	OnStartSocketThread(param);
}

bool CMainDlg::CheckAIFunction()
{
	if (!CheckIp(0, L"edit_cameraIp", _T("请输入摄像机IP"), m_cameraIp))
		return false;
	UINT16 memsize = 0;
	int sum = 0;
	unsigned int jpgmem = 2, decimal = 0;
	unsigned char head[4] = { 0x72, 0x2a, 0x3c, 0x6e };

	SEdit *edit_jpgmem = FindChildByName2<SEdit>(L"edit_jpgmem");
	SCheckBox *check_lan = FindChildByName2<SCheckBox>(L"check_lan");
	SCheckBox *check_network = FindChildByName2<SCheckBox>(L"check_network");
	SCheckBox *check_savejpg = FindChildByName2<SCheckBox>(L"check_savejpg");
	SComboBox * pCbx = FindChildByName2<SComboBox>(L"cbx_ai");
	//SComboBox * jCbx = FindChildByName2<SComboBox>(L"cbx_size");

	SStringT tjpgmem = edit_jpgmem->GetWindowTextW();
	if (tjpgmem.GetLength() > 0)
	{
		if (!((swscanf(tjpgmem, L"%d.%d", &jpgmem, &decimal) == 1 || swscanf(tjpgmem, L"%d.%d", &jpgmem, &decimal) == 2) && (jpgmem >= 0 && jpgmem <= 0xffff)))
		{
			MessageBox(NULL, _T("是否只抓正脸输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
			return false;
		}
	}

	SStringA ajpgmem = S_CT2A(tjpgmem);
	string sjpgmem = ajpgmem;
	double djpgmem = atof(sjpgmem.c_str()); 
	if (djpgmem > -0.0000001 && djpgmem < 0.000001)
		jpgmem = 2;
	else if (djpgmem >= 1)
		jpgmem = (unsigned int)djpgmem;
	else
		jpgmem = 0;

	memset(&m_aifunc, 0, sizeof(AIFunction));
	memcpy(m_aifunc.head, head, sizeof(head));
	m_aifunc.lan = (check_lan->IsChecked() ? (1 << 7) : 0) | (check_network->IsChecked() ? (1 << 6) : 0) | (check_savejpg->IsChecked() ? (1 << 5) : 0);
	m_aifunc.comparison = pCbx->GetCurSel();
	//memsize = jCbx->GetCurSel();

	if (jpgmem >= 1)
		m_aifunc.jpgmem = jpgmem | (1 << 15);
	else
	{
		m_aifunc.jpgmem = (int)(djpgmem * 1000);
	}
		
	return true;
}


void CMainDlg::SetAIFunction()
{
	if (CheckAIFunction())
	{
		memset(&m_data, 0, sizeof(SocketData));
		m_data.opt = OPT_SETAIFUNC_FROMCAMERAIP;
		LPVOID param = (LPVOID)&m_data;

		OnStartSocketThread(param);
	}

}

// Dpflag  flase： 隐藏  true: 显示  
void CMainDlg::SetRefreshDisplay(bool Dpflag)
{
	SWindow *WinRefresh = FindChildByName(L"win_refresh");

	if (WinRefresh)
	{
		if (Dpflag)
			WinRefresh->SetAttribute(L"show", L"1", FALSE);
		else
			WinRefresh->SetAttribute(L"show", L"0", FALSE);
	}
}

void CMainDlg::BackToSetLocalIp()
{
	STabCtrl *tTab = FindChildByName2<STabCtrl>(L"hide_main");
	if (tTab)
	{
		SetLocalIPView();
		tTab->SetCurSel(_T("ip_setpage"));
	}
}

void CMainDlg::RefreshLocalIp()
{
	SetRefreshDisplay(true);
	OnPlayFromLocalIp();
}

void CMainDlg::OnPlay()
{
	if (!m_cameraUrl.empty() && !m_cameraIp.empty())
	{
		m_player.ffplayWithUrl(m_cameraUrl.c_str());
	}	
	else
	{
		MessageBox(NULL, _T("获取到摄像机链接不成功"), _T("提示"), MB_OK | MB_ICONERROR);
	}
}

void CMainDlg::OnPlayFromCameraIp()
{
	if (CheckIp(0, L"edit_cameraIp", _T("请输入摄像机IP"), m_cameraIp))
	{
		memset(&m_data, 0, sizeof(SocketData));
		m_data.opt = OPT_PLAY_FROMCAMERAIP;
		m_data.data = m_localIp;

		LPVOID param = (LPVOID)&m_data;
		OnStartSocketThread(param);
	}	
}

void CMainDlg::OnPlayFromLocalIp()
{
	if (CheckIp(1, L"edit_localIp", _T("请输入本机IP"), m_localIp))
	{
		memset(&m_data, 0, sizeof(SocketData));
		m_data.opt = OPT_PLAY_FROMLOCALIP;

		LPVOID param = (LPVOID)&m_data;
		OnStartSocketThread(param);
	}
}

UINT CMainDlg::Run(LPVOID data)
{
	MainSocketThread *pEvt = new MainSocketThread(this);
	SocketData *param = (SocketData *)data;

	pEvt->opt = param->opt;
	switch (pEvt->opt)
	{
		case OPT_GETCAMERA_INFO:
		{
			WinSocketClient client;
			memset(&m_cinfo, 0, sizeof(AICameraInfo));
			if (!(pEvt->retOK = client.GetFaceParameter(m_cameraIp.c_str(), m_cinfo.fp)))
				break;
			if (!(pEvt->retOK = client.GetHostIPAddr(m_cameraIp.c_str(), m_cinfo.ipp)))
				break;
			if (!(pEvt->retOK = client.GetAIFunction(m_cameraIp.c_str(), m_cinfo.af)))
				break;
			if (!(pEvt->retOK = client.GetFaceFuncSetting(m_cameraIp.c_str(), m_cinfo.ffs)))
				break;
			pEvt->retOK = client.GetVmd(m_cameraIp.c_str(), m_cinfo.rec);
			break;
		}
		case OPT_UPLOADIP:
		{
			WinSocketClient client;
			pEvt->retOK = client.SetHostIPAddr(m_cameraIp.c_str(), param->ipp);
			break;
		}

		case OPT_SETAIFUNC_FROMCAMERAIP:
		{
			WinSocketClient client;
			pEvt->retOK = client.SetAIFunction(m_cameraIp.c_str(), m_aifunc);
			break;
		}
		
		case OPT_SETFACE_FROMCAMERAIP:
		{
			WinSocketClient client;
			if (!(pEvt->retOK = client.SetFaceParameter(m_cameraIp.c_str(), m_fpara)))
				break;
			if (!(pEvt->retOK = client.SetFaceFuncSetting(m_cameraIp.c_str(), m_ffset)))
				break;
			if (!(pEvt->retOK = client.SetHostIPAddr(m_cameraIp.c_str(), m_dm)))
				break;
			pEvt->retOK = client.SetVmd(m_cameraIp.c_str(), m_rec);
			break;
		}

		case OPT_SETTIME_FROMLOCALIP:
		case OPT_PLAY_FROMLOCALIP:
		{
			char ip[30] = { 0 };
			char url[128] = { 0 };
			UpnpTool Supnp;
			WinSocketClient client;
			UINT8		rec;
			FaceFuncSetting ffset;
			FaceParameter fpara;
			IPPort domain;
			AIFunction aifunc;
			//string panotemplate;
			pEvt->retOK = Supnp.upnpDiscover(3000, m_localIp, m_iplist);
			if (!pEvt->retOK)
				break;

			m_fplist.RemoveAll();
			m_domainlist.RemoveAll();
			m_aflist.RemoveAll();
			m_ffsetlist.RemoveAll(); 
			m_reclist.RemoveAll();
			for (unsigned int i = 0; i < m_iplist.GetCount(); i++)
			{
				rec = 0;
				memset(&ffset, 0, sizeof(FaceFuncSetting));
				memset(&fpara, 0, sizeof(FaceParameter));
				memset(&domain, 0, sizeof(IPPort));
				memset(&aifunc, 0, sizeof(AIFunction));
				//client.GetPanorama(m_iplist.GetAt(i).cameraip, panotemplate);
				client.GetFaceParameter(m_iplist.GetAt(i).cameraip, fpara);
				client.GetHostIPAddr(m_iplist.GetAt(i).cameraip, domain);
				client.GetAIFunction(m_iplist.GetAt(i).cameraip, aifunc);
				client.GetFaceFuncSetting(m_iplist.GetAt(i).cameraip, ffset);
				client.GetVmd(m_iplist.GetAt(i).cameraip, rec);
				m_domainlist.Add(domain);
				m_fplist.Add(fpara);
				m_aflist.Add(aifunc);
				m_ffsetlist.Add(ffset);
				m_reclist.Add(rec);
			}
			break;
		}

		case OPT_SETTIME_FROMCAMERAIP:
		{
			WinSocketClient client;
			DateTime dt;
			GetTime(dt);
			pEvt->retOK = client.SetCameraTime(m_cameraIp.c_str(), dt);
			break;
		}

		case OPT_PLAY_FROMCAMERAIP:
		{
			string id;
			WinSocketClient client;
			pEvt->retOK = client.GetDeviceID(m_cameraIp.c_str(), id);
			if (pEvt->retOK)
			{
				m_cameraUrl = "rtmp://";
				m_cameraUrl.append(m_cameraIp).append(":1935/live/").append(id);
			}
			break;
		}	
	}
	Sleep(500);
	SNotifyCenter::getSingleton().FireEventAsync(pEvt);
	pEvt->Release();
	return 0;
}

void CMainDlg::OnStartSocketThread(LPVOID data)
{
	if (IsRunning())
	{
		MessageBox(NULL, _T("操作太频繁，请稍后再试！"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	SNotifyCenter::getSingleton().addEvent(EVENTID(MainSocketThread));

	BeginThread(data);
}


bool CMainDlg::OnMainSocketThread(EventArgs *e)
{
	MainSocketThread *pEvt = sobj_cast<MainSocketThread>(e);
	switch (pEvt->opt)
	{
		case OPT_GETCAMERA_INFO:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("获取配置失败！"), _T("提示"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				UpdateCameraInfo();
				MessageBox(NULL, _T("获取配置成功！"), _T("提示"), MB_OK);
			}
			
			break;
		case OPT_UPLOADIP:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("没有获取到局域网的设备，\n请重新设置！"), _T("提示"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				MessageBox(NULL, _T("设置主机IP成功！"), _T("提示"), MB_OK);
			}

			break;

		case OPT_SETAIFUNC_FROMCAMERAIP:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("设置功能项失败！"), _T("提示"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				MessageBox(NULL, _T("设置功能项成功！"), _T("提示"), MB_OK);
			}
			break;

		case OPT_SETFACE_FROMCAMERAIP:
			if (!pEvt->retOK || m_cameraIp.empty())
			{
				MessageBox(NULL, _T("设置人脸参数失败！"), _T("提示"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				MessageBox(NULL, _T("设置人脸参数成功！"), _T("提示"), MB_OK);
			}

			break;

		case OPT_SETTIME_FROMLOCALIP:
			if (!pEvt->retOK || !m_iplist.GetCount())
			{
				MessageBox(NULL, _T("设置时间失败，\n请重新设置！"), _T("提示"), MB_OK | MB_ICONERROR);
				return false;
			}

			MessageBox(NULL, _T("设置时间成功！"), _T("提示"), MB_OK);
			break;

		case OPT_PLAY_FROMLOCALIP:
			if (!pEvt->retOK || !m_iplist.GetCount())
			{
				MessageBox(NULL, _T("没有获取到局域网的设备，\n请重新设置！"), _T("提示"), MB_OK | MB_ICONERROR);
				return false;
			}
			PlayFromLocalPage();
			break;

		case OPT_SETTIME_FROMCAMERAIP:
			if (!pEvt->retOK)
			{
				MessageBox(NULL, _T("设置时间失败，\n请重新设置！"), _T("提示"), MB_OK | MB_ICONERROR);
				return false;
			}
			else
			{
				MessageBox(NULL, _T("设置时间成功！"), _T("提示"), MB_OK);
			}
			break;

		case OPT_PLAY_FROMCAMERAIP:
			if (!pEvt->retOK)
			{
				MessageBox(NULL, _T("获取设备ID失败，\n请重新点击按钮！"), _T("提示"), MB_OK | MB_ICONERROR);
				return false;
			}
			OnPlay();
			break;

		default:
			MessageBox(NULL, _T("设置失败，\n请重新设置！"), _T("提示"), MB_OK | MB_ICONERROR);
			break;
	}

	return true;
}
