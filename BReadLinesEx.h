//////////////////////////////////////////////////////////////////////
// CBReadLinesEx �ࣺһ��һ�еض��ļ� �� ��֧�ֳ���4G�Ĵ��ļ���
//
// �÷�ʾ����=================================
// CBReadLinesEx file(TEXT("C:\\abc.txt"));
// LPTSTR szLine;
// 
// // file.TrimSpaces=true;		// ɾ���ո�Tab �ȿհ��ַ�������ɲ����ã���Ĭ��Ϊ false
// // file.TrimControls=true;	// ɾ�����������ַ���0x01 -- 0x08, 0x0e -- 0x1F, 0x127��������ɲ����ã���Ĭ��Ϊ false
// // file.IgnoreEmpty=true;	// ����ɲ����ã���Ĭ��Ϊ false��ע�����һ���Կ��ܷ��ؿ��У�
// while ( !file.IsEndRead() )
// {
// 	file.GetNextLine(szLine);
// 	if (file.IsErrOccured()) break;
// 	cout<<szLine<<endl;
// }
// ============================================
// Support: ��Ҫ BWindows ģ���֧�֣�ʹ�����еĶ�д�ļ����� EFxxx()��

//////////////////////////////////////////////////////////////////////

#pragma once

#include "BWindows.h"

#define mc_RL_BufLen 131072				// ÿ���������ֽڳ���Ĭ��ֵ��ÿ�ζ�ȡ���ֽ�����

class CBReadLinesEx
{
private:
	static const TCHAR mcszFailInfoDefault[];	// Ĭ�϶�ȡ�����ַ�����Ϣ
	static const TCHAR mcszFileNotFoundInfoDefault[];	// Ĭ���ļ��������ַ�����Ϣ

private:
	typedef struct _RLBuffType		// һ�������� ����������
	{	
		LONGLONG llStartPosAbso;	// �û��������ļ��еľ���λ�ã���0��ʼ��m_llFileLength-1��
		LONG iBufLen;				// �������ܳ�
		LONG iPtrInBuf;				// �������ڲ�ָ�루��0��ʼ��iBufLen-1��
		LONG iNextLineStaInBuf;		// ��һ�����ݿ�ʼλ��(�Ӵ˴��㵽��һ��cr/lfΪ��һ��)����0��ʼ��iBufLen-1��
		int iIgnoreFirstLf;			// �Ƿ���Ա��������Ŀ�ͷ vblf��=0�����ԣ�=1��ͷ��0��lf��lf+0�����ԣ�=2��ͷ��lf��lf+0�����ԣ�=3��ͷ��0�����
		char bufBytes[mc_RL_BufLen];// �������ֽ����ݣ��±��0��ʼ bufBytes[0] �� bufBytes[mc_RL_BufLen-1]
	} RLBuffType;

	typedef struct _RLLastBuffType	// ������ʣ����ֽڵ���������
	{
		char * bytsLeft;			// ����������壨����mc_RL_BufLen����δ�ҵ� \r\n����˿ռ�ᱻ�����ӳ�
		LONG sizeBytsLeft;
	} RLLastBuffType;

public:  // ����
	bool TrimSpaces;		// �Ƿ��Զ�ȥ��ÿ�пո��ַ����ո�Tab �ȿհ��ַ�����ɾ����
	bool TrimControls;		// �Ƿ��Զ�ȥ��ÿ�п����ַ���0x01 -- 0x08, 0x0e -- 0x1F, 0x127��
	bool AutoOpen;			// �Ƿ����� FileName ����ʱ�Զ����ļ���Ĭ��Ϊtrue(���ʼ��ʱ��Ϊtrue)
	bool AutoClose;			// �Ƿ� ��ȡ�ж����ļ������ʱ �Զ��ر��ļ���Ĭ��Ϊtrue(���ʼ��ʱ��Ϊtrue)
	bool IgnoreEmpty;		// �Ƿ��Զ����Կ��У�ע�⣺��������һ���Կ��ܷ��ؿ��У�
	bool ShowMsgIfErrRead;	// ��ȡʧ���Ƿ��Զ���ʾ
	bool AsUnicode;			// �Ƿ�Դ�ļ�ʹ�� Unicode ��ʽ������ͨ�����������⣬�� OpenFile ʱ���Զ����ô�ֵ
	bool AsUTF8;			// �Ƿ�Դ�ļ�ʹ�� UTF8 ��ʽ������ͨ�����������⣬�� OpenFile ʱ���Զ����ô�ֵ

