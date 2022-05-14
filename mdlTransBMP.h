#include <Windows.h>
#include <math.h>

// TBDrawTransParentBitmap 绘制透明位图
// 把 hDCSrc 中的一幅“白背景红花图”的白色背景变透明后，贴到 hDCDest 中为例
// hDCDest 贴图将要放置的内存环境的句柄
// destX 贴图要放置的 x 坐标
// destY 贴图要放置的 y 坐标
// hDCSrc 贴图的内存环境的句柄
// colorTransParent 要进行透明处理的贴图的颜色
// srcX 从资源图像的哪个 X 部分进行截取
// srcY 从资源图像的哪个 Y 部分进行截取
// srcWidth 要透明处理的位图的宽度
// srcHeight 要透明的位图的高度
void TBDrawTransParentBitmap( HDC hDCDest,
	int destX,
	int destY,
	HDC hDCSrc,
	COLORREF colorTransParent = 0,
	int srcX = 0,
	int srcY = 0,
	int srcWidth = -1,
	int srcHeight = -1
	);

// 旋转位图指定的角度
// hDCDest 表示贴图将要放置的内存环境的句柄
// destX 贴图要放置的 x 坐标
// destY 贴图要放置的 y 坐标
// hDCSrc 表示要旋转的位图所在的设备环境的句柄
// angle 表示要旋转的角度，默认是 角度制 °
// cColor 指的是如果旋转，中间的空缺用什么颜色来填充
// SrcWidth，SrcHeight 要旋转的源位图的宽度和高度
// 为了更加方便使用，此处决定对其进行一定的封装，让它旋转是以圆的方式旋转，摒弃掉方形旋转会导致区域变大的问题
void TBRotateAnyAngle(HDC hDCDest, int DestX, int DestY, HDC hDCSrc, double angle=0, COLORREF cColor=RGB(255,255,255), int SrcWidth=-1, int SrcHeight=-1);
