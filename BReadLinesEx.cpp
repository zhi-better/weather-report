//////////////////////////////////////////////////////////////////////
// CBReadLinesEx ���ʵ�֣�һ��һ�еض��ļ� �� ��֧�ֳ���4G�Ĵ��ļ���
// 
//////////////////////////////////////////////////////////////////////

#include "BReadLinesEx.h"


const TCHAR CBReadLinesEx::mcszFailInfoDefault[]=TEXT("����޷���ȡָ�����ļ���\r\n��ȷ�����̿��ò����ļ����Է��ʡ�");
const TCHAR CBReadLinesEx::mcszFileNotFoundInfoDefault[]=TEXT("�ļ������ڣ�����һ���Ϸ����ļ�����");

//////////////////////////////////////////////////////////////////////
// ���������
//////////////////////////////////////////////////////////////////////

CBReadLinesEx::CBReadLinesEx(LPCTSTR szFile/*=NULL*/)
{   
	// ����ֵ
	TrimSpaces = false;
	TrimControls = false;
	AutoOpen = true;
	AutoClose = true;
	IgnoreEmpty = false;
	ShowMsgIfErrRead = true;
	AsUnicode = false;
	AsUTF8 = false;
	
	memset(m_szFileName, 0, sizeof(m_szFileName));
	m_hFile = INVALID_HANDLE_VALUE;
	m_iStatus = 0;

	m_szOneLine = NULL;
	m_LastBuff.sizeBytsLeft = 0;
	m_LastBuff.bytsLeft = NULL;

	Init();

	// ��������� szFile �������Զ����ļ�
	if (szFile) FileNameSet(szFile);
}


CBReadLinesEx::~CBReadLinesEx()
{
	CloseFile();

	if (m_szOneLine) { delete []m_szOneLine; m_szOneLine=NULL; }
	if (m_LastBuff.bytsLeft) { delete [] m_LastBuff.bytsLeft; m_LastBuff.bytsLeft = NULL; }
}



//////////////////////////////////////////////////////////////////////
// ���к���
//////////////////////////////////////////////////////////////////////

void CBReadLinesEx::Init()
{
	if (m_LastBuff.bytsLeft) { delete [] m_LastBuff.bytsLeft; m_LastBuff.bytsLeft = NULL; }
	m_LastBuff.sizeBytsLeft = 0;
	
	memset(m_buff.bufBytes, 0, sizeof(m_buff.bufBytes));
	m_buff.iBufLen = 0;
	m_buff.iNextLineStaInBuf = 0;
	m_buff.iPtrInBuf = -1;			// ����Ϊ��־��=-1��ʾ�´����� GetNextLine Ҫ���¶�ȡ�µĻ�����
	m_buff.iIgnoreFirstLf = 0;		// ��ʼ����־����ǰ����������Ҫ���Ե�һ���ֽڣ�����\n��
									//   �������¶�ȡ����ʹ�õ�ǰ�������� .iPtrInBuf ָ��
	m_buff.llStartPosAbso = 0;		// ��ǰ����������ʼ�����ڵ��ļ�λ��
	
	m_szOneLine=NULL;				// ����һ���ַ������ݵĿռ��ַ
	m_llFileLength = 0;

	m_bOneEndRead = true;			// ���ñ�־���رպ��ٵ���һ�� GetNextLine ������
	m_bIsEndRead = false;
	m_bErrOccured = false;
	mEmptyStr[0]=0; mEmptyStr[1]=0;
	FailInfoRead = NULL;				// ��ȡ����ʱ��������ʾ��Ϣ�������ô�������� mcszFailInfoDefault
	FailInfoFileNotFound = NULL;		// �ļ�������ʱ��������ʾ��Ϣ�������ô�������� mcszFileNotFoundInfoDefault

	iEndLineSign = 0;
	iEndLineSignLast = 0;
}


