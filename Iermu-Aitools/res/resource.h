//stamp:074e0f07eb35d7a3
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#pragma once
#include <res.mgr/snamedvalue.h>
#define ROBJ_IN_CPP \
namespace SOUI \
{\
    const _R R;\
    const _UIRES UIRES;\
}
namespace SOUI
{
	class _UIRES{
		public:
		class _UIDEF{
			public:
			_UIDEF(){
				XML_INIT = _T("UIDEF:XML_INIT");
			}
			const TCHAR * XML_INIT;
		}UIDEF;
		class _LAYOUT{
			public:
			_LAYOUT(){
				XML_MAINWND = _T("LAYOUT:XML_MAINWND");
			}
			const TCHAR * XML_MAINWND;
		}LAYOUT;
		class _values{
			public:
			_values(){
				string = _T("values:string");
				color = _T("values:color");
				skin = _T("values:skin");
			}
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
		}values;
		class _IMG{
			public:
			_IMG(){
				ID_TAB_MAIN = _T("IMG:ID_TAB_MAIN");
				png_tab_left = _T("IMG:png_tab_left");
			}
			const TCHAR * ID_TAB_MAIN;
			const TCHAR * png_tab_left;
		}IMG;
		class _ICON{
			public:
			_ICON(){
				ICON_LOGO = _T("ICON:ICON_LOGO");
			}
			const TCHAR * ICON_LOGO;
		}ICON;
	};
	const SNamedID::NAMEDVALUE namedXmlID[]={
		{L"btn_back",65543},
		{L"btn_close",65536},
		{L"btn_localIpSet",65542},
		{L"btn_max",65537},
		{L"btn_min",65539},
		{L"btn_play_fc",65609},
		{L"btn_ref",65544},
		{L"btn_restore",65538},
		{L"btn_setAI_function",65607},
		{L"btn_setFace_parameter",65601},
		{L"btn_timeSet",65608},
		{L"cbx_account",65541},
		{L"cbx_ai",65605},
		{L"cbx_lib",65598},
		{L"cbx_rec",65600},
		{L"check_1",65586},
		{L"check_2",65587},
		{L"check_3",65588},
		{L"check_4",65589},
		{L"check_5",65590},
		{L"check_6",65591},
		{L"check_7",65592},
		{L"check_autoserver",65615},
		{L"check_dirbrowse",65616},
		{L"check_https",65599},
		{L"check_lan",65602},
		{L"check_network",65603},
		{L"check_savejpg",65604},
		{L"edit_api_key",65559},
		{L"edit_cameraIp",65583},
		{L"edit_cphostIp",65596},
		{L"edit_cphostIp_local",65570},
		{L"edit_cphostPort",65597},
		{L"edit_cphostPort_local",65571},
		{L"edit_hostIp",65593},
		{L"edit_hostIp_local",65567},
		{L"edit_hostPort",65594},
		{L"edit_hostPort_local",65568},
		{L"edit_isnot_facial",65561},
		{L"edit_jpgmem",65606},
		{L"edit_jpgmem_local",65580},
		{L"edit_libname",65595},
		{L"edit_libname_local",65569},
		{L"edit_picturepath",65613},
		{L"edit_pixel_height",65565},
		{L"edit_pixel_width",65564},
		{L"edit_reliability",65563},
		{L"edit_residence_time",65566},
		{L"edit_secret_key",65560},
		{L"edit_serverport",65617},
		{L"eidt_faces_second",65562},
		{L"get_camera_info",65584},
		{L"grp_face_img",65585},
		{L"hide_main",65540},
		{L"ip_item",65547},
		{L"item_cbx_ai",65579},
		{L"item_cbx_lib",65572},
		{L"item_cbx_rec",65574},
		{L"item_check_1",65552},
		{L"item_check_2",65553},
		{L"item_check_3",65554},
		{L"item_check_4",65555},
		{L"item_check_5",65556},
		{L"item_check_6",65557},
		{L"item_check_7",65558},
		{L"item_check_https",65573},
		{L"item_check_lan",65576},
		{L"item_check_network",65577},
		{L"item_check_savejpg",65578},
		{L"item_face_img",65551},
		{L"item_play",65549},
		{L"item_radio_1",2000},
		{L"item_radio_2",2001},
		{L"item_radio_3",2002},
		{L"item_radio_4",2003},
		{L"item_setAI_function",65582},
		{L"item_setFace_parameter",65575},
		{L"item_timeSet",65548},
		{L"jpgmem_size",65581},
		{L"lv_ip_play",65546},
		{L"radio_1",1000},
		{L"radio_2",1001},
		{L"radio_3",1002},
		{L"radio_4",1003},
		{L"radio_face_img",65550},
		{L"server_status",65610},
		{L"set_picturepath",65614},
		{L"set_server",65618},
		{L"start_server",65611},
		{L"stop_server",65612},
		{L"win_refresh",65545}		};
	class _R{
	public:
		class _name{
		public:
		_name(){
			btn_back = namedXmlID[0].strName;
			btn_close = namedXmlID[1].strName;
			btn_localIpSet = namedXmlID[2].strName;
			btn_max = namedXmlID[3].strName;
			btn_min = namedXmlID[4].strName;
			btn_play_fc = namedXmlID[5].strName;
			btn_ref = namedXmlID[6].strName;
			btn_restore = namedXmlID[7].strName;
			btn_setAI_function = namedXmlID[8].strName;
			btn_setFace_parameter = namedXmlID[9].strName;
			btn_timeSet = namedXmlID[10].strName;
			cbx_account = namedXmlID[11].strName;
			cbx_ai = namedXmlID[12].strName;
			cbx_lib = namedXmlID[13].strName;
			cbx_rec = namedXmlID[14].strName;
			check_1 = namedXmlID[15].strName;
			check_2 = namedXmlID[16].strName;
			check_3 = namedXmlID[17].strName;
			check_4 = namedXmlID[18].strName;
			check_5 = namedXmlID[19].strName;
			check_6 = namedXmlID[20].strName;
			check_7 = namedXmlID[21].strName;
			check_autoserver = namedXmlID[22].strName;
			check_dirbrowse = namedXmlID[23].strName;
			check_https = namedXmlID[24].strName;
			check_lan = namedXmlID[25].strName;
			check_network = namedXmlID[26].strName;
			check_savejpg = namedXmlID[27].strName;
			edit_api_key = namedXmlID[28].strName;
			edit_cameraIp = namedXmlID[29].strName;
			edit_cphostIp = namedXmlID[30].strName;
			edit_cphostIp_local = namedXmlID[31].strName;
			edit_cphostPort = namedXmlID[32].strName;
			edit_cphostPort_local = namedXmlID[33].strName;
			edit_hostIp = namedXmlID[34].strName;
			edit_hostIp_local = namedXmlID[35].strName;
			edit_hostPort = namedXmlID[36].strName;
			edit_hostPort_local = namedXmlID[37].strName;
			edit_isnot_facial = namedXmlID[38].strName;
			edit_jpgmem = namedXmlID[39].strName;
			edit_jpgmem_local = namedXmlID[40].strName;
			edit_libname = namedXmlID[41].strName;
			edit_libname_local = namedXmlID[42].strName;
			edit_picturepath = namedXmlID[43].strName;
			edit_pixel_height = namedXmlID[44].strName;
			edit_pixel_width = namedXmlID[45].strName;
			edit_reliability = namedXmlID[46].strName;
			edit_residence_time = namedXmlID[47].strName;
			edit_secret_key = namedXmlID[48].strName;
			edit_serverport = namedXmlID[49].strName;
			eidt_faces_second = namedXmlID[50].strName;
			get_camera_info = namedXmlID[51].strName;
			grp_face_img = namedXmlID[52].strName;
			hide_main = namedXmlID[53].strName;
			ip_item = namedXmlID[54].strName;
			item_cbx_ai = namedXmlID[55].strName;
			item_cbx_lib = namedXmlID[56].strName;
			item_cbx_rec = namedXmlID[57].strName;
			item_check_1 = namedXmlID[58].strName;
			item_check_2 = namedXmlID[59].strName;
			item_check_3 = namedXmlID[60].strName;
			item_check_4 = namedXmlID[61].strName;
			item_check_5 = namedXmlID[62].strName;
			item_check_6 = namedXmlID[63].strName;
			item_check_7 = namedXmlID[64].strName;
			item_check_https = namedXmlID[65].strName;
			item_check_lan = namedXmlID[66].strName;
			item_check_network = namedXmlID[67].strName;
			item_check_savejpg = namedXmlID[68].strName;
			item_face_img = namedXmlID[69].strName;
			item_play = namedXmlID[70].strName;
			item_radio_1 = namedXmlID[71].strName;
			item_radio_2 = namedXmlID[72].strName;
			item_radio_3 = namedXmlID[73].strName;
			item_radio_4 = namedXmlID[74].strName;
			item_setAI_function = namedXmlID[75].strName;
			item_setFace_parameter = namedXmlID[76].strName;
			item_timeSet = namedXmlID[77].strName;
			jpgmem_size = namedXmlID[78].strName;
			lv_ip_play = namedXmlID[79].strName;
			radio_1 = namedXmlID[80].strName;
			radio_2 = namedXmlID[81].strName;
			radio_3 = namedXmlID[82].strName;
			radio_4 = namedXmlID[83].strName;
			radio_face_img = namedXmlID[84].strName;
			server_status = namedXmlID[85].strName;
			set_picturepath = namedXmlID[86].strName;
			set_server = namedXmlID[87].strName;
			start_server = namedXmlID[88].strName;
			stop_server = namedXmlID[89].strName;
			win_refresh = namedXmlID[90].strName;
		}
		 const wchar_t * btn_back;
		 const wchar_t * btn_close;
		 const wchar_t * btn_localIpSet;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_play_fc;
		 const wchar_t * btn_ref;
		 const wchar_t * btn_restore;
		 const wchar_t * btn_setAI_function;
		 const wchar_t * btn_setFace_parameter;
		 const wchar_t * btn_timeSet;
		 const wchar_t * cbx_account;
		 const wchar_t * cbx_ai;
		 const wchar_t * cbx_lib;
		 const wchar_t * cbx_rec;
		 const wchar_t * check_1;
		 const wchar_t * check_2;
		 const wchar_t * check_3;
		 const wchar_t * check_4;
		 const wchar_t * check_5;
		 const wchar_t * check_6;
		 const wchar_t * check_7;
		 const wchar_t * check_autoserver;
		 const wchar_t * check_dirbrowse;
		 const wchar_t * check_https;
		 const wchar_t * check_lan;
		 const wchar_t * check_network;
		 const wchar_t * check_savejpg;
		 const wchar_t * edit_api_key;
		 const wchar_t * edit_cameraIp;
		 const wchar_t * edit_cphostIp;
		 const wchar_t * edit_cphostIp_local;
		 const wchar_t * edit_cphostPort;
		 const wchar_t * edit_cphostPort_local;
		 const wchar_t * edit_hostIp;
		 const wchar_t * edit_hostIp_local;
		 const wchar_t * edit_hostPort;
		 const wchar_t * edit_hostPort_local;
		 const wchar_t * edit_isnot_facial;
		 const wchar_t * edit_jpgmem;
		 const wchar_t * edit_jpgmem_local;
		 const wchar_t * edit_libname;
		 const wchar_t * edit_libname_local;
		 const wchar_t * edit_picturepath;
		 const wchar_t * edit_pixel_height;
		 const wchar_t * edit_pixel_width;
		 const wchar_t * edit_reliability;
		 const wchar_t * edit_residence_time;
		 const wchar_t * edit_secret_key;
		 const wchar_t * edit_serverport;
		 const wchar_t * eidt_faces_second;
		 const wchar_t * get_camera_info;
		 const wchar_t * grp_face_img;
		 const wchar_t * hide_main;
		 const wchar_t * ip_item;
		 const wchar_t * item_cbx_ai;
		 const wchar_t * item_cbx_lib;
		 const wchar_t * item_cbx_rec;
		 const wchar_t * item_check_1;
		 const wchar_t * item_check_2;
		 const wchar_t * item_check_3;
		 const wchar_t * item_check_4;
		 const wchar_t * item_check_5;
		 const wchar_t * item_check_6;
		 const wchar_t * item_check_7;
		 const wchar_t * item_check_https;
		 const wchar_t * item_check_lan;
		 const wchar_t * item_check_network;
		 const wchar_t * item_check_savejpg;
		 const wchar_t * item_face_img;
		 const wchar_t * item_play;
		 const wchar_t * item_radio_1;
		 const wchar_t * item_radio_2;
		 const wchar_t * item_radio_3;
		 const wchar_t * item_radio_4;
		 const wchar_t * item_setAI_function;
		 const wchar_t * item_setFace_parameter;
		 const wchar_t * item_timeSet;
		 const wchar_t * jpgmem_size;
		 const wchar_t * lv_ip_play;
		 const wchar_t * radio_1;
		 const wchar_t * radio_2;
		 const wchar_t * radio_3;
		 const wchar_t * radio_4;
		 const wchar_t * radio_face_img;
		 const wchar_t * server_status;
		 const wchar_t * set_picturepath;
		 const wchar_t * set_server;
		 const wchar_t * start_server;
		 const wchar_t * stop_server;
		 const wchar_t * win_refresh;
		}name;

