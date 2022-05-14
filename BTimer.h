//////////////////////////////////////////////////////////////////////////
// BTimer.h: CBTimer ��Ķ���
// ʵ�ֶ�ʱ��
//
// Programmed by Zhizhi
// Last Modified: 2019-5-7
//////////////////////////////////////////////////////////////////////////

#include <Windows.h>

class CBTimer
{
public:
	// =======================================================================
	// �๫�г�Ա�ͳ�Ա����
	//==================================================================================

	// ���캯��
	// pFuncCallBack Ϊ��ʱ�������Ļص������ĵ�ַ
	//	�ú���ԭ��Ϊ
	//	void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
	// uElapse Ϊ��ʱ���Ĵ���ʱ��������λ�����룩
	//	hWndAsso Ϊ�����Ĵ��ھ��������Ϊ NULL ���ʾ���봰�ڹ���
	//	���ϲ����������ڴ˹��캯���и�����Ҳ����ͨ����Ա����������
	//	���Ǳ��������� pFuncCallBack �� uElapse �����������ʱ��
	//	����ڹ��캯����ָ���� pFuncCallBack �� uElapse ����Ϊ0����ô��ʱ������������
	//	�������ͨ����Ա�����������ԣ����Ժ��� EnnabledSet ��������
	CBTimer(HWND hWndAsso=NULL, UINT uElapse=1000, TIMERPROC ptrFunCallBack=NULL);

	//��������
	~CBTimer();
	
	//���ض�ʱ���� ID �����������õ�ʱ����Ч������ʱΪ 0 
	UINT Identifier();

	// ���ػ������ö�ʱ��������/����״̬
	// ����δ���ú�ʱ�����ͻص�������ַ�����޷�����
	// ��ͨ�����캯������ IntervalSet �� pFuncCallBackSet ��Ա��������
	bool Ennabled();
	void EnnabledSet(bool value);

	//���ػ������ö�ʱ���Ĵ������ʱ�䣨��λ�����룩
	UINT Interval();
	void IntervalSet(UINT uInterval);

	// ���ػ����趨��ʱ������ʱ����Ҫ���õĻص������ĵ�ַ
	// ����ԭ��Ϊ
	// void CALLBACK Timer1_Timer(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
	TIMERPROC pFuncCallBack();
	void pFuncCallBackSet(TIMERPROC ptrFuncCallBack);

	//���ػ����ù������ھ���� Ϊ0��ʾ��ʱ��δ��������
	HWND hWndAsso();
	void hWndAssoSet(HWND hWnd);

protected:

private:
	// ===================================================================
	// ���˽�г�Ա�ͳ�Ա����
	// ===================================================================

	//====================================================================
	// ˽�о�̬��Ա�ͳ�Ա����
	// ��̬��Ա��ָ���и���Ķ����õġ�ȫ�֡����ݳ�Ա����
	// ���������֡�ȫ�ֱ�����������Ķ�����ʹ�ã�

	static UINT ms_uIDIncrement;	//��ö�ʱ�� ID ����������

	UINT m_uTimerID;			//��ʱ����ʶ��δ��������ʱΪAPI�����ID������Ϊ��������ID��
	HWND m_hWndAsso;			//�������Ĵ��ھ��������Ϊ0�ɱ�ʶ��ʱ��δ�������ڣ�
	UINT m_uElapse;				//��ʱ���������ʱ�䣨��λ�����룩	/*UINT=unsigned int �޷�������*/
								//����ʱ����ÿ�� m_uElapse ����ͻ���ûص�����һ��
	TIMERPROC m_pFuncCallBack;	//����ʱ������ʱ��Ļص������ĵ�ַ
	bool m_bEnabled;			//��ǰ��ʱ��������/����״̬
	

};