int CBReadLinesEx::GetNextLine( LPTSTR &szLine )
{
    // ���� ��ӳ���з��� iEndLineSign �� iEndLineSignLast ��־����
    iEndLineSignLast = iEndLineSign;// ����һ�еķ��з�����Ϊ��ǰ�еķ��з�
    iEndLineSign = 0;				// ����ǰ�еķ��з�����Ϊ0���ں��������걾�к��پ�������
 
	// ���÷�ӳ����ı�־����
    m_bErrOccured = false;			// ��ʾ��δ������������������з����˴����ٸ�Ϊ true
 
	// �жϺ�����״̬
    if (m_iStatus == 0)				// ��ǰ״̬�Ƿ�����δ���ļ����޷���ȡ
		goto errExit;				
	else if (m_iStatus < 0)			// m_iStatus<0����ʾ��ʱ�ļ���δ���򿪣����߱�ǿ�ƹرգ�
	{								//   �����Ѿ������ļ����Զ��رգ���֮�ǲ����ټ�����ȡ�ļ���
		if (m_bOneEndRead)			// ���ļ��Ѷ�ȡ��ϣ������ٶ���ص���һ��GetNextLine����
		{							// ��������������һ��
			m_bOneEndRead = false;	// ���ñ�־Ϊ false���������ٶ������
			return 0;				// ������������0�� ��ʱ m_bErrOccured ��Ϊ false
        }
		else
			goto errExit;				// �������������ˣ�����
	}

	// ==================================================================
	// ������ȡ���������ʱ m_iStatus ҪôΪ1ҪôΪ2����Ҫô�ļ��Ѿ��򿪣�
	//   Ҫô�Ѿ������ȡ״̬�ˣ���֮��ȡ��һ����û�������
	m_iStatus = 2;					// ����Ϊ2��ʾ�Ѿ������ȡ״̬

    // //////////////// ��ȡ�ļ������ҵ���һ�С������� ////////////////
    // �����������ļ�ǰ����ֱ����������ʼλ�ó����ļ��ܳ������ļ�
    while ( m_buff.llStartPosAbso <= m_llFileLength-1 )
	{
        // ============ ��1��������Ҫ��ȡ�ļ�����һ������������ ============
        // �� .iPtrInBuf == -1 ��ʾҪ��ȡ��һ�������������򲻶�ȡ��һ������ʹ��
        //   ��ǰ�������� .iPtrInBuf ָ��
        if ( m_buff.iPtrInBuf < 0 ) 
		{
            // ----�� .llStartPosAbso ��ʼ��ȡһЩ�ֽڴ��뻺���� .bufBytes[)
            m_buff.iBufLen = EFGetBytes(m_hFile, m_buff.llStartPosAbso, m_buff.bufBytes, mc_RL_BufLen, 0); 
            if (m_buff.iBufLen < 0) goto errExit;	// ��ȡ����
            
            // ----��ʼ��������ָ��
            m_buff.iPtrInBuf = 0;

            // ���Ƿ���Ҫ���Ե�һ�� \n
			// .iIgnoreFirstLf==1��ͷ��0��lf��lf+0�����ԣ�==2��ͷ��lf��lf+0�����ԣ�
			//   ==3��ͷ��0�����
            if ( m_buff.iIgnoreFirstLf==1 )   // ��ͷ��0��lf��lf+0������
			{
				if (m_buff.iPtrInBuf < m_buff.iBufLen)
					if (m_buff.bufBytes[m_buff.iPtrInBuf]==0) m_buff.iPtrInBuf++;

				m_buff.iIgnoreFirstLf = 2;  // �������� =2 ��֧
			}  // ==1 ��֧����

			if ( m_buff.iIgnoreFirstLf==2 ) // ��ͷ��lf��lf+0������
			{
				if (m_buff.iPtrInBuf < m_buff.iBufLen)
				{
					if (m_buff.bufBytes[m_buff.iPtrInBuf]==10) 
					{
						m_buff.iPtrInBuf++;
						iEndLineSignLast = 2573;	// �ϴεķ��з�Ϊ \r\n
						m_buff.iIgnoreFirstLf = 3;  // �������� =3 ��֧
					}
					else // if (m_buff.bufBytes(m_buff.iPtrInBuf)==10) 
					{
						// ��1���ֽڲ��� Lf������ΪҪ���Ե�1�� Lf
						//   ˵����һ������� Cr����������һ�з��з�Ϊ Cr
						iEndLineSignLast = 13;
					}   // if (m_buff.bufBytes(m_buff.iPtrInBuf)==10) 
				}  // if (m_buff.iPtrInBuf < m_buff.iBufLen)
			}  // ==2 ��֧����


			if ( m_buff.iIgnoreFirstLf==3 ) // ��ͷ��0�����
			{
				if (m_buff.iPtrInBuf < m_buff.iBufLen)
					if (m_buff.bufBytes[m_buff.iPtrInBuf]==0) m_buff.iPtrInBuf++;
			}

            m_buff.iIgnoreFirstLf = 0; // �ָ���־�������Ե�һ�� Lf(\n)
            
            // ��ʼ����һ����ʼλ�� iNextLineStaInBuf ����һ�����ݰ������ֽڣ�
            m_buff.iNextLineStaInBuf = m_buff.iPtrInBuf;
        } // end if ( m_buff.iPtrInBuf < 0 )
        
        // ============ ��2�����ɨ�軺�����е��ֽڣ����ҷ��з� ============
        // ɨ�軺�����е��ֽڣ�ֱ���ҵ� \r��\n ��ɨ���껺����
        for ( ; m_buff.iPtrInBuf < m_buff.iBufLen; m_buff.iPtrInBuf++ )
            if (m_buff.bufBytes[m_buff.iPtrInBuf] == 13 ||
             m_buff.bufBytes[m_buff.iPtrInBuf] == 10 ) break;

		//////////////////////////////////////////////////////////////////////////
        // �˳� for ���ж��Ƿ��ҵ��˷��з� \r��\n
        if (m_buff.iPtrInBuf < m_buff.iBufLen)    // �Ƿ��� .bufBytes[] ���ҵ��� \r��\n
        {

			//////////////////////////////////////////////////////////////////////////
			// ============ ��3���ҵ�һ�����з� \r��\n ============
            // ��������Ϊ���� .iNextLineStaInBuf ��λ�ã�af_Buff.iPtrInBuf - 1

			// ���ٶ�̬�ռ䣬����һ�����ݵ��ֽڣ�pByts �� [0] ��ʼ�� �� [sizeByts-1]����
			// ��󽫴��е��ֽڰ� Ansi �� Unicode ת������������ m_szOneLine��ͨ������ BytsToString ��ɣ���
			//   ���� szLine = m_szOneLine ������ szLine
			// ����֧����ǰ��delete [] pByts;
			char * pByts = NULL;  LONG sizeByts = 0; 

            // ---- ���ñ��л��з� ----
            iEndLineSign = m_buff.bufBytes[m_buff.iPtrInBuf];
            
            // ---- ����Ҫ���صı����ַ������ֽ����� => pByts ----
			sizeByts = m_buff.iPtrInBuf - m_buff.iNextLineStaInBuf + m_LastBuff.sizeBytsLeft + 1;	// +1 ΪԤ�� \0
            if ( sizeByts < 2  )
			{
                // .iPtrInBuf == .iNextLineStaInBuf ʱ������ .iPtrInBuf
                //   == .iNextLineStaInBuf == 0 ʱ������ʼ���� \r/\n
                pByts = new char [2];  sizeByts = 2;	// �մ�ֻ��1���ֽڣ�����࿪����1���ֽ�
				*pByts = 0;	// ׼�����ַ������ֽ�����
			}
            else
			{
                // -- ��Ҫ���ص��ַ����������ֽ����ϴ�ʣ����ֽ� �� ���� m_buff.bufBytes[] �� ��
                //   .PtrInBuff λ�õ��ֽ�(���� .PtrInBuff λ��)������Щ�ֽ�ȫ������ pByts[] ���� --
				pByts = new char [sizeByts];

                // �Ƚ��ϴ�ʣ����ֽ� leftBytes ���� pByts �Ŀ�ʼ
				memcpy(pByts, m_LastBuff.bytsLeft, m_LastBuff.sizeBytsLeft);
                
                // �ٽ����� [.iNextLineStaInBuf, .iPtrInBuf) ��Χ���ֽ� ���Ŵ��� pByts 
                //   �������� .iPtrInBuf λ�ã���Ϊ .iPtrInBuf �� \r��\n
                memcpy(pByts + m_LastBuff.sizeBytsLeft, 
				   m_buff.bufBytes + m_buff.iNextLineStaInBuf, 
				   m_buff.iPtrInBuf - m_buff.iNextLineStaInBuf);
                
				// �� pByts[] �����ĩβ \0
				pByts [sizeByts-1] = 0;	// Ҳ�� pByts [m_LastBuff.sizeBytsLeft + m_buff.iPtrInBuf - m_buff.iNextLineStaInBuf] = 0; 

                // -- ����ϴ�ʣ����ֽڻ����� LeftBytes --
				if (m_LastBuff.bytsLeft) { delete []m_LastBuff.bytsLeft; m_LastBuff.bytsLeft = NULL; }
                m_LastBuff.sizeBytsLeft = 0; 
            }	// end if ( m_buff.iPtrInBuf - m_buff.iNextLineStaInBuf + m_LastBuff.sizeBytsLeft < 1  )

			// ---- �������ݽṹ��ָ�� ----

			// ����� vbLf������һ���ֽ����� 0 ��ֱ��������Unicode��ʽ�� ----
            if (m_buff.bufBytes[m_buff.iPtrInBuf] == 10)
			{
				if (m_buff.iPtrInBuf + 1 < m_buff.iBufLen)
				{
					if (m_buff.bufBytes[m_buff.iPtrInBuf+1] == 0) m_buff.iPtrInBuf++;
				}
				else
				{
					m_buff.iIgnoreFirstLf = 3;  // =3��ͷ��0�����
				}
            }

			// �ж��Ƿ��������� \r+\n�����ǣ�������һ�� \n
            if (m_buff.bufBytes[m_buff.iPtrInBuf] == 13)  
			{
                if ( m_buff.iPtrInBuf + 1 >= m_buff.iBufLen )
				{
                    // �����һ���ֽ��Ѿ�������������������޷��ж���һ���ֽ�
                    //   �Ƿ��� \n������ֻ���ñ�־���Ժ��ж��Ƿ� \n ����������
                    m_buff.iIgnoreFirstLf = 1;
                    iEndLineSign = -1;
				}
                else
				{
                    // ��һ���ֽ�û�����������������һ���ֽ����� \n ��ֱ������
                    if (m_buff.bufBytes[m_buff.iPtrInBuf + 1] == 10 ) 
                    {
						m_buff.iPtrInBuf ++ ;
                        iEndLineSign = 2573;
                    }
					else if (m_buff.bufBytes[m_buff.iPtrInBuf + 1] == 0 ) 
					{
						// ��һ���ֽ����� 0 ��ֱ��������Unicode��ʽ��
						m_buff.iPtrInBuf ++; 
						// ������Ѱ�Һ���� \n��10��
						if (m_buff.iPtrInBuf + 1 >= m_buff.iBufLen )
						{
							m_buff.iIgnoreFirstLf = 2;
							iEndLineSign = -1;
						}
						else  // if (m_buff.iPtrInBuf + 1 >= m_buff.iBufLen )
						{
							// �� 0 ���� \n��10��//////////////////////////////////////
							if (m_buff.bufBytes[m_buff.iPtrInBuf + 1] == 10 )
							{
								m_buff.iPtrInBuf++;
								iEndLineSign = 2573; 
								// ������Ѱ�Һ���� 0
								if (m_buff.iPtrInBuf + 1 >= m_buff.iBufLen )
								{
									m_buff.iIgnoreFirstLf = 3;
									iEndLineSign = 2573;
								}
								else
								{
									if (m_buff.bufBytes[m_buff.iPtrInBuf + 1] == 0 )
									{
										m_buff.iPtrInBuf++;
										iEndLineSign = 2573; 
									}
								}
							}
							/////////////////////////////////////////////////////////////

						}	  // if (m_buff.iPtrInBuf + 1 >= m_buff.iBufLen )
					}

                }
            }	// if (.bufBytes[.iPtrInBuf] == 13)
            
            // ���õ�ǰ�������ڲ�����һ�е���ʼλ�ã�ע�����ﻹδʹ .iPtrInBuf + 1��
            m_buff.iNextLineStaInBuf = m_buff.iPtrInBuf + 1;  // ��һ���ַ���������ֽ�
            
            // ---- �ж��Ƿ��Ѿ������ļ������Ѷ����ļ������ø���־���� CloseFile()
			//   .iPtrInBuf Ҫ + 1 �����жϣ���Ϊ����ѭ���� .iPtrInBuf Ҫ +1�����ڻ�δ +1
            //   �Ƿ�����ļ��ı�־�浽 lIsEndRead���� if ��ݴ˾�������ֵ
            if (m_buff.iPtrInBuf + 1 >= m_buff.iBufLen &&
               m_buff.llStartPosAbso + m_buff.iBufLen >= m_llFileLength )
			{
                // �Ѿ������ļ�
                m_bIsEndRead = true;
                if (AutoClose) CloseFile();
			}
            else
			{
                // ��δ�����ļ������ж��Ƿ��ļ�ֻʣ1-3���ֽڣ���ֻʣ1-3���ֽڲ���
                //   ʣ�µ������� \n�������´�Ҫ���Ե� \n���������Ѿ������ļ�

				if (m_llFileLength - m_buff.iBufLen - m_buff.llStartPosAbso >=1 &&
					m_llFileLength - m_buff.iBufLen - m_buff.llStartPosAbso <=3 &&
					m_buff.iIgnoreFirstLf > 0)
				{
					// ��ȡ�ļ��е����һ���ֽڣ�ֻ����һ��
					char tByt[mc_RL_BufLen]; LONG tRet;

                    tRet = EFGetBytes(m_hFile, m_buff.llStartPosAbso + m_buff.iBufLen, tByt, mc_RL_BufLen, 0);
                    if (tRet < 0)  goto errExit;   // ������


                    switch (m_buff.iIgnoreFirstLf) 
					{
                    case 1:   // ��ͷ��0��lf��lf+0������
						switch (tRet)
						{
						case 1:
							if (tByt[0] == 0)  {m_bIsEndRead = true; iEndLineSign = 13; }
							if (tByt[0] == 10) {m_bIsEndRead = true; iEndLineSign = 2573; }
							break;
						case 2:
							if (tByt[0] == 0 && tByt[1] == 10) {m_bIsEndRead = true; iEndLineSign = 2573; }
							break;
						case 3:
							if (tByt[0] == 0 && tByt[1] == 10 && tByt[2] == 0) {m_bIsEndRead = true; iEndLineSign = 2573;}
							break;
						}
						break;
					case 2:   // ��ͷ��lf��lf+0������
                        switch (tRet)
                        {
						case 1:
                            if (tByt[0] == 10 ) {m_bIsEndRead = true; iEndLineSign = 2573;}
							break;
                        case 2:
                            if (tByt[0] == 10 && tByt[1] == 0 ) {m_bIsEndRead = true; iEndLineSign = 2573;}
							break;
						}
						break;
					case 3:   // ��ͷ��0�����
                        switch (tRet)
						{
                        case 1:
                            if (tByt[0] == 0 ) {m_bIsEndRead = true; iEndLineSign = 2573;}
							break;
						}
						break;
                    } // end switch
                    
                    // �Ѿ������ļ�
                    if (m_bIsEndRead)  if (AutoClose) CloseFile();

				}
			}

            m_buff.iPtrInBuf ++ ;
            
			// ---- �������ַ������ֽ����� pByts �� Ansi �� Unicode ת��
			//   Ϊ�ַ��� => szLine �����ء�delete []pByts���˳����� ---- 
			szLine = BytsToString(pByts, sizeByts);
			delete []pByts; pByts=NULL;

            // ����Ҫ�ж��Ƿ���Կ��У�����ǿ��оͲ��˳������� Loop
			if (m_bIsEndRead || !(IgnoreEmpty && *szLine == 0) )
			{
                // �Ѿ������ļ������߲����Կ��У����ߺ��Կ��е����ǿ��У�һ���˳�����
                if ( m_bIsEndRead && IgnoreEmpty && *szLine == 0  )
                    return 0;	// �����ڶ����ļ�������Ҫ���Կ��У������һ��Ϊ���еģ�����0
                else if ( m_bIsEndRead )
					return -1;	// �����ļ�������Ҫ���Կ��л�����ǿ��У����������ļ� ����-1
				else
					return 1;	// û�ж����ļ������� 1
			}
			// else ���� Loop

			//
			// ============ ������3���ҵ�һ�����з� \r��\n ============
			//////////////////////////////////////////////////////////////////////////


        }		// if (m_buff.iPtrInBuf < m_buff.iBufLen)    // �Ƿ��� .bufBytes[] ���ҵ��� \r��\n  
        else	// if (m_buff.iPtrInBuf < m_buff.iBufLen)    // �Ƿ��� .bufBytes[] ���ҵ��� \r��\n  
		{

			//////////////////////////////////////////////////////////////////////////
            // ============ ��4��û���ҵ����з���\r��\n���Ĵ��� ============
            // ���ñ�־��=-1 ��ʾ�´�Ҫ���¶�ȡ�µĻ������� _
            //   ���򽫲����¶�ȡ����ʹ�õ�ǰ�������� .iPtrInBuf ָ��
            m_buff.iPtrInBuf = -1;
            
            // ==== �����������Ƿ���ʣ��δ������ֽڣ����У�
            //    ��ʣ��Ĵ��� m_LastBuff.bytsLeft[] ====
            if (m_buff.iNextLineStaInBuf < m_buff.iBufLen)  
			{
				// ���¶��� m_LastBuff.bytsLeft[] ���飨-1�ĺ����ǽ�����ת��Ϊ ubound��
				Redim(m_LastBuff.bytsLeft, m_LastBuff.sizeBytsLeft + m_buff.iBufLen - m_buff.iNextLineStaInBuf - 1, 
				  m_LastBuff.sizeBytsLeft - 1, true);
                memcpy( m_LastBuff.bytsLeft + m_LastBuff.sizeBytsLeft, 
						m_buff.bufBytes + m_buff.iNextLineStaInBuf, 
						m_buff.iBufLen - m_buff.iNextLineStaInBuf);
                m_LastBuff.sizeBytsLeft +=  m_buff.iBufLen - m_buff.iNextLineStaInBuf;
            }
            
            // ==== ׼����������һ�������� ====
            m_buff.llStartPosAbso +=  m_buff.iBufLen;

			//
			// ============ ������4��û���ҵ����з���\r��\n���Ĵ��� ============
			//////////////////////////////////////////////////////////////////////////
        } // end if (m_buff.iPtrInBuf < m_buff.iBufLen)    // �Ƿ��� .bufBytes[] ���ҵ��� \r��\n  

    }	// while (m_buff.llStartPosAbso <= m_llFileLength) �����������ļ�ǰ����ֱ����������ʼλ�ó����ļ��ܳ������ļ�
    
    
    // //////////// ȫ�������ļ�����������ʣ����ֽ� ////////////
    if ( m_LastBuff.sizeBytsLeft )
    {
		// ��ʣ����ֽڣ��� m_LastBuff.iLeftBLen ���� m_LastBuff.leftBytes[] 
		//   �� Ansi �� Unicode ת��Ϊ�ַ������� szLine ����
		szLine = BytsToString(m_LastBuff.bytsLeft, m_LastBuff.sizeBytsLeft);
		delete []m_LastBuff.bytsLeft; m_LastBuff.bytsLeft = NULL;
		m_LastBuff.sizeBytsLeft = 0;

        if (AutoClose) CloseFile();
        iEndLineSign = -2;		// �����ļ�ĩβ��ĩβ���һ���ޱ�ʶ��
        m_bIsEndRead = true;

		// ��ʱ�����ļ������뷵��
        if ( IgnoreEmpty && *szLine == 0 )  
            return 0;
        else
            return -1;
    }	// end if ( m_LastBuff.iLeftBLen )
	
	// ���е��˴���ʲô��û�ж�ȡ��
	if (m_llFileLength==0)
	{
		// �ļ��ֽ�Ϊ0�����ؿմ������ö����ļ���־
		iEndLineSign = -2; 
		
		// �Ѿ������ļ�
		m_bIsEndRead = true;
		if (AutoClose) CloseFile();
		
		// ���ؿմ�
		char * pByts=new char [1];
		*pByts = 0;  
		szLine = BytsToString(pByts, 1);
		delete []pByts; pByts=NULL;

		// �Ѿ������ļ������߲����Կ��У����ߺ��Կ��е����ǿ��У�һ���˳�����
		if ( IgnoreEmpty )
			return 0;	// �����ڶ����ļ�������Ҫ���Կ��У������һ��Ϊ���еģ�����0
		else
			return -1;	// �����ļ�������Ҫ���Կ��л�����ǿ��У����������ļ� ����-1
	}
	
errExit:
    if (ShowMsgIfErrRead) 
	{
		if (FailInfoRead)
			MsgBox(FailInfoRead, NULL, mb_OK, mb_IconError);
		else
			MsgBox(mcszFailInfoDefault, NULL, mb_OK, mb_IconError);
    }
    m_bErrOccured = true;
    iEndLineSign = 0;
    // Ϊһ����󣬲����� lIsEndRead = True
    if (AutoClose) CloseFile();

	return 0;
}

