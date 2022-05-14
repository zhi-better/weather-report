//////////////////////////////////////////////////////////////////////////
// BGdi.cpp: CBGdi 类的实现
// 实现 GDI 绘图
//
// Programmed by Zhizhi
// Last Modified: 2020-6-23
//////////////////////////////////////////////////////////////////////////

#include "BWindGDI.h"


CBWindGDI::CBWindGDI( HWND hWnd /* = NULL */ )
{
	// 全体赋初值，否则返回的内容可能很混乱
	// 此处赋初值，那么后面如果没有创建，返回的内容为 NULL， 不会很乱
	m_hWnd = NULL;
	m_hDc = NULL;
	m_hDcMem = NULL;
	m_hPen = NULL;
	m_hPenOld = NULL;
	m_hBrush = NULL;
	m_hBrushOld = NULL;
	m_hBMP = NULL;
	m_hBMPOld = NULL;
	m_hFont = NULL;
	m_fShowStatus = false;
	m_fNeedNewFont = true;
	m_uFormatFlag = 0;
	m_hDCBmpSet = NULL;
	m_hBmpSet = NULL;
	m_hBmpSetOld = NULL;

	//笔的配置的内容的初始化
	memset(&m_hFontLog, 0, sizeof(LOGFONT));
	m_hFontLog.lfHeight = 30;						//字体大小
	m_hFontLog.lfWeight = 400;						//字体不加粗
	m_hFontLog.lfItalic = false;					//字体不是斜体
	m_hFontLog.lfUnderline = false;					//字体不加下划线
	m_hFontLog.lfCharSet = GB2312_CHARSET;			//字体的编码格式
	_tcscpy(m_hFontLog.lfFaceName, TEXT("宋体"));	//字体的名称
	m_hFontLog.lfEscapement = 0;					//设置文本显示的偏移角度

	//设置文本输出的范围变量的初始化
	m_RectClip.left=0;	m_RectClip.top=0;
	m_RectClip.right=0;	m_RectClip.bottom=0;

	// 如果非空，那么交由函数处理
	if (hWnd != NULL)
		hWndDCWindSet(hWnd);
}

CBWindGDI::~CBWindGDI()
{
	//如果没有拷贝到设备环境，自动拷贝显示
	if(!m_fShowStatus)	Show();

	//删除内存环境，释放 DC 资源
	FreeDc();
}

void CBWindGDI::UpdateDCRange()
{
	//先删除原来的控件的内容，然后重新设置成该句柄，重新获得对应的控件的大小
	FreeDc();
	hWndDCWindSet(m_hWnd);
}

bool CBWindGDI::hWndDCWindSet( HWND hWnd )
{
	//创建一个兼容的内存环境和设备环境

	// 如果已经创建过内存环境，此处应该先绘制已经绘制过的东西
	// 然后释放原来的资源，然后重新创建一个设备环境
	if(m_hDc != NULL && m_hWnd != hWnd)
	{
		// 先显示内容，然后删除这个设备环境，然后重新新建一个
		// 第二次进入此函数 m_hDc 经过 FreeDc(); 就被清空了，所以不会进来这个分支了
		Show();
		FreeDc();
		hWndDCWindSet(hWnd);
	}

	m_hDc = GetDC(hWnd);	//创建设备环境

	// 如果创建失败直接返回失败
	//成功则给模块变量 m_hWnd 赋值
	if(m_hDc == NULL)	return false;
	else	m_hWnd = hWnd;
	
	//首先建立兼容的内存环境 hDcMem
	m_hDcMem = CreateCompatibleDC(m_hDc);
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	m_hBMP = CreateCompatibleBitmap(m_hDc, rc.right-rc.left, rc.bottom-rc.top);		//建立兼容的位图大小
	m_hBMPOld = (HBITMAP)SelectObject(m_hDcMem, m_hBMP);							//选中新的位图

	//此处默认的文本输出位置在整个控件的大小的控件里面
	m_RectClip.right=rc.right-rc.left;
	m_RectClip.bottom=rc.bottom-rc.top;
	m_RectClip.left=0;
	m_RectClip.top=0;
	
	//创建属于自己的画笔和刷子
	// 默认笔为黑色实线，宽度为 1 
	// 默认刷子为白色
	m_hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
	m_hBrush = CreateSolidBrush(RGB(255,255,255));
	m_hBrushOld = (HBRUSH)SelectObject(m_hDcMem, m_hBrush);

	return true;
}

HWND CBWindGDI::hWndDCWind()
{
	return m_hWnd;
}

HDC CBWindGDI::hWndCompatibleDC()
{
	return m_hDcMem;
}

HDC CBWindGDI::hWndDC()
{
	return m_hDc;
}

void CBWindGDI::Show()
{
	//如果没有创建绘图环境，那么直接返回
	if(m_hDc == NULL)	return;

	//将绘制的图像全部复制到指定的设备环境中
	RECT rc;
	GetWindowRect(m_hWnd, &rc);		//通过控件的句柄获得控件的大小

	//然后将控件大小的范围内所有的内容复制过去
	BitBlt(m_hDc, 0, 0, rc.right-rc.left, rc.bottom-rc.top, m_hDcMem, 0, 0, SRCCOPY);

	m_fShowStatus = true;	//表示已经执行过拷贝环节了
}

void CBWindGDI::FreeDc()
{
	// 做好一切的善后工作
	// 如果设置过环境，那么 m_hDc 不为 NULL
	// 否则，不进行此操作
	if (m_hDc != NULL)
	{
		//首先要删除新创建的资源，使得 m_hDcMem 对象为创建之初的样子
		SelectObject(m_hDcMem, m_hBMPOld);				//选入原来的位图对象
		DeleteObject(m_hBMP);							//hBMP选出内存环境，可删除
		DeletePenObject();								//删除新的笔的资源
		DeleteBrushObject();							//如果不是空刷子，那么删除新的刷子资源
														//空刷子是系统资源，不能删除
		DeleteFontObject();								//删除字体对象

		if(m_hDCBmpSet)			//如果已经创建过放置位图的内存环境，释放资源
		{
			//由于内部位图，每次操作都会自动释放，所以不用此处释放了
			DeleteDC(m_hDCBmpSet);
		}

		DeleteDC(m_hDcMem);					//需要保证此处的 m_hDcMem 对象为原来的创建的对象
		ReleaseDC(m_hWnd, m_hDc);			//释放设备环境资源

		// 记录的变量全部清空，全为 NULL
		m_hDc = NULL;	m_hDcMem = NULL;
		m_hBMP = NULL;	m_hBMPOld = NULL;

		//控件大小包括文本输出的控件位置全部清空
		m_RectClip.left=0;	m_RectClip.top=0;
		m_RectClip.right=0;	m_RectClip.bottom=0;
		m_uFormatFlag = 0;			//文本输出的标志变量

		m_fShowStatus = false;		//注意，释放资源所有变量归为初始变量数值
	}
}

