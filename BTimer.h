//////////////////////////////////////////////////////////////////////////
// BTimer.h: CBTimer 类的定义
// 实现定时器
//
// Programmed by Zhizhi
// Last Modified: 2019-5-7
//////////////////////////////////////////////////////////////////////////

#include <Windows.h>

class CBTimer
{
public:
	// =======================================================================
	// 类公有成员和成员函数
	//==================================================================================

	// 构造函数
	// pFuncCallBack 为定时器触发的回调函数的地址
	//	该函数原型为
	//	void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
	// uElapse 为定时器的触发时间间隔（单位：毫秒）
	//	hWndAsso 为关联的窗口句柄，此项为 NULL 则表示不与窗口关联
	//	以上参数都即可在此构造函数中给出，也可以通过成员函数来设置
	//	但是必须已设置 pFuncCallBack 和 uElapse 后才能启动定时器
	//	如果在构造函数中指定了 pFuncCallBack 和 uElapse 都不为0，那么定时器立即被启动
	//	否则必须通过成员函数设置属性，并以后用 EnnabledSet 来启动它
	CBTimer(HWND hWndAsso=NULL, UINT uElapse=1000, TIMERPROC ptrFunCallBack=NULL);

	//析构函数
	~CBTimer();
	
	//返回定时器的 ID ，仅仅在启用的时候有效，禁用时为 0 
	UINT Identifier();

	// 返回或者设置定时器的启用/禁用状态
	// 若尚未设置好时间间隔和回调函数地址，则无法启动
	// 可通过构造函数或者 IntervalSet 和 pFuncCallBackSet 成员函数设置
	bool Ennabled();
	void EnnabledSet(bool value);

	//返回或者设置定时器的触发间隔时间（单位：毫秒）
	UINT Interval();
	void IntervalSet(UINT uInterval);

	// 返回或者设定定时器触发时候所要调用的回调函数的地址
	// 函数原型为
	// void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
	TIMERPROC pFuncCallBack();
	void pFuncCallBackSet(TIMERPROC ptrFuncCallBack);

	//返回或设置关联窗口句柄， 为0表示定时器未关联窗口
	HWND hWndAsso();
	void hWndAssoSet(HWND hWnd);

protected:

private:
	// ===================================================================
	// 类的私有成员和成员函数
	// ===================================================================

	//====================================================================
	// 私有静态成员和成员函数
	// 静态成员是指所有该类的对象共用的“全局”数据成员函数
	// （但是这种“全局变量”其他类的对象不能使用）

	static UINT ms_uIDIncrement;	//获得定时器 ID 的增量变量

	UINT m_uTimerID;			//定时器标识（未关联窗口时为API分配的ID，否则为本类分配的ID）
	HWND m_hWndAsso;			//所关联的窗口句柄（该项为0可标识定时器未关联窗口）
	UINT m_uElapse;				//定时器触发间隔时间（单位：毫秒）	/*UINT=unsigned int 无符号整数*/
								//启动时，将每隔 m_uElapse 毫秒就会调用回调函数一次
	TIMERPROC m_pFuncCallBack;	//当定时器触发时候的回调函数的地址
	bool m_bEnabled;			//当前定时器的启动/禁用状态
	

};