LPTSTR CBReadLinesEx::GetFileContent()
{
	if (! OpenFile()) // ���ļ�ʧ�ܣ����ؿմ�
		{ 	mEmptyStr[0]=0; mEmptyStr[1]=0; return mEmptyStr; }
	
	LPTSTR szLine;
	TCHAR * szResult = NULL; 
	LONG iUbound=1023, iUboundNew=0, lenUsed = 0;
	Redim(szResult, iUbound, -1, false);
	*szResult=0;

	// ��ȡ��һ�У���һ�����⴦���򲻼� \r\n
	if (! m_bIsEndRead )
	{
		GetNextLine(szLine);
		if (m_bErrOccured)
		{
			if ( iUbound - lenUsed < lstrlen(szLine)+2 )		// iUbound - lenUsed+1-1��-1��ʾԤ��\0����+2 ��ʾ \r\n
			{
				iUboundNew = iUbound+lstrlen(szLine)+2 + 1000; 
				Redim(szResult, iUboundNew, iUbound, true);
				iUbound = iUboundNew; 
			}
			_tcscpy(szResult, szLine);
			lenUsed += lstrlen(szLine);
		}
	}
	
	// ��ȡ�ڶ��е���ĩ��
	while (!m_bIsEndRead && !m_bErrOccured )
	{
		GetNextLine(szLine);
		if (m_bErrOccured) break;

		if ( iUbound - lenUsed < lstrlen(szLine)+2 )		// iUbound - lenUsed+1-1��-1��ʾԤ��\0����+2 ��ʾ \r\n
		{
			iUboundNew = iUbound+lstrlen(szLine)+2 + 1000;
			Redim(szResult, iUboundNew, iUbound, true);
			iUbound = iUboundNew; 
		}
		_tcscat(szResult, TEXT("\r\n"));
		_tcscat(szResult, szLine);
		lenUsed += lstrlen(szLine)+2;
	}
	CloseFile();

	HM.AddPtr(szResult);
	return szResult;
}