void CBWindGDI::DeletePenObject()
{
	//如果新的笔为空，那么直接返回
	if(m_hPen == NULL)	return;

	//删除新的画笔，所有只要改变画笔颜色都需要释放重新添加
	SelectObject(m_hDcMem, m_hPenOld);	//选入原来的画笔
	DeleteObject(m_hPen);				// hPen 选出内存环境，可删除
	m_hPen = NULL;						//手动设置 m_hPen = NULL;
}

void CBWindGDI::DeleteBrushObject()
{
	//如果新的刷子句柄为空或者是空刷子，那么直接返回
	if(m_hBrush == NULL)	return;

	if (!BrushNULL())
	{
		//删除新的刷子，所有只要改变画笔颜色都需要释放重新添加
		SelectObject(m_hDcMem, m_hBrushOld);	//选入原来的刷子
		DeleteObject(m_hBrush);					// hPen 选出内存环境，可删除
	}
	m_hBrush = NULL;						//手动设置 m_hBrush = NULL;
}

COLORREF CBWindGDI::PenColor()
{
	if(m_hPen == NULL)	return 0;

	// 获得当前的笔的颜色
	LOGPEN pPen;
	int ret = GetObject(m_hPen, sizeof(LOGPEN), &pPen);

	//如果成功返回颜色，失败返回 0
	return (ret == 0) ? 0 : pPen.lopnColor;
}

void CBWindGDI::PenColorSet( COLORREF cPenColor )
{
	if(m_hPen == NULL)	return;

	int iWidth = PenWidth();	//获得当前的笔的宽度
	int iStyle = PenStyle();	//获得当前的笔的样式
	DeletePenObject();			//删除原来的创建的笔
	
	m_hPen = CreatePen(iStyle, iWidth, cPenColor);
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
}

int CBWindGDI::PenWidth()
{
	if(m_hPen == NULL)	return 0;

	// 获得当前的笔的颜色
	LOGPEN pPen;
	int ret = GetObject(m_hPen, sizeof(LOGPEN), &pPen);

	//如果成功返回颜色，失败返回 0
	// pPen.lopnWidth.x 双指针，真的服了，哈哈哈哈
	return (ret == 0) ? 0 : pPen.lopnWidth.x;
}

void CBWindGDI::PenWidthSet( int iWidth )
{
	if(m_hPen == NULL)	return;

	//宽度小于0，直接返回
	if(iWidth <= 0)	return;

	int iStyle = PenStyle();	//获得当前的笔的宽度
	COLORREF iColor = PenColor();	//获得当前的笔的样式
	DeletePenObject();			//删除原来的创建的笔

	m_hPen = CreatePen(iStyle, iWidth, iColor);
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
}

int CBWindGDI::PenStyle()
{
	if(m_hPen == NULL)	return -1;

	// 获得当前的笔的颜色
	LOGPEN pPen;
	int ret = GetObject(m_hPen, sizeof(LOGPEN), &pPen);

	//如果成功返回颜色，失败返回 0
	return (ret == 0) ? -1 : int(pPen.lopnStyle);
}

void CBWindGDI::PenStyleSet(int iStyle)
{
	if(m_hPen == NULL)	return;

	//如果样式小于0，不存在，直接返回
	if(iStyle < 0)	return;

	int iWidth = PenWidth();	//获得当前的笔的宽度
	COLORREF iColor = PenColor();	//获得当前的笔的样式
	DeletePenObject();			//删除原来的创建的笔

	m_hPen = CreatePen(iStyle, iWidth, iColor);
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
}

COLORREF CBWindGDI::BrushColor()
{
	if(m_hBrush == NULL)	return 0;

	// 获得当前的刷子的颜色
	LOGBRUSH pBrush;
	int ret = GetObject(m_hBrush, sizeof(LOGBRUSH), &pBrush);

	//如果成功返回颜色，失败返回 0
	return (ret == 0) ? 0 : pBrush.lbColor;
}

void CBWindGDI::BrushColorSet( COLORREF cBrushColor )
{
	if(m_hBrush == NULL)	return;

	DeleteBrushObject();			//删除原来的创建的刷子
	
	// 创建并选中现在的刷子
	m_hBrush = CreateSolidBrush(cBrushColor);
	m_hBrushOld = (HBRUSH)SelectObject(m_hDcMem, m_hBrush);
}

void CBWindGDI::InvalidateAll()
{
	// 清空控件的所有的内容
	// 很简单的语句 0x0009083e
	InvalidateRect(m_hWnd, NULL, true);
}

void CBWindGDI::FillRectAll()
{
	if(m_hDc == NULL)	return;

	//将所创建内存环境所关系的控件全部涂满 m_hBrush 的颜色
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	//此处要转换为相对于绘制区域的坐标系
	rc.right = rc.right-rc.left;
	rc.bottom = rc.bottom-rc.top;
	rc.left = 0;
	rc.top = 0;
	//涂色函数
	FillRect(m_hDcMem, &rc, m_hBrush);

}

void CBWindGDI::FillRectArea( int iLeft, int iTop, int iRight, int iBottom )
{
	if(m_hDc == NULL)	return;

	//填充背景
	//在参数所围的区域用刷子的颜色涂满
	RECT rc;
	rc.left = iLeft;
	rc.top = iTop;
	rc.right = iRight;
	rc.bottom = iBottom;
	//涂色函数
	FillRect(m_hDcMem, &rc, m_hBrush);
}

