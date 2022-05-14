//////////////////////////////////////////////////////////////////////////
// CBWindGDI.h: CBWindGDI 类的定义
// 实现 GDI 绘图
//
// Programmed by Zhizhi
// Last Modified: 2020-6-23
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "BForm.h"
#include "mdlTransBMP.h"	//为了能够实现添加位图的时候能够自动对位图进行透明处理
//==================================================================================
#include <GdiPlus.h>
#pragma comment(lib, "GdiPlus.lib")
using namespace Gdiplus;

//定义一个枚举用来控制内沿边框的效果和外延效果边框的组合情况
//一共有四种组合样式：镶嵌，蚀刻，抬起，按下
enum EdgeType
{
	eEdgeBump = (BDR_RAISEDOUTER | BDR_SUNKENINNER),		//镶嵌样式
	eEdgeEtched = (BDR_SUNKENOUTER | BDR_RAISEDINNER),		//蚀刻样式
	eEdgeRaised = (BDR_RAISEDOUTER | BDR_RAISEDINNER),		//抬起样式
	eEdgeSunken = (BDR_SUNKENOUTER | BDR_SUNKENINNER)		//按下样式
};

//定义一个用于控制标题栏按钮的枚举
//其中包括的按钮样式：关闭，最大化，最小化，向下还原，帮助
enum eDCCaptionButton
{
	eCaptionClose = DFCS_CAPTIONCLOSE,		//关闭
	eCaptionMin = DFCS_CAPTIONMIN,			//最小化
	eCaptionMax = DFCS_CAPTIONMAX,			//最大化
	eCaptionRestore = DFCS_CAPTIONRESTORE,	//向下还原
	eCaptionHelp = DFCS_CAPTIONHELP			//帮助
};

//定义用于控制当前的元素的状态的枚举
enum eDCButtonState
{
	eButtonStateNormal = 0,						//默认格式
	eButtonStateChecked = DFCS_CHECKED,			//复选框被选择
	eButtonStateFlat = DFCS_FLAT,				//按钮具有平面边框效果
	eButtonStateHot = DFCS_HOT,					//当鼠标移过时按钮的热跟踪效果
	eButtonStateInactive = DFCS_INACTIVE,		//按钮被禁用（ 灰色 ）
	eButtonStateMono = DFCS_MONO,				//按钮具有单色边框
	eButtonStatePushed = DFCS_PUSHED,			//按钮被按下时的效果
	eButtonStateTransparent = DFCS_TRANSPARENT	//按钮的背景透明
};

//定义用于绘制标准的按钮的种类
//标准按钮包括筲通的 CommandButton 样式的按钮，也 可以是复选框、单选框，
enum eDCButton
{
	eButton3State = DFCS_BUTTON3STATE,			//3 态按钮
	eButtonCheck = DFCS_BUTTONCHECK,			//复选框
	eButtonPush = DFCS_BUTTONPUSH,				//普通按下式按钮
	eButtonRadio = DFCS_BUTTONRADIO,			//单选框
	eButtonRadioImage = DFCS_BUTTONRADIOIMAGE,	//单选框的图片
	eButtonRadioMask = DFCS_BUTTONRADIOMASK		//单选框的屏蔽（ Mask ) 图片
};

//定义用于绘制菜单的元素种类
//具体包括：右箭头、左箭头、菜单圆点、对勾
enum eDCMenuButton
{
	eMenuArrow = DFCS_MENUARROW,			//指示包含子菜单的右箭头
	eMenuArrowRight = DFCS_MENUARROWRIGHT,	//指示包含子菜单的向左的箭头
	eMenuBullet = DFCS_MENUBULLET,			//指示单选标记的菜单圆点
	eMenuCheck = DFCS_MENUCHECK				//指示复选标记的菜单 “对勾”
};

//定义用于绘制滚动条元素的内容
enum eDCScrollButton
{
	eScrollLCombobox = DFCS_SCROLLCOMBOBOX,				//下拉列表框（ ComboBox ) 的向下箭头
	eScrollLDown = DFCS_SCROLLDOWN,						//滚动条的下箭头按钮
	eScrollLLeft = DFCS_SCROLLLEFT,						//滚动条的左箭头按钮
	eScrollLRight = DFCS_SCROLLRIGHT,					//滾动条的右箭头按钮
	eScrollLSizeGrip = DFCS_SCROLLSIZEGRIP,				//位于窗口右下角的 “斜纹”
	eScrollLSizeGripRight = DFCS_SCROLLSIZEGRIPRIGHT,	//位于窗口左下角的反方向 “斜纹”
	eScrollLUp = DFCS_SCROLLUP							//滾动条的上箭头按钮
};