	// �еĽ�����־:0=δ�衣13,10 or 2573(vbcrlf) ��
	// -1:unknown(��ʱ�ٴε���GetNextLine��EndLineSignLast���)��
	// -2:δ֪�������ļ�ĩβ���ļ�ĩβ�޻��з�
	int iEndLineSign;		

	// ��һ�еĽ�����־ 0=δ��
	int iEndLineSignLast;	

	LPTSTR FailInfoRead;		// ��ȡ����ʱ��������ʾ��Ϣ���粻���ô�������� mcszFailInfoDefault
								// ��ShowMsgIfErrRead Ϊ True ʱ�Żᵯ����
	LPTSTR FailInfoFileNotFound;// �ļ�δ�ҵ�ʱ��������ʾ��Ϣ���粻���ô�������� mcszFileNotFoundInfoDefault
								// ��ShowMsgIfErrRead Ϊ True ʱ�Żᵯ����

public:  // ����
	// ���캯����szFile Ϊ NULL ʱ���ݲ������ļ���
	//   ������ szFile ��Ϊ�ļ��������豾������
	//   ���� m_szFileName�����Զ� OpenFile
	CBReadLinesEx(LPCTSTR szFile=NULL);
	~CBReadLinesEx();

	// ��ʼ��
	void Init();	

	// ��ȡ�ļ�����һ���ı���֧�� \r\n��\n��\r �Ķ��ַ��з�
	// ���޸Ĳ��� szLine ��ֵ��Ϊ����ȡ�Ĵ����ı����׵�ַ
	// �����ַ����ռ��ɱ����Զ����٣�ָ��δ�� HM ��������
	//   ���浽 m_szOneLine �У������һ������ֻ��
	//   �������һ�е��ַ��������´� GetNextLine ʱ����һ�е�
	//   �ַ����ռ�ͱ��ͷţ����һ��һ�е��ַ����ռ������������ͷ�
	// ���� 1 ��ʾ������ȡ��
	//   ���� -1 Ҳ��ʾ���������������ļ�
	//   ���� 0 ��ʾ�����Ƿ�
	//     1. һ������� 0�������� IsErrOccured=True
	//     2. ����ϴζ������ļ����������ٶ������һ�� GetNextLine (���� 0 ��
	//        ����ʾ����IsErrOccured ��Ϊ false������Ƿ�)������ٵ��þͳ�����
	//        (�����Է���0���� IsErrOccured Ϊ true �������)
	//     3. IgnoreEmpty=True ʱ�Զ����Կ��У�����ӵ�ǰһֱ�����ļ�����
	//        ���ǿ��У��򶼺��ԣ�������0����ʱ IsErrOccured=false������Ƿ���
	// ֻ��Ҫ���� IsErrOccured=true �Ż��� ShowMsgIfErrRead=true ʱ����������ʾ
	int GetNextLine(LPTSTR &szLine);

	// ��ȡ�ļ��������ļ�������������ɵ�һ���ַ������� \r\n �ָ���
	// �ռ��ɱ������Զ����٣������浽 HM �й�������������Ҳ������գ�
	LPTSTR GetFileContent();

    // ���ļ�ָ���ƶ��� llToPos λ�ã��ٵ��� GetNextLine ʱ������ȡ
	//   ���������޸� lIsEndRead=false
	void SeekFile(LONGLONG llToPos);

	// ���ص�ǰ��ȡ����ļ��Ľ���
	//   iDotNum ������λС����<0 �� >7 Ϊ������С��
	float GetPercent(int iDotNum = 2);

