#include "BWinHttp.h"



CBWinInet::CBWinInet()
{
	//其他变量初始化
	m_hInternet = NULL;
	m_hConnect = NULL;
	m_hRequest = NULL;

	//初始化请求头
	sHeaders = L"";
	sHeaders += TEXT("Connection: keep-alive");
	sHeaders += TEXT("Cache-Control: max-age=0");
	sHeaders += TEXT("Upgrade-Insecure-Requests: 1");
	sHeaders += TEXT("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36";);
	sHeaders += TEXT("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
	sHeaders += TEXT("Accept-Encoding: gzip, deflate");
	sHeaders += TEXT("Accept-Language: zh-CN,zh;q=0.9\r\n");
}

CBWinInet::~CBWinInet()
{
	CloseRequest();
}

bool CBWinInet::HttpOpen(LPCTSTR lpszUrl, HTTP_REQ_METHOD nReqMethod /*= REQ_METHOD_GET*/)
{
	if (m_hRequest || m_hInternet || m_hConnect)
		CloseRequest();

	TCHAR szScheme[INTERNET_MAX_URL_LENGTH] = { 0 };
	TCHAR szHostName[INTERNET_MAX_URL_LENGTH] = { 0 };
	TCHAR szUrlPath[INTERNET_MAX_URL_LENGTH] = { 0 };
	WORD nPort = 0;
	DWORD dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
	LPCTSTR lpszProxy = NULL;
	// 解析Url
	BOOL bRet = ParseURL(lpszUrl, szScheme, INTERNET_MAX_URL_LENGTH, szHostName, INTERNET_MAX_URL_LENGTH, nPort, szUrlPath, INTERNET_MAX_URL_LENGTH);
	if (!bRet)
		return false;

	m_hInternet = (HINSTANCE)InternetOpen(L"Microsoft Internet Explorer",
		dwAccessType, lpszProxy, NULL, 0);
	if (!m_hInternet)
	{
		CloseRequest();
		return false;
	}
	m_hConnect = (HINSTANCE)InternetConnect(m_hInternet, szHostName,
		nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	
	if (!m_hConnect)
	{
		CloseRequest();
		return false;
	}
	m_hRequest = (HINSTANCE)HttpOpenRequest(m_hConnect, 
		(nReqMethod == REQ_METHOD_GET) ? _T("GET") : _T("POST"), szUrlPath,
		NULL, L"", NULL, 0, 0);
	if (!m_hRequest)
	{
		CloseRequest();
		return false;
	}

	return true;
}

BOOL CBWinInet::AddHeaders(LPCTSTR lpHeaders /*= NULL*/)
{
	if (!m_hRequest)
		return FALSE;

	return HttpAddRequestHeaders(m_hRequest, 
		lpHeaders ? lpHeaders : sHeaders.c_str(), -1, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);  
}

BOOL CBWinInet::SendRequest(const TCHAR* lpData /*= NULL*/ , DWORD dwLen /*= 0*/ )
{
	if (!m_hRequest)
		return FALSE;

	return HttpSendRequest(m_hRequest, NULL, 0, (LPVOID)lpData, dwLen);
}

BOOL CBWinInet::EndSendRequest()
{
	if (NULL == m_hRequest)
		return FALSE;

	return HttpEndRequest(m_hRequest, NULL, HSR_INITIATE, (DWORD_PTR)this);
}

DWORD CBWinInet::GetRespCode()
{
	DWORD dwRespCode = 0;
	DWORD dwSize = sizeof(dwRespCode);
	BOOL bRet = HttpQueryInfo(m_hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRespCode, &dwSize, NULL);
	if (bRet)
		return dwRespCode;
	else
		return 0;
}

void CBWinInet::SetConnectTimeOut(DWORD dwTimeOut)
{
	m_dwConnectTimeOut = dwTimeOut;
}

void CBWinInet::CloseRequest()
{
	if (m_hRequest)
	{
		InternetCloseHandle(m_hRequest);
		m_hRequest = NULL;
	}

	if (m_hConnect)
	{
		InternetCloseHandle(m_hConnect);
		m_hConnect = NULL;
	}

	if (m_hInternet)
	{
		InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
	}
}

BOOL CBWinInet::ParseURL(LPCTSTR lpszUrl, LPTSTR lpszScheme, DWORD dwSchemeLength, LPTSTR lpszHostName, DWORD dwHostNameLength, WORD& nPort, LPTSTR lpszUrlPath, DWORD dwUrlPathLength)
{
	URL_COMPONENTS stUrlComponents = { 0 };
	stUrlComponents.dwStructSize = sizeof(URL_COMPONENTS);
	stUrlComponents.lpszScheme = lpszScheme;
	stUrlComponents.dwSchemeLength = dwSchemeLength;
	stUrlComponents.lpszHostName = lpszHostName;
	stUrlComponents.dwHostNameLength = dwHostNameLength;
	stUrlComponents.lpszUrlPath = lpszUrlPath;
	stUrlComponents.dwUrlPathLength = dwUrlPathLength;

	BOOL bRet = ::InternetCrackUrl(lpszUrl, 0, 0, &stUrlComponents);
	if (bRet)
	{
		nPort = stUrlComponents.nPort;
	}

	return bRet;
}

// 获取全部HTTP头
tstring CBWinInet::GetRespHeader()
{
	CHAR* lpRespHeader = NULL;
	DWORD dwRespHeaderLen = 0;
	tstring strRespHeader;
	BOOL bRet = ::HttpQueryInfo(m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)lpRespHeader, &dwRespHeaderLen, NULL);
	if (!bRet)
	{
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpRespHeader = new CHAR[dwRespHeaderLen];
			if (lpRespHeader != NULL)
			{
				memset(lpRespHeader, 0, dwRespHeaderLen);
				bRet = ::HttpQueryInfo(m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)lpRespHeader, &dwRespHeaderLen, NULL);
				if (bRet)
				{
					strRespHeader = (TCHAR*)lpRespHeader;
				}
			}
		}
	}

	if (lpRespHeader != NULL)
	{
		delete[]lpRespHeader;
		lpRespHeader = NULL;
	}

	return strRespHeader;
}

