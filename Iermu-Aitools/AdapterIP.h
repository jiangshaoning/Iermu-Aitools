#pragma once

#include <helper/SAdapterBase.h>
#include "UpnpTool.h"
#include "MainDlg.h"
#include "WinSocketClient.h"

class CMainDlg;

class CAdapterIP : public SAdapterBase
{

public:
	int					m_nItemHeight[2];
	CMainDlg			*m_dlgMain;

	CAdapterIP(CMainDlg *dlg)
	{
		m_dlgMain = dlg;
		m_curopenpos = -1;
	}

	~CAdapterIP()
	{
	}

	virtual void InitByTemplate(pugi::xml_node xmlTemplate)
	{
		m_nItemHeight[0] = xmlTemplate.attribute(L"evenHeight").as_int(50);
		m_nItemHeight[1] = xmlTemplate.attribute(L"evenSelHeight").as_int(60);
	}

	virtual int getCount()
	{
		return m_iplist.GetCount();
	}

	virtual int getViewTypeCount(){ return 2; }

	virtual int getItemViewType(int position, DWORD dwState)
	{
		if (dwState & (WndState_PushDown | WndState_Check))
			return 1;//even lines with check state
		else
			return 0;//even lines 
	}

	bool OnItemStateChanged(EventArgs *e)
	{
		EventSwndStateChanged *e2 = sobj_cast<EventSwndStateChanged>(e);
		if (!e2->CheckState(WndState_PushDown | WndState_Check)) return false;
		//通知界面重绘
		notifyDataSetInvalidated();
		return true;
	}

