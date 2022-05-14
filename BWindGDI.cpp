//////////////////////////////////////////////////////////////////////////
// BGdi.cpp: CBGdi ���ʵ��
// ʵ�� GDI ��ͼ
//
// Programmed by Zhizhi
// Last Modified: 2020-6-23
//////////////////////////////////////////////////////////////////////////

#include "BWindGDI.h"


CBWindGDI::CBWindGDI( HWND hWnd /* = NULL */ )
{
	// ȫ�帳��ֵ�����򷵻ص����ݿ��ܻܺ���
	// �˴�����ֵ����ô�������û�д��������ص�����Ϊ NULL�� �������
	m_hWnd = NULL;
	m_hDc = NULL;
	m_hDcMem = NULL;
	m_hPen = NULL;
	m_hPenOld = NULL;
	m_hBrush = NULL;
	m_hBrushOld = NULL;
	m_hBMP = NULL;
	m_hBMPOld = NULL;
	m_hFont = NULL;
	m_fShowStatus = false;
	m_fNeedNewFont = true;
	m_uFormatFlag = 0;
	m_hDCBmpSet = NULL;
	m_hBmpSet = NULL;
	m_hBmpSetOld = NULL;

	//�ʵ����õ����ݵĳ�ʼ��
	memset(&m_hFontLog, 0, sizeof(LOGFONT));
	m_hFontLog.lfHeight = 30;						//�����С
	m_hFontLog.lfWeight = 400;						//���岻�Ӵ�
	m_hFontLog.lfItalic = false;					//���岻��б��
	m_hFontLog.lfUnderline = false;					//���岻���»���
	m_hFontLog.lfCharSet = GB2312_CHARSET;			//����ı����ʽ
	_tcscpy(m_hFontLog.lfFaceName, TEXT("����"));	//���������
	m_hFontLog.lfEscapement = 0;					//�����ı���ʾ��ƫ�ƽǶ�

	//�����ı�����ķ�Χ�����ĳ�ʼ��
	m_RectClip.left=0;	m_RectClip.top=0;
	m_RectClip.right=0;	m_RectClip.bottom=0;

	// ����ǿգ���ô���ɺ�������
	if (hWnd != NULL)
		hWndDCWindSet(hWnd);
}

CBWindGDI::~CBWindGDI()
{
	//���û�п������豸�������Զ�������ʾ
	if(!m_fShowStatus)	Show();

	//ɾ���ڴ滷�����ͷ� DC ��Դ
	FreeDc();
}

void CBWindGDI::UpdateDCRange()
{
	//��ɾ��ԭ���Ŀؼ������ݣ�Ȼ���������óɸþ�������»�ö�Ӧ�Ŀؼ��Ĵ�С
	FreeDc();
	hWndDCWindSet(m_hWnd);
}

bool CBWindGDI::hWndDCWindSet( HWND hWnd )
{
	//����һ�����ݵ��ڴ滷�����豸����

	// ����Ѿ��������ڴ滷�����˴�Ӧ���Ȼ����Ѿ����ƹ��Ķ���
	// Ȼ���ͷ�ԭ������Դ��Ȼ�����´���һ���豸����
	if(m_hDc != NULL && m_hWnd != hWnd)
	{
		// ����ʾ���ݣ�Ȼ��ɾ������豸������Ȼ�������½�һ��
		// �ڶ��ν���˺��� m_hDc ���� FreeDc(); �ͱ�����ˣ����Բ�����������֧��
		Show();
		FreeDc();
		hWndDCWindSet(hWnd);
	}

	m_hDc = GetDC(hWnd);	//�����豸����

	// �������ʧ��ֱ�ӷ���ʧ��
	//�ɹ����ģ����� m_hWnd ��ֵ
	if(m_hDc == NULL)	return false;
	else	m_hWnd = hWnd;
	
	//���Ƚ������ݵ��ڴ滷�� hDcMem
	m_hDcMem = CreateCompatibleDC(m_hDc);
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	m_hBMP = CreateCompatibleBitmap(m_hDc, rc.right-rc.left, rc.bottom-rc.top);		//�������ݵ�λͼ��С
	m_hBMPOld = (HBITMAP)SelectObject(m_hDcMem, m_hBMP);							//ѡ���µ�λͼ

	//�˴�Ĭ�ϵ��ı����λ���������ؼ��Ĵ�С�Ŀؼ�����
	m_RectClip.right=rc.right-rc.left;
	m_RectClip.bottom=rc.bottom-rc.top;
	m_RectClip.left=0;
	m_RectClip.top=0;
	
	//���������Լ��Ļ��ʺ�ˢ��
	// Ĭ�ϱ�Ϊ��ɫʵ�ߣ����Ϊ 1 
	// Ĭ��ˢ��Ϊ��ɫ
	m_hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
	m_hBrush = CreateSolidBrush(RGB(255,255,255));
	m_hBrushOld = (HBRUSH)SelectObject(m_hDcMem, m_hBrush);

	return true;
}

HWND CBWindGDI::hWndDCWind()
{
	return m_hWnd;
}

HDC CBWindGDI::hWndCompatibleDC()
{
	return m_hDcMem;
}

HDC CBWindGDI::hWndDC()
{
	return m_hDc;
}

void CBWindGDI::Show()
{
	//���û�д�����ͼ��������ôֱ�ӷ���
	if(m_hDc == NULL)	return;

	//�����Ƶ�ͼ��ȫ�����Ƶ�ָ�����豸������
	RECT rc;
	GetWindowRect(m_hWnd, &rc);		//ͨ���ؼ��ľ����ÿؼ��Ĵ�С

	//Ȼ�󽫿ؼ���С�ķ�Χ�����е����ݸ��ƹ�ȥ
	BitBlt(m_hDc, 0, 0, rc.right-rc.left, rc.bottom-rc.top, m_hDcMem, 0, 0, SRCCOPY);

	m_fShowStatus = true;	//��ʾ�Ѿ�ִ�й�����������
}

