#include "stdafx.h"
#include "AdapterIP.h"
#include "PlayerInterface.h"

void CAdapterIP::SetTags(const SArray<CameraAddr> &tags)
{
	int count = tags.GetCount();
	m_iplist.RemoveAll();
	for (int i = 0; i<count; i++)
	{
		CameraAddr obj = tags[i];
		m_iplist.Add(obj);
	}
	//qsort(m_cameraList.GetData(), m_cameraList.GetCount(), sizeof(CameraItem), TagCheckCmp);
	notifyDataSetChanged();
}

void CAdapterIP::SetFPTags(const SArray<FaceParameter> &tags)
{
	int count = tags.GetCount();
	m_fplist.RemoveAll();
	for (int i = 0; i<count; i++)
	{
		FaceParameter obj = tags[i];
		bool show = true;
		m_fplist.Add(obj);
		m_edit_show.Add(show);
	}
	//qsort(m_cameraList.GetData(), m_cameraList.GetCount(), sizeof(CameraItem), TagCheckCmp);
	notifyDataSetChanged();
}

void CAdapterIP::SetAFTags(const SArray<AIFunction> &tags)
{
	int count = tags.GetCount();
	m_aifunclist.RemoveAll();
	for (int i = 0; i<count; i++)
	{
		AIFunction obj = tags[i];
		bool show = true;
		m_aifunclist.Add(obj);
	}
	//qsort(m_cameraList.GetData(), m_cameraList.GetCount(), sizeof(CameraItem), TagCheckCmp);
	notifyDataSetChanged();
}


void CAdapterIP::SetDMTags(const SArray<IPPort> &tags)
{
	int count = tags.GetCount();
	m_domainlist.RemoveAll();
	for (int i = 0; i<count; i++)
	{
		IPPort obj = tags[i];
		m_domainlist.Add(obj);
	}
	//qsort(m_cameraList.GetData(), m_cameraList.GetCount(), sizeof(CameraItem), TagCheckCmp);
	notifyDataSetChanged();
}

void CAdapterIP::SetFFSTags(const SArray<FaceFuncSetting> &tags)
{
	int count = tags.GetCount();
	m_ffslist.RemoveAll();
	for (int i = 0; i<count; i++)
	{
		FaceFuncSetting obj = tags[i];
		m_ffslist.Add(obj);
	}
	//qsort(m_cameraList.GetData(), m_cameraList.GetCount(), sizeof(CameraItem), TagCheckCmp);
	notifyDataSetChanged();
}

void CAdapterIP::SetRECTags(const SArray<UINT8> &tags)
{
	int count = tags.GetCount();
	m_reclist.RemoveAll();
	for (int i = 0; i<count; i++)
	{
		UINT8 obj = tags[i];
		m_reclist.Add(obj);
	}
	//qsort(m_cameraList.GetData(), m_cameraList.GetCount(), sizeof(CameraItem), TagCheckCmp);
	notifyDataSetChanged();
}

bool CAdapterIP::OnPlayButtonClick(EventCmd *pEvt)
{
	//ffmfc_quit();
	SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
	int iItem = pBtn->GetRoot()->GetUserData();
	
	char *url = m_iplist.GetAt(iItem).url;
	if (url && strlen(url) > 0)
	{
		//ffmfc_play((LPVOID)url);
		m_dlgMain->m_player.ffplayWithUrl(url);
	}
	else
	{
		MessageBox(NULL, _T("获取到摄像机链接不成功"), _T("提示"), MB_OK | MB_ICONERROR);
	}
	return true;
}

bool CAdapterIP::OnSetTimeButtonClick(EventCmd *pEvt)
{
	SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
	int iItem = pBtn->GetRoot()->GetUserData();

	string ip = m_iplist.GetAt(iItem).cameraip;
	if (!ip.empty())
	{
		m_dlgMain->SetCameraIP(ip);
		m_dlgMain->SetCameraTime();
	}
	else
	{
		MessageBox(NULL, _T("获取到摄像机IP不成功"), _T("提示"), MB_OK | MB_ICONERROR);
	}
	return true;
}