//定义结构体用于保存图形热区矩形的信息
struct HotRectType 
{
	int Left;					//热区矩形左边界
	int Top;					//热区矩形上边界
	int Right;					//热区矩形右边界
	int Bottom;					//热区矩形下边界
	long int RectData;			//热区矩形附加数据（长整数）
	tstring RectString;			//热区矩形的附加数据（字符串），需要时候使用
	long int ReservedData;		//热区矩形的其他附加数据（长整数），需要时候使用
	double ReservedDbl;			//热区矩形的其他附加数据（双精度数），需要时候使用
	tstring ReservedString;		//热区矩形的其他附加数据（字符串），需要时候使用
};

//////////////////////////////////////////////////////////////////////////
// CBWindGDI 类的实现

class CBWindGDI
{
public:
	// =======================================================================
	// 类公有成员和成员函数
	//========================================================================

	// 构造函数
	// 构造函数，如果直接写出对应控件的句柄
	// 那么直接创造内存环境，此处默认均创建内存环境
	CBWindGDI(HWND hWnd = NULL);

	// 析构函数
	~CBWindGDI();

	//设置设备环境的大小
	//如果控件的大小改变，本函数会自动释放之前的设备环境，重新创建一个大小和控件适配的设备环境
	void UpdateDCRange();

	//设置和查看内存环境的关联的控件或者窗口
	//如果用户此时已经绑定了一个句柄，再次设置其他的句柄，本函数将会自动将原来的句柄绘制的图形全部显示
	//然后释放资源，重新选择这个新的句柄作为设备环境关联的控件的句柄
	// 即：实现了一个对象可以前后绑定多个设备环境，大大提高了使用的灵活性
	bool hWndDCWindSet(HWND hWnd);
	HWND hWndDCWind();

	//为了增加类的可扩展性，决定提供获得类的内存环境的句柄的接口
	//获得内存环境的句柄
	HDC hWndCompatibleDC();
	//获得设备环境的句柄
	HDC hWndDC();

	// 将绘制内容从内存设备环境中拷贝到 DC 环境中
	// 然后一并显示出来，（如果用户忘记执行此函数，对象析构，自动执行）
	void Show();

	//////////////////////////////////////////////////////////////////////////
	// 统一设置笔的各种属性，一次性设置全部
	// 笔的：颜色、宽度、样式，此函数三个参数必须全部设定，不可缺省
	// 0 -- 实线（宽度大于1时，在图形内、外分别绘制线条宽的一部分）
	// 1 -- 虚线
	// 2 -- 点虚线
	// 3 -- 点划线
	// 4 -- 点点划线
	// 5 -- 无线条
	// 6 -- 实线（宽度大于1时仅向图形内扩展）
	bool PenSet(COLORREF cPenColor, int iWidth, int iStyle);

	// 返回和设置当前笔的颜色
	COLORREF PenColor();
	void PenColorSet(COLORREF cPenColor);

	// 返回和设置当前笔的宽度
	int PenWidth();
	void PenWidthSet(int iWidth);

	// 返回和设置当前笔的样式
	// 0 -- 实线（宽度大于1时，在图形内、外分别绘制线条宽的一部分）
	// 1 -- 虚线
	// 2 -- 点虚线
	// 3 -- 点划线
	// 4 -- 点点划线
	// 5 -- 无线条
	// 6 -- 实线（宽度大于1时仅向图形内扩展）
	int PenStyle();
	void PenStyleSet(int iStyle);

	//////////////////////////////////////////////////////////////////////////
	// 返回和设置当前刷子的颜色
	COLORREF BrushColor();
	void BrushColorSet(COLORREF cBrushColor);

	// 返回和设置刷子是否是空刷子
	// 如果 fBrushNULL 为 false，后一个参数可以直接设置刷子的颜色，默认是白色的刷子
	//如果第一个参数为 true，那么第二个参数自动忽略
	bool BrushNULL();
	void BrushNullSet(bool fBrushNULL, COLORREF cBrushColor = 0xffffff);