void CBReadLinesEx::SeekFile( LONGLONG llToPos )
{

	if (m_iStatus <= 0)  OpenFile();	// ���´��ļ�
	m_buff.llStartPosAbso = llToPos;
	memset(m_buff.bufBytes, 0, sizeof(m_buff.bufBytes));
	m_buff.iBufLen = 0;
	m_buff.iIgnoreFirstLf = 0;
	m_buff.iPtrInBuf = -1;  // ����Ϊ��־��=-1��ʾ�´����� GetNextLine Ҫ���¶�ȡ�µĻ�����
							// �������¶�ȡ����ʹ�õ�ǰ�������� .iPtrInBuf ָ��
	m_buff.iNextLineStaInBuf = 1;

	m_LastBuff.sizeBytsLeft = 0;
	if (m_LastBuff.bytsLeft) {delete []m_LastBuff.bytsLeft; m_LastBuff.bytsLeft=NULL;}
	
	m_bIsEndRead = false;
	m_bOneEndRead = true;
}

float CBReadLinesEx::GetPercent( int idotNum /*= 2*/ )
{
    // dotNum������λС����<0��>7Ϊ������С��
	float fPerc = 0;
	if (m_llFileLength > 0)
	{
		if (m_buff.iPtrInBuf < 0)
			fPerc = (float)(m_buff.llStartPosAbso / m_llFileLength);
		else
			fPerc = (float)(m_buff.llStartPosAbso + m_buff.iPtrInBuf) / m_llFileLength;
	}
	if (idotNum>=0 || idotNum<=7)
	{
		float fWeight = 1;  int i;
		for (i=1; i<=idotNum; i++) fWeight *= 10;
		fPerc = (int)(fPerc * fWeight + 0.5) / fWeight;
	}
    return fPerc;
}