	// ���ļ���
	// szFileName Ϊ NULL ʱ��ʹ�ñ������ڲ��� m_szFileName ��Ϊ�ļ���
	//   ������ szFileName ��Ϊ�ļ��������豾�����ڲ��� m_szFileName
	// �˺������Ǳ���������������õģ��� AutoOpen ����Ϊ true ʱ��
	//   ���������캯���и������������ FileNameSet ʱ����Ҳ���Զ����ñ�����
	bool OpenFile(LPCTSTR szFileName= NULL);
	
	// �ر��ļ����� AutoClose ����Ϊ true ʱ�����ཫ�Զ����ñ�����
	void CloseFile();

	// ���ļ���ȡָ��ָ���ļ���ͷ�����ļ��ѹرգ������´��ļ�
	void SetReadPtrToStart();		

	// ���ػ���������ֵ�ķ���
	void FileNameSet(LPCTSTR szFile);		// ����Ҫ��ȡ���ļ����ļ�����
											// �� AutoOpen ����Ϊ true ʱ�����Զ� OpenFile
	LPTSTR FileName();		// ����ļ���
	HANDLE hFile();			// ����ļ��򿪵ľ��
	int Status();			// ���״̬��-1=�ѹر�;1=�Ѵ�;2=�Ѿ���ʼ��ȡ;0=δ��
	bool IsEndRead();		// ��ö���״̬��=true��ʾ���߶����ļ����߳����������ټ������ˣ�������Ӧ�˳���ȡ
	bool IsErrOccured();	// ����Ƿ����˴���״̬���Ƿ��ϴ� GetNextLine ������һ������

private:
	RLBuffType m_buff;			// һ��������
	RLLastBuffType m_LastBuff;	// ������ʣ����ֽ�

	LPTSTR m_szOneLine;			// ����һ���ַ����Ŀռ��ַ��Ansi�汣��Ansi�ַ�����Unicode �汣��Unicode�ַ�����
								//   ÿ�� GetNextLine �е� BytsToString �� Delete ��һ�εĿռ䣻
								//  ����ʱ�� Delete ����ָ��Ŀռ�
	LPTSTR BytsToString(const char * pByts,		// ��һ���ֽ����鰴 Ansi �� Unicode ��ת��Ϊ�ַ������أ�
						LONG sizeByts);			//   �� TrimSpaces ������ɾ���ո�Ϳհ��ַ�����
												//   �� TrimControls ������ɾ�����������ַ�����0x01 -- 0x08, 0x0e -- 0x1F, 0x127��
												// �ռ��Զ����٣������ַ����ռ��׵�ַ���ҽ��ռ��׵�ַ���浽 m_szOneLine											
												// ÿ��ִ�иú���ʱ���� Delete ��һ�ε��ַ����ռ�
																

	bool m_bOneEndRead;			// �Ƿ��ڹر��ļ��������ٵ���һ�� GetNextLine
	LONGLONG m_llFileLength;	// �ļ��ܳ��ȣ��ֽ�����

	TCHAR m_szFileName[2048];	// �ļ���
	HANDLE m_hFile;				// �ļ��򿪾��
	int m_iStatus;				// ״̬��-1=�ѹر�;1=�Ѵ�;2=�Ѿ���ʼ��ȡ;0=δ��
	bool m_bIsEndRead;			// =true��ʾ���߶����ļ����߳����������ټ������ˣ�������Ӧ�˳���ȡ
	bool m_bErrOccured;			// �Ƿ��ϴ� GetNextLine ������һ������
	TCHAR mEmptyStr[2];			// �մ��������������ַ����ĺ�������ʱ���ش�����ĵ�ַ��Ϊ���ؿմ�
};

// Dim af_strBuf As String
// Dim af_bytsBuf() As Byte '�˽�ΪGetNextLine�����ã�Ϊ�˲�ÿ�ε���GetNextLineʱ�����¶��壬�ʽ�֮��Ϊȫ�ֵ��ˣ���ʵӦ�Ǿֲ���
// Dim j As Long