	//////////////////////////////////////////////////////////////////////////
	// 设置一个位图显示到指定的位置，还可以设定位图的大小和是否透明，感觉编写的差不多最复杂的单个函数吧
	// 但是，位图设置后不可以删除，只有通过 InvalidateAll() 函数重新清除才可以
	// idBmp 位图的ID
	// xDest, yDest 分别为位图放置的位置
	// iWidth, iHeight 分别为位图放置位置的宽度和高度，如果是 -1 表示到设备环境的最右侧最下侧
	// f_Transparent 为是否透明处理，如果是 false，自动忽略最后的参数，否则，最后的参数是要透明的颜色
	//f_Stretch 是是否进行缩放处理，如果为true，需要设置最后的两个参数为最终的拷贝的最终矩形区域的宽和高 
	// xOrigin, yOrigin 分别为原始位图的裁剪的位置，默认从 0 开始
	//iWidthSrc, iHeightSrc 分别为源位图裁剪的宽度和高度, -1 代表到最右最下的位置
	void BitmapSet(unsigned short idBmp, 
		int xDest=0,int yDest=0,int iDestWidth=-1,int iDestHeight=-1,			// x, y 分别为位图放置的位置，iWidth, iHeight 分别为位图放置位置的宽度和高度
		bool f_Transparent=false,										// f_Transparent 为是否透明处理，
		COLORREF c_TransColor=0x000000,									//如果是 false，自动忽略最后的参数，否则，最后的参数是要透明的颜色
		double dAngle=0,												//图片的旋转角度，增加功能提高通用性	
		bool f_Stretch=false,											//f_Stretch 是是否进行缩放处理，
		int xOrigin=0,int yOrigin=0,int iWidthSrc=-1,int iHeightSrc=-1	//缩放的原始图片的起始地点的x和y坐标以及宽度和高度
		);
	
	//////////////////////////////////////////////////////////////////////////
	//向窗口指定区域打印文字
	//szText 待打印文本内容
	void PrintText(LPTSTR szText, int ileft=-1, int itop=-1, int iwidth=-1, int iheight=-1);

	//设置是否为单行显示文本内容
	void SingleLineSet(bool fSingleLine);
	bool SingleLine();

	//设置文本输出的时候的旋转角度
	void FontSpinDegreeSet(double iDegree);
	int FontSpinDegree();

	//设置文本的横向对齐方式
	//0 = 左对齐 1 = 中间对齐 2 = 右对齐
	void AlignSet(UINT uAlign);
	int Align();

	//设置文本的纵向对齐方式
	//注意此处需要是单行模式，本函数会自动将内容设置成单行模式
	//0 = 顶部对齐 1 = 中间对齐 2 = 底部对齐
	void VAlignSet(UINT uVAlign);
	int VAlign();

	//设置将要打印文本的范围
	void SetClip(int ileft=-1, int itop=-1, int iwidth=-1, int iheight=-1);

	//获得和设置文本的颜色
	void FontColorSet(COLORREF cColor);
	COLORREF FontColor();

	//获得和设置文本的背景色
	//只有当文本不透明时候有效
	void FontBKColorSet(COLORREF cColor);
	COLORREF FontBKColor();

	//获得和设置文本背景是否透明
	void BackTransparentSet(bool fTransparent);
	bool BackTransparent();

	//返回和设置字体的字体大小
	void FontSizeSet(int iFontSize);
	int FontSize();

	//设置字体的名称
	void FontNameSet(LPTSTR szFontName);
	LPTSTR FontName();

	//设置字体是否需要加粗
	void FontBondSet(bool fBond);
	bool FontBond();

	//设置字体是否需要加下划线
	void FontUnderLineSet(bool fUnderLine);
	bool FontUnderLine();

	//设置字体是否为斜体
	void FontItalicSet(bool fItalic);
	bool FontItalic();