		class _id{
		public:
		const static int btn_back	=	65543;
		const static int btn_close	=	65536;
		const static int btn_localIpSet	=	65542;
		const static int btn_max	=	65537;
		const static int btn_min	=	65539;
		const static int btn_play_fc	=	65609;
		const static int btn_ref	=	65544;
		const static int btn_restore	=	65538;
		const static int btn_setAI_function	=	65607;
		const static int btn_setFace_parameter	=	65601;
		const static int btn_timeSet	=	65608;
		const static int cbx_account	=	65541;
		const static int cbx_ai	=	65605;
		const static int cbx_lib	=	65598;
		const static int cbx_rec	=	65600;
		const static int check_1	=	65586;
		const static int check_2	=	65587;
		const static int check_3	=	65588;
		const static int check_4	=	65589;
		const static int check_5	=	65590;
		const static int check_6	=	65591;
		const static int check_7	=	65592;
		const static int check_autoserver	=	65615;
		const static int check_dirbrowse	=	65616;
		const static int check_https	=	65599;
		const static int check_lan	=	65602;
		const static int check_network	=	65603;
		const static int check_savejpg	=	65604;
		const static int edit_api_key	=	65559;
		const static int edit_cameraIp	=	65583;
		const static int edit_cphostIp	=	65596;
		const static int edit_cphostIp_local	=	65570;
		const static int edit_cphostPort	=	65597;
		const static int edit_cphostPort_local	=	65571;
		const static int edit_hostIp	=	65593;
		const static int edit_hostIp_local	=	65567;
		const static int edit_hostPort	=	65594;
		const static int edit_hostPort_local	=	65568;
		const static int edit_isnot_facial	=	65561;
		const static int edit_jpgmem	=	65606;
		const static int edit_jpgmem_local	=	65580;
		const static int edit_libname	=	65595;
		const static int edit_libname_local	=	65569;
		const static int edit_picturepath	=	65613;
		const static int edit_pixel_height	=	65565;
		const static int edit_pixel_width	=	65564;
		const static int edit_reliability	=	65563;
		const static int edit_residence_time	=	65566;
		const static int edit_secret_key	=	65560;
		const static int edit_serverport	=	65617;
		const static int eidt_faces_second	=	65562;
		const static int get_camera_info	=	65584;
		const static int grp_face_img	=	65585;
		const static int hide_main	=	65540;
		const static int ip_item	=	65547;
		const static int item_cbx_ai	=	65579;
		const static int item_cbx_lib	=	65572;
		const static int item_cbx_rec	=	65574;
		const static int item_check_1	=	65552;
		const static int item_check_2	=	65553;
		const static int item_check_3	=	65554;
		const static int item_check_4	=	65555;
		const static int item_check_5	=	65556;
		const static int item_check_6	=	65557;
		const static int item_check_7	=	65558;
		const static int item_check_https	=	65573;
		const static int item_check_lan	=	65576;
		const static int item_check_network	=	65577;
		const static int item_check_savejpg	=	65578;
		const static int item_face_img	=	65551;
		const static int item_play	=	65549;
		const static int item_radio_1	=	2000;
		const static int item_radio_2	=	2001;
		const static int item_radio_3	=	2002;
		const static int item_radio_4	=	2003;
		const static int item_setAI_function	=	65582;
		const static int item_setFace_parameter	=	65575;
		const static int item_timeSet	=	65548;
		const static int jpgmem_size	=	65581;
		const static int lv_ip_play	=	65546;
		const static int radio_1	=	1000;
		const static int radio_2	=	1001;
		const static int radio_3	=	1002;
		const static int radio_4	=	1003;
		const static int radio_face_img	=	65550;
		const static int server_status	=	65610;
		const static int set_picturepath	=	65614;
		const static int set_server	=	65618;
		const static int start_server	=	65611;
		const static int stop_server	=	65612;
		const static int win_refresh	=	65545;
		}id;

		class _string{
		public:
		const static int title	=	0;
		const static int ver	=	1;
		}string;

		class _color{
		public:
		const static int black	=	0;
		const static int blue	=	1;
		const static int dpblue	=	2;
		const static int gray	=	3;
		const static int green	=	4;
		const static int red	=	5;
		const static int white	=	6;
		}color;

	};

#ifdef R_IN_CPP
	 extern const _R R;
	 extern const _UIRES UIRES;
#else
	 extern const __declspec(selectany) _R & R = _R();
	 extern const __declspec(selectany) _UIRES & UIRES = _UIRES();
#endif//R_IN_CPP
}