// 获取HTTP头指定字段名称的值
tstring CBWinInet::GetRespHeader(LPCTSTR lpszName, int nIndex/* = 0*/)
{
	if (NULL == lpszName)
		return _T("");

	tstring strLine;
	int nNameLen, nIndex2 = 0;
	nNameLen = _tcslen(lpszName);
	if (nNameLen <= 0)
		return _T("");

	if (m_arrRespHeader.size() <= 0)
	{
		if (!__GetRespHeader())
			return _T("");
	}

	for (int i = 0; i < (int)m_arrRespHeader.size(); i++)
	{
		strLine = m_arrRespHeader[i];
		if (!_tcsnicmp(strLine.c_str(), lpszName, nNameLen))
		{
			if (nIndex == nIndex2)
			{
				int nPos = nNameLen;
				int nLineLen = (int)strLine.size();

				for (; nPos < nLineLen && strLine[nPos] == _T(' '); ++nPos)
					; // 跳过前导空格
				if (strLine[nPos] == _T(':'))   // 跳过“:”符号
					nPos++;

				for (; nPos < nLineLen && strLine[nPos] == _T(' '); ++nPos)
					; // 跳过前导空格

				return strLine.substr(nPos);
			}
			nIndex2++;
		}
	}

	return _T("");
}

// 获取HTTP头指定字段名称的值(Int)
int CBWinInet::GetRespHeaderByInt(LPCTSTR lpszName, int nIndex/* = 0*/)
{
	tstring strValue = GetRespHeader(lpszName, nIndex);
	return _tstoi(strValue.c_str());
}

