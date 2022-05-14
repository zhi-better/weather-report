//////////////////////////////////////////////////////////////////////////
// BTimer.cpp: CBTimer ���ʵ��
//
//
// Programed by: Zhizhi
// Last Modified: 2020-5-14
//////////////////////////////////////////////////////////////////////////

#include "BTimer.h"

//������ĳ�Ա����

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
	//���ʱ�������䣬ֱ�ӷ���
	if(m_uElapse==uInterval)	return;

	// �����µ�ʱ����
	m_uElapse=uInterval;

	//ʹ�µ�ʱ������Ч
	if (m_bEnabled)
	{
		EnnabledSet(false);		//��ɾ����ʱ����
		EnnabledSet(true);		//Ȼ���������ö�ʱ��
	}
	//else	//�����ʱ��û�����ã���ô����ԭ�е�״̬���ȴ����������
	//	EnnabledSet ������ʱ�򣬾����µ�ʱ����������

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
	//�������������ͬ��ֱ�ӷ���
	if(m_hWndAsso==hWnd) return;

	//������������Ѿ��ı䣬��ɾ����ʱ��
	bool blLastEnnabled = m_bEnabled;	//�ȼ�¼ԭ���Ķ�ʱ��״̬
	if(m_bEnabled) EnnabledSet(false);	//���ԭ����ʱ��Ϊ����״̬�������ڸı� m_hWndAsso ֮ǰ�Ƚ�����
										//��Ϊ���õ�ʱ����Ҫԭ���ľ������ǰ�ı�ᵼ��֮ǰ�ľ����ʧ

	//�����µĹ�������
	m_hWndAsso=hWnd;

	//ʹ�µĹ���������Ч
	if (blLastEnnabled)		//���֮ǰ������������������
		EnnabledSet(true);
	// else //���û������������ԭ����δ����״̬
	// EnnabledSet ���õ�ʱ�򣬾ͻ�ʹ���µĴ��ھ��

}

CBTimer::CBTimer( HWND hWndAsso/*=NULL*/, UINT uElapse/*=1000*/, TIMERPROC ptrFunCallBack/*=NULL*/ )
{
	//�����Ա����
	ms_uIDIncrement++;
	m_uTimerID = ms_uIDIncrement;		//ÿ�ζ���һ������Ķ���m_uTimerID���� 1 
	m_hWndAsso = NULL;
	m_uElapse = 0;
	m_pFuncCallBack = NULL;
	m_bEnabled = false;

	//ͨ�����ó�Ա������ʹ�ò�����������Ϣ��Ч
	// �������� m_bEnabled Ϊ false���������µ��õĳ�Ա����
	// ������������ʱ������ֻ�����ñ������Ա��ֵ
	hWndAssoSet(hWndAsso);	//�˺������ܻ�ı� m_uTimerID 
	IntervalSet(uElapse);
	pFuncCallBackSet(ptrFunCallBack);

	//���ó�Ա���� EnnabledSet(true);������ʱ��
	//	��� m_pFuncCallBack Ϊ 0 ������ m_uElapse Ϊ0����ʱ����������
	EnnabledSet(true);
	
}

CBTimer::~CBTimer()
{
	// ����������ʱ�򣬵��ú��� EnnabledSet(false);ɾ����ʱ��
	EnnabledSet(false);

}

void CBTimer::EnnabledSet( bool value )
{
	//�����ʱ��״̬�� value һ�£�ֱ�ӷ��أ����ظ�ִ�н���/����״̬
	//�˴��� value �ǹ��ⶨ����������ɫ�ı������𣿣���������������������
	if (value==m_bEnabled)	return;

	if(value)
	{
		if (m_pFuncCallBack!=NULL && m_uElapse!=0)
		{
			//==============������ʱ��====================
			// ����ϵͳ API ���� SetTimer ����������ʱ��
			// �����ʱ m_hWndAsso ��Ϊ�㣬��ʱ�� ID ��ʹ�� m_uTimerID 
			// SetTimer ���óɹ���ʱ�򷵻ص�Ҳ�Ƕ�ʱ���� ID 
			// �����ʱ m_hWndAsso Ϊ0����ô SetTimer ������� m_uTimerID ���е�ֵ
			// SetTimer ���óɹ���ʱ�򷵻� API �����һ����ʱ�� ID
			UINT ret=SetTimer(m_hWndAsso, m_uTimerID, m_uElapse, m_pFuncCallBack);

			// ��� SetTimer �������óɹ�����ô SetTimer �ķ���ֵ��0�����ͱ����Ѿ������ɹ��Ķ�ʱ��ID
			// ��� SetTimer ��������ʧ�ܣ���ô SetTimer �ķ���ֵΪ0��������m_uElapse ����
			// ��ע��˴����ܽ� m_uTimerID ��Ϊ0����Ϊ��������ؽ������ʱ������Ҫ�õ���� ID��
			// �˴���������ɹ����Ƿ��صĻ���ԭ���� ID ��ΪʲôҪ���¸�ֵѽ����
			if (ret)	m_uTimerID = ret;

			//���³�Ա������ֵ
			m_bEnabled=(bool)(ret != 0);	//�˴� ret ֵ���㣬֤�������ɹ����պ�ת���� true, �����Զ�ת��Ϊ false

		}// end if( m_pFuncCallBack!=NULL && m_uElapse==0 )
	}
	else	//if(value)	else
	{
		//================���ö�ʱ��===================
		KillTimer(m_hWndAsso, m_uTimerID);

		if (NULL==m_hWndAsso)
		{
			//���ﲻ�Ǻܶ���Ϊʲôϵͳ����ı����û����������
			ms_uIDIncrement++;
			m_uTimerID=ms_uIDIncrement;
		}

		m_bEnabled = false;	//��ʱ�������ã����±�־����
	}

}

UINT CBTimer::Identifier()
{
	//��ʱ������ʱ�򷵻ض�ʱ��ID,���򷵻ص���0
	if (m_bEnabled)
		return m_uTimerID;
	else
		return 0;
}






