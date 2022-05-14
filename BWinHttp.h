//////////////////////////////////////////////////////////////////////////
//BWinnet 类的声明
//时间：2020/12/30
//制作者：治治
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "BForm.h"
#include <WinInet.h>
#include <vector>

#pragma comment(lib, "wininet.lib")

// HTTP请求方法
enum HTTP_REQ_METHOD
{
	REQ_METHOD_GET,
	REQ_METHOD_POST,
};

class CBWinInet
{
public:
	//////////////////////////////////////////////////////////////////////////
	//公有函数
	//////////////////////////////////////////////////////////////////////////

	//构造函数
	CBWinInet();

	//析构函数
	~CBWinInet();

	//=================================== 发送部分 ==========================================

	//打开一个 url ，此处最简单的封装了三个函数，直接解析 url ，自动发送内容
	//sUrl 表示要连接的 url 是什么
	//nReqMethod 表示http请求的方法
	bool HttpOpen(LPCTSTR lpszUrl, HTTP_REQ_METHOD nReqMethod = REQ_METHOD_GET);

	//增加 http 头
	BOOL AddHeaders(LPCTSTR lpHeaders = NULL);

	//发送 Http 请求
	BOOL SendRequest(const TCHAR* lpData = NULL, DWORD dwLen = 0);

	//结束发送 http 请求，暂时不太会用
	BOOL EndSendRequest();

	//=========================== 接收部分 ===================================

	//获得返回的状态码
	DWORD GetRespCode();

	// 获取全部HTTP头
	tstring GetRespHeader();

	// 获取HTTP头指定字段名称的值
	tstring GetRespHeader(LPCTSTR lpszName, int nIndex = 0);

	// 获取HTTP头指定字段名称的值(Int)
	int GetRespHeaderByInt(LPCTSTR lpszName, int nIndex = 0);
	tstring GetRespHeaderByStr(LPCTSTR lpszName, int nIndex = 0);

	// 获取HTTP响应消息体数据函数
	BOOL GetRespBodyData(CHAR* lpBuf, DWORD dwLen, DWORD& dwRecvLen);
	tstring GetRespBodyData();

	//============================ 其他函数 ==================================

	//设置超时时间，此函数不太会用
	void SetConnectTimeOut(DWORD dwTimeOut);

	//关闭请求函数
	void CloseRequest();

private:
	// 解析Url函数(协议，主机名，端口，文件路径)
	BOOL ParseURL(LPCTSTR lpszUrl, LPTSTR lpszScheme, DWORD dwSchemeLength, LPTSTR lpszHostName, DWORD dwHostNameLength, WORD& nPort, LPTSTR lpszUrlPath, DWORD dwUrlPathLength);
	// 获取HTTP响应头长度
	DWORD __GetRespHeaderLen();
	// 获取HTTP响应头，按行保存在m_arrRespHeader数组
	BOOL __GetRespHeader();

private:
	//////////////////////////////////////////////////////////////////////////
	//类变量定义
	HINTERNET m_hInternet;                  // 会话句柄
	HINTERNET m_hConnect;                   // 连接句柄
	HINTERNET m_hRequest;                   // 请求句柄

	tstring sHeaders;						// http 请求头的内容
	DWORD m_dwConnectTimeOut;				//响应超时的时间
	std::vector<tstring> m_arrRespHeader;   // Http 响应头数组

	//此处采用 string 是因为单个字节接收，后面还需要转换
	string m_sRespBody;					//用于保存对应的返回回来的内容，编码方式为 UTF8
};

