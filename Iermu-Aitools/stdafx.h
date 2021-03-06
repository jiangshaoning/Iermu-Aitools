// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#define  _CRT_SECURE_NO_WARNINGS
#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <event/notifycenter.h>
#include <com-cfg.h>
#include "resource.h"

#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"
using namespace SOUI;

#define INT64_MIN       (-9223372036854775807i64 - 1)
#define INT64_MAX       9223372036854775807i64
#define SIZE_T_MAX      0xffffffff
#define DEBUG_NEW		new(THIS_FILE, __LINE__)
/*
* 禁止复制基类
*/
class INoCopy
{
private:
	INoCopy(const INoCopy& rhs);
	INoCopy& operator = (const INoCopy& rhs);

public:
	INoCopy() {};
	~INoCopy() {};
};

char playurl[60];
BOOL AutoLaunch(BOOL bRun /* = TRUE */);
BOOL IsAutoLaunch();


#define RELEASEPLAYER(player) \
if (player) \
{ \
	player_close(player); \
	player = NULL; \
}

#define REMOVE_LINE_COUNT 100
#define SO_UPDATE_ACCEPT_CONTEXT 0x700B
#define MS_REALWND					WM_USER+200
#define MS_OPENVIDEO_REALWND		WM_USER+201				//父子窗口通信
#define MS_CLOSEVIDEO_REALWND		WM_USER+202				//父子窗口通信