void CBWindGDI::FreeDc()
{
	// ����һ�е��ƺ���
	// ������ù���������ô m_hDc ��Ϊ NULL
	// ���򣬲����д˲���
	if (m_hDc != NULL)
	{
		//����Ҫɾ���´�������Դ��ʹ�� m_hDcMem ����Ϊ����֮��������
		SelectObject(m_hDcMem, m_hBMPOld);				//ѡ��ԭ����λͼ����
		DeleteObject(m_hBMP);							//hBMPѡ���ڴ滷������ɾ��
		DeletePenObject();								//ɾ���µıʵ���Դ
		DeleteBrushObject();							//������ǿ�ˢ�ӣ���ôɾ���µ�ˢ����Դ
														//��ˢ����ϵͳ��Դ������ɾ��
		DeleteFontObject();								//ɾ���������

		if(m_hDCBmpSet)			//����Ѿ�����������λͼ���ڴ滷�����ͷ���Դ
		{
			//�����ڲ�λͼ��ÿ�β��������Զ��ͷţ����Բ��ô˴��ͷ���
			DeleteDC(m_hDCBmpSet);
		}

		DeleteDC(m_hDcMem);					//��Ҫ��֤�˴��� m_hDcMem ����Ϊԭ���Ĵ����Ķ���
		ReleaseDC(m_hWnd, m_hDc);			//�ͷ��豸������Դ

		// ��¼�ı���ȫ����գ�ȫΪ NULL
		m_hDc = NULL;	m_hDcMem = NULL;
		m_hBMP = NULL;	m_hBMPOld = NULL;

		//�ؼ���С�����ı�����Ŀؼ�λ��ȫ�����
		m_RectClip.left=0;	m_RectClip.top=0;
		m_RectClip.right=0;	m_RectClip.bottom=0;
		m_uFormatFlag = 0;			//�ı�����ı�־����

		m_fShowStatus = false;		//ע�⣬�ͷ���Դ���б�����Ϊ��ʼ������ֵ
	}
}

void CBWindGDI::DeletePenObject()
{
	//����µı�Ϊ�գ���ôֱ�ӷ���
	if(m_hPen == NULL)	return;

	//ɾ���µĻ��ʣ�����ֻҪ�ı仭����ɫ����Ҫ�ͷ��������
	SelectObject(m_hDcMem, m_hPenOld);	//ѡ��ԭ���Ļ���
	DeleteObject(m_hPen);				// hPen ѡ���ڴ滷������ɾ��
	m_hPen = NULL;						//�ֶ����� m_hPen = NULL;
}

void CBWindGDI::DeleteBrushObject()
{
	//����µ�ˢ�Ӿ��Ϊ�ջ����ǿ�ˢ�ӣ���ôֱ�ӷ���
	if(m_hBrush == NULL)	return;

	if (!BrushNULL())
	{
		//ɾ���µ�ˢ�ӣ�����ֻҪ�ı仭����ɫ����Ҫ�ͷ��������
		SelectObject(m_hDcMem, m_hBrushOld);	//ѡ��ԭ����ˢ��
		DeleteObject(m_hBrush);					// hPen ѡ���ڴ滷������ɾ��
	}
	m_hBrush = NULL;						//�ֶ����� m_hBrush = NULL;
}

COLORREF CBWindGDI::PenColor()
{
	if(m_hPen == NULL)	return 0;

	// ��õ�ǰ�ıʵ���ɫ
	LOGPEN pPen;
	int ret = GetObject(m_hPen, sizeof(LOGPEN), &pPen);

	//����ɹ�������ɫ��ʧ�ܷ��� 0
	return (ret == 0) ? 0 : pPen.lopnColor;
}

void CBWindGDI::PenColorSet( COLORREF cPenColor )
{
	if(m_hPen == NULL)	return;

	int iWidth = PenWidth();	//��õ�ǰ�ıʵĿ��
	int iStyle = PenStyle();	//��õ�ǰ�ıʵ���ʽ
	DeletePenObject();			//ɾ��ԭ���Ĵ����ı�
	
	m_hPen = CreatePen(iStyle, iWidth, cPenColor);
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
}

int CBWindGDI::PenWidth()
{
	if(m_hPen == NULL)	return 0;

	// ��õ�ǰ�ıʵ���ɫ
	LOGPEN pPen;
	int ret = GetObject(m_hPen, sizeof(LOGPEN), &pPen);

	//����ɹ�������ɫ��ʧ�ܷ��� 0
	// pPen.lopnWidth.x ˫ָ�룬��ķ��ˣ���������
	return (ret == 0) ? 0 : pPen.lopnWidth.x;
}

void CBWindGDI::PenWidthSet( int iWidth )
{
	if(m_hPen == NULL)	return;

	//���С��0��ֱ�ӷ���
	if(iWidth <= 0)	return;

	int iStyle = PenStyle();	//��õ�ǰ�ıʵĿ��
	COLORREF iColor = PenColor();	//��õ�ǰ�ıʵ���ʽ
	DeletePenObject();			//ɾ��ԭ���Ĵ����ı�

	m_hPen = CreatePen(iStyle, iWidth, iColor);
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
}

int CBWindGDI::PenStyle()
{
	if(m_hPen == NULL)	return -1;

	// ��õ�ǰ�ıʵ���ɫ
	LOGPEN pPen;
	int ret = GetObject(m_hPen, sizeof(LOGPEN), &pPen);

	//����ɹ�������ɫ��ʧ�ܷ��� 0
	return (ret == 0) ? -1 : int(pPen.lopnStyle);
}

void CBWindGDI::PenStyleSet(int iStyle)
{
	if(m_hPen == NULL)	return;

	//�����ʽС��0�������ڣ�ֱ�ӷ���
	if(iStyle < 0)	return;

	int iWidth = PenWidth();	//��õ�ǰ�ıʵĿ��
	COLORREF iColor = PenColor();	//��õ�ǰ�ıʵ���ʽ
	DeletePenObject();			//ɾ��ԭ���Ĵ����ı�

	m_hPen = CreatePen(iStyle, iWidth, iColor);
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
}

COLORREF CBWindGDI::BrushColor()
{
	if(m_hBrush == NULL)	return 0;

	// ��õ�ǰ��ˢ�ӵ���ɫ
	LOGBRUSH pBrush;
	int ret = GetObject(m_hBrush, sizeof(LOGBRUSH), &pBrush);

	//����ɹ�������ɫ��ʧ�ܷ��� 0
	return (ret == 0) ? 0 : pBrush.lbColor;
}

void CBWindGDI::BrushColorSet( COLORREF cBrushColor )
{
	if(m_hBrush == NULL)	return;

	DeleteBrushObject();			//ɾ��ԭ���Ĵ�����ˢ��
	
	// ������ѡ�����ڵ�ˢ��
	m_hBrush = CreateSolidBrush(cBrushColor);
	m_hBrushOld = (HBRUSH)SelectObject(m_hDcMem, m_hBrush);
}

void CBWindGDI::InvalidateAll()
{
	// ��տؼ������е�����
	// �ܼ򵥵���� 0x0009083e
	InvalidateRect(m_hWnd, NULL, true);
}

void CBWindGDI::FillRectAll()
{
	if(m_hDc == NULL)	return;

	//���������ڴ滷������ϵ�Ŀؼ�ȫ��Ϳ�� m_hBrush ����ɫ
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	//�˴�Ҫת��Ϊ����ڻ������������ϵ
	rc.right = rc.right-rc.left;
	rc.bottom = rc.bottom-rc.top;
	rc.left = 0;
	rc.top = 0;
	//Ϳɫ����
	FillRect(m_hDcMem, &rc, m_hBrush);

}

