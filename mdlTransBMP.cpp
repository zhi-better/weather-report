#include "mdlTransBMP.h"

// TBDrawTransParentBitmap 绘制透明位图
// 把 hDCSrc 中的一幅“白背景红花图”的白色背景变透明后，贴到 hDCDest 中为例

void TBDrawTransParentBitmap( HDC hDCDest, 
							int destX, 
							int destY, 
							HDC hDCSrc, 
							COLORREF colorTransParent /*= 0*/, 
							int srcX /*= 0*/, 
							int srcY /*= 0*/, 
							int srcWidth /*= -1*/, 
							int srcHeight /*= -1 */ )
{
	// ===================== 确定 srcWidth 和 srcHeight=========================
	// 如果 srcWidth<0 或者 srcHeight<0 ，表示使用 hDCSrc 中位图的实际的宽和高
	// 自动获得 hDCSrc 中位图的实际宽和高
	if (srcHeight<0 || srcWidth<0)
	{
		// 获得当前的被选入的 hDCSrc 中的位图的句柄
		HBITMAP hBmpSrc = (HBITMAP)GetCurrentObject(hDCSrc, OBJ_BITMAP);
		// 获得该位图对象的实际的宽和高
		BITMAP bmpInfo;
		memset(&bmpInfo, 0, sizeof(bmpInfo));
		GetObject(hBmpSrc, sizeof(BITMAP), &bmpInfo);
		if(srcHeight<0)	srcHeight = bmpInfo.bmHeight;
		if(srcWidth<0)	srcWidth = bmpInfo.bmWidth;
	}

	// ==================== 生成掩码位图（黑花白背景） ===============================
	// 建立兼容的，保存掩码位图的内存设备环境 hDCMask
	HDC hDCMask = CreateCompatibleDC(hDCSrc);
	// 在 hDCMask 中安放一个与原图像同样大小的位图（即：画图板）
	HBITMAP hBmpMask = CreateCompatibleBitmap(hDCSrc, srcWidth, srcHeight);
	HBITMAP hBmpMaskOld = (HBITMAP)SelectObject(hDCMask, hBmpMask);

	// ------------------------------------------------------------------------------
	// 建立临时的，兼容的，仅仅用于单色位图的 设备环境 和 单色位图
	// 以下变量为建立掩码位图的临时单色位图和设备环境
	HDC hDCMono = CreateCompatibleDC(hDCSrc);
	// 建立与源图像同样大小的单色位图
	HBITMAP hBmpMono = CreateBitmap(srcWidth, srcHeight, 1, 1, NULL);
	// 将单色位图选入 hBmpMono
	// 该单色位图就是 hBmpMono 设备环境的“画图板”，由于这个画图板仅仅支持
	// 单色，所以以后在 hBmpMono 中绘制的图像都是单色的
	HBITMAP hBmpMonoOld = (HBITMAP)SelectObject(hDCMono, hBmpMono);

	// 将源位图以复制的方式绘制到 hBmpMono，在 hBmpMono 中
	// 获得了源位图的“单色样本”
	// 要透明的部分颜色变为白色，其他部分变成黑色（黑花白背景）
	// 在复制之前，先设置源位图的背景色 colorTransParent，这样的话
	// 源位图复制过去 colorTransParent 的颜色才是白色
	COLORREF colorOlkBk = SetBkColor(hDCSrc, colorTransParent);
	BitBlt(hDCMono, 0, 0, srcWidth, srcHeight, hDCSrc, srcX, srcY, SRCCOPY);

	// ^^^^^^^^^^^^^^^^^^^^ 掩码位图已经拷贝到了 hDCMono 中 ^^^^^^^^^^^^^^^^^^^^^
	// --------------------------------------------------------------------------

	// 由于 hDCMono 中的是单色位图，所以不能直接 BitBlt 到目标环境 hDCDest 中
	// 所以应该将 hDCMono 中的位图复制到 hDCMask 中（hDCMask 支持彩色位图）
	BitBlt(hDCMask, 0, 0, srcWidth, srcHeight, hDCMono, 0, 0, SRCCOPY);
	// ^^^^^^^^^^^^^^^^^^^^ 掩码位图已经拷贝到了 hDCMask 中 ^^^^^^^^^^^^^^^^^^^^^
	// --------------------------------------------------------------------------

	// 删除临时绘制的 hDCMono 和其中的位图
	SelectObject(hDCMono, hBmpMonoOld);
	DeleteObject(hBmpMono);
	DeleteDC(hDCMono);
	// ==========================================================================

	// ==================== 生成透明色为黑色的源图像（红花黑背景）===============
	// 如果源位图中的透明的颜色 colorTransParent 本来就是黑色，那么不必此处多操作
	// 如果不是黑色，需要将它变为黑色，创建一个兼容的内存设备环境 hDCModi 
	// 将其中放置“透明色被改为黑色的”源图像
	HDC hDCModi(NULL);	
	HBITMAP hBmpModi(NULL), hBmpModiOld(NULL);
	if (colorTransParent)	// if(colorTransParent != 0)，不为黑色
	{
		// 建立兼容的设备环境 hDCModi
		hDCModi = CreateCompatibleDC(hDCSrc);
		// 为设备环境 hDCModi 安置“画图板”，和源图像一致的位图 hBmpModi
		hBmpModi = CreateCompatibleBitmap(hDCSrc, srcWidth, srcHeight);
		hBmpModiOld = (HBITMAP)SelectObject(hDCModi, hBmpModi);

		// 将源图像的透明色改为黑色（图像内容仍未彩色），保存到 hDCModi 中
		// 首先将花变成白色，背景变为黑色（即：反转 hDCModi）然后和源图像进行 ADD运算
		// 首先，将 hDCMask 中的“黑花白背景”NOTSRCCOPY 到 hDCModi 变为“黑背景白花”
		BitBlt(hDCModi, 0, 0, srcWidth, srcHeight, hDCMask, 0, 0, NOTSRCCOPY);
		// 将源位图 SRCAND 到“黑背景白花”中，使得 hDCModi 为“黑背景白花”
		// （ SRCAND 后，背景色仍为黑色，其他颜色和白色与之后仍为原来的颜色）
		BitBlt(hDCModi, 0, 0, srcWidth, srcHeight, hDCSrc, srcX, srcY, SRCAND);
	}
	// =========================================================================

	// ====================== 制作透明贴图 ======================================
	// 现在，hDCMask 中保存的是掩码位图，即：黑花白背景
	// hDCModi 中保存的是“黑背景红花”
	// 制作透明贴图，方法是：
	// （1）目标和 hDCMask 做与运算
	// （2）再和 hDCModi 做或运算
	BitBlt(hDCDest, destX, destY, srcWidth, srcHeight, hDCMask, 0, 0, SRCAND);
	if (colorTransParent)
	{
		// 如果要透明的颜色不是黑色，那就用加工后的 hDCModi
		BitBlt(hDCDest, destX, destY, srcWidth, srcHeight, hDCModi, 0, 0, SRCPAINT);
	} 
	else
	{
		// 如果要透明的颜色是黑色，直接用 hDCSrc
		BitBlt(hDCDest, destX, destY, srcWidth, srcHeight, hDCSrc, srcX, srcY, SRCPAINT);
	}
	// ========================================================================

	// ==================== 释放资源 ==========================================
	// 恢复 hDCSrc 的背景色
	SetBkColor(hDCSrc, colorOlkBk);

	//释放资源
	SelectObject(hDCMask, hBmpMaskOld);
	DeleteObject(hBmpMask);
	DeleteDC(hDCMask);

	if (colorTransParent)	// if(colorTransParent != 0)，不为黑色
	{
		SelectObject(hDCModi, hBmpModiOld);
		DeleteObject(hBmpModi);
		DeleteDC(hDCModi);
	}
}

