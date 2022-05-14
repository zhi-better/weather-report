#include "BForm.h"
#include "resource.h"
#include "BWindGDI.h"
#include "BReadLinesEx.h"
#include "BWinHttp.h"
#include "mdlFileSys.h"
#include "BHashStrK.h"
#include <WinUser.h>
#include "BTimer.h"

struct RespWeather 
{
	tstring sCityX;					//城市的精度
	tstring sCityY;					//城市的维度
	tstring sCityName;				//城市名称
	tstring sTemperature1;			//气温
	tstring sTemperature2;			//气温2
	tstring sWindState;				//风相关参数
	tstring sHumidity;				//湿度
	tstring sWindDir;				//风向
	tstring sUpdataTime;			//更新时间
	tstring sWeather;				//表示天气具体内容
};
struct CityandID 
{
	tstring sCityName;
	int iCityID;
};

CBForm form1(ID_form1);
CBForm frmChooseCity(ID_frmChooseCity);
CBTimer timUpdate;

CBWinInet m_getWeather;
CBWindGdiPlus m_gdiPlusMain;
CBWindGDI m_gdiMain;

tstring m_sCityID = L"101030100";		//默认显示的城市 id
tstring m_sCityPinyin = L"tianjin";		//表示的是城市的拼音
tstring m_sRespData = L"";				//返回的消息内容，便于以后查询
RespWeather m_weatherInfo;				//表示的是天气的结构体
CityandID m_CityIDAll[447];				//用来保存对应的可读取的所有的城市 id
tstring m_sHost1 = L"http://www.weather.com.cn/data/sk/";
tstring m_sHost2 = L"http://www.weather.com.cn/data/cityinfo/";
tstring m_sHost3 = L"http://flash.weather.com.cn/wmaps/xml/";
const int m_iWidth = 700;
const int m_iHeight = 300;