bool CBReadLinesEx::OpenFile(LPCTSTR szFileName /*= NULL*/)
{
    if ( m_hFile != INVALID_HANDLE_VALUE )  CloseFile();   // ����Ѵ����ļ������ȹر���
	
	// ��������˲��� szFileName�������������ļ��� m_szFileName
	if (szFileName) 
		if (lstrlen(szFileName) < sizeof(m_szFileName)/sizeof(TCHAR))
			_tcscpy(m_szFileName, szFileName);

	// ����ļ������ڣ��ͷ��� false
	bool fRet = true;
	TCHAR * p=m_szFileName;  
	if (m_szFileName == 0) 
		fRet = false;	// ���ļ���
	else if (*m_szFileName == 0) 
		fRet = false;	// �ļ����ַ���Ϊ ""
	else
	{
		while (*p) p++; p--;
		if (*p == TEXT('\\') || *p == TEXT(':') ) { fRet=false;	goto finishFindFile; } // �� : �� \\ ��β
		WIN32_FIND_DATA fd;
		HANDLE hr=FindFirstFile(m_szFileName, &fd);
		if (hr==INVALID_HANDLE_VALUE) { fRet=false; goto finishFindFile; }
		FindClose(hr);	// �رղ��Ҿ��
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { fRet=false; goto finishFindFile; } // Ϊ�ļ���
		
		m_hFile = EFOpen(m_szFileName, EF_OpStyle_Input);
		if (m_hFile == INVALID_HANDLE_VALUE )  
		{
			CloseFile();
			fRet=false;
			goto finishFindFile;
		}
	}
finishFindFile:
	if (! fRet)
	{
		if (ShowMsgIfErrRead) 
		{
			if (FailInfoFileNotFound)
				MsgBox(StrAppend(m_szFileName, TEXT("\r\n"), FailInfoFileNotFound), NULL, mb_OK, mb_IconError);
			else
				MsgBox(StrAppend(m_szFileName, TEXT("\r\n"), mcszFileNotFoundInfoDefault), NULL, mb_OK, mb_IconError);
		}
		return false;
	}

	
	Init();								// ��ʼ������
	m_iStatus = 1;						// ��ʾ�ļ��Ѵ�
	m_llFileLength = EFLOF(m_hFile);	// �����ļ��ܴ�С

    // ����Ƿ�Ϊ UTF8 �� Unicode
	char byt3[3];
	if (EFGetBytes(m_hFile, 0, byt3, 3, 0)==3) 
	{
		if ((unsigned char)byt3[0]==0xEF && (unsigned char)byt3[1]==0xBB && (unsigned char)byt3[2]==0xBF)
		{
			AsUTF8 = true;
			m_buff.llStartPosAbso = 3;  // ��ǰ����������ʼ�����ڵ��ļ�λ������ǰ3���ֽ�
		}
		else if ((unsigned char)byt3[0]==0xFF && (unsigned char)byt3[1]==0xFE)
		{
			AsUnicode = true;
			m_buff.llStartPosAbso = 2; // ��ǰ����������ʼ�����ڵ��ļ�λ������ǰ2���ֽ�
		}
	}

	return true;
}