	virtual SIZE getViewDesiredSize(int position, SWindow *pItem, LPCRECT prcContainer)
	{
		DWORD dwState = pItem->GetState();
		int viewType = getItemViewType(position, dwState);
		return CSize(0, m_nItemHeight[viewType]);//cx在listview，mclistview中没有使用，不需要计算
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		//nViewType: 0:收缩状态，1：打开状态
		int nViewType = getItemViewType(position, pItem->GetState());
		if (pItem->GetChildrenCount() == 0)
		{	
			if (nViewType == 0)
				pItem->InitFromXml(xmlTemplate.child(L"itemEven"));
			else
				pItem->InitFromXml(xmlTemplate.child(L"itemEvenHover"));
		}
		pItem->GetEventSet()->subscribeEvent(EventSwndStateChanged::EventID, Subscriber(&CAdapterIP::OnItemStateChanged, this));

		if (m_fplist.GetCount() && nViewType == 1 && m_curopenpos != position)
		{	
			UINT8 rec = m_reclist.GetAt(position);
			FaceParameter fp = m_fplist.GetAt(position);
			AIFunction af = m_aifunclist.GetAt(position);
			IPPort domain = m_domainlist.GetAt(position);
			FaceFuncSetting ffset = m_ffslist.GetAt(position);

			SCheckBox *check_1 = pItem->FindChildByName2<SCheckBox>(L"item_check_1");
			SCheckBox *check_2 = pItem->FindChildByName2<SCheckBox>(L"item_check_2");
			SCheckBox *check_3 = pItem->FindChildByName2<SCheckBox>(L"item_check_3");
			SCheckBox *check_4 = pItem->FindChildByName2<SCheckBox>(L"item_check_4");
			SCheckBox *check_5 = pItem->FindChildByName2<SCheckBox>(L"item_check_5");
			SCheckBox *check_6 = pItem->FindChildByName2<SCheckBox>(L"item_check_6");
			SCheckBox *check_7 = pItem->FindChildByName2<SCheckBox>(L"item_check_7");
			SRadioBox *radio_1 = pItem->FindChildByName2<SRadioBox>(L"item_radio_1");
			SRadioBox *radio_2 = pItem->FindChildByName2<SRadioBox>(L"item_radio_2");
			SRadioBox *radio_3 = pItem->FindChildByName2<SRadioBox>(L"item_radio_3");
			SRadioBox *radio_4 = pItem->FindChildByName2<SRadioBox>(L"item_radio_4");
			SCheckBox *item_check_lan = pItem->FindChildByName2<SCheckBox>(L"item_check_lan");
			SCheckBox *item_check_https = pItem->FindChildByName2<SCheckBox>(L"item_check_https");
			SCheckBox *item_check_network = pItem->FindChildByName2<SCheckBox>(L"item_check_network");
			SCheckBox *item_check_savejpg = pItem->FindChildByName2<SCheckBox>(L"item_check_savejpg");

			SEdit *edit_api_key = pItem->FindChildByName2<SEdit>(L"edit_api_key");
			SEdit *edit_secret_key = pItem->FindChildByName2<SEdit>(L"edit_secret_key");
			SEdit *edit_isnot_facial = pItem->FindChildByName2<SEdit>(L"edit_isnot_facial");
			SEdit *eidt_faces_second = pItem->FindChildByName2<SEdit>(L"eidt_faces_second");
			SEdit *edit_pixel_width = pItem->FindChildByName2<SEdit>(L"edit_pixel_width");
			SEdit *edit_pixel_height = pItem->FindChildByName2<SEdit>(L"edit_pixel_height");
			SEdit *edit_reliability = pItem->FindChildByName2<SEdit>(L"edit_reliability");
			SEdit *edit_residence_time = pItem->FindChildByName2<SEdit>(L"edit_residence_time");
			//SEdit *edit_group_id = pItem->FindChildByName2<SEdit>(L"edit_group_id");

			SEdit *edit_hostIp_local = pItem->FindChildByName2<SEdit>(L"edit_hostIp_local");
			SEdit *edit_hostPort_local = pItem->FindChildByName2<SEdit>(L"edit_hostPort_local");

			SEdit *edit_cphostIp_local = pItem->FindChildByName2<SEdit>(L"edit_cphostIp_local");
			SEdit *edit_cphostPort_local = pItem->FindChildByName2<SEdit>(L"edit_cphostPort_local");

			SEdit *edit_libname_local = pItem->FindChildByName2<SEdit>(L"edit_libname_local");

			SEdit *edit_jpgmem = pItem->FindChildByName2<SEdit>(L"edit_jpgmem_local");



			SComboBox *tCbx = pItem->FindChildByName2<SComboBox>(L"item_cbx_rec");
			SComboBox *pCbx = pItem->FindChildByName2<SComboBox>(L"item_cbx_ai");
			SComboBox *sCbx = pItem->FindChildByName2<SComboBox>(L"item_cbx_lib");
			//SComboBox *jCbx = pItem->FindChildByName2<SComboBox>(L"item_cbx_size");

			SStringA Aapi = (char *)fp.api_key;
			SStringT Tapi = S_CA2T(Aapi);
			edit_api_key->SetWindowTextW(SStringT().Format(_T("%s"), Tapi));

			SStringA Asecr = (char *)fp.secret_key;
			SStringT Tsecr = S_CA2T(Asecr);
			edit_secret_key->SetWindowTextW(SStringT().Format(_T("%s"), Tsecr));

			(fp.facial >= 0 && fp.facial <0xff) ? edit_isnot_facial->SetWindowTextW(SStringT().Format(_T("%d"), fp.facial)) :
						edit_isnot_facial->SetWindowTextW(SStringT().Format(_T("")));

			(fp.faces >= 0 && fp.faces <0xff) ? eidt_faces_second->SetWindowTextW(SStringT().Format(_T("%d"), fp.faces)) :
						eidt_faces_second->SetWindowTextW(SStringT().Format(_T("")));

			(fp.width >= 0 && fp.width <0xffff)  ? edit_pixel_width->SetWindowTextW(SStringT().Format(_T("%d"), fp.width)) :
							edit_pixel_width->SetWindowTextW(SStringT().Format(_T("")));

			(fp.height >= 0 && fp.height <0xffff) ? edit_pixel_height->SetWindowTextW(SStringT().Format(_T("%d"), fp.height)) :
							edit_pixel_height->SetWindowTextW(SStringT().Format(_T("")));

			(fp.reliability >= 0 && fp.reliability <0xffff) ? edit_reliability->SetWindowTextW(SStringT().Format(_T("%d"), fp.reliability)) :
								edit_reliability->SetWindowTextW(SStringT().Format(_T("")));

			(fp.res_time >= 0 && fp.res_time <0xffff) ? edit_residence_time->SetWindowTextW(SStringT().Format(_T("%d"), fp.res_time)) :
							edit_residence_time->SetWindowTextW(SStringT().Format(_T("")));


			SStringA Ahoip= (char *)domain.domain;
			SStringT Thoip = S_CA2T(Ahoip);
			edit_hostIp_local->SetWindowTextW(SStringT().Format(_T("%s"), Thoip));

			(domain.port >= 0 && domain.port <0xffff) ? edit_hostPort_local->SetWindowTextW(SStringT().Format(_T("%d"), domain.port)) :
				edit_hostPort_local->SetWindowTextW(SStringT().Format(_T("80")));

			SStringA Acphoip = (char *)domain.cp_domain;
			SStringT Tcphoip = S_CA2T(Acphoip);
			edit_cphostIp_local->SetWindowTextW(SStringT().Format(_T("%s"), Tcphoip));

			(domain.cp_port >= 0 && domain.cp_port <0xffff) ? edit_cphostPort_local->SetWindowTextW(SStringT().Format(_T("%d"), domain.cp_port)) :
				edit_cphostPort_local->SetWindowTextW(SStringT().Format(_T("80")));

			SStringA Aswid = (char *)domain.sw_id;
			SStringT Tswid = S_CA2T(Aswid);
			edit_libname_local->SetWindowTextW(SStringT().Format(_T("%s"), Tswid));

			if (af.jpgmem >= 0 && af.jpgmem < 0xffff)
			{
				(af.jpgmem >> 15 > 0) ? edit_jpgmem->SetWindowTextW(SStringT().Format(_T("%d"), af.jpgmem & 0x7FFF)) : edit_jpgmem->SetWindowTextW(SStringT().Format(_T("%.03f"), (double)(af.jpgmem & 0x7FFF) / 1000));
			}
			else
			{
				edit_jpgmem->SetWindowTextW(SStringT().Format(_T("")));
			}
			//(af.jpgmem >= 0 && af.jpgmem <0xffff) ? edit_jpgmem->SetWindowTextW(SStringT().Format(_T("%d"), af.jpgmem & 0x7FFF)) :
			//	edit_jpgmem->SetWindowTextW(SStringT().Format(_T("")));
			//jCbx->SetCurSel(af.jpgmem >> 15);

			if (fp.face_img == 0)
				radio_1->SetCheck(true);
			else
				radio_1->SetCheck(false);

			if (fp.face_img == 1)
				radio_2->SetCheck(true);
			else
				radio_2->SetCheck(false);

			if (fp.face_img == 2)
				radio_3->SetCheck(true);
			else
				radio_3->SetCheck(false);

			if (fp.face_img == 3)
				radio_4->SetCheck(true);
			else
				radio_4->SetCheck(false);

			if (fp.box & 1)
				check_1->SetCheck(true);
			else
				check_1->SetCheck(false);

			if (fp.box>>3 & 1)
				check_2->SetCheck(true);
			else
				check_2->SetCheck(false);

			if (fp.box>>4 & 1)
				check_3->SetCheck(true);
			else
				check_3->SetCheck(false);

			if (fp.box>>6 & 1)
				check_4->SetCheck(true);
			else
				check_4->SetCheck(false);

			if (ffset.face_rec & 1)
				check_5->SetCheck(true);
			else
				check_5->SetCheck(false);

			if (ffset.face_cod & 1)
				check_6->SetCheck(true);
			else
				check_6->SetCheck(false);

			if (ffset.face_cou & 1)
				check_7->SetCheck(true);
			else
				check_7->SetCheck(false);
			
			if(af.lan>>7 & 1)
				item_check_lan->SetCheck(true);
			else
				item_check_lan->SetCheck(false);

			if (af.lan >> 6 & 1)
				item_check_network->SetCheck(true);
			else
				item_check_network->SetCheck(false);

			if (af.lan >> 5 & 1)
				item_check_savejpg->SetCheck(true);
			else
				item_check_savejpg->SetCheck(false);


			if (domain.lib_type >> 7 & 1)
				item_check_https->SetCheck(true);
			else
				item_check_https->SetCheck(false);

			pCbx->SetCurSel(af.comparison);
			sCbx->SetCurSel(domain.lib_type & 1);

			switch (rec)
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

			//SStringA Aid = (char *)fp.groupID;
			//SStringT Tid = S_CA2T(Aid);
			//edit_group_id->SetWindowTextW(SStringT().Format(_T("%s"), Tid));
			m_edit_show.GetAt(position) = false;
			m_curopenpos = position;
		}


		SWindow *ptext = pItem->FindChildByName2<SWindow>(L"ip_item");
		SStringA cameraip = m_iplist.GetAt(position).url;
		ptext->SetWindowText(S_CA2T(cameraip));

		
		if (m_fplist.GetCount() && nViewType == 1)
		{
			SWindow *pBtn_play = pItem->FindChildByName2<SButton>(L"item_play");
			pBtn_play->GetRoot()->SetUserData(position);

			SWindow *pBtn_setTime = pItem->FindChildByName2<SButton>(L"item_timeSet");
			pBtn_setTime->GetRoot()->SetUserData(position);


			SWindow *pBtn_setip = pItem->FindChildByName2<SButton>(L"item_setFace_parameter");
			pBtn_setip->GetRoot()->SetUserData(position);

			//SWindow *pBtn_hostip = pItem->FindChildByName2<SButton>(L"item_hostIp");
			//pBtn_hostip->GetRoot()->SetUserData(position);

			SWindow *pBtn_setAiFunc = pItem->FindChildByName2<SButton>(L"item_setAI_function");
			pBtn_setAiFunc->GetRoot()->SetUserData(position);

			//由 修改的新事件订阅方式,采用模板函数从响应函数中自动提取事件类型，
			pBtn_play->GetEventSet()->subscribeEvent(&CAdapterIP::OnPlayButtonClick, this);
			pBtn_setTime->GetEventSet()->subscribeEvent(&CAdapterIP::OnSetTimeButtonClick, this);
			pBtn_setip->GetEventSet()->subscribeEvent(&CAdapterIP::OnSetFaceButtonClick, this);
			//pBtn_hostip->GetEventSet()->subscribeEvent(&CAdapterIP::OnSetIPButtonClick, this);
			pBtn_setAiFunc->GetEventSet()->subscribeEvent(&CAdapterIP::OnSetAIFuncButtonClick, this);
		}

	}

	void SetTags(const SArray<CameraAddr> &tags);
	void SetFPTags(const SArray<FaceParameter> &tags);
	void SetAFTags(const SArray<AIFunction> &tags);
	void SetDMTags(const SArray<IPPort> &tags);
	void SetFFSTags(const SArray<FaceFuncSetting> &tags);
	void SetRECTags(const SArray<UINT8> &tags);
	bool OnPlayButtonClick(EventCmd *pEvt);
	bool OnSetTimeButtonClick(EventCmd *pEvt);
	bool OnSetFaceButtonClick(EventCmd *pEvt);
	//bool OnSetIPButtonClick(EventCmd *pEvt);
	bool OnSetAIFuncButtonClick(EventCmd *pEvt);
private:
	int				  m_curopenpos;
	SArray<bool>      m_edit_show;
	SArray<UINT8>      m_reclist;
	SArray<FaceFuncSetting> m_ffslist;
	SArray<IPPort>	 m_domainlist;
	SArray<FaceParameter> m_fplist;
	SArray<CameraAddr>  m_iplist;
	SArray<AIFunction> m_aifunclist;
};