//bool CAdapterIP::OnSetIPButtonClick(EventCmd *pEvt)
//{
//	IPPort dm;
//	//int port = 0xffff, cpport = 0xffff;
//	//unsigned char default[2] = { 0xff, 0xff };
//	int port = 0, cpport = 0;
//
//	memset(&dm, 0, sizeof(IPPort));
//	SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
//	SWindow *win = pBtn->GetRoot();
//	int iItem = win->GetUserData();
//
//	string ip = m_iplist.GetAt(iItem).cameraip;
//
//	SEdit *edit_hostIp = win->FindChildByName2<SEdit>(L"edit_hostIp_local");
//	SEdit *edit_hostPort = win->FindChildByName2<SEdit>(L"edit_hostPort_local");
//	SEdit *edit_libname = win->FindChildByName2<SEdit>(L"edit_libname_local");
//	SEdit *edit_cphostIp = win->FindChildByName2<SEdit>(L"edit_cphostIp_local");
//	SEdit *edit_cphostPort = win->FindChildByName2<SEdit>(L"edit_cphostPort_local");
//	SComboBox *sCbx = win->FindChildByName2<SComboBox>(L"item_cbx_lib");
//
//	SStringT  hostIp = edit_hostIp->GetWindowTextW();
//	SStringT  hostPort = edit_hostPort->GetWindowTextW();
//	SStringT  libname = edit_libname->GetWindowTextW();
//	SStringT  cphostIp = edit_cphostIp->GetWindowTextW();
//	SStringT  cphostPort = edit_cphostPort->GetWindowTextW();
//
//	if (hostIp.GetLength() > 60)
//	{
//		MessageBox(NULL, _T("上传域名或IP长度不得超过60字符"), _T("提示"), MB_OK | MB_ICONERROR);
//		return false;
//	}
//
//	if (cphostIp.GetLength() > 60)
//	{
//		MessageBox(NULL, _T("比对域名或IP长度不得超过60字符"), _T("提示"), MB_OK | MB_ICONERROR);
//		return false;
//	}
//
//	if (libname.GetLength() > 119)
//	{
//		MessageBox(NULL, _T("库名不得超过119字符"), _T("提示"), MB_OK | MB_ICONERROR);
//		return false;
//	}
//	if (hostPort.GetLength() > 0)
//	{
//		if (!((swscanf(hostPort, L"%d", &port) == 1) && (port >= 0 && port <= 65535)))
//		{
//			MessageBox(NULL, _T("上传端口号值不正确，请输入正确的端口号"), _T("提示"), MB_OK | MB_ICONERROR);
//			return false;
//		}
//	}
//
//	if (cphostPort.GetLength() > 0)
//	{
//		if (!((swscanf(cphostPort, L"%d", &cpport) == 1) && (cpport >= 0 && cpport <= 65535)))
//		{
//			MessageBox(NULL, _T("比对端口号值不正确，请输入正确的端口号"), _T("提示"), MB_OK | MB_ICONERROR);
//			return false;
//		}
//	}
//
//	SStringA hostipA = S_CT2A(hostIp);
//	hostipA.GetLength() > 0 ? memcpy(dm.domain, hostipA.GetBuffer(hostipA.GetLength()), hostipA.GetLength())
//						: memset(dm.domain, 0, sizeof(dm.domain));
//	SStringA cphostipA = S_CT2A(cphostIp);
//	cphostipA.GetLength() > 0 ? memcpy(dm.cp_domain, cphostipA.GetBuffer(cphostipA.GetLength()), cphostipA.GetLength())
//						: memset(dm.cp_domain, 0, sizeof(dm.cp_domain));
//	SStringA libnameA = S_CT2A(libname);
//	libnameA.GetLength() > 0 ? memcpy(dm.sw_id, libnameA.GetBuffer(libnameA.GetLength()), libnameA.GetLength())
//						: memset(dm.sw_id, 0, sizeof(dm.sw_id));
//	dm.port = port;
//	dm.cp_port = cpport;
//	dm.lib_type = sCbx->GetCurSel();
//
//	if (!ip.empty())
//	{
//		m_dlgMain->SetCameraIP(ip);
//		m_dlgMain->UploadIPRequest(dm);
//		m_domainlist.SetAt(iItem, dm);
//	}
//	else
//	{
//		MessageBox(NULL, _T("获取到摄像机IP不成功"), _T("提示"), MB_OK | MB_ICONERROR);
//	}
//
//	return true;
//}