void TBRotateAnyAngle(HDC hDCDest, int DestX, int DestY, HDC hDCSrc, double angle/*=0*/, COLORREF cColor/*=RGB(255,255,255)*/, int SrcWidth/*=-1*/,int SrcHeight/*=-1*/)
{
	//盗取他人代码，但是好用就好了，管他呢

	//======================= 自动获得高度宽度 ===============================
	//如果用户没有输入源图像的宽度和高度，自动获得
	if (SrcWidth<0 || SrcHeight<0)
	{
		// 获得当前的被选入的 hDCSrc 中的位图的句柄
		HBITMAP hBmpSrc = (HBITMAP)GetCurrentObject(hDCSrc, OBJ_BITMAP);
		// 获得该位图对象的实际的宽和高
		BITMAP bmpInfo;
		memset(&bmpInfo, 0, sizeof(bmpInfo));
		GetObject(hBmpSrc, sizeof(BITMAP), &bmpInfo);
		if(SrcHeight<0)	SrcHeight = bmpInfo.bmHeight;
		if(SrcWidth<0)	SrcWidth = bmpInfo.bmWidth;
	}

	//==================== 计算最终图像的宽度和高度 ============================
	//一大堆计算，算了，不想看了
	double x1,x2,x3;
	double y1,y2,y3;
	double maxWidth, maxHeight, minWidth, minHeight;
	double srcX,srcY;
	double sinA,cosA;
	double DstWidth;
	double DstHeight;
	HDC dcDst;			//旋转后的内存设备环境
	HBITMAP newBitmap;
	angle = angle/180.0*3.1415926;	//首先将弧度制转换为角度制才行，便于使用
	sinA = sin(angle);
	cosA = cos(angle);
	x1 = -SrcHeight * sinA;
	y1 = SrcHeight * cosA;
	x2 = SrcWidth * cosA - SrcHeight * sinA;
	y2 = SrcHeight * cosA + SrcWidth * sinA;
	x3 = SrcWidth * cosA;
	y3 = SrcWidth * sinA;
	minWidth = x3>(x1>x2?x2:x1)?(x1>x2?x2:x1):x3;
	minWidth = minWidth>0?0:minWidth;
	minHeight = y3>(y1>y2?y2:y1)?(y1>y2?y2:y1):y3;
	minHeight = minHeight>0?0:minHeight;
	maxWidth = x3>(x1>x2?x1:x2)?x3:(x1>x2?x1:x2);
	maxWidth = maxWidth>0?maxWidth:0;
	maxHeight = y3>(y1>y2?y1:y2)?y3:(y1>y2?y1:y2);
	maxHeight = maxHeight>0?maxHeight:0;
	DstWidth = maxWidth - minWidth;
	DstHeight = maxHeight - minHeight;

	//====================== 好戏开始，用来控制旋转位图的核心代码 ============================
	//创建内存环境
	dcDst = CreateCompatibleDC(hDCSrc);
	newBitmap = CreateCompatibleBitmap(hDCSrc,(int)DstWidth,(int)DstHeight);
	SelectObject(dcDst,newBitmap);
	//选入画笔，然后便于绘制背景色
	HBRUSH hBrush = NULL;	HBRUSH hBrushOld = NULL;
	hBrush = CreateSolidBrush(cColor);
	hBrushOld = (HBRUSH)SelectObject(dcDst, hBrush);
	//填充背景色，用于填补空白，否则这个将会中间有空缺
	RECT rc;
	rc.left=0;		rc.top=0;
	rc.right=(int)DstWidth;	rc.bottom=(int)DstHeight;
	FillRect(dcDst, &rc, hBrush);
	for( int I = 0 ;I<DstHeight;I++)
	{
		for(int J = 0 ;J< DstWidth;J++)
		{
			srcX = (J + minWidth) * cosA + (I + minHeight) * sinA;
			srcY = (I + minHeight) * cosA - (J + minWidth) * sinA;
			if( (srcX >= 0) && (srcX <= SrcWidth) &&(srcY >= 0) && (srcY <= SrcHeight))
			{
				BitBlt(dcDst, J, I, 1, 1, hDCSrc,(int)srcX, (int)srcY, SRCCOPY);
			}
		}
	}
	// =================================== 显示旋转后的位图 ===============================
	//不减 1。就会有左侧的黑线，可能是中间哪里出问题了
	//这里最经典的内容就是这个算法是计算后将旋转后的内容放大放到一个大的矩形里面，
	//但是此处就可以通过拷贝的起始地点来控制最终结果的内容，我真的是太机智了
	BitBlt(hDCDest,DestX,DestY,(int)SrcWidth,(int)SrcHeight,dcDst,
		int((DstWidth-SrcWidth)/2.0),int((DstHeight-SrcHeight)/2.0),
		SRCCOPY);
	

	//==================================== 释放资源 ====================================
	SelectObject(dcDst, hBrushOld);
	DeleteObject(hBrush);
	DeleteObject(newBitmap);
	DeleteDC(dcDst);
}