tstring CBWinInet::GetRespHeaderByStr(LPCTSTR lpszName, int nIndex /*= 0*/)
{


	return TEXT("");
}

BOOL CBWinInet::GetRespBodyData(CHAR* lpBuf, DWORD dwLen, DWORD& dwRecvLen)
{


	return TRUE;
}

tstring CBWinInet::GetRespBodyData()
{
	BOOL bRet = 0;
	DWORD dwNumRead = 0;

	TCHAR buf[64] = { 0 };
	DWORD dwSizeOfReq = sizeof(buf);
	DWORD dwRespLen = 0;
	//需要注意的是，如果适用HttpQueryInfoW，那么buf必须也是宽字符版本，
	//虽然HttpQueryInfo()之前只是一个缓冲区，因为如果不使用宽字符，
	//buf 得到的字节数可能会因为 \0的原因被截断。    
	if (HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH, buf, &dwSizeOfReq, NULL))
		dwRespLen = _wtol(buf);

	if (dwRespLen == 0)	return L"";

	m_sRespBody.resize(dwRespLen+1);
	while (TRUE)
	{
		bRet = InternetReadFile(m_hRequest, (LPTSTR)m_sRespBody.c_str(), dwRespLen, &dwNumRead);
// 		if (!bRet || !dwNumRead)
// 			break;
		m_sRespBody[dwNumRead] = 0;
		break;
	}
	//预转换，得到所需空间的大小;
	int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, m_sRespBody.c_str(), m_sRespBody.length(), NULL, 0);
	wchar_t* wszString = new wchar_t[wcsLen + 1];	//分配空间要给'\0'留个空间，
													//MultiByteToWideChar不会给'\0'空间
	MultiByteToWideChar(CP_UTF8, NULL, m_sRespBody.c_str(), m_sRespBody.length(), 
		wszString, wcsLen);
	wszString[wcsLen] = '\0';	//最后加上'\0'
	//然后将对应的内容转换为 tstring
	tstring unicodeString(wszString);
	delete[] wszString;
	wszString = NULL;

	return unicodeString;
}

DWORD CBWinInet::__GetRespHeaderLen()
{
	BOOL bRet;
	LPVOID lpBuffer = NULL;
	DWORD dwBufferLength = 0;
	bRet = HttpQueryInfo(m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, lpBuffer, &dwBufferLength, NULL);
	if (!bRet)
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			return dwBufferLength;
		}
	}
	return 0;
}

// 获取HTTP响应头，按行保存在m_arrRespHeader数组
BOOL CBWinInet::__GetRespHeader()
{
	CHAR* lpRespHeader;
	DWORD dwRespHeaderLen;
	m_arrRespHeader.clear();

	dwRespHeaderLen = __GetRespHeaderLen();
	if (dwRespHeaderLen <= 0)
		return FALSE;

	lpRespHeader = new CHAR[dwRespHeaderLen];
	if (NULL == lpRespHeader)
		return FALSE;

	memset(lpRespHeader, 0, dwRespHeaderLen);

	BOOL bRet = ::HttpQueryInfo(m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, (LPVOID)lpRespHeader, &dwRespHeaderLen, NULL);
	if (!bRet)
	{
		delete[]lpRespHeader;
		lpRespHeader = NULL;
		return FALSE;
	}

	tstring strHeader = (TCHAR*)lpRespHeader;
	tstring strLine;
	int nStart = 0;
	tstring::size_type nPos = strHeader.find(_T("\r\n"), nStart);
	while (nPos != tstring::npos)
	{
		strLine = strHeader.substr(nStart, nPos - nStart);
		if (strLine != _T(""))
			m_arrRespHeader.push_back(strLine);

		nStart = nPos + 2;
		nPos = strHeader.find(_T("\r\n"), nStart);
	}

	delete[]lpRespHeader;
	lpRespHeader = NULL;
	return TRUE;
}