bool CAdapterIP::OnSetFaceButtonClick(EventCmd *pEvt)
{
	IPPort dm;
	UINT8 rec;
	FaceParameter fpara;
	FaceFuncSetting ffset;
	//unsigned int  facialval = 0xff, facesval = 0xff, widthval = 0xffff, heightval = 0xffff, relival = 0xffff, timeval = 0xffff;
	//unsigned char default[2] = { 0xff, 0xff };
	unsigned int  facialval = 0, facesval = 0, widthval = 0, heightval = 0, relival = 0, timeval = 0, port = 0, cpport = 0;

	SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
	SWindow *win = pBtn->GetRoot();
	int iItem = win->GetUserData();

	string ip = m_iplist.GetAt(iItem).cameraip;

	if (!ip.empty())
	{
		m_dlgMain->SetCameraIP(ip);
	}
	else
	{
		MessageBox(NULL, _T("获取到摄像机IP不成功"), _T("提示"), MB_OK | MB_ICONERROR);
	}

	SWindow * item_face_img = win->FindChildByName(L"item_face_img")->GetSelectedChildInGroup();
	SCheckBox *item_check_1 = win->FindChildByName2<SCheckBox>(L"item_check_1");
	SCheckBox *item_check_2 = win->FindChildByName2<SCheckBox>(L"item_check_2");
	SCheckBox *item_check_3 = win->FindChildByName2<SCheckBox>(L"item_check_3");
	SCheckBox *item_check_4 = win->FindChildByName2<SCheckBox>(L"item_check_4");
	SCheckBox *item_check_5 = win->FindChildByName2<SCheckBox>(L"item_check_5");
	SCheckBox *item_check_6 = win->FindChildByName2<SCheckBox>(L"item_check_6");
	SCheckBox *item_check_7 = win->FindChildByName2<SCheckBox>(L"item_check_7");

	SEdit *edit_api_key = win->FindChildByName2<SEdit>(L"edit_api_key");
	SEdit *edit_secret_key = win->FindChildByName2<SEdit>(L"edit_secret_key");
	SEdit *edit_isnot_facial = win->FindChildByName2<SEdit>(L"edit_isnot_facial");
	SEdit *eidt_faces_second = win->FindChildByName2<SEdit>(L"eidt_faces_second");
	SEdit *edit_pixel_width = win->FindChildByName2<SEdit>(L"edit_pixel_width");
	SEdit *edit_pixel_height = win->FindChildByName2<SEdit>(L"edit_pixel_height");
	SEdit *edit_reliability = win->FindChildByName2<SEdit>(L"edit_reliability");
	SEdit *edit_residence_time = win->FindChildByName2<SEdit>(L"edit_residence_time");
	//SEdit *edit_group_id = win->FindChildByName2<SEdit>(L"edit_group_id");

	SComboBox * tCbx = win->FindChildByName2<SComboBox>(L"item_cbx_rec");

	//图片上传配置检查
	SEdit *edit_hostIp = win->FindChildByName2<SEdit>(L"edit_hostIp_local");
	SEdit *edit_hostPort = win->FindChildByName2<SEdit>(L"edit_hostPort_local");
	SEdit *edit_libname = win->FindChildByName2<SEdit>(L"edit_libname_local");
	SEdit *edit_cphostIp = win->FindChildByName2<SEdit>(L"edit_cphostIp_local");
	SEdit *edit_cphostPort = win->FindChildByName2<SEdit>(L"edit_cphostPort_local");
	SComboBox *sCbx = win->FindChildByName2<SComboBox>(L"item_cbx_lib");
	SCheckBox *item_check_https = win->FindChildByName2<SCheckBox>(L"item_check_https");


	int face_img_id = item_face_img->GetID();
	SStringT tapi_key = edit_api_key->GetWindowTextW();
	SStringT tsecret_key = edit_secret_key->GetWindowTextW();
	SStringT tisnot_facial = edit_isnot_facial->GetWindowTextW();
	SStringT tfaces_second = eidt_faces_second->GetWindowTextW();
	SStringT tpixel_width = edit_pixel_width->GetWindowTextW();
	SStringT tpixel_height = edit_pixel_height->GetWindowTextW();
	SStringT treliability = edit_reliability->GetWindowTextW();
	SStringT tresidence_time = edit_residence_time->GetWindowTextW();
	//SStringT tgroup_id = edit_group_id->GetWindowTextW();

	SStringT  hostIp = edit_hostIp->GetWindowTextW();
	SStringT  hostPort = edit_hostPort->GetWindowTextW();
	SStringT  libname = edit_libname->GetWindowTextW();
	SStringT  cphostIp = edit_cphostIp->GetWindowTextW();
	SStringT  cphostPort = edit_cphostPort->GetWindowTextW();

	if (tapi_key.GetLength() > 32)
	{
		MessageBox(NULL, _T("API_KEY输入格式不正确"), _T("提示"), MB_OK | MB_ICONERROR);
		return false;
	}

	if (tsecret_key.GetLength() > 32)
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

	//if (tgroup_id.GetLength() > 4)
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
	memset(fpara.api_key, 0, sizeof(fpara.api_key));
	api_key.length() > 0 ? memcpy(fpara.api_key, api_key.c_str(), api_key.length()) : memset(fpara.api_key, 0, sizeof(fpara.api_key));


	tmpstr = S_CT2A(tsecret_key);
	string secret_key = tmpstr;
	memset(fpara.secret_key, 0, sizeof(fpara.secret_key));
	secret_key.length() > 0 ? memcpy(fpara.secret_key, secret_key.c_str(), secret_key.length()) : memset(fpara.secret_key, 0, sizeof(fpara.secret_key));

	switch (face_img_id)
	{
		case 2000:
			fpara.face_img = 0;
			break;
		case 2001:
			fpara.face_img = 1;
			break;
		case 2002:
			fpara.face_img = 2;
			break;
		case 2003:
			fpara.face_img = 3;
			break;
	}

	fpara.box = (item_check_1->IsChecked() ? 1 : 0) | (item_check_2->IsChecked() ? (1 << 3) : 0) |
		(item_check_3->IsChecked() ? (1 << 4) : 0) | (item_check_4->IsChecked() ? (1 << 6) : 0);

	ffset.face_rec = (item_check_5->IsChecked() ? 1 : 0);
	ffset.face_cod = (item_check_6->IsChecked() ? 1 : 0);
	ffset.face_cou = (item_check_7->IsChecked() ? 1 : 0);
	m_ffslist.SetAt(iItem, ffset);

	fpara.facial = facialval;
	fpara.faces = facesval;
	fpara.width = widthval;
	fpara.height = heightval;
	fpara.reliability = relival;
	fpara.res_time = timeval;

	tCbx->GetCurSel();
	switch (tCbx->GetCurSel())
	{
	case 0:
		rec = 0;
		break;
	case 1:
		rec = 0x80;
		break;
	case 2:
		rec = 1;
		break;
	}
	m_reclist.SetAt(iItem, rec);

	//tmpstr = S_CT2A(tgroup_id);
	//string group_id = tmpstr;
	//memset(fpara.groupID, 0, sizeof(fpara.groupID));
	//group_id.length() > 0 ? memcpy(fpara.groupID, group_id.c_str(), group_id.length()) : memcpy(fpara.groupID, default, sizeof(default));
	memset(fpara.groupID, 0, sizeof(fpara.groupID));
	m_fplist.SetAt(iItem, fpara);

	memset(&dm, 0, sizeof(IPPort));
	SStringA hostipA = S_CT2A(hostIp);
	hostipA.GetLength() > 0 ? memcpy(dm.domain, hostipA.GetBuffer(hostipA.GetLength()), hostipA.GetLength())
		: memset(dm.domain, 0, sizeof(dm.domain));
	SStringA cphostipA = S_CT2A(cphostIp);
	cphostipA.GetLength() > 0 ? memcpy(dm.cp_domain, cphostipA.GetBuffer(cphostipA.GetLength()), cphostipA.GetLength())
		: memset(dm.cp_domain, 0, sizeof(dm.cp_domain));
	SStringA libnameA = S_CT2A(libname);
	libnameA.GetLength() > 0 ? memcpy(dm.sw_id, libnameA.GetBuffer(libnameA.GetLength()), libnameA.GetLength())
		: memset(dm.sw_id, 0, sizeof(dm.sw_id));
	dm.port = port;
	dm.cp_port = cpport;
	dm.lib_type = sCbx->GetCurSel() | (item_check_https->IsChecked() ? (1 << 7) : 0);
	m_domainlist.SetAt(iItem, dm);

	m_dlgMain->SetFaceParameterFromLoalIP(rec, fpara, ffset, dm);
	return true;
}