bool CBWindGDI::BrushNULL()
{
	if(m_hBrush == NULL)	return false;

	// 获得当前的笔的颜色
	LOGBRUSH pBrush;
	int ret = GetObject(m_hBrush, sizeof(LOGPEN), &pBrush);

	//如果为空，返回真，如果是非空，返回假
	return (pBrush.lbStyle == BS_NULL);
}

void CBWindGDI::BrushNullSet( bool fBrushNULL , COLORREF cBrushColor/* = 0xffffff*/)
{
	if(m_hBrush == NULL)	return;

	if (fBrushNULL)
	{
		// 如果此时刷子不是空，创建空刷子，否则直接返回
		if (!BrushNULL())
		{
			DeleteBrushObject();			//删除原来的创建的刷子
			// 创建并选中现在的刷子
			m_hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			m_hBrushOld = (HBRUSH)SelectObject(m_hDcMem, m_hBrush);
		}
	}
	else
	{
		if (BrushNULL())
		{
			//注意此处最不一样的就是，这里不能删除空刷子
			//所以要自己重新写一下代码，特殊处理
			// 创建并选中现在的刷子，白色
			m_hBrush = CreateSolidBrush(cBrushColor);
			m_hBrushOld = (HBRUSH)SelectObject(m_hDcMem, m_hBrush);
		}
	}
	
}

bool CBWindGDI::PenSet( COLORREF cPenColor, int iWidth, int iStyle )
{
	// 本函数主要是为了减少后面的代码数量，一次性设置，省得中间得多次
	//但是，使用此函数必须三个参数全部设定，如果缺省感觉就失去了创建这个函数的意义了
	
	if(iWidth<=0 && iStyle<0)	return false;		//如果参数不合理，返回失败
	if(m_hPen == NULL)	return false;
	
	// 如果满足条件，删除原来的画笔资源，重新设置新的画笔资源
	DeletePenObject();

	m_hPen = CreatePen(iStyle, iWidth, cPenColor);
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
	return true;
}

void CBWindGDI::BitmapSet( unsigned short idBmp, int xDest/*=0*/,int yDest/*=0*/,int iDestWidth/*=-1*/,int iDestHeight/*=-1*/,bool f_Transparent/*=false*/,COLORREF c_TransColor/*=0x000000*/,double dAngle/*=0*/,bool f_Stretch/*=false*/,int xOrigin/*=0*/,int yOrigin/*=0*/,int iWidthSrc/*=-1*/,int iHeightSrc/*=-1 */ )
{
	// 设置一个位图显示到指定的位置，还可以设定位图的大小和是否透明
	//真的是醉了，这简直就是自己写的改的次数最多的一个函数的代码，从刚开始的很混乱，到后来的功能全面且强大，太不容易了

	//如果没有创建位图，那么就创建一个
	if (!m_hDCBmpSet)
		m_hDCBmpSet = CreateCompatibleDC(m_hDc);

	//=========================== 加载资源 ===============================
	// 创建设备环境 hDCBmp，保存“要放置的位图”的图案
	//注意这里是在设备环境重新创建一个内存环境，所以参数是 m_hDc
	m_hBmpSet = LoadBitmap(pApp->hInstance, MAKEINTRESOURCE(idBmp));
	m_hBmpSetOld = (HBITMAP)SelectObject(m_hDCBmpSet, m_hBmpSet);

	//======================== 对于默认参数的处理工作 ===============================
	//将位图的图案绘制到内存设备环境
	//注意此处是将现在的内存环境的内容拷贝到类的内存环境，所以参数是 m_hDcMem
	//获得位图的宽度和长度
	BITMAP pBmp;
	GetObject(m_hBmpSet, sizeof(BITMAP), &pBmp);
	//获得位图的位置，如果不合法，全部转换为 0，0
	if(xDest<0)	xDest=0;
	if(yDest<0)	yDest=0;
	//统一处理参数问题，所有的合法不合法统一变量来表示
	RECT rc;
	GetWindowRect(m_hWnd,&rc);
	iDestWidth = (iDestWidth<0 ? rc.right-rc.left : iDestWidth);
	iDestHeight = (iDestHeight<0 ? rc.bottom-rc.top : iDestHeight);
// 	iDestWidth = (iDestWidth<0 ? pBmp.bmWidth : iDestWidth);
// 	iDestHeight = (iDestHeight<0 ? pBmp.bmHeight : iDestHeight);

	//首先此处必定需要创建一个临时的 设备环境用来接收中间的内容
	//这个使用方法真的是一点都不能出错，建立环境，建立位图，选中位图，！！！！！！！！

	//============================ 准备工作 =========================================
	//首先创建的内存设备环境是用于缩放的时候储存缩放的图片内容
	HDC hDcStretch = CreateCompatibleDC(m_hDc);
	HBITMAP hBmpStretch = CreateCompatibleBitmap(m_hDc, iDestWidth, iDestHeight);
	HBITMAP hBmpStretchOld = (HBITMAP)SelectObject(hDcStretch, hBmpStretch);
	//第二个是如果进行了旋转，用于存储旋转的位图的环境
	HDC hDcAngle = CreateCompatibleDC(m_hDc);
	HBITMAP hBmpAngle = CreateCompatibleBitmap(m_hDc, iDestWidth, iDestHeight);
	HBITMAP hBmpAngleOld = (HBITMAP)SelectObject(hDcAngle, hBmpAngle);

	//============================ 进行核心图片处理环节 ======================================
	//裂开了，刚开始以为仅仅是创建多个内存环境，不重叠使用就可以了，后来发现每一个内存环境还得选入背景色的画笔
	//否则黑色特别丑，这个真的是烦死了，还得重新创建刷子
	HDC hDcTemp = hDcStretch;		//主要用于中间的内容的转换，并不会建立新的设备环境
	HBRUSH hBrush = CreateSolidBrush(c_TransColor);
	rc.left=0;				rc.top=0;
	rc.right=iDestWidth;	rc.bottom=iDestHeight;

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 图片缩放处理 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//为了类里面更加有顺序性，决定此处将模块内容升级，先进行位图的缩放，然后进行位图的旋转，最后进行透明处理
	//背景填充
	FillRect(hDcStretch,&rc,hBrush);
	if(f_Stretch)
	{
		//注意此处操作不能改变位图再里面的位置，全部为0，最后一步统一设置位置
		StretchBlt(hDcStretch, 0, 0,iDestWidth,iDestHeight,
		m_hDCBmpSet, xOrigin,yOrigin,
		(iWidthSrc<0 ? pBmp.bmWidth-xOrigin : iWidthSrc),
		(iHeightSrc<0 ? pBmp.bmHeight-yOrigin: iHeightSrc),
		SRCCOPY);
	}
	else
		BitBlt(hDcStretch, 0, 0, iDestWidth, iDestHeight, m_hDCBmpSet,0,0, SRCCOPY);
	
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 图片旋转处理 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//如果角度不为零，那么就执行旋转的函数，否则就直接跳过此步骤
	if(dAngle)
	{
		FillRect(hDcAngle,&rc,hBrush);
		//注意此处操作和上面一样，不能改变位图再里面的位置，全部为0，最后一步统一设置位置
		TBRotateAnyAngle(hDcAngle, 0,0, hDcTemp, dAngle,RGB(255,255,255),iDestWidth,iDestHeight);
		hDcTemp = hDcAngle;
	}

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ 图片透明处理 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//然后判断是否透明处理，由于上面已经使用一个临时变量把这里的源设备环境统一了，所以此处直接调用
	if(f_Transparent)
	{
		TBDrawTransParentBitmap(m_hDcMem,xDest, yDest, hDcTemp, c_TransColor,
			0,0, iDestWidth, iDestHeight);
	}
	else
	{
		//最后统一把上面处理过的位图给直接拷贝到内存中，这样每一步分开，逻辑清晰
		BitBlt(m_hDcMem, xDest, yDest, iDestWidth, iDestHeight, hDcTemp,0,0, SRCCOPY);
	}

	//======================================= 释放资源 ========================================
	//释放临时用于数据交换的内存环境，此处作用就在于不同的操作之间的交换
	SelectObject(hDcStretch,hBmpStretchOld);
	DeleteObject(hBmpStretch);
	DeleteDC(hDcStretch);
	SelectObject(hDcAngle,hBmpAngleOld);
	DeleteObject(hBmpAngle);
	DeleteDC(hDcAngle);
	//删除刷子
	DeleteObject(hBrush);

	//仅仅删除位图对象，内存环境保留继续使用
	SelectObject(m_hDCBmpSet, m_hBmpSetOld);
	DeleteObject(m_hBmpSet);
	m_hBmpSet = NULL;	m_hBmpSetOld = NULL;

}

