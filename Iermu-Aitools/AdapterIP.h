#pragma once

#include <helper/SAdapterBase.h>
#include "UpnpTool.h"
#include "MainDlg.h"
#include "WinSocketClient.h"

class CMainDlg;

class CAdapterIP : public SMcAdapterBase
{

public:

	struct SORTCTX
	{
		int iCol;
		SHDSORTFLAG stFlag;
	};

	int					m_nItemHeight[2];
	CMainDlg			*m_dlgMain;

	CAdapterIP(CMainDlg *dlg)
	{
		m_dlgMain = dlg;
	}

	~CAdapterIP()
	{
	}

	virtual int getCount()
	{
		return m_iplist.GetCount();
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}

		CameraAddr *psi = m_iplist.GetData() + position%m_iplist.GetCount();
		pItem->FindChildByName2<SWindow>(L"device_id")->SetWindowText(S_CA2T(m_iplist.GetAt(position).cameraid));
		pItem->FindChildByName2<SWindow>(L"device_ip")->SetWindowText(S_CA2T(m_iplist.GetAt(position).cameraip));
		pItem->FindChildByName2<SWindow>(L"play_url")->SetWindowText(S_CA2T(m_iplist.GetAt(position).url));

		pItem->SetUserData(position);
		pItem->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&CAdapterIP::OnItemClick, this));
	}

	bool OnSort(int iCol, SHDSORTFLAG * stFlags, int nCols)
	{
		if (iCol >= 3) 
			return false;

		SHDSORTFLAG stFlag = stFlags[iCol];
		switch (stFlag)
		{
		case ST_NULL:stFlag = ST_UP; break;
		case ST_DOWN:stFlag = ST_UP; break;
		case ST_UP:stFlag = ST_DOWN; break;
		}
		for (int i = 0; i < nCols; i++)
		{
			stFlags[i] = ST_NULL;
		}
		stFlags[iCol] = stFlag;

		SORTCTX ctx = { iCol, stFlag };
		qsort_s(m_iplist.GetData(), m_iplist.GetCount(), sizeof(CameraAddr), SortCmp, &ctx);
		return true;
	}

	static int __cdecl SortCmp(void *context, const void * p1, const void * p2)
	{
		SORTCTX *pctx = (SORTCTX*)context;
		const CameraAddr *pSI1 = (const CameraAddr*)p1;
		const CameraAddr *pSI2 = (const CameraAddr*)p2;
		int nRet = 0;
		switch (pctx->iCol)
		{
		case 0://ID
			nRet = strcmp(pSI1->cameraid, pSI2->cameraid);
			break;
		case 1://IP
			nRet = strcmp(pSI1->cameraip, pSI2->cameraip);
			break;
		case 2://Url
			nRet = strcmp(pSI1->url, pSI2->url);
			break;
		}
		if (pctx->stFlag == ST_UP)
			nRet = -nRet;
		return nRet;
	}

	SStringW GetColumnName(int iCol) const {
		return SStringW().Format(L"col%d", iCol + 1);
	}


	void SetTags(const SArray<CameraAddr> &tags);
	bool OnItemClick(EventArgs *pEvt);
private:
	SArray<CameraAddr>  m_iplist;
};