bool CAdapterIP::OnSetAIFuncButtonClick(EventCmd *pEvt)
{
	AIFunction aifunc;
	UINT8 memsize = 0;
	unsigned int jpgmem = 2, decimal = 0;;
	unsigned char head[4] = { 0x72, 0x2a, 0x3c, 0x6e };
	SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
	SWindow *win = pBtn->GetRoot();
	int iItem = win->GetUserData();

	string ip = m_iplist.GetAt(iItem).cameraip;

	if (!ip.empty())
	{
		m_dlgMain->SetCameraIP(ip);
	}

	SEdit *edit_jpgmem = win->FindChildByName2<SEdit>(L"edit_jpgmem_local");
	SCheckBox *check_lan = win->FindChildByName2<SCheckBox>(L"item_check_lan");
	SCheckBox *check_network = win->FindChildByName2<SCheckBox>(L"item_check_network");
	SCheckBox *check_savejpg = win->FindChildByName2<SCheckBox>(L"item_check_savejpg");
	SComboBox * pCbx = win->FindChildByName2<SComboBox>(L"item_cbx_ai");
	//SComboBox * jCbx = win->FindChildByName2<SComboBox>(L"item_cbx_size");

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

	memset(&aifunc, 0, sizeof(AIFunction));
	memcpy(aifunc.head, head, sizeof(head));
	aifunc.lan = (check_lan->IsChecked() ? (1 << 7) : 0) | (check_network->IsChecked() ? (1 << 6) : 0) | (check_savejpg->IsChecked() ? (1 << 5) : 0);
	aifunc.comparison = pCbx->GetCurSel();
	//memsize = jCbx->GetCurSel();
	if (jpgmem >= 1)
		aifunc.jpgmem = jpgmem | (1 << 15);
	else
	{
		aifunc.jpgmem = (int)(djpgmem * 1000);
	}

	m_aifunclist.SetAt(iItem, aifunc);

	m_dlgMain->SetAIFunctionFromLoalIP(aifunc);
	return true;
}