void CBWindGDI::PrintText( LPTSTR szText, int ileft/*=-1*/, int itop/*=-1*/, int iwidth/*=-1*/, int iheight/*=-1*/)
{
	//如果需要创建过字体，如果已经创建过字体，先删除
	if(m_fNeedNewFont)
	{
		DeleteFontObject();
		m_hFont = CreateFontIndirect(&m_hFontLog); 
		m_fNeedNewFont = false;
	}
	SelectObject(m_hDcMem, m_hFont);	//选中当前创建的字体，因为每次都会自动选出，所以此处需要重复选择

	SetClip(ileft,itop,iwidth,iheight);		//执行一次设置输出位置的函数吧，更加方便使用

	//打印文本内容
	DrawText(m_hDcMem, szText, _tcslen(szText), &m_RectClip, m_uFormatFlag);
	
}

void CBWindGDI::DeleteFontObject()
{
	//如果文本配置非空，那么删除，否则直接返回
	//由于每次字体都会自动恢复，所以不需要选出然后删除
	if(m_hFont)	
	{
		DeleteObject(m_hFont);				//如果创建了文本的句柄，此处需要释放
		m_hFont = NULL;
	}
}

void CBWindGDI::FontSizeSet( int iFontSize )
{
	//如果设置的字体大小和当前相等，直接返回，否则改变配置变量，设置标志变量为 true
	if (FontSize() == iFontSize)
		return;
	else
	{
		m_hFontLog.lfHeight = iFontSize;
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontNameSet( LPTSTR szFontName )
{
	//如果设置的字体名称和当前相等，直接返回，否则改变配置变量，设置标志变量为 true
	//裂开了，这里真的不知道怎么弄了 , _tcscmp(m_hFontLog.lfFaceName, TEXT("宋体"))
	tstring name1 = szFontName;
	tstring name2 = m_hFontLog.lfFaceName;
	if (name1 == name2)
	{
		return;
	}
	else
	{
		_tcscpy(m_hFontLog.lfFaceName, szFontName);
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontBondSet( bool fBond )
{
	//如果设置的字体名称和当前相等，直接返回，否则改变配置变量，设置标志变量为 true
	if (FontBond() && fBond)
	{
		//如果是否加粗的标志变量和当前的宽度值是否一致，如果一致，直接返回
		return;
	}
	else if (m_hFontLog.lfWeight == FW_NORMAL && !fBond)
	{
		return;
	}
	else
	{
		m_hFontLog.lfWeight = fBond ? FW_BOLD : FW_NORMAL;
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontUnderLineSet( bool fUnderLine )
{
	//判断设置的字体是否具有下划线，如果和当前的设定一致直接返回，否则改变配置变量，设置标志变量为 true
	if (FontUnderLine() == fUnderLine)
	{
		return;
	}
	else
	{
		m_hFontLog.lfUnderline = fUnderLine ? TRUE:FALSE;
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontItalicSet( bool fItalic )
{
	//判断设置的字体是否斜体，如果和当前的设定一致直接返回，否则改变配置变量，设置标志变量为 true
	if (FontItalic() == fItalic)
	{
		return;
	}
	else
	{
		m_hFontLog.lfItalic = fItalic ? TRUE:FALSE;
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontColorSet( COLORREF cColor )
{
	//设置当前文本的颜色
	if (FontColor() == cColor)
	{
		return;
	}
	else
		SetTextColor(m_hDcMem, cColor);
}

void CBWindGDI::FontBKColorSet( COLORREF cColor )
{
	//设置当前文本背景的颜色
	if (FontBKColor() == cColor)
	{
		return;
	}
	else
		SetBkColor(m_hDcMem, cColor);
}

void CBWindGDI::BackTransparentSet( bool fTransparent )
{
	//设置当前文本背景的颜色
	if (fTransparent)
		SetBkMode(m_hDcMem, TRANSPARENT);		// 文字背景透明
	else
		SetBkMode(m_hDcMem, OPAQUE);			// 文字背景不透明

}

void CBWindGDI::SetClip( int ileft/*=-1*/, int itop/*=-1*/, int iwidth/*=-1*/, int iheight/*=-1*/ )
{
	//设置打印文本的范围
	if(ileft>=0)	m_RectClip.left=ileft;		
	if(itop>=0)	m_RectClip.top=itop;
	if(iwidth>=0)	m_RectClip.right=iwidth;	
	if(iheight>=0)	m_RectClip.bottom=iheight;
	
}

void CBWindGDI::AlignSet( UINT uAlign )
{
	//如果标志变量不同，那么直接返回
	if (uAlign != 0 && uAlign != 1 && uAlign != 2)
		return;

	m_uFormatFlag = m_uFormatFlag & (~DT_LEFT);
	m_uFormatFlag = m_uFormatFlag & (~DT_CENTER);
	m_uFormatFlag = m_uFormatFlag & (~DT_RIGHT);

	m_uFormatFlag = m_uFormatFlag | uAlign;
}

void CBWindGDI::VAlignSet( UINT uVAlign )
{
	//如果标志变量不同，那么直接返回
	if (uVAlign != 0 && uVAlign != 1 && uVAlign != 2)
		return;

	m_uFormatFlag = m_uFormatFlag & (~DT_TOP);
	m_uFormatFlag = m_uFormatFlag & (~DT_VCENTER);
	m_uFormatFlag = m_uFormatFlag & (~DT_BOTTOM);

	m_uFormatFlag = m_uFormatFlag | uVAlign*4;
	//纵向的位置是需要时单行的内容
	m_uFormatFlag = m_uFormatFlag | DT_SINGLELINE;
	
}

void CBWindGDI::FontSpinDegreeSet( double iDegree )
{
	//由于那里是角度的 10 倍，所以要乘10
	int iAngle = int(iDegree*10);
	m_hFontLog.lfEscapement = iAngle;
	m_fNeedNewFont = true;
}

void CBWindGDI::SingleLineSet( bool fSingleLine )
{
	//通过标志变量来控制打印文本的标志的内容
	if (fSingleLine)
		m_uFormatFlag = m_uFormatFlag | DT_SINGLELINE;
	else
		m_uFormatFlag = m_uFormatFlag & (DT_SINGLELINE);
}

void CBWindGDI::Draw3DBorder( int iLeft, int iTop, int iWidth, int iHeight, EdgeType eType/*=eEdgeBump*/, int iFlatMono/*=0*/, UINT iBorderType/*=15*/, bool fDiagonal/*=false*/ )
{
	//绘制3D边框
	//首先确定绘制范围
	RECT rc;	
	GetWindowRect(m_hWnd, &rc);
	//如果参数为负的，那么默认到控件的右下边缘，否则用默认的边框大小
	if (iWidth<0)	
		rc.right = rc.right-rc.left-iLeft;
	else
		rc.right=iLeft+iWidth;
	if (iHeight<0)	
		rc.top = rc.bottom-rc.top-iHeight;
	else
		rc.bottom=iTop+iHeight;

	rc.left=iLeft;			rc.top=iTop;
	//确定绘制的边框位置
	UINT iFlag = 0;
	if(iBorderType & 1)	iFlag = iFlag | BF_LEFT;
	if(iBorderType & 2)	iFlag = iFlag | BF_TOP;
	if(iBorderType & 4)	iFlag = iFlag | BF_RIGHT;
	if(iBorderType & 8)	iFlag = iFlag | BF_BOTTOM;
	if (iFlatMono == 1)
		iFlag = iFlag | BF_FLAT;
	else if(iFlatMono == 2)
		iFlag = iFlag | BF_MONO;

	if(fDiagonal)	iFlag = iFlag| BF_DIAGONAL;

	DrawEdge(m_hDcMem, &rc, eType, iFlag);
}

void CBWindGDI::DrawCaptionButton( eDCCaptionButton eCaptionButton, eDCButtonState eButtonState/*=eButtonNormal*/, int iLeft/*=0*/, int iTop/*=0*/, int iWidth/*=40*/, int iHeight/*=20*/ )
{
	//绘制标题栏的按钮元素
	//首先确定绘制范围
	RECT rc;	
	rc.left=iLeft;			rc.top=iTop;
	rc.right=iLeft+iWidth;	rc.bottom=iTop+iHeight;

	DrawFrameControl(m_hDcMem, &rc, DFC_CAPTION, eButtonState | eCaptionButton);
}

void CBWindGDI::DrawButton( eDCButton eButton, eDCButtonState eButtonState/*=eButtonNormal*/, int iLeft/*=0*/, int iTop/*=0*/, int iWidth/*=40*/, int iHeight/*=20*/ )
{
	//绘制按钮元素
	//首先确定绘制范围
	RECT rc;	
	rc.left=iLeft;			rc.top=iTop;
	rc.right=iLeft+iWidth;	rc.bottom=iTop+iHeight;

	DrawFrameControl(m_hDcMem, &rc, DFC_BUTTON, eButtonState | eButton);
}

void CBWindGDI::DrawMenuButton( eDCMenuButton eMenuButton, eDCButtonState eButtonState/*=eButtonStateNormal*/, int iLeft/*=0*/, int iTop/*=0*/, int iWidth/*=40*/, int iHeight/*=20*/ )
{
	//绘制菜单的元素
	//首先确定绘制范围
	RECT rc;	
	rc.left=iLeft;			rc.top=iTop;
	rc.right=iLeft+iWidth;	rc.bottom=iTop+iHeight;

	DrawFrameControl(m_hDcMem, &rc, DFC_MENU, eButtonState | eMenuButton);
}

void CBWindGDI::DrawScrollButton( eDCScrollButton eScrollButton, eDCButtonState eButtonState/*=eButtonStateNormal*/, int iLeft/*=0*/, int iTop/*=0*/, int iWidth/*=40*/, int iHeight/*=20*/ )
{
	//绘制滚动条的元素
	//首先确定绘制范围
	RECT rc;	
	rc.left=iLeft;			rc.top=iTop;
	rc.right=iLeft+iWidth;	rc.bottom=iTop+iHeight;

	DrawFrameControl(m_hDcMem, &rc, DFC_SCROLL, eButtonState | eScrollButton);
}

COLORREF CBWindGDI::FontColor()
{
	return GetTextColor(m_hDcMem);
}

COLORREF CBWindGDI::FontBKColor()
{
	return GetBkColor(m_hDcMem);
}

bool CBWindGDI::BackTransparent()
{
	return (GetBkMode(m_hDcMem) == TRANSPARENT);
}

int CBWindGDI::FontSize()
{
	return m_hFontLog.lfHeight;
}

LPTSTR CBWindGDI::FontName()
{
	return m_hFontLog.lfFaceName;
}

bool CBWindGDI::FontBond()
{
	return (m_hFontLog.lfWeight == FW_BOLD);
}

bool CBWindGDI::FontUnderLine()
{
	return (m_hFontLog.lfUnderline == TRUE);
}

bool CBWindGDI::FontItalic()
{
	return (m_hFontLog.lfItalic == TRUE);
}

bool CBWindGDI::SingleLine()
{
	return (m_uFormatFlag & DT_SINGLELINE) ? true : false;
}

int CBWindGDI::FontSpinDegree()
{
	return m_hFontLog.lfEscapement/10;
}

int CBWindGDI::Align()
{
	if (m_uFormatFlag & DT_LEFT)
		return DT_LEFT;
	else if (m_uFormatFlag & DT_CENTER)
		return DT_CENTER;
	else if(m_uFormatFlag & DT_RIGHT)
		return DT_RIGHT;
	else
		return -1;
}

int CBWindGDI::VAlign()
{
	if (m_uFormatFlag & DT_VCENTER)
		return 1;
	else if (m_uFormatFlag & DT_TOP)
		return 0;
	else if(m_uFormatFlag & DT_BOTTOM)
		return 2;
	else
		return -1;
}


//////////////////////////////////////////////////////////////////////////
// BWindGDIPlus 类的实现
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 由于变量周期的限制，此处就在类外面定义对应的画笔等内容吧
//////////////////////////////////////////////////////////////////////////

CBWindGdiPlus::CBWindGdiPlus()
{
	InitialGdiPlus();

	//初始化对应的变量内容
	m_hWnd = NULL;
	m_pPen = new Pen(Color::Black, 2);
	m_pBrush = new SolidBrush(Color::White);
	m_pLinearBrush = NULL;
	m_pGraphics = NULL;				//此处没有对应的句柄，不能创建graphics对象
	m_pImage = NULL;
	m_pBmpInstruments = NULL;
	m_pGraphicInstruments = NULL;

	//字体对象的参数初始化
	m_iFontStyle = 0;
	m_fFontChange = true;	//字体设置函数会自动改变此参数，从而控制是否绘制的时候创建新的字体对象
	m_pFont = NULL;
	m_pFontName = NULL;
	m_sFontName = TEXT("微软雅黑");
	m_iFontSize = 10;
}

CBWindGdiPlus::CBWindGdiPlus(HWND hWnd /*= NULL*/)
{
	CBWindGdiPlus();

	m_hWnd = hWnd;
}

CBWindGdiPlus::~CBWindGdiPlus()
{
	//删除动态创建的对象内容
	if (m_pPen)			delete m_pPen;
	if (m_pBrush)		delete m_pBrush;
	if (m_pGraphics)	delete m_pGraphics;
	if (m_pLinearBrush)	delete m_pLinearBrush;
	if (m_pImage)		delete m_pImage;
	if (m_pFont)		delete m_pFont;
	if (m_pFontName)	delete m_pFontName;
	if (m_pBmpInstruments)	delete m_pBmpInstruments;
	if (m_pGraphicInstruments)	delete m_pGraphicInstruments;

	UnInitialGdiplus();
}

void CBWindGdiPlus::hWindowSet(HWND hWnd)
{
	if (hWnd == m_hWnd)		//创建过直接返回
		return;

	//如果已经创建过内容，那么删除原来的内容，重新创建
	if (m_pGraphics)
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
	}
	//重新创建一个新的绘图对象
	m_pGraphics = new Graphics(hWnd);
	m_hWnd = hWnd;			//更新窗口句柄

}

HWND CBWindGdiPlus::hWindow()
{
	return m_hWnd;
}

Pen* CBWindGdiPlus::GetPen()
{
	return m_pPen;
}

SolidBrush* CBWindGdiPlus::GetBrush()
{
	return m_pBrush;
}

LinearGradientBrush* CBWindGdiPlus::GetLinearBrush()
{
	//如果不存在自动创建后返回对应的内容
	if (!m_pLinearBrush)
	{
		m_pLinearBrush = new LinearGradientBrush(Point(300, 50), Point(500, 150),
			Color(255, 0, 255, 0), Color(0, 0, 255, 0));
	}
	return m_pLinearBrush;
}

Graphics* CBWindGdiPlus::GetGraphics()
{
	return m_pGraphics;
}

Image* CBWindGdiPlus::CreateImage(LPTSTR sImagePath)
{
	//如果不存在自动创建后返回对应的内容
	if (!m_pImage)
	{
		m_pImage = new Image(sImagePath);
	}
	else
	{
		delete m_pImage;
		m_pImage = NULL;
		m_pImage = new Image(sImagePath);
	}
	return m_pImage;
}

Image* CBWindGdiPlus::GetImage()
{
	return m_pImage;
}

void CBWindGdiPlus::FontSizeSet(int iSize /*= 10*/)
{
	//字号必须合法
	if (iSize > 0)
		m_iFontSize = iSize;
	m_fFontChange = true;		//表示字体格式发生了改变
}

int CBWindGdiPlus::FontSize()
{
	return m_iFontSize;
}

void CBWindGdiPlus::FontNameSet(LPCTSTR sFontName /*= TEXT("微软雅黑")*/)
{
	//字体必须为真的才行
	if (sFontName)
		m_sFontName = sFontName;
	m_fFontChange = true;		//表示字体格式发生了改变
}

tstring CBWindGdiPlus::FontName()
{
	return m_sFontName;
}

void CBWindGdiPlus::FontUnderLineSet(bool fIsUnderline /*= false*/)
{
	fIsUnderline ? m_iFontStyle &= 4 : m_iFontStyle &= ~4;
	m_fFontChange = true;		//表示字体格式发生了改变
}

bool CBWindGdiPlus::FontUnderLine()
{
	return bool(m_iFontStyle & 4);
}

void CBWindGdiPlus::FontBoldSet(bool fIsBold /*= false*/)
{
	fIsBold ? m_iFontStyle &= 1 : m_iFontStyle &= ~1;
	m_fFontChange = true;		//表示字体格式发生了改变
}

bool CBWindGdiPlus::FontBold()
{
	return m_iFontStyle & 1;
}

void CBWindGdiPlus::FontItalicSet(bool fIsItalic /*= false*/)
{
	fIsItalic ? m_iFontStyle &= 2 : m_iFontStyle &= ~2;
	m_fFontChange = true;		//表示字体格式发生了改变
}

bool CBWindGdiPlus::FontItalic()
{
	return bool(m_iFontStyle & 2);
}

FontFamily* CBWindGdiPlus::GetFontName()
{
	//如果为空，自动创建默认的字体
	CreateMyFont();

	return m_pFontName;
}

Font* CBWindGdiPlus::GetFont()
{
	//如果为空，自动创建默认的字体
	CreateMyFont();

	return m_pFont;
}

bool CBWindGdiPlus::DrawString(LPCTSTR sText, float x, float y)
{
	//如果此处设备环境不存在，自动返回失败
	if (!m_pGraphics)
		return false;
	CreateMyFont();		//每次都要重新判断是否要创建对应的新字体

	PointF pPosToShow(x, y);
	m_pGraphics->DrawString(sText, -1, m_pFont, pPosToShow, m_pBrush);

	return true;
}

Bitmap* CBWindGdiPlus::DrawAttitudeInstrument(float fPitchAngle, float fRollAngle, float fYawAngle)
{
	//角度值预处理
	if (fPitchAngle > 90)	fPitchAngle = 90;
	if (fPitchAngle < -90)	fPitchAngle = -90;
	if (fRollAngle > 90)	fRollAngle = 90;
	if (fPitchAngle < -90)	fRollAngle = -90;

	const int iWidth = 500;
	const int iHeight = 300;
	Point center(int(0.5 * iWidth), int(0.5 * iHeight));

	//绘图参数计算
	int PitchScaleWidth = 15;		//绘图宽度
	int PitchScaleHeight = 30;		//间隔高度
	double PitchRange = 40;
	int PitchTicksCount = 10;		//这个是绘制的线条的条数
	int i;
	int iStart = int((fPitchAngle / 5) * PitchScaleHeight);	//初始的要补偿的高度

	//画布和设备对象一起创建，同生共死
	if (m_pBmpInstruments)
	{
		//只有当区域不同的时候重新创建
		if (m_pBmpInstruments->GetWidth()!=500 || m_pBmpInstruments->GetHeight() != 300)
		{
			delete m_pBmpInstruments;
			m_pBmpInstruments = NULL;
			m_pBmpInstruments = new Bitmap(500, 300);
			if (m_pGraphicInstruments)	delete m_pGraphicInstruments;
			m_pGraphicInstruments = new Graphics(m_pBmpInstruments);
		}
	}
	else
	{
		m_pBmpInstruments = new Bitmap(500, 300);
		if (m_pGraphicInstruments)	delete m_pGraphicInstruments;
		m_pGraphicInstruments = new Graphics(m_pBmpInstruments);
	}

	m_pGraphicInstruments->ResetTransform();		//重置坐标面
	m_pGraphicInstruments->TranslateTransform((REAL)center.X, (REAL)center.Y);	//平移变换,将坐标原点平移至客户区中心
	m_pGraphicInstruments->RotateTransform((REAL)-fRollAngle);					//旋转变换,将横滚角作为参数,实现俯仰刻度带和横滚游标绕坐标原点旋转

	//================================ 绘制背景图 ====================================
	m_pBrush->SetColor(Color::Blue);
	m_pGraphicInstruments->FillRectangle(m_pBrush,
		int(-0.5 * iWidth * sqrt(2.0)), iStart + int(-3 * iHeight * sqrt(2.0)),
		int(4 * iWidth * sqrt(2.0)), int(3 * iWidth * sqrt(2.0)));

	m_pBrush->SetColor(Color::Brown);
	m_pGraphicInstruments->FillRectangle(m_pBrush,
		int(-0.5 * iWidth * sqrt(2.0)), iStart,
		int(4 * iWidth * sqrt(2.0)), int(3 * iWidth * sqrt(2.0)));

	//================================= 绘制俯仰角 =====================================
	//字体的设置
	m_pPen->SetWidth(2);
	m_pBrush->SetColor(Color::Black);
	FontSizeSet(10);
	FontNameSet(TEXT("宋体"));
	CreateMyFont();
	PointF pointF(30, 0);

	for (i = -(PitchTicksCount / 2) + int(fPitchAngle / 5); i <= 0.5 * PitchTicksCount + int(fPitchAngle / 5); i++)
	{
		//绘制坐标提示数值
		pointF.Y = -(REAL)PitchScaleHeight * i + iStart;
		m_pGraphicInstruments->DrawString(Str(i * 5), -1, m_pFont, pointF, m_pBrush);
		//感觉绘制直线不是问题，最重要的是标注
		if (i % 2 == 0)
			m_pGraphicInstruments->DrawLine(m_pPen,
				-25, -i * PitchScaleHeight + iStart,
				25, -i * PitchScaleHeight + iStart);
		else
			m_pGraphicInstruments->DrawLine(m_pPen,
				-15, -i * PitchScaleHeight + iStart,
				15, -i * PitchScaleHeight + iStart);
	}

	//=============================== 绘制指示箭头 =======================================
	Point pArrowShow[] = { Point(-5, -125), Point(5, -125), Point(0, -145) };
	m_pBrush->SetColor(Color::Yellow);
	m_pGraphicInstruments->FillClosedCurve(m_pBrush, pArrowShow, 3);
	m_pGraphicInstruments->DrawClosedCurve(m_pPen, pArrowShow, 3);

	//============================= 然后绘制对应的滚转角的标尺 ===============================
	double dAngleDu = -50;
	double dAngelReal = dAngleDu * 3.14 / 180;;
	int iRadiusIn(140), iRadiusOut(160);
	m_pGraphicInstruments->ResetTransform(); //重置坐标面
	m_pGraphicInstruments->TranslateTransform((REAL)center.X, (REAL)center.Y);	//平移变换,将坐标原点平移至客户区中心
	//buffer.RotateTransform((REAL)-2*dPitchAngle);
	m_pBrush->SetColor(Color::White);
	for (i = 0; i < 11; i++)
	{
		m_pGraphicInstruments->DrawLine(m_pPen,
			int(iRadiusIn * sin(dAngelReal)), int(-iRadiusIn * cos(dAngelReal)),
			int(iRadiusOut * sin(dAngelReal)), int(-iRadiusOut * cos(dAngelReal)));
		pointF.X = (REAL)(iRadiusIn * sin(dAngelReal) - 10);
		pointF.Y = (REAL)(-iRadiusIn * cos(dAngelReal) + 6);
		m_pGraphicInstruments->DrawString(StrAppend(Str(dAngleDu), TEXT("°")), -1, m_pFont, pointF, m_pBrush);
		dAngleDu += 10;
		dAngelReal = dAngleDu * 3.14 / 180;
	}
	//90°的也画一下
	m_pGraphicInstruments->DrawLine(m_pPen,
		int(iRadiusIn), 0,
		int(iRadiusOut), 0);
	pointF.X = (REAL)(iRadiusIn - 5);
	pointF.Y = (REAL)(6);
	m_pGraphicInstruments->DrawString(TEXT("90°"), -1, m_pFont, pointF, m_pBrush);
	m_pGraphicInstruments->DrawLine(m_pPen,
		-int(iRadiusIn), 0,
		-int(iRadiusOut), 0);
	pointF.X = (REAL)(-iRadiusIn - 15);
	pointF.Y = (REAL)(6);
	m_pGraphicInstruments->DrawString(TEXT("-90°"), -1, m_pFont, pointF, m_pBrush);

	//================================ 绘制中心的指示标志 =====================================
	//buffer.ResetTransform();					//重置坐标面
	m_pPen->SetWidth(5);
	m_pGraphicInstruments->DrawLine(m_pPen, -70, 0, -20, 0);
	m_pGraphicInstruments->DrawLine(m_pPen, -20, -2, -20, 10);
	m_pGraphicInstruments->DrawLine(m_pPen, 20, 0, 70, 0);
	m_pGraphicInstruments->DrawLine(m_pPen, 20, -2, 20, 10);
	m_pGraphicInstruments->DrawEllipse(m_pPen, 0, 0, 2, 2);

	//============================== 左侧速度标识和右侧的高度标识 ============================


	//调用 ADI绘制函数,并将客户区中心坐标和图形对象作为参数
	m_pGraphicInstruments->SetSmoothingMode(SmoothingModeHighQuality);
	//设置图形和文本反走样模式
	m_pGraphicInstruments->SetTextRenderingHint(TextRenderingHintAntiAlias);

	return m_pBmpInstruments;

}

void CBWindGdiPlus::InitialGdiPlus()
{
	if (!m_gdiplusToken)
	{
		GdiplusStartupInput m_gdiplusStartupInput;
		//初始化gdi+
		GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	}
}

void CBWindGdiPlus::UnInitialGdiplus()
{
	//释放gdi+
	GdiplusShutdown(m_gdiplusToken);
}

void CBWindGdiPlus::CreateMyFont()
{
	// 	//如果字体对象为真，判断对应的字体对象的名称是否和现有的一致，不一致重新创建
	// 	if (m_pFontName)
	// 	{
	// 		TCHAR sFontName[64] = { 0 };
	// 		m_pFontName->GetFamilyName(sFontName);
	// 		//如果二者不同，重新创建 FontFamily
	// 		if (_tcscmp(sFontName, m_sFontName.c_str()))
	// 		{
	// 			delete m_pFontName;
	// 			m_pFontName = NULL;
	// 			m_pFontName = new FontFamily(m_sFontName.c_str());
	// 		}
	// 	}
	// 	else
	// 		m_pFontName = new FontFamily(m_sFontName.c_str());
	// 
	// 	//删除线的设置，需要时候再添加吧
	// 	// 	FontStyleStrikeout = 8
	// 	if (m_pFont)
	// 	{
	// 		//此处如果字体的风格和字体的大小不符合，删除原来的然后重新创建
	// 		if (m_pFont->GetSize() != m_iFontSize
	// 			|| m_pFont->GetStyle() != m_iFontStyle)
	// 		{
	// 			delete m_pFont;
	// 			m_pFont = NULL;
	// 			m_pFont = new Font(m_pFontName, (REAL)m_iFontSize, m_iFontStyle);
	// 		}
	// 	}
	// 	else
	// 		m_pFont = new Font(m_pFontName, m_iFontSize, m_iFontStyle);

		//如果创建过并且参数没改变的话，直接返回
	if (!m_fFontChange)	return;

	//先清空原来的内容，然后创建新的内容
	if (m_pFontName)
		delete m_pFontName;
	if (m_pFont)
		delete m_pFont;
	//创建新的字体对象
	m_pFontName = new FontFamily(m_sFontName.c_str());
	m_pFont = new Font(m_pFontName, (REAL)m_iFontSize, m_iFontStyle);

	m_fFontChange = false;
}

ULONG_PTR CBWindGdiPlus::m_gdiplusToken = NULL;




