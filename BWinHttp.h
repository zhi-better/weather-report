//////////////////////////////////////////////////////////////////////////
//BWinnet �������
//ʱ�䣺2020/12/30
//�����ߣ�����
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "BForm.h"
#include <WinInet.h>
#include <vector>

#pragma comment(lib, "wininet.lib")

// HTTP���󷽷�
enum HTTP_REQ_METHOD
{
	REQ_METHOD_GET,
	REQ_METHOD_POST,
};

class CBWinInet
{
public:
	//////////////////////////////////////////////////////////////////////////
	//���к���
	//////////////////////////////////////////////////////////////////////////

	//���캯��
	CBWinInet();

	//��������
	~CBWinInet();

	//=================================== ���Ͳ��� ==========================================

	//��һ�� url ���˴���򵥵ķ�װ������������ֱ�ӽ��� url ���Զ���������
	//sUrl ��ʾҪ���ӵ� url ��ʲô
	//nReqMethod ��ʾhttp����ķ���
	bool HttpOpen(LPCTSTR lpszUrl, HTTP_REQ_METHOD nReqMethod = REQ_METHOD_GET);

	//���� http ͷ
	BOOL AddHeaders(LPCTSTR lpHeaders = NULL);

	//���� Http ����
	BOOL SendRequest(const TCHAR* lpData = NULL, DWORD dwLen = 0);

	//�������� http ������ʱ��̫����
	BOOL EndSendRequest();

	//=========================== ���ղ��� ===================================

	//��÷��ص�״̬��
	DWORD GetRespCode();

	// ��ȡȫ��HTTPͷ
	tstring GetRespHeader();

	// ��ȡHTTPͷָ���ֶ����Ƶ�ֵ
	tstring GetRespHeader(LPCTSTR lpszName, int nIndex = 0);

	// ��ȡHTTPͷָ���ֶ����Ƶ�ֵ(Int)
	int GetRespHeaderByInt(LPCTSTR lpszName, int nIndex = 0);
	tstring GetRespHeaderByStr(LPCTSTR lpszName, int nIndex = 0);

	// ��ȡHTTP��Ӧ��Ϣ�����ݺ���
	BOOL GetRespBodyData(CHAR* lpBuf, DWORD dwLen, DWORD& dwRecvLen);
	tstring GetRespBodyData();

	//============================ �������� ==================================

	//���ó�ʱʱ�䣬�˺�����̫����
	void SetConnectTimeOut(DWORD dwTimeOut);

	//�ر�������
	void CloseRequest();

private:
	// ����Url����(Э�飬���������˿ڣ��ļ�·��)
	BOOL ParseURL(LPCTSTR lpszUrl, LPTSTR lpszScheme, DWORD dwSchemeLength, LPTSTR lpszHostName, DWORD dwHostNameLength, WORD& nPort, LPTSTR lpszUrlPath, DWORD dwUrlPathLength);
	// ��ȡHTTP��Ӧͷ����
	DWORD __GetRespHeaderLen();
	// ��ȡHTTP��Ӧͷ�����б�����m_arrRespHeader����
	BOOL __GetRespHeader();

private:
	//////////////////////////////////////////////////////////////////////////
	//���������
	HINTERNET m_hInternet;                  // �Ự���
	HINTERNET m_hConnect;                   // ���Ӿ��
	HINTERNET m_hRequest;                   // ������

	tstring sHeaders;						// http ����ͷ������
	DWORD m_dwConnectTimeOut;				//��Ӧ��ʱ��ʱ��
	std::vector<tstring> m_arrRespHeader;   // Http ��Ӧͷ����

	//�˴����� string ����Ϊ�����ֽڽ��գ����滹��Ҫת��
	string m_sRespBody;					//���ڱ����Ӧ�ķ��ػ��������ݣ����뷽ʽΪ UTF8
};