void CreateFileFromResourse(unsigned int idResourse, LPCTSTR sResourseType, LPCTSTR sFilePath)
{
	HMODULE hThis = GetModuleHandle(NULL);
	HRSRC   hRes = FindResource(hThis, MAKEINTRESOURCE(idResourse), sResourseType);   //第二个参加是资源ID，第三个参数是，添加写的名字
	if (!hThis)	return;		//如果此处不对，建议重新打开，一般不会

	HGLOBAL hGres = LoadResource(hThis, hRes);
	PVOID   pRes = LockResource(hGres);
	DWORD  dwSize = SizeofResource(NULL, hRes);
	//首先判断这个文件是否存在，如果存在，直接删除，重新创建一个新的内容覆盖
	WIN32_FIND_DATA fd;
	HANDLE hFileFind = FindFirstFile(sFilePath, &fd);
	//如果已经找到该文件，那么就直接删除该文件
	if (hFileFind != INVALID_HANDLE_VALUE)
		return;
	HANDLE  hFile = CreateFile(sFilePath, GENERIC_WRITE, NULL, NULL, CREATE_NEW,
		FILE_ATTRIBUTE_ARCHIVE, NULL);

	//如果创建失败，那么直接返回失败
	if (!hFile)	return;
	//文件创建成功，此处继续进行文件的读写操作，写入文件内容
	DWORD dwSizeWritten = 0;
	WriteFile(hFile, pRes, dwSize, &dwSizeWritten, NULL);
	CloseHandle(hFile);
}
bool Read_Config()
{
	tstring sContent = TEXT("");
	LPTSTR sLine = NULL;
	CBReadLinesEx file;
	int i = 0;

	//文件不存在，直接返回，啥也别干了
	if (FMFileExist(TEXT("C:\\AppData\\城市id和名称.txt")) == 0)
	{
		CreateDirectory(TEXT("C:\\AppData"), NULL);
		CreateFileFromResourse(IDR_TxtCityCode, TEXT("TXT"),L"C:\\AppData\\城市id和名称.txt");
	}

	//文件打开失败，返回，也别干了
	if (!file.OpenFile(TEXT("C:\\AppData\\城市id和名称.txt")))
		return false;	//或者定义一个LPTSTR类型的变量，但是我喜欢tstring

	tstring sLineText = L"";
	int iPos = 0;
	while (!file.IsEndRead() && i<447)	//这里检测是不是最后的行
	{
		file.GetNextLine(sLine);	//将下一行的数据存到sLine里面
		if (file.IsErrOccured())	
			Read_Config();	//出错了重新读
		sLineText = sLine;
		iPos = sLineText.find(L":");
		m_CityIDAll[i].iCityID = (int)Val(sLineText.substr(iPos + 1).c_str());
		m_CityIDAll[i].sCityName = sLineText.substr(0, iPos).c_str();
		i++;
	}
	return true;
}
bool GetAvliCity()
{
	//获得可用城市并且用控件保存可用城市的内容
	m_getWeather.HttpOpen((m_sHost3 + m_sCityPinyin + L".xml").c_str());
	m_getWeather.AddHeaders();
	m_getWeather.SendRequest();
	DWORD dwCode = m_getWeather.GetRespCode();
	if (dwCode >= 400)
	{
		MsgBox(L"网络返回错误，请检查拼音输入是否正确，全部用小写，拼音中间不能有空格",
			StrAppend(L"错误代码：", Str((int)dwCode)));
		m_sRespData = L"";		//清空接收的信息的内容
		frmChooseCity.Control(ID_cboCityAvli).ListClear();
		return false;
	}
	m_sRespData = m_getWeather.GetRespBodyData();		//获得对应的返回的内容
	if (m_sRespData == L"" || m_sRespData.find(L"抱歉") != string::npos)
	{
		MsgBox(L"查询失败，区域查询最多支持输入市级城市以上，另外请检查拼写是否正确",L"查询失败",mb_OK,mb_IconInformation);
		return false;
	}

	int iPosStart = 0;			//就是查询内容，字符串操作
	int iPosStop = 0;
	frmChooseCity.Control(ID_cboCityAvli).ListClear();
	frmChooseCity.Control(ID_cboCityAvli).AddItem(L"请选择你要查询的城市");
	frmChooseCity.Control(ID_cboCityAvli).ListIndexSet(1);
	while(true)
	{
		iPosStart = m_sRespData.find(L"cityname", iPosStop);
		if(iPosStart == string::npos)		//如果找不到，直接跳出
			break;
		iPosStart = iPosStart + 10;
		iPosStop = m_sRespData.find(L"\"", iPosStart);
		frmChooseCity.Control(ID_cboCityAvli).AddItem(m_sRespData.substr(iPosStart, iPosStop - iPosStart));
	}
	return true;
}
void GetWeatherInfo()
{
// 	m_getWeather.HttpOpen(StrAppend(m_sHost1.c_str(), m_sCityID.c_str(), L".html"));
// 
// 	m_getWeather.AddHeaders();
// 	m_getWeather.SendRequest();
// 	DWORD dwCode = m_getWeather.GetRespCode();
// 	if (dwCode >= 400)
// 	{
// 		MsgBox(L"网络访问出现错误，请检查网络连接或者关闭后重新启动程序后再试一试！",
// 			StrAppend(L"错误代码：",Str((int)dwCode)));
// 		return;
// 	}
// 	tstring sRespData = m_getWeather.GetRespBodyData();		//获得对应的返回的内容
// 
// 	if (sRespData == L"")
// 		return;
// 
// 	int iPosStart = 24;			//就是查询内容，字符串操作
// 	int iPosStop = 0;
// 
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sCityName = sRespData.substr(iPosStart,iPosStop-iPosStart);
// 
// 	iPosStart = sRespData.find(L":", iPosStop)+2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	iPosStart = sRespData.find(L":", iPosStop) + 2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sTemperature = sRespData.substr(iPosStart, iPosStop - iPosStart);
// 
// 	iPosStart = sRespData.find(L":", iPosStop) + 2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sWindDir = sRespData.substr(iPosStart, iPosStop - iPosStart);
// 
// 	iPosStart = sRespData.find(L":", iPosStop) + 2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sWindDegree = sRespData.substr(iPosStart, iPosStop - iPosStart);
// 
// 	iPosStart = sRespData.find(L":", iPosStop) + 2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sHumidity = sRespData.substr(iPosStart, iPosStop - iPosStart);
// 
// 	iPosStart = sRespData.find(L":", iPosStop) + 2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sAPressure = sRespData.substr(iPosStart, iPosStop - iPosStart);
// 
// 	iPosStart = sRespData.find(L":", iPosStop) + 2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sHumidity = sRespData.substr(iPosStart, iPosStop - iPosStart);
// 
// 	iPosStart = sRespData.find(L":", iPosStop) + 2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sWindF = sRespData.substr(iPosStart, iPosStop - iPosStart);
// 
// 	iPosStart = sRespData.find(L":", iPosStop) + 2;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sUpdataTime = sRespData.substr(iPosStart, iPosStop - iPosStart);
// 
// 	//访问第二个网页，用来获得天气
// 	m_getWeather.HttpOpen(StrAppend(m_sHost2.c_str(), m_sCityID.c_str(), L".html"));
// 
// 	m_getWeather.AddHeaders();
// 	m_getWeather.SendRequest();
// 	sRespData = m_getWeather.GetRespBodyData();		//获得对应的返回的内容
// 
// 	if (sRespData == L"")	return;
// 
// 	//开始查询内容
// 	int i;
// 	iPosStart = 0;
// 	for (i=0;i<6;i++)
// 	{
// 		iPosStart = sRespData.find(L":", iPosStart) + 1;
// 	}
// 	iPosStart += 1;
// 	iPosStop = sRespData.find(L"\"", iPosStart);
// 	m_weatherInfo.sWeather = sRespData.substr(iPosStart, iPosStop - iPosStart);

	tstring sCity = frmChooseCity.Control(ID_cboCityAvli).Text();	//获得城市内容
	//如果没有可用的数据，并且无法获得对应的数据，直接返回
	if (m_sRespData == L"" && !GetAvliCity())
		return;

	//进行数据处理，抓取到对的气象数据
	int iPosStart = 0;			//就是查询内容，字符串操作
	int iPosStop = 0;
	int i;

	for (i=0;i<frmChooseCity.Control(ID_cboCityAvli).ListIndex()-2;i++)
	{
		iPosStart = m_sRespData.find(L"\r\n", iPosStart) + 1;
	}
	iPosStop = m_sRespData.find(L"\r\n", iPosStart);

	//用变量存储要查找的天气的具体位置的信息
	tstring sCityWeather = m_sRespData.substr(iPosStart, iPosStop - iPosStart);	
	iPosStart = 0;	iPosStop = 0;
	
	iPosStart = sCityWeather.find(L"cityX", iPosStop) + 7;
	iPosStop = sCityWeather.find(L"\"", iPosStart);
	m_weatherInfo.sCityX = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"cityY", iPosStop) + 7;
	iPosStop = sCityWeather.find(L"\"", iPosStart);
	m_weatherInfo.sCityY = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"cityname", iPosStop) + 10;
	iPosStop = sCityWeather.find(L"\"", iPosStart);
	m_weatherInfo.sCityName = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"stateDetailed", iPosStop) + 15;
	iPosStop = sCityWeather.find(L"\"", iPosStart);
	m_weatherInfo.sWeather = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"tem1", iPosStop) + 6;
	iPosStop = sCityWeather.find(L"\"", iPosStart);
	m_weatherInfo.sTemperature1 = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"tem2", iPosStop) + 6;
	iPosStop = sCityWeather.find(L"\"", iPosStart);
	m_weatherInfo.sTemperature2 = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"windState", iPosStop) + 11;
	iPosStop = sCityWeather.find(L"\"", iPosStart);
	m_weatherInfo.sWindState = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"windDir", iPosStop) + 9;
	iPosStop = sCityWeather.find(L"\"", iPosStart); 
	m_weatherInfo.sWindDir = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"humidity", iPosStop) + 10;
	iPosStop = sCityWeather.find(L"\"", iPosStart); 
	m_weatherInfo.sHumidity = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	iPosStart = sCityWeather.find(L"time", iPosStop) + 6;
	iPosStop = sCityWeather.find(L"\"", iPosStart);
	m_weatherInfo.sUpdataTime = sCityWeather.substr(iPosStart, iPosStop - iPosStart);
	
	m_gdiMain.InvalidateAll();	//刷新显示
}
void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	//刷新显示内容，如果不变就不变吧，管它呢，能查询就行
	GetAvliCity();
	GetWeatherInfo();
}
void form1_Load()
{
	//SetParent(frmChooseCity.hWnd(), form1.hWnd());		//给它设个爸爸省得关闭不同时关闭

	timUpdate.pFuncCallBackSet(Timer1_Timer);
	timUpdate.IntervalSet(1800000);		//半个小时更新一次
	timUpdate.hWndAssoSet(form1.hWnd());
	timUpdate.EnnabledSet(true);

	//新版的采用拼音查询，这里不需要读取城市 id 了
	//Read_Config();		//读取对应城市和对应的 id

	//预定义的城市为天津，市中心
	frmChooseCity.Control(ID_cboCityAvli).ListIndexSet(7);
	GetWeatherInfo();

	form1.Control(ID_lblSelectCity).MousePointerSet(IDC_Hand);
	form1.BackColorSet(RGB(255, 255, 255));
	form1.Control(ID_lblSelectCity).BackColorSet(RGB(255, 255, 255));
	form1.Control(ID_lblSelectCity).FontSizeSet(12);
	form1.Control(ID_lblSelectCity).BackStyleTransparentSet(true);
	form1.MoveToScreenCenter(m_iWidth, m_iHeight+40);
	form1.Control(ID_picMain).Move(0, 0, m_iWidth, m_iHeight);
	form1.Control(ID_lblSelectCity).Move(m_iWidth - 200, m_iHeight - 28);
	form1.Control(ID_lblSelectCity).ToolTipAdd(L"点我可以改变城市哦，٩(⊙o⊙*)و");
	form1.Control(ID_lblSelectCity).ToolTipInitTimeSet(10);
	form1.Control(ID_lblAbout).FontSizeSet(12);
	form1.Control(ID_lblAbout).Move(m_iWidth - 100, m_iHeight - 30);
	form1.Control(ID_lblAbout).BackStyleTransparentSet(true);
	form1.Control(ID_lblAbout).MousePointerSet(IDC_Hand);

	//创建所有的图片资源
	if (FMFileExist(TEXT("C:\\AppData")) == 0)
		CreateDirectory(TEXT("C:\\AppData"), NULL);
	if (FMFileExist(TEXT("C:\\AppData\\WeatherImages")) == 0)
		CreateDirectory(TEXT("C:\\AppData\\WeatherImages"), NULL);

	CreateFileFromResourse(IDB_Sunrise, L"PNG", L"C:\\AppData\\WeatherImages\\日出.png");
	CreateFileFromResourse(IDB_CloudDay, L"PNG", L"C:\\AppData\\WeatherImages\\多云-白天.png");
	CreateFileFromResourse(IDB_CloudNight, L"PNG", L"C:\\AppData\\WeatherImages\\多云-晚上.png");
	CreateFileFromResourse(IDB_CloudRain, L"PNG", L"C:\\AppData\\WeatherImages\\多云小雨.png");
	CreateFileFromResourse(IDB_MidRain, L"PNG", L"C:\\AppData\\WeatherImages\\中雨.png");
	CreateFileFromResourse(IDB_Moon, L"PNG", L"C:\\AppData\\WeatherImages\\月亮.png");
	CreateFileFromResourse(IDB_MoreCloud, L"PNG", L"C:\\AppData\\WeatherImages\\阴天.png");
	CreateFileFromResourse(IDB_SmallSnow, L"PNG", L"C:\\AppData\\WeatherImages\\小雪.png");
	CreateFileFromResourse(IDB_Sun, L"PNG", L"C:\\AppData\\WeatherImages\\太阳.png");
	CreateFileFromResourse(IDB_SunUp, L"PNG", L"C:\\AppData\\WeatherImages\\日落.png");
	CreateFileFromResourse(IDR_BG, L"JPG", L"C:\\AppData\\WeatherImages\\背景.jpg");

	m_gdiPlusMain.hWindowSet(form1.Control(ID_picMain).hWnd());
	m_gdiMain.hWndDCWindSet(form1.Control(ID_picMain).hWnd());
}
void lblSelectCity_MouseUp(int button, int shift, int x, int y)
{
	frmChooseCity.Show(0, form1.hWnd());
}
void cmdOK_click()
{
// 		//查询现有的城市内容，如果查询不到，表示提示
// 		int i;
// 		for (i=0;i<447;i++)
// 		{
// 			if (m_CityIDAll[i].sCityName == sCityName)
// 			{
// 				m_sCityID = Str(m_CityIDAll[i].iCityID);
// 				GetWeatherInfo();						//刷新对应的内容
// 				frmChooseCity.UnLoad();
// 				return;
// 			}
// 		}
// 		MsgBox(L"查询不到对应的城市，可能因为城市地区较小，暂时不支持查询\r\n请改为更高一级的地区查询。", L"非常抱歉");
// 		frmChooseCity.Control(ID_txtCity).TextSet(L"");	

	//此处如果城市内容非空，直接查找对应的天气信息
	tstring sText = frmChooseCity.Control(ID_cboCityAvli).Text();
	if (sText != L"请选择你要查询的城市")
	{
		GetWeatherInfo();
	}
}
void picMain_Paint()
{
	Image imageBG(L"C:\\AppData\\WeatherImages\\背景.jpg");
	m_gdiPlusMain.GetGraphics()->DrawImage(&imageBG,0,0,
		form1.Control(ID_picMain).Width(), form1.Control(ID_picMain).Height());

	m_gdiPlusMain.FontBoldSet(true);
	m_gdiPlusMain.FontSizeSet(40);
	m_gdiPlusMain.GetBrush()->SetColor(Color::White);
	m_gdiPlusMain.DrawString((m_weatherInfo.sTemperature1 + L"°~" + m_weatherInfo.sTemperature2 + L"°").c_str(), 50, 50);
	m_gdiPlusMain.FontSizeSet(16);
	m_gdiPlusMain.DrawString(m_weatherInfo.sWeather.c_str(), 280, 85);
	m_gdiPlusMain.FontSizeSet(12);
	m_gdiPlusMain.DrawString(L"当前城市：", 49, 180);
	m_gdiPlusMain.DrawString((m_weatherInfo.sCityName+L"\t经度："+m_weatherInfo.sCityX + L" 纬度：" + m_weatherInfo.sCityY).c_str(), 130, 180);	//城市名称

	m_gdiPlusMain.DrawString(m_weatherInfo.sWindState.c_str(),
								50,140);
	m_gdiPlusMain.DrawString(L"湿度：", 270, 139);
	m_gdiPlusMain.DrawString(m_weatherInfo.sHumidity.c_str(), 320, 139);
// 	m_gdiPlusMain.DrawString(L"气压：", 300, 140);
// 	m_gdiPlusMain.DrawString(m_weatherInfo.sAPressure.c_str(), 350, 140);

	Image imageLogo(L"C:\\AppData\\WeatherImages\\月亮.png");
	m_gdiPlusMain.GetGraphics()->DrawImage(&imageLogo, 550, 50, 100, 100);
}
void frmChooseCity_Load()
{
	frmChooseCity.Control(ID_txtCity).FontSizeSet(11);
	frmChooseCity.Control(ID_cboCityAvli).FontSizeSet(11);
	frmChooseCity.Control(ID_txtCity).TextSet(m_sCityPinyin);
	frmChooseCity.Control(ID_txtCity).ToolTipAdd(L"注意：此处必须填写拼音格式，其他格式无法识别！",true);
	frmChooseCity.Control(ID_txtCity).ToolTipInitTimeSet(500);
	frmChooseCity.Control(ID_txtCity).ToolTipAlignSet(2);
	GetAvliCity();
}
void lblAbout_MouseUp(int button, int shift, int x, int y)
{
	tstring sAbout = L"本程序由阿治制作，当前版本完成时间为2020/12/31 0:35\r\n感谢您的使用，有什么建议可以联系QQ:3167038449\r\n";
	sAbout += L"使用方法：先以拼音的格式输入区域名称，点击查询即可找到对应的区域的下属城市";
	sAbout += L"然后通过下拉菜单选择对应的要查询的城市，选择完毕后点击确定，就可以看到选择的城市的天气信息了\r\n";
	sAbout += L"天气信息来源网站：http://flash.weather.com.cn/wmaps/xml/tianjin.xml";
	MsgBox(sAbout,L"关于");
}
void cmdGetAvliCity_Click()
{
	tstring sCityName = frmChooseCity.Control(ID_txtCity).Text();

	if (sCityName != TEXT(""))		//如果非空，改变对应的城市 id
	{
		//对输入内容进行保存
		m_sCityPinyin = sCityName;
		if (GetAvliCity())
			MsgBox(L"获取城市信息成功，请选择待查询的城市名称，点击确定即可获得该城市的天气信息", L"查询成功");
		
	}
}
void form1_Unload()
{
	frmChooseCity.UnLoad();
}
void txtCity_Change()
{
	if (*frmChooseCity.Control(ID_txtCity).Text() == L'\0')
	{
		frmChooseCity.Control(ID_txtCity).TooltipEnabledSet(true);
	}
	else
		frmChooseCity.Control(ID_txtCity).TooltipEnabledSet(false);
}
int main()
{
	form1.EventAdd(0, eForm_Load, form1_Load);
	form1.EventAdd(0, eForm_Unload, form1_Unload);
	form1.EventAdd(ID_lblSelectCity, eMouseUp, lblSelectCity_MouseUp);
	form1.EventAdd(ID_picMain, ePaint, picMain_Paint);
	form1.EventAdd(ID_lblAbout, eMouseUp, lblAbout_MouseUp);

	frmChooseCity.EventAdd(0, eForm_Load, frmChooseCity_Load);
	frmChooseCity.EventAdd(ID_cmdOK, eCommandButton_Click, cmdOK_click);
	frmChooseCity.EventAdd(ID_cmdGetAvliCity, eCommandButton_Click, cmdGetAvliCity_Click);
	//frmChooseCity.EventAdd(ID_txtCity, eEdit_Change, txtCity_Change);

	form1.IconSet(IDI_ICON1);
	form1.Show();
	//frmChooseCity.Show(0, form1.hWnd());
	return 0;
}