	//////////////////////////////////////////////////////////////////////////
	//绘制 3D 效果的边框
	//iLeft,iTop,iWidth,iHeight 分别表示要绘制的边框的位置
	//eEdgeBump 表示要绘制的边框的样式：eEdgeBump = 镶嵌，eEdgeEtched = 蚀刻，eEdgeRaised = 抬起，eEdgeSunken = 按下
	//iFlatMono：0=表示的是绘制3D边框 1=表示的是平面效果 2=表示的是单色效果
	//iBorderType 表示的是绘制的边框的内容，1，2，4，8分别表示的左、上、右、下边框，可以相互相加获得最终需要绘制的边框内容
	//			默认的是 15，即全部绘制
	//fDiagonal 表示是否绘制对角线，默认是绘制朝向右上的对角线，如果iBorderType为12，绘制的是朝向右下的对角线
	void Draw3DBorder(int iLeft=0, int iTop=0, int iWidth=-1, int iHeight=-1, EdgeType eType=eEdgeBump, int iFlatMono=0, UINT iBorderType=15, bool fDiagonal=false);

	//////////////////////////////////////////////////////////////////////////
	//绘制 Windows 按钮元素

	//绘制标题栏的按钮元素
	//eCaptionButton 用来控制要绘制的是哪种的按钮
	//eButtonState 用于控制绘制的按钮的状态是什么
	void DrawCaptionButton(eDCCaptionButton eCaptionButton, eDCButtonState eButtonState=eButtonStateNormal, int iLeft=0, int iTop=0, int iWidth=40, int iHeight=20);

	//绘制标准的按钮
	//标准按钮包括筲通的 CommandButton 样式的按钮，也 可以是复选框、单选框，
	void DrawButton(eDCButton eButton, eDCButtonState eButtonState=eButtonStateNormal, int iLeft=0, int iTop=0, int iWidth=40, int iHeight=20);

	//绘制菜单的资源图形
	//具体包括：右箭头、左箭头、菜单圆点、对勾
	void DrawMenuButton(eDCMenuButton eMenuButton, eDCButtonState eButtonState=eButtonStateNormal, int iLeft=0, int iTop=0, int iWidth=40, int iHeight=20);

	//绘制滚动条的资源图形
	void DrawScrollButton(eDCScrollButton eScrollButton, eDCButtonState eButtonState=eButtonStateNormal, int iLeft=0, int iTop=0, int iWidth=40, int iHeight=20);

	//////////////////////////////////////////////////////////////////////////
	// 清空控件内部所有内容
	//清空内容，便会向窗口发送 WM_PAINT 消息
	void InvalidateAll();

	// 填充 GDI 部分范围区域为某一种颜色
	// 注意颜色即为此时的刷子的颜色
	void FillRectArea(int iLeft, int iTop, int iRight, int iBottom);

	// 填充整个 GDI 内存区域为某种颜色
	// 注意颜色即为此时的刷子的颜色
	void FillRectAll();

private:
	//释放内存资源，没有参数
	//这件事必须和析构函数不能写在一块，如果用户执行过程中又切换别的控件怎么办
	// 此时的内存环境和DC内存环境是同生共死的，如果用户重新设置关联的控件，
	// 删除原来的，重新创造一个环境
	void FreeDc();

	//释放资源，仍然使用和 API函数相同的方式
	//删除新创建的笔或者刷子或者是位图的资源，如果没有，直接返回，
	// 不能删除系统预置的画笔或者刷子
	void DeletePenObject();
	void DeleteBrushObject();
	void DeleteFontObject();

private:
	// =======================================================================
	// 类私有成员
	//========================================================================

	HWND m_hWnd;			//用于保存内存环境对应的控件或者窗口的句柄

	HDC m_hDc;				//这里用来保存为控件创建的内存环境		
	HDC m_hDcMem;			//这个用来保存设备兼容的内存环境
	HBITMAP m_hBMP;			//这个用来保存最大的新的位图内容
	HBITMAP m_hBMPOld;		//用于保存最大的旧的位图的内容
	HPEN m_hPen;			//用于保存当前的笔的信息
	HPEN m_hPenOld;			//用于保存原来的笔的信息，便于最后释放资源
	HBRUSH m_hBrush;		//用于保存当前的刷子的信息
	HBRUSH m_hBrushOld;		//用于保存原来的刷子的信息，便于最后释放资源

	HDC m_hDCBmpSet;		//临时创建一个可以内存环境，可以放置位图的内容		
	HBITMAP m_hBmpSet;			//用来保存新的位图的句柄
	HBITMAP m_hBmpSetOld;		//用来保存旧的位图的句柄