void CBWindGDI::FillRectArea( int iLeft, int iTop, int iRight, int iBottom )
{
	if(m_hDc == NULL)	return;

	//��䱳��
	//�ڲ�����Χ��������ˢ�ӵ���ɫͿ��
	RECT rc;
	rc.left = iLeft;
	rc.top = iTop;
	rc.right = iRight;
	rc.bottom = iBottom;
	//Ϳɫ����
	FillRect(m_hDcMem, &rc, m_hBrush);
}

bool CBWindGDI::BrushNULL()
{
	if(m_hBrush == NULL)	return false;

	// ��õ�ǰ�ıʵ���ɫ
	LOGBRUSH pBrush;
	int ret = GetObject(m_hBrush, sizeof(LOGPEN), &pBrush);

	//���Ϊ�գ������棬����Ƿǿգ����ؼ�
	return (pBrush.lbStyle == BS_NULL);
}

void CBWindGDI::BrushNullSet( bool fBrushNULL , COLORREF cBrushColor/* = 0xffffff*/)
{
	if(m_hBrush == NULL)	return;

	if (fBrushNULL)
	{
		// �����ʱˢ�Ӳ��ǿգ�������ˢ�ӣ�����ֱ�ӷ���
		if (!BrushNULL())
		{
			DeleteBrushObject();			//ɾ��ԭ���Ĵ�����ˢ��
			// ������ѡ�����ڵ�ˢ��
			m_hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
			m_hBrushOld = (HBRUSH)SelectObject(m_hDcMem, m_hBrush);
		}
	}
	else
	{
		if (BrushNULL())
		{
			//ע��˴��һ���ľ��ǣ����ﲻ��ɾ����ˢ��
			//����Ҫ�Լ�����дһ�´��룬���⴦��
			// ������ѡ�����ڵ�ˢ�ӣ���ɫ
			m_hBrush = CreateSolidBrush(cBrushColor);
			m_hBrushOld = (HBRUSH)SelectObject(m_hDcMem, m_hBrush);
		}
	}
	
}

bool CBWindGDI::PenSet( COLORREF cPenColor, int iWidth, int iStyle )
{
	// ��������Ҫ��Ϊ�˼��ٺ���Ĵ���������һ�������ã�ʡ���м�ö��
	//���ǣ�ʹ�ô˺���������������ȫ���趨�����ȱʡ�о���ʧȥ�˴������������������
	
	if(iWidth<=0 && iStyle<0)	return false;		//�����������������ʧ��
	if(m_hPen == NULL)	return false;
	
	// �������������ɾ��ԭ���Ļ�����Դ�����������µĻ�����Դ
	DeletePenObject();

	m_hPen = CreatePen(iStyle, iWidth, cPenColor);
	m_hPenOld = (HPEN)SelectObject(m_hDcMem, m_hPen);
	return true;
}