void CBReadLinesEx::CloseFile()
{
    if (m_hFile != INVALID_HANDLE_VALUE) 
	{
		EFClose(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
	m_iStatus = -1; // ��ʾ�ļ��ѹر�
    // ��Init����ֹ��ȡ�к��Զ��ر��ļ�ʱ״̬��������ʼ������OpenFileʱ��Init
}


void CBReadLinesEx::SetReadPtrToStart()
{
    // ���ļ���ȡָ��ָ���ļ���ͷ�����ļ��ѹرգ������´��ļ�
	if (m_hFile == INVALID_HANDLE_VALUE ||  m_iStatus <= 0)
	{
		OpenFile();
	}
	else
	{
		Init();
		m_llFileLength = EFLOF(m_hFile);
	}
}




//////////////////////////////////////////////////////////////////////
// ˽�к���
//////////////////////////////////////////////////////////////////////


LPTSTR CBReadLinesEx::BytsToString( const char * pByts, LONG sizeByts )
{
	// ת����� => m_szOneLine����󷵻� m_szOneLine
	// ÿ�� BytsToString ��ɾ���ϴ�һ���ַ����Ŀռ䣺m_szOneLine
	if (m_szOneLine) { delete []m_szOneLine; m_szOneLine=NULL; }
	
	// �� pByts �е������ӳ�2���ֽ� 0����֤��������� 0 => pByts2
	//��Unicode��Ҫ����0��������Ҫһ��0������ͳһΪ����0��
	int wlen = 0;
	char *pByts2 = new char [sizeByts+2];
	memcpy(pByts2, pByts, sizeByts+2);
	pByts2[sizeByts] = TEXT('\0');
	pByts2[sizeByts+1] = TEXT('\0');

	// ���� pByts2 �е�����
	if (AsUTF8)
	{
		// pByts2 �е������� UTF8 ��ʽ���� pByts2 �е�����ת��Ϊ Unicode ��ʽ
		
		// ��ת��Ϊ Unicode => wszTemp
		// ��ý���ַ�������ռ��С���ַ���λ��=> wlen������ -1 ʹ�����Զ����� pByts2 �ĳ���
		wlen = MultiByteToWideChar(CP_UTF8, 0, pByts2, -1, NULL, 0);
		WCHAR * wszTemp = new WCHAR [wlen];
		MultiByteToWideChar(CP_UTF8, 0, pByts2, -1, wszTemp, wlen);

		// �ٽ� wszTemp �е����ݵ��� pByts2	
		delete []pByts2;
		pByts2 = new char [wlen*sizeof(WCHAR)];
		memcpy(pByts2, wszTemp, wlen*sizeof(WCHAR));

		// ɾ����ʱ�ռ�
		delete []wszTemp; wszTemp = NULL; 
	}

	if (AsUnicode || AsUTF8)
	{
		// pByts2 �е����ݶ��� Unicode ��ʽ
		#if UNICODE
			// Unicode �����ֱ���� pByts2������ => m_szOneLine
			wlen = _tcslen((TCHAR*)pByts2) + 1;
			m_szOneLine = new WCHAR [wlen];
			memcpy(m_szOneLine, pByts2, wlen*sizeof(WCHAR));
		#else
			// ANSI ����� pByts2 ת��Ϊ ANSI������ => m_szOneLine

			// ��ý���ַ�������ռ��С���ֽڵ�λ�������� -1 ʹ�����Զ����� pByts2 �ĳ���
			wlen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pByts2, -1, NULL, 0, NULL, NULL);
			m_szOneLine = new char [wlen];
			WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pByts2, -1, m_szOneLine, wlen, NULL, NULL);  
		#endif
	}
	else 
	{
		// pByts2 �е����ݶ��� ANSI ��ʽ
		#if UNICODE
			// ���� UNICODE ��ĳ��򣬽� pByts2 �е�����ת��Ϊ Unicode �ַ�������=> m_szOneLine
			// ��ý���ַ�������ռ��С���ַ���λ��=> wlen������ -1 ʹ�����Զ����� pByts �ĳ���
			wlen = MultiByteToWideChar(CP_ACP, 0, pByts2, -1, NULL, 0);
			m_szOneLine = new WCHAR [wlen];
			MultiByteToWideChar(CP_ACP, 0, pByts2, -1, m_szOneLine, wlen);
		#else
			// ���� ANSI ��ĳ���pByts2 �е����ݾ��� ANSI��ֱ�Ӵ���=> m_szOneLine
			wlen = _tcslen((TCHAR*)pByts2) + 1;
			m_szOneLine = new TCHAR [wlen];
			memcpy(m_szOneLine, pByts2, wlen*sizeof(TCHAR));
		#endif	
	}

	delete []pByts2; pByts2=NULL;
	
	if (TrimSpaces || TrimControls)
	{
		TCHAR * szOneLineTemp = new TCHAR [lstrlen(m_szOneLine)+1];
		// ɾ��ǰ���ո�
		TCHAR *p = (TCHAR *)m_szOneLine;
		while (*p && 
		   ( (TrimSpaces && (*p==TEXT(' ') || _istspace(*p))) ||
		     (TrimControls && ((*p>=0x01 && *p<=0x08) || (*p>=0x0e && *p<=0x1F) || (*p==0x127) )) 
		   )
		) 	p++;	// ָ��Դ�ַ����ĵ�һ���ǿո�
		_tcscpy(szOneLineTemp, p);	// �� p ��λ�ÿ����ַ���
		
		// ɾ��β���ո�
		p=(TCHAR *)szOneLineTemp;
		while (*p) p++; p--;	// ָ�����һ���ַ�
		while (*p && 
		   ( (TrimSpaces && (*p==TEXT(' ') || _istspace(*p))) ||
		     (TrimControls && ((*p>=0x01 && *p<=0x08) || (*p>=0x0e && *p<=0x1F) || (*p==0x127) )) 
		   )
		) p--;
		p++;
		*p=0;
		
		_tcscpy(m_szOneLine, szOneLineTemp);
		
		delete []szOneLineTemp;
	}

	return m_szOneLine;
}



//////////////////////////////////////////////////////////////////////
// ���ػ���������ֵ�ķ���
//////////////////////////////////////////////////////////////////////

void CBReadLinesEx::FileNameSet( LPCTSTR szFile )
{
	if (m_hFile != INVALID_HANDLE_VALUE) 
		{ EFClose (m_hFile);m_hFile=INVALID_HANDLE_VALUE;}
	
	if (lstrlen(szFile) < sizeof(m_szFileName)/sizeof(TCHAR))
		_tcscpy(m_szFileName, szFile);
    if (AutoOpen) OpenFile();	// �Զ����ļ�
}

LPTSTR CBReadLinesEx::FileName()
{
	return m_szFileName;
}

HANDLE CBReadLinesEx::hFile()
{
	return m_hFile;
}

int CBReadLinesEx::Status()
{
	return m_iStatus;
}

bool CBReadLinesEx::IsEndRead()
{
	return m_bIsEndRead;
}

bool CBReadLinesEx::IsErrOccured()
{
	return m_bErrOccured;
}