	LOGFONT m_hFontLog;		//用于保存当前的字体内容，每次执行绘制文本的时候就不需要再次赋值了
	HFONT m_hFont;			//用于存储当前的文本的句柄，如果没有使用为 NULL
	bool m_fNeedNewFont;	//用于存储当前打印字体是否需要重新创建字体

	RECT m_RectClip;		//用于保存文字输出的范围
	UINT m_uFormatFlag;		//用于保存字符输出的位置的标志变量，横向纵向居中或者对其方式

	bool m_fShowStatus;			//为真表示执行过了 Show() ，对象销毁不需要自动执行，若为假，先询问提示，自动执行 Show() 函数
								//专为懒人设计，如果用户在绘图后忘记从内存环境转出，
								//即，最后没有执行 Show() 函数，自动帮助执行

};

//////////////////////////////////////////////////////////////////////////
// BWindGDIPlus 类的实现
//////////////////////////////////////////////////////////////////////////


class CBWindGdiPlus
{
public:
	//////////////////////////////////////////////////////////////////////////
	/* GDI+ 常用函数说明：
	* 1.DrawLine 绘制直线
	* 2.SetSmoothingMode 设置绘制的效果品质，抗锯齿绘制等设置
	* 3.FillEllipse 绘制椭圆
	* 4.DrawImage 绘制图片
	* 5.DrawString 绘制文本字符串
	* 6.RotateTransform 绘制图像，从左上角进行旋转
	*/
	//////////////////////////////////////////////////////////////////////////

	//构造函数
	CBWindGdiPlus();
	CBWindGdiPlus(HWND hWnd);

	//析构函数
	~CBWindGdiPlus();

	//感觉这里就是强制类型转换
	//重载运算符，此处这个功能太强了，这样子用起来就很方便了，提供了强制类型转换的接口
	operator Graphics* () { return GetGraphics(); }
	operator Pen* () { return GetPen(); }
	operator Brush* () { return GetBrush(); }
	operator Font* () { return GetFont(); }
	operator FontFamily* () { return GetFontName(); }

	//////////////////////////////////////////////////////////////////////////
	//内部对应窗口的设备对象和缓冲对象的设置
	//////////////////////////////////////////////////////////////////////////

	//获得和设置对应的控件或者窗口的句柄
	void hWindowSet(HWND hWnd);
	HWND hWindow();

	//////////////////////////////////////////////////////////////////////////
	// 获取内部的绘图工具的函数
	//////////////////////////////////////////////////////////////////////////

	//获得对应的类内部的画笔的指针
	//对于 gdi+ ，用户可以自己调用其中的函数来实现功能，此处不需要自己再编写对应的内容了
	Pen* GetPen();

	//获取内部的画刷的指针
	SolidBrush* GetBrush();

	//获取内部的渐变画刷的指针
	LinearGradientBrush* GetLinearBrush();

	//获取内部的绘图环境的指针
	Graphics* GetGraphics();

	//获取或重新创建图片对象的指针
	Image* CreateImage(LPTSTR sImagePath);
	Image* GetImage();

	//////////////////////////////////////////////////////////////////////////
	//由于绘制文本的函数太过于难用，此处打算重新写一个简单一点的绘制文本的函数
	//////////////////////////////////////////////////////////////////////////

	//获得和设置字体的大小
	void FontSizeSet(int iSize = 10);
	int FontSize();

	//获得和设置字体的名称
	void FontNameSet(LPCTSTR sFontName = TEXT("微软雅黑"));
	tstring FontName();

	//获得和设置是否字体有下划线
	void FontUnderLineSet(bool fIsUnderline = false);
	bool FontUnderLine();

	//获得和设置是否字体加粗
	void FontBoldSet(bool fIsBold = false);
	bool FontBold();

	//获得和设置是否字体加粗
	void FontItalicSet(bool fIsItalic = false);
	bool FontItalic();

	//==========================================================
	//为了方便类外可以使用内部的对象，所以此处增加扩展接口
	//==========================================================

	//获得类里面的对应字体名称的 FontFamily 对象指针
	FontFamily* GetFontName();

	//获得类里面的 Font 对象的指针
	Font* GetFont();

