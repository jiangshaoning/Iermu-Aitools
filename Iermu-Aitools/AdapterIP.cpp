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
	notifyDataSetChanged();
}

bool CAdapterIP::OnItemClick(EventArgs *pEvt)
{
	SWindow *pItem = sobj_cast<SWindow>(pEvt->sender);
	int pos = pItem->GetUserData();

	if (m_dlgMain)
	{
		m_dlgMain->SetCameraInfoPage(pos);
	}

	return true;
}