void CBWindGDI::BitmapSet( unsigned short idBmp, int xDest/*=0*/,int yDest/*=0*/,int iDestWidth/*=-1*/,int iDestHeight/*=-1*/,bool f_Transparent/*=false*/,COLORREF c_TransColor/*=0x000000*/,double dAngle/*=0*/,bool f_Stretch/*=false*/,int xOrigin/*=0*/,int yOrigin/*=0*/,int iWidthSrc/*=-1*/,int iHeightSrc/*=-1 */ )
{
	// ����һ��λͼ��ʾ��ָ����λ�ã��������趨λͼ�Ĵ�С���Ƿ�͸��
	//��������ˣ����ֱ�����Լ�д�ĸĵĴ�������һ�������Ĵ��룬�Ӹտ�ʼ�ĺܻ��ң��������Ĺ���ȫ����ǿ��̫��������

	//���û�д���λͼ����ô�ʹ���һ��
	if (!m_hDCBmpSet)
		m_hDCBmpSet = CreateCompatibleDC(m_hDc);

	//=========================== ������Դ ===============================
	// �����豸���� hDCBmp�����桰Ҫ���õ�λͼ����ͼ��
	//ע�����������豸�������´���һ���ڴ滷�������Բ����� m_hDc
	m_hBmpSet = LoadBitmap(pApp->hInstance, MAKEINTRESOURCE(idBmp));
	m_hBmpSetOld = (HBITMAP)SelectObject(m_hDCBmpSet, m_hBmpSet);

	//======================== ����Ĭ�ϲ����Ĵ����� ===============================
	//��λͼ��ͼ�����Ƶ��ڴ��豸����
	//ע��˴��ǽ����ڵ��ڴ滷�������ݿ���������ڴ滷�������Բ����� m_hDcMem
	//���λͼ�Ŀ�Ⱥͳ���
	BITMAP pBmp;
	GetObject(m_hBmpSet, sizeof(BITMAP), &pBmp);
	//���λͼ��λ�ã�������Ϸ���ȫ��ת��Ϊ 0��0
	if(xDest<0)	xDest=0;
	if(yDest<0)	yDest=0;
	//ͳһ����������⣬���еĺϷ����Ϸ�ͳһ��������ʾ
	RECT rc;
	GetWindowRect(m_hWnd,&rc);
	iDestWidth = (iDestWidth<0 ? rc.right-rc.left : iDestWidth);
	iDestHeight = (iDestHeight<0 ? rc.bottom-rc.top : iDestHeight);
// 	iDestWidth = (iDestWidth<0 ? pBmp.bmWidth : iDestWidth);
// 	iDestHeight = (iDestHeight<0 ? pBmp.bmHeight : iDestHeight);

	//���ȴ˴��ض���Ҫ����һ����ʱ�� �豸�������������м������
	//���ʹ�÷��������һ�㶼���ܳ�����������������λͼ��ѡ��λͼ������������������

	//============================ ׼������ =========================================
	//���ȴ������ڴ��豸�������������ŵ�ʱ�򴢴����ŵ�ͼƬ����
	HDC hDcStretch = CreateCompatibleDC(m_hDc);
	HBITMAP hBmpStretch = CreateCompatibleBitmap(m_hDc, iDestWidth, iDestHeight);
	HBITMAP hBmpStretchOld = (HBITMAP)SelectObject(hDcStretch, hBmpStretch);
	//�ڶ����������������ת�����ڴ洢��ת��λͼ�Ļ���
	HDC hDcAngle = CreateCompatibleDC(m_hDc);
	HBITMAP hBmpAngle = CreateCompatibleBitmap(m_hDc, iDestWidth, iDestHeight);
	HBITMAP hBmpAngleOld = (HBITMAP)SelectObject(hDcAngle, hBmpAngle);

	//============================ ���к���ͼƬ������ ======================================
	//�ѿ��ˣ��տ�ʼ��Ϊ�����Ǵ�������ڴ滷�������ص�ʹ�þͿ����ˣ���������ÿһ���ڴ滷������ѡ�뱳��ɫ�Ļ���
	//�����ɫ�ر���������Ƿ����ˣ��������´���ˢ��
	HDC hDcTemp = hDcStretch;		//��Ҫ�����м�����ݵ�ת���������Ὠ���µ��豸����
	HBRUSH hBrush = CreateSolidBrush(c_TransColor);
	rc.left=0;				rc.top=0;
	rc.right=iDestWidth;	rc.bottom=iDestHeight;

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ͼƬ���Ŵ��� ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//Ϊ�������������˳���ԣ������˴���ģ�������������Ƚ���λͼ�����ţ�Ȼ�����λͼ����ת��������͸������
	//�������
	FillRect(hDcStretch,&rc,hBrush);
	if(f_Stretch)
	{
		//ע��˴��������ܸı�λͼ�������λ�ã�ȫ��Ϊ0�����һ��ͳһ����λ��
		StretchBlt(hDcStretch, 0, 0,iDestWidth,iDestHeight,
		m_hDCBmpSet, xOrigin,yOrigin,
		(iWidthSrc<0 ? pBmp.bmWidth-xOrigin : iWidthSrc),
		(iHeightSrc<0 ? pBmp.bmHeight-yOrigin: iHeightSrc),
		SRCCOPY);
	}
	else
		BitBlt(hDcStretch, 0, 0, iDestWidth, iDestHeight, m_hDCBmpSet,0,0, SRCCOPY);
	
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ͼƬ��ת���� ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//����ǶȲ�Ϊ�㣬��ô��ִ����ת�ĺ����������ֱ�������˲���
	if(dAngle)
	{
		FillRect(hDcAngle,&rc,hBrush);
		//ע��˴�����������һ�������ܸı�λͼ�������λ�ã�ȫ��Ϊ0�����һ��ͳһ����λ��
		TBRotateAnyAngle(hDcAngle, 0,0, hDcTemp, dAngle,RGB(255,255,255),iDestWidth,iDestHeight);
		hDcTemp = hDcAngle;
	}

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ͼƬ͸������ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	//Ȼ���ж��Ƿ�͸���������������Ѿ�ʹ��һ����ʱ�����������Դ�豸����ͳһ�ˣ����Դ˴�ֱ�ӵ���
	if(f_Transparent)
	{
		TBDrawTransParentBitmap(m_hDcMem,xDest, yDest, hDcTemp, c_TransColor,
			0,0, iDestWidth, iDestHeight);
	}
	else
	{
		//���ͳһ�����洦�����λͼ��ֱ�ӿ������ڴ��У�����ÿһ���ֿ����߼�����
		BitBlt(m_hDcMem, xDest, yDest, iDestWidth, iDestHeight, hDcTemp,0,0, SRCCOPY);
	}

	//======================================= �ͷ���Դ ========================================
	//�ͷ���ʱ�������ݽ������ڴ滷�����˴����þ����ڲ�ͬ�Ĳ���֮��Ľ���
	SelectObject(hDcStretch,hBmpStretchOld);
	DeleteObject(hBmpStretch);
	DeleteDC(hDcStretch);
	SelectObject(hDcAngle,hBmpAngleOld);
	DeleteObject(hBmpAngle);
	DeleteDC(hDcAngle);
	//ɾ��ˢ��
	DeleteObject(hBrush);

	//����ɾ��λͼ�����ڴ滷����������ʹ��
	SelectObject(m_hDCBmpSet, m_hBmpSetOld);
	DeleteObject(m_hBmpSet);
	m_hBmpSet = NULL;	m_hBmpSetOld = NULL;

}

void CBWindGDI::PrintText( LPTSTR szText, int ileft/*=-1*/, int itop/*=-1*/, int iwidth/*=-1*/, int iheight/*=-1*/)
{
	//�����Ҫ���������壬����Ѿ����������壬��ɾ��
	if(m_fNeedNewFont)
	{
		DeleteFontObject();
		m_hFont = CreateFontIndirect(&m_hFontLog); 
		m_fNeedNewFont = false;
	}
	SelectObject(m_hDcMem, m_hFont);	//ѡ�е�ǰ���������壬��Ϊÿ�ζ����Զ�ѡ�������Դ˴���Ҫ�ظ�ѡ��

	SetClip(ileft,itop,iwidth,iheight);		//ִ��һ���������λ�õĺ����ɣ����ӷ���ʹ��

	//��ӡ�ı�����
	DrawText(m_hDcMem, szText, _tcslen(szText), &m_RectClip, m_uFormatFlag);
	
}

void CBWindGDI::DeleteFontObject()
{
	//����ı����÷ǿգ���ôɾ��������ֱ�ӷ���
	//����ÿ�����嶼���Զ��ָ������Բ���Ҫѡ��Ȼ��ɾ��
	if(m_hFont)	
	{
		DeleteObject(m_hFont);				//����������ı��ľ�����˴���Ҫ�ͷ�
		m_hFont = NULL;
	}
}

void CBWindGDI::FontSizeSet( int iFontSize )
{
	//������õ������С�͵�ǰ��ȣ�ֱ�ӷ��أ�����ı����ñ��������ñ�־����Ϊ true
	if (FontSize() == iFontSize)
		return;
	else
	{
		m_hFontLog.lfHeight = iFontSize;
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontNameSet( LPTSTR szFontName )
{
	//������õ��������ƺ͵�ǰ��ȣ�ֱ�ӷ��أ�����ı����ñ��������ñ�־����Ϊ true
	//�ѿ��ˣ�������Ĳ�֪����ôŪ�� , _tcscmp(m_hFontLog.lfFaceName, TEXT("����"))
	tstring name1 = szFontName;
	tstring name2 = m_hFontLog.lfFaceName;
	if (name1 == name2)
	{
		return;
	}
	else
	{
		_tcscpy(m_hFontLog.lfFaceName, szFontName);
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontBondSet( bool fBond )
{
	//������õ��������ƺ͵�ǰ��ȣ�ֱ�ӷ��أ�����ı����ñ��������ñ�־����Ϊ true
	if (FontBond() && fBond)
	{
		//����Ƿ�Ӵֵı�־�����͵�ǰ�Ŀ��ֵ�Ƿ�һ�£����һ�£�ֱ�ӷ���
		return;
	}
	else if (m_hFontLog.lfWeight == FW_NORMAL && !fBond)
	{
		return;
	}
	else
	{
		m_hFontLog.lfWeight = fBond ? FW_BOLD : FW_NORMAL;
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontUnderLineSet( bool fUnderLine )
{
	//�ж����õ������Ƿ�����»��ߣ�����͵�ǰ���趨һ��ֱ�ӷ��أ�����ı����ñ��������ñ�־����Ϊ true
	if (FontUnderLine() == fUnderLine)
	{
		return;
	}
	else
	{
		m_hFontLog.lfUnderline = fUnderLine ? TRUE:FALSE;
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontItalicSet( bool fItalic )
{
	//�ж����õ������Ƿ�б�壬����͵�ǰ���趨һ��ֱ�ӷ��أ�����ı����ñ��������ñ�־����Ϊ true
	if (FontItalic() == fItalic)
	{
		return;
	}
	else
	{
		m_hFontLog.lfItalic = fItalic ? TRUE:FALSE;
		m_fNeedNewFont = true;
	}
}

void CBWindGDI::FontColorSet( COLORREF cColor )
{
	//���õ�ǰ�ı�����ɫ
	if (FontColor() == cColor)
	{
		return;
	}
	else
		SetTextColor(m_hDcMem, cColor);
}

void CBWindGDI::FontBKColorSet( COLORREF cColor )
{
	//���õ�ǰ�ı���������ɫ
	if (FontBKColor() == cColor)
	{
		return;
	}
	else
		SetBkColor(m_hDcMem, cColor);
}

void CBWindGDI::BackTransparentSet( bool fTransparent )
{
	//���õ�ǰ�ı���������ɫ
	if (fTransparent)
		SetBkMode(m_hDcMem, TRANSPARENT);		// ���ֱ���͸��
	else
		SetBkMode(m_hDcMem, OPAQUE);			// ���ֱ�����͸��

}

void CBWindGDI::SetClip( int ileft/*=-1*/, int itop/*=-1*/, int iwidth/*=-1*/, int iheight/*=-1*/ )
{
	//���ô�ӡ�ı��ķ�Χ
	if(ileft>=0)	m_RectClip.left=ileft;		
	if(itop>=0)	m_RectClip.top=itop;
	if(iwidth>=0)	m_RectClip.right=iwidth;	
	if(iheight>=0)	m_RectClip.bottom=iheight;
	
}

void CBWindGDI::AlignSet( UINT uAlign )
{
	//�����־������ͬ����ôֱ�ӷ���
	if (uAlign != 0 && uAlign != 1 && uAlign != 2)
		return;

	m_uFormatFlag = m_uFormatFlag & (~DT_LEFT);
	m_uFormatFlag = m_uFormatFlag & (~DT_CENTER);
	m_uFormatFlag = m_uFormatFlag & (~DT_RIGHT);

	m_uFormatFlag = m_uFormatFlag | uAlign;
}

void CBWindGDI::VAlignSet( UINT uVAlign )
{
	//�����־������ͬ����ôֱ�ӷ���
	if (uVAlign != 0 && uVAlign != 1 && uVAlign != 2)
		return;

	m_uFormatFlag = m_uFormatFlag & (~DT_TOP);
	m_uFormatFlag = m_uFormatFlag & (~DT_VCENTER);
	m_uFormatFlag = m_uFormatFlag & (~DT_BOTTOM);

	m_uFormatFlag = m_uFormatFlag | uVAlign*4;
	//�����λ������Ҫʱ���е�����
	m_uFormatFlag = m_uFormatFlag | DT_SINGLELINE;
	
}

void CBWindGDI::FontSpinDegreeSet( double iDegree )
{
	//���������ǽǶȵ� 10 ��������Ҫ��10
	int iAngle = int(iDegree*10);
	m_hFontLog.lfEscapement = iAngle;
	m_fNeedNewFont = true;
}

void CBWindGDI::SingleLineSet( bool fSingleLine )
{
	//ͨ����־���������ƴ�ӡ�ı��ı�־������
	if (fSingleLine)
		m_uFormatFlag = m_uFormatFlag | DT_SINGLELINE;
	else
		m_uFormatFlag = m_uFormatFlag & (DT_SINGLELINE);
}

void CBWindGDI::Draw3DBorder( int iLeft, int iTop, int iWidth, int iHeight, EdgeType eType/*=eEdgeBump*/, int iFlatMono/*=0*/, UINT iBorderType/*=15*/, bool fDiagonal/*=false*/ )
{
	//����3D�߿�
	//����ȷ�����Ʒ�Χ
	RECT rc;	
	GetWindowRect(m_hWnd, &rc);
	//�������Ϊ���ģ���ôĬ�ϵ��ؼ������±�Ե��������Ĭ�ϵı߿��С
	if (iWidth<0)	
		rc.right = rc.right-rc.left-iLeft;
	else
		rc.right=iLeft+iWidth;
	if (iHeight<0)	
		rc.top = rc.bottom-rc.top-iHeight;
	else
		rc.bottom=iTop+iHeight;

	rc.left=iLeft;			rc.top=iTop;
	//ȷ�����Ƶı߿�λ��
	UINT iFlag = 0;
	if(iBorderType & 1)	iFlag = iFlag | BF_LEFT;
	if(iBorderType & 2)	iFlag = iFlag | BF_TOP;
	if(iBorderType & 4)	iFlag = iFlag | BF_RIGHT;
	if(iBorderType & 8)	iFlag = iFlag | BF_BOTTOM;
	if (iFlatMono == 1)
		iFlag = iFlag | BF_FLAT;
	else if(iFlatMono == 2)
		iFlag = iFlag | BF_MONO;

	if(fDiagonal)	iFlag = iFlag| BF_DIAGONAL;

	DrawEdge(m_hDcMem, &rc, eType, iFlag);
}

void CBWindGDI::DrawCaptionButton( eDCCaptionButton eCaptionButton, eDCButtonState eButtonState/*=eButtonNormal*/, int iLeft/*=0*/, int iTop/*=0*/, int iWidth/*=40*/, int iHeight/*=20*/ )
{
	//���Ʊ������İ�ťԪ��
	//����ȷ�����Ʒ�Χ
	RECT rc;	
	rc.left=iLeft;			rc.top=iTop;
	rc.right=iLeft+iWidth;	rc.bottom=iTop+iHeight;

	DrawFrameControl(m_hDcMem, &rc, DFC_CAPTION, eButtonState | eCaptionButton);
}

void CBWindGDI::DrawButton( eDCButton eButton, eDCButtonState eButtonState/*=eButtonNormal*/, int iLeft/*=0*/, int iTop/*=0*/, int iWidth/*=40*/, int iHeight/*=20*/ )
{
	//���ư�ťԪ��
	//����ȷ�����Ʒ�Χ
	RECT rc;	
	rc.left=iLeft;			rc.top=iTop;
	rc.right=iLeft+iWidth;	rc.bottom=iTop+iHeight;

	DrawFrameControl(m_hDcMem, &rc, DFC_BUTTON, eButtonState | eButton);
}

void CBWindGDI::DrawMenuButton( eDCMenuButton eMenuButton, eDCButtonState eButtonState/*=eButtonStateNormal*/, int iLeft/*=0*/, int iTop/*=0*/, int iWidth/*=40*/, int iHeight/*=20*/ )
{
	//���Ʋ˵���Ԫ��
	//����ȷ�����Ʒ�Χ
	RECT rc;	
	rc.left=iLeft;			rc.top=iTop;
	rc.right=iLeft+iWidth;	rc.bottom=iTop+iHeight;

	DrawFrameControl(m_hDcMem, &rc, DFC_MENU, eButtonState | eMenuButton);
}

void CBWindGDI::DrawScrollButton( eDCScrollButton eScrollButton, eDCButtonState eButtonState/*=eButtonStateNormal*/, int iLeft/*=0*/, int iTop/*=0*/, int iWidth/*=40*/, int iHeight/*=20*/ )
{
	//���ƹ�������Ԫ��
	//����ȷ�����Ʒ�Χ
	RECT rc;	
	rc.left=iLeft;			rc.top=iTop;
	rc.right=iLeft+iWidth;	rc.bottom=iTop+iHeight;

	DrawFrameControl(m_hDcMem, &rc, DFC_SCROLL, eButtonState | eScrollButton);
}

COLORREF CBWindGDI::FontColor()
{
	return GetTextColor(m_hDcMem);
}

COLORREF CBWindGDI::FontBKColor()
{
	return GetBkColor(m_hDcMem);
}

bool CBWindGDI::BackTransparent()
{
	return (GetBkMode(m_hDcMem) == TRANSPARENT);
}

int CBWindGDI::FontSize()
{
	return m_hFontLog.lfHeight;
}

LPTSTR CBWindGDI::FontName()
{
	return m_hFontLog.lfFaceName;
}

bool CBWindGDI::FontBond()
{
	return (m_hFontLog.lfWeight == FW_BOLD);
}

bool CBWindGDI::FontUnderLine()
{
	return (m_hFontLog.lfUnderline == TRUE);
}

bool CBWindGDI::FontItalic()
{
	return (m_hFontLog.lfItalic == TRUE);
}

bool CBWindGDI::SingleLine()
{
	return (m_uFormatFlag & DT_SINGLELINE) ? true : false;
}

int CBWindGDI::FontSpinDegree()
{
	return m_hFontLog.lfEscapement/10;
}

int CBWindGDI::Align()
{
	if (m_uFormatFlag & DT_LEFT)
		return DT_LEFT;
	else if (m_uFormatFlag & DT_CENTER)
		return DT_CENTER;
	else if(m_uFormatFlag & DT_RIGHT)
		return DT_RIGHT;
	else
		return -1;
}

int CBWindGDI::VAlign()
{
	if (m_uFormatFlag & DT_VCENTER)
		return 1;
	else if (m_uFormatFlag & DT_TOP)
		return 0;
	else if(m_uFormatFlag & DT_BOTTOM)
		return 2;
	else
		return -1;
}


//////////////////////////////////////////////////////////////////////////
// BWindGDIPlus ���ʵ��
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ���ڱ������ڵ����ƣ��˴����������涨���Ӧ�Ļ��ʵ����ݰ�
//////////////////////////////////////////////////////////////////////////

CBWindGdiPlus::CBWindGdiPlus()
{
	InitialGdiPlus();

	//��ʼ����Ӧ�ı�������
	m_hWnd = NULL;
	m_pPen = new Pen(Color::Black, 2);
	m_pBrush = new SolidBrush(Color::White);
	m_pLinearBrush = NULL;
	m_pGraphics = NULL;				//�˴�û�ж�Ӧ�ľ�������ܴ���graphics����
	m_pImage = NULL;
	m_pBmpInstruments = NULL;
	m_pGraphicInstruments = NULL;

	//�������Ĳ�����ʼ��
	m_iFontStyle = 0;
	m_fFontChange = true;	//�������ú������Զ��ı�˲������Ӷ������Ƿ���Ƶ�ʱ�򴴽��µ��������
	m_pFont = NULL;
	m_pFontName = NULL;
	m_sFontName = TEXT("΢���ź�");
	m_iFontSize = 10;
}

CBWindGdiPlus::CBWindGdiPlus(HWND hWnd /*= NULL*/)
{
	CBWindGdiPlus();

	m_hWnd = hWnd;
}

CBWindGdiPlus::~CBWindGdiPlus()
{
	//ɾ����̬�����Ķ�������
	if (m_pPen)			delete m_pPen;
	if (m_pBrush)		delete m_pBrush;
	if (m_pGraphics)	delete m_pGraphics;
	if (m_pLinearBrush)	delete m_pLinearBrush;
	if (m_pImage)		delete m_pImage;
	if (m_pFont)		delete m_pFont;
	if (m_pFontName)	delete m_pFontName;
	if (m_pBmpInstruments)	delete m_pBmpInstruments;
	if (m_pGraphicInstruments)	delete m_pGraphicInstruments;

	UnInitialGdiplus();
}

void CBWindGdiPlus::hWindowSet(HWND hWnd)
{
	if (hWnd == m_hWnd)		//������ֱ�ӷ���
		return;

	//����Ѿ����������ݣ���ôɾ��ԭ�������ݣ����´���
	if (m_pGraphics)
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
	}
	//���´���һ���µĻ�ͼ����
	m_pGraphics = new Graphics(hWnd);
	m_hWnd = hWnd;			//���´��ھ��

}

HWND CBWindGdiPlus::hWindow()
{
	return m_hWnd;
}

Pen* CBWindGdiPlus::GetPen()
{
	return m_pPen;
}

SolidBrush* CBWindGdiPlus::GetBrush()
{
	return m_pBrush;
}

LinearGradientBrush* CBWindGdiPlus::GetLinearBrush()
{
	//����������Զ������󷵻ض�Ӧ������
	if (!m_pLinearBrush)
	{
		m_pLinearBrush = new LinearGradientBrush(Point(300, 50), Point(500, 150),
			Color(255, 0, 255, 0), Color(0, 0, 255, 0));
	}
	return m_pLinearBrush;
}

Graphics* CBWindGdiPlus::GetGraphics()
{
	return m_pGraphics;
}

Image* CBWindGdiPlus::CreateImage(LPTSTR sImagePath)
{
	//����������Զ������󷵻ض�Ӧ������
	if (!m_pImage)
	{
		m_pImage = new Image(sImagePath);
	}
	else
	{
		delete m_pImage;
		m_pImage = NULL;
		m_pImage = new Image(sImagePath);
	}
	return m_pImage;
}

Image* CBWindGdiPlus::GetImage()
{
	return m_pImage;
}

void CBWindGdiPlus::FontSizeSet(int iSize /*= 10*/)
{
	//�ֺű���Ϸ�
	if (iSize > 0)
		m_iFontSize = iSize;
	m_fFontChange = true;		//��ʾ�����ʽ�����˸ı�
}

int CBWindGdiPlus::FontSize()
{
	return m_iFontSize;
}

void CBWindGdiPlus::FontNameSet(LPCTSTR sFontName /*= TEXT("΢���ź�")*/)
{
	//�������Ϊ��Ĳ���
	if (sFontName)
		m_sFontName = sFontName;
	m_fFontChange = true;		//��ʾ�����ʽ�����˸ı�
}

tstring CBWindGdiPlus::FontName()
{
	return m_sFontName;
}

void CBWindGdiPlus::FontUnderLineSet(bool fIsUnderline /*= false*/)
{
	fIsUnderline ? m_iFontStyle &= 4 : m_iFontStyle &= ~4;
	m_fFontChange = true;		//��ʾ�����ʽ�����˸ı�
}

bool CBWindGdiPlus::FontUnderLine()
{
	return bool(m_iFontStyle & 4);
}

void CBWindGdiPlus::FontBoldSet(bool fIsBold /*= false*/)
{
	fIsBold ? m_iFontStyle &= 1 : m_iFontStyle &= ~1;
	m_fFontChange = true;		//��ʾ�����ʽ�����˸ı�
}

bool CBWindGdiPlus::FontBold()
{
	return m_iFontStyle & 1;
}

void CBWindGdiPlus::FontItalicSet(bool fIsItalic /*= false*/)
{
	fIsItalic ? m_iFontStyle &= 2 : m_iFontStyle &= ~2;
	m_fFontChange = true;		//��ʾ�����ʽ�����˸ı�
}

bool CBWindGdiPlus::FontItalic()
{
	return bool(m_iFontStyle & 2);
}

FontFamily* CBWindGdiPlus::GetFontName()
{
	//���Ϊ�գ��Զ�����Ĭ�ϵ�����
	CreateMyFont();

	return m_pFontName;
}

Font* CBWindGdiPlus::GetFont()
{
	//���Ϊ�գ��Զ�����Ĭ�ϵ�����
	CreateMyFont();

	return m_pFont;
}

bool CBWindGdiPlus::DrawString(LPCTSTR sText, float x, float y)
{
	//����˴��豸���������ڣ��Զ�����ʧ��
	if (!m_pGraphics)
		return false;
	CreateMyFont();		//ÿ�ζ�Ҫ�����ж��Ƿ�Ҫ������Ӧ��������

	PointF pPosToShow(x, y);
	m_pGraphics->DrawString(sText, -1, m_pFont, pPosToShow, m_pBrush);

	return true;
}

Bitmap* CBWindGdiPlus::DrawAttitudeInstrument(float fPitchAngle, float fRollAngle, float fYawAngle)
{
	//�Ƕ�ֵԤ����
	if (fPitchAngle > 90)	fPitchAngle = 90;
	if (fPitchAngle < -90)	fPitchAngle = -90;
	if (fRollAngle > 90)	fRollAngle = 90;
	if (fPitchAngle < -90)	fRollAngle = -90;

	const int iWidth = 500;
	const int iHeight = 300;
	Point center(int(0.5 * iWidth), int(0.5 * iHeight));

	//��ͼ��������
	int PitchScaleWidth = 15;		//��ͼ���
	int PitchScaleHeight = 30;		//����߶�
	double PitchRange = 40;
	int PitchTicksCount = 10;		//����ǻ��Ƶ�����������
	int i;
	int iStart = int((fPitchAngle / 5) * PitchScaleHeight);	//��ʼ��Ҫ�����ĸ߶�

	//�������豸����һ�𴴽���ͬ������
	if (m_pBmpInstruments)
	{
		//ֻ�е�����ͬ��ʱ�����´���
		if (m_pBmpInstruments->GetWidth()!=500 || m_pBmpInstruments->GetHeight() != 300)
		{
			delete m_pBmpInstruments;
			m_pBmpInstruments = NULL;
			m_pBmpInstruments = new Bitmap(500, 300);
			if (m_pGraphicInstruments)	delete m_pGraphicInstruments;
			m_pGraphicInstruments = new Graphics(m_pBmpInstruments);
		}
	}
	else
	{
		m_pBmpInstruments = new Bitmap(500, 300);
		if (m_pGraphicInstruments)	delete m_pGraphicInstruments;
		m_pGraphicInstruments = new Graphics(m_pBmpInstruments);
	}

	m_pGraphicInstruments->ResetTransform();		//����������
	m_pGraphicInstruments->TranslateTransform((REAL)center.X, (REAL)center.Y);	//ƽ�Ʊ任,������ԭ��ƽ�����ͻ�������
	m_pGraphicInstruments->RotateTransform((REAL)-fRollAngle);					//��ת�任,���������Ϊ����,ʵ�ָ����̶ȴ��ͺ���α�������ԭ����ת

	//================================ ���Ʊ���ͼ ====================================
	m_pBrush->SetColor(Color::Blue);
	m_pGraphicInstruments->FillRectangle(m_pBrush,
		int(-0.5 * iWidth * sqrt(2.0)), iStart + int(-3 * iHeight * sqrt(2.0)),
		int(4 * iWidth * sqrt(2.0)), int(3 * iWidth * sqrt(2.0)));

	m_pBrush->SetColor(Color::Brown);
	m_pGraphicInstruments->FillRectangle(m_pBrush,
		int(-0.5 * iWidth * sqrt(2.0)), iStart,
		int(4 * iWidth * sqrt(2.0)), int(3 * iWidth * sqrt(2.0)));

	//================================= ���Ƹ����� =====================================
	//���������
	m_pPen->SetWidth(2);
	m_pBrush->SetColor(Color::Black);
	FontSizeSet(10);
	FontNameSet(TEXT("����"));
	CreateMyFont();
	PointF pointF(30, 0);

	for (i = -(PitchTicksCount / 2) + int(fPitchAngle / 5); i <= 0.5 * PitchTicksCount + int(fPitchAngle / 5); i++)
	{
		//����������ʾ��ֵ
		pointF.Y = -(REAL)PitchScaleHeight * i + iStart;
		m_pGraphicInstruments->DrawString(Str(i * 5), -1, m_pFont, pointF, m_pBrush);
		//�о�����ֱ�߲������⣬����Ҫ���Ǳ�ע
		if (i % 2 == 0)
			m_pGraphicInstruments->DrawLine(m_pPen,
				-25, -i * PitchScaleHeight + iStart,
				25, -i * PitchScaleHeight + iStart);
		else
			m_pGraphicInstruments->DrawLine(m_pPen,
				-15, -i * PitchScaleHeight + iStart,
				15, -i * PitchScaleHeight + iStart);
	}

	//=============================== ����ָʾ��ͷ =======================================
	Point pArrowShow[] = { Point(-5, -125), Point(5, -125), Point(0, -145) };
	m_pBrush->SetColor(Color::Yellow);
	m_pGraphicInstruments->FillClosedCurve(m_pBrush, pArrowShow, 3);
	m_pGraphicInstruments->DrawClosedCurve(m_pPen, pArrowShow, 3);

	//============================= Ȼ����ƶ�Ӧ�Ĺ�ת�ǵı�� ===============================
	double dAngleDu = -50;
	double dAngelReal = dAngleDu * 3.14 / 180;;
	int iRadiusIn(140), iRadiusOut(160);
	m_pGraphicInstruments->ResetTransform(); //����������
	m_pGraphicInstruments->TranslateTransform((REAL)center.X, (REAL)center.Y);	//ƽ�Ʊ任,������ԭ��ƽ�����ͻ�������
	//buffer.RotateTransform((REAL)-2*dPitchAngle);
	m_pBrush->SetColor(Color::White);
	for (i = 0; i < 11; i++)
	{
		m_pGraphicInstruments->DrawLine(m_pPen,
			int(iRadiusIn * sin(dAngelReal)), int(-iRadiusIn * cos(dAngelReal)),
			int(iRadiusOut * sin(dAngelReal)), int(-iRadiusOut * cos(dAngelReal)));
		pointF.X = (REAL)(iRadiusIn * sin(dAngelReal) - 10);
		pointF.Y = (REAL)(-iRadiusIn * cos(dAngelReal) + 6);
		m_pGraphicInstruments->DrawString(StrAppend(Str(dAngleDu), TEXT("��")), -1, m_pFont, pointF, m_pBrush);
		dAngleDu += 10;
		dAngelReal = dAngleDu * 3.14 / 180;
	}
	//90���Ҳ��һ��
	m_pGraphicInstruments->DrawLine(m_pPen,
		int(iRadiusIn), 0,
		int(iRadiusOut), 0);
	pointF.X = (REAL)(iRadiusIn - 5);
	pointF.Y = (REAL)(6);
	m_pGraphicInstruments->DrawString(TEXT("90��"), -1, m_pFont, pointF, m_pBrush);
	m_pGraphicInstruments->DrawLine(m_pPen,
		-int(iRadiusIn), 0,
		-int(iRadiusOut), 0);
	pointF.X = (REAL)(-iRadiusIn - 15);
	pointF.Y = (REAL)(6);
	m_pGraphicInstruments->DrawString(TEXT("-90��"), -1, m_pFont, pointF, m_pBrush);

	//================================ �������ĵ�ָʾ��־ =====================================
	//buffer.ResetTransform();					//����������
	m_pPen->SetWidth(5);
	m_pGraphicInstruments->DrawLine(m_pPen, -70, 0, -20, 0);
	m_pGraphicInstruments->DrawLine(m_pPen, -20, -2, -20, 10);
	m_pGraphicInstruments->DrawLine(m_pPen, 20, 0, 70, 0);
	m_pGraphicInstruments->DrawLine(m_pPen, 20, -2, 20, 10);
	m_pGraphicInstruments->DrawEllipse(m_pPen, 0, 0, 2, 2);

	//============================== ����ٶȱ�ʶ���Ҳ�ĸ߶ȱ�ʶ ============================


	//���� ADI���ƺ���,�����ͻ������������ͼ�ζ�����Ϊ����
	m_pGraphicInstruments->SetSmoothingMode(SmoothingModeHighQuality);
	//����ͼ�κ��ı�������ģʽ
	m_pGraphicInstruments->SetTextRenderingHint(TextRenderingHintAntiAlias);

	return m_pBmpInstruments;

}

void CBWindGdiPlus::InitialGdiPlus()
{
	if (!m_gdiplusToken)
	{
		GdiplusStartupInput m_gdiplusStartupInput;
		//��ʼ��gdi+
		GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	}
}

void CBWindGdiPlus::UnInitialGdiplus()
{
	//�ͷ�gdi+
	GdiplusShutdown(m_gdiplusToken);
}

void CBWindGdiPlus::CreateMyFont()
{
	// 	//����������Ϊ�棬�ж϶�Ӧ���������������Ƿ�����е�һ�£���һ�����´���
	// 	if (m_pFontName)
	// 	{
	// 		TCHAR sFontName[64] = { 0 };
	// 		m_pFontName->GetFamilyName(sFontName);
	// 		//������߲�ͬ�����´��� FontFamily
	// 		if (_tcscmp(sFontName, m_sFontName.c_str()))
	// 		{
	// 			delete m_pFontName;
	// 			m_pFontName = NULL;
	// 			m_pFontName = new FontFamily(m_sFontName.c_str());
	// 		}
	// 	}
	// 	else
	// 		m_pFontName = new FontFamily(m_sFontName.c_str());
	// 
	// 	//ɾ���ߵ����ã���Ҫʱ������Ӱ�
	// 	// 	FontStyleStrikeout = 8
	// 	if (m_pFont)
	// 	{
	// 		//�˴��������ķ�������Ĵ�С�����ϣ�ɾ��ԭ����Ȼ�����´���
	// 		if (m_pFont->GetSize() != m_iFontSize
	// 			|| m_pFont->GetStyle() != m_iFontStyle)
	// 		{
	// 			delete m_pFont;
	// 			m_pFont = NULL;
	// 			m_pFont = new Font(m_pFontName, (REAL)m_iFontSize, m_iFontStyle);
	// 		}
	// 	}
	// 	else
	// 		m_pFont = new Font(m_pFontName, m_iFontSize, m_iFontStyle);

		//������������Ҳ���û�ı�Ļ���ֱ�ӷ���
	if (!m_fFontChange)	return;

	//�����ԭ�������ݣ�Ȼ�󴴽��µ�����
	if (m_pFontName)
		delete m_pFontName;
	if (m_pFont)
		delete m_pFont;
	//�����µ��������
	m_pFontName = new FontFamily(m_sFontName.c_str());
	m_pFont = new Font(m_pFontName, (REAL)m_iFontSize, m_iFontStyle);

	m_fFontChange = false;
}

ULONG_PTR CBWindGdiPlus::m_gdiplusToken = NULL;