	//类里面的对于文本绘制的封装函数，可以比较简单的绘制文本内容
	//文本颜色为刷子颜色，如果要设置文本颜色，需要提前设置 Brush 的颜色
	bool DrawString(LPCTSTR sText, float x, float y);

	///////////////////////////////////////////////////////////////////////////
	// 自定义的绘图函数，此处绘制自定义的仪表内容
	//////////////////////////////////////////////////////////////////////////

	//绘制姿态仪,此处绘制姿态仪到对应的内存画布上面，返回的是画布的句柄，如果要在对象上面绘制该内容
	//需要调用函数将其复制到对应的对象上面，默认的绘制长宽比是：5:3，建议用类似的比例，否则会造成失真
	//使用示例：
	/* 其中后两个参数是对应的要绘制的起点坐标
	* 	m_gdiPlus.GetGraphics()->DrawImage(
	*				m_graphics->DrawAttitudeInstrument(0,0,0),
	*				0, 0, iWidth, iHeight);
	*/
	//主要参数：
	// fPitchAngle 俯仰角，范围从 -90 到 90
	// fRollAngle 滚动角，范围从 -90 到 90 
	// fYawAngle 偏航角，范围从 -180 到 180
	//其他参数：
	//暂时略去，不知道哪里可以改动的，感觉就这吧
	Bitmap* DrawAttitudeInstrument(float fPitchAngle, float fRollAngle, float fYawAngle);

	//绘制雷达扫描仪器
	/*
	*/
	//主要参数：
	// dMaxDistance 表示最外圈的距离大小，超出范围的数据将无法有效的绘制出来
	// fAutoDrawData 表示是否自动绘制数据，如果用户想要自己绘制对应的数据，此处选择 false
	// fScanAngDiff 每一次扫描变动的角度大小
	// iScanTimDiff 扫描的时间间隔是多少，动态刷新率
	Bitmap* DrawRada(double dMaxDistance, bool fAutoDrawData=true, float fScanAngDiff = 3,DWORD iScanTimDiff = 100);

	//为雷达输入数据，在显示一次后会自动清空数据，需要不断补充数据才能不断的显示数据内容
	//dDistance 和 dAngle 分别表示距离和角度
	//iBmpShape 如果用户想要显示自己的图片什么的，此处可以设置对应的内容，扩展参数，默认为空，小圆点
	void AddRadaData(double dDistance,double dAngle, Bitmap* iBmpShape = NULL);

private:
	//gdi+ 的初始化函数，此处直接用静态函数，仅仅一个就行了
	static void InitialGdiPlus();

	//卸载gdi+
	static void UnInitialGdiplus();

	//创建一个字体，此函数被调用，自动判断对应的各个参数内容，然后
	//自动改变对应的 FontFamily 和 Font 指针对应的内容
	void CreateMyFont();

private:

	static ULONG_PTR m_gdiplusToken;	//用于初始化gdi的一个变量

	//////////////////////////////////////////////////////////////////////////
	// 基础变量的定义
	HWND m_hWnd;						//内部保存的要绘制的控件的句柄
	Graphics* m_pGraphics;				//绘图的物理设备句柄
	Graphics* m_pGraphicInstruments;	//用于绘图的设备对象
	Bitmap* m_pBmpInstruments;				//此bitmap用于绘制仪表类的东西
	//基本绘图对象
	Pen* m_pPen;						//用于保存类内部对应的笔
	SolidBrush* m_pBrush;				//内部的刷子
	//高级绘图对象
	LinearGradientBrush* m_pLinearBrush;	//渐变画刷
	//图片对象
	Image* m_pImage;						//图片的对象
	//字体对象的设置
	FontFamily* m_pFontName;				//字体的名称
	Font* m_pFont;							//字体的字号，字体的加粗下划线等效果设置
	/*
	* 此处为了减少不断创建和删除字体浪费资源，此处采用暂时保存字体字号的方式，
	* 使用的时候再创建字体和字号内容，这样子就可以减少创建字体这些对象的次数
	* 更好的节省系统资源。
	*/
	tstring m_sFontName;			//保存类里面要使用的字体的名称
	int m_iFontSize;				//保存类里面对应的字体的字号
	int m_iFontStyle;				//字体的风格
	bool m_fFontChange;				//表示字体是否改变，如果改变就自动重新创建
};




