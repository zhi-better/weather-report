//////////////////////////////////////////////////////////////////////////
// BTimer.cpp: CBTimer 类的实现
//
//
// Programed by: Zhizhi
// Last Modified: 2020-5-14
//////////////////////////////////////////////////////////////////////////

#include "BTimer.h"

//定义类的成员函数

UINT CBTimer::ms_uIDIncrement=0;

bool CBTimer::Ennabled()
{
	return m_bEnabled;
}

UINT CBTimer::Interval()
{
	return m_uElapse;
}

void CBTimer::IntervalSet( UINT uInterval )
{
	//如果时间间隔不变，直接返回
	if(m_uElapse==uInterval)	return;

	// 设置新的时间间隔
	m_uElapse=uInterval;

	//使新的时间间隔生效
	if (m_bEnabled)
	{
		EnnabledSet(false);		//先删除定时器，
		EnnabledSet(true);		//然后重新启用定时器
	}
	//else	//如果定时器没有启用，那么保持原有的状态，等待主程序调用
	//	EnnabledSet 启动它时候，就用新的时间间隔启动它

}

TIMERPROC CBTimer::pFuncCallBack()
{
	return m_pFuncCallBack;
}

void CBTimer::pFuncCallBackSet( TIMERPROC ptrFuncCallBack )
{
	m_pFuncCallBack=ptrFuncCallBack;
}

HWND CBTimer::hWndAsso()
{
	return m_hWndAsso;
}

void CBTimer::hWndAssoSet(HWND hWnd)
{
	//如果关联窗口相同，直接返回
	if(m_hWndAsso==hWnd) return;

	//如果关联窗口已经改变，先删除定时器
	bool blLastEnnabled = m_bEnabled;	//先记录原来的定时器状态
	if(m_bEnabled) EnnabledSet(false);	//如果原来定时器为启用状态，必须在改变 m_hWndAsso 之前先禁用它
										//因为禁用的时候需要原来的句柄，提前改变会导致之前的句柄丢失

	//设置新的关联窗口
	m_hWndAsso=hWnd;

	//使新的关联窗口生效
	if (blLastEnnabled)		//如果之前启动，这里重新启动
		EnnabledSet(true);
	// else //如果没有启动，保持原来的未启用状态
	// EnnabledSet 启用的时候，就会使用新的窗口句柄

}

CBTimer::CBTimer( HWND hWndAsso/*=NULL*/, UINT uElapse/*=1000*/, TIMERPROC ptrFunCallBack/*=NULL*/ )
{
	//清零成员变量
	ms_uIDIncrement++;
	m_uTimerID = ms_uIDIncrement;		//每次定义一个本类的对象，m_uTimerID增加 1 
	m_hWndAsso = NULL;
	m_uElapse = 0;
	m_pFuncCallBack = NULL;
	m_bEnabled = false;

	//通过调用成员函数，使得参数所给的信息有效
	// 由于现在 m_bEnabled 为 false，所以以下调用的成员函数
	// 都不会启动定时器，而只会设置本对象成员的值
	hWndAssoSet(hWndAsso);	//此函数可能会改变 m_uTimerID 
	IntervalSet(uElapse);
	pFuncCallBackSet(ptrFunCallBack);

	//调用成员函数 EnnabledSet(true);启动定时器
	//	如果 m_pFuncCallBack 为 0 ，或者 m_uElapse 为0，则定时器不会启动
	EnnabledSet(true);
	
}

CBTimer::~CBTimer()
{
	// 当析构对象时候，调用函数 EnnabledSet(false);删除定时器
	EnnabledSet(false);

}

void CBTimer::EnnabledSet( bool value )
{
	//如果定时器状态和 value 一致，直接返回，不重复执行禁用/启用状态
	//此处的 value 是故意定义成这个蓝颜色的变量的吗？？？？？？？？？？？？
	if (value==m_bEnabled)	return;

	if(value)
	{
		if (m_pFuncCallBack!=NULL && m_uElapse!=0)
		{
			//==============启动定时器====================
			// 调用系统 API 函数 SetTimer 函数创建定时器
			// 如果此时 m_hWndAsso 不为零，定时器 ID 将使用 m_uTimerID 
			// SetTimer 设置成功的时候返回的也是定时器的 ID 
			// 如果此时 m_hWndAsso 为0，那么 SetTimer 将会忽略 m_uTimerID 现有的值
			// SetTimer 调用成功的时候返回 API 分配的一个定时器 ID
			UINT ret=SetTimer(m_hWndAsso, m_uTimerID, m_uElapse, m_pFuncCallBack);

			// 如果 SetTimer 函数调用成功（那么 SetTimer 的返回值非0），就保存已经创建成功的定时器ID
			// 如果 SetTimer 函数调用失败（那么 SetTimer 的返回值为0），保持m_uElapse 不变
			// （注意此处不能将 m_uTimerID 改为0，因为如果后续重建这个定时器还需要用到这个 ID）
			// 此处如果创建成功不是返回的还是原来的 ID 吗，为什么要重新赋值呀？？
			if (ret)	m_uTimerID = ret;

			//更新成员变量的值
			m_bEnabled=(bool)(ret != 0);	//此处 ret 值非零，证明创建成功，刚好转换成 true, 否则自动转换为 false

		}// end if( m_pFuncCallBack!=NULL && m_uElapse==0 )
	}
	else	//if(value)	else
	{
		//================禁用定时器===================
		KillTimer(m_hWndAsso, m_uTimerID);

		if (NULL==m_hWndAsso)
		{
			//这里不是很懂，为什么系统分配的保存就没有意义了呢
			ms_uIDIncrement++;
			m_uTimerID=ms_uIDIncrement;
		}

		m_bEnabled = false;	//定时器被禁用，更新标志变量
	}

}

UINT CBTimer::Identifier()
{
	//定时器启动时候返回定时器ID,否则返回的是0
	if (m_bEnabled)
		return m_uTimerID;
	else
		return 0;
}






