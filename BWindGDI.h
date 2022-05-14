//////////////////////////////////////////////////////////////////////////
// CBWindGDI.h: CBWindGDI ��Ķ���
// ʵ�� GDI ��ͼ
//
// Programmed by Zhizhi
// Last Modified: 2020-6-23
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "BForm.h"
#include "mdlTransBMP.h"	//Ϊ���ܹ�ʵ�����λͼ��ʱ���ܹ��Զ���λͼ����͸������
//==================================================================================
#include <GdiPlus.h>
#pragma comment(lib, "GdiPlus.lib")
using namespace Gdiplus;

//����һ��ö�������������ر߿��Ч��������Ч���߿��������
//һ�������������ʽ����Ƕ��ʴ�̣�̧�𣬰���
enum EdgeType
{
	eEdgeBump = (BDR_RAISEDOUTER | BDR_SUNKENINNER),		//��Ƕ��ʽ
	eEdgeEtched = (BDR_SUNKENOUTER | BDR_RAISEDINNER),		//ʴ����ʽ
	eEdgeRaised = (BDR_RAISEDOUTER | BDR_RAISEDINNER),		//̧����ʽ
	eEdgeSunken = (BDR_SUNKENOUTER | BDR_SUNKENINNER)		//������ʽ
};

//����һ�����ڿ��Ʊ�������ť��ö��
//���а����İ�ť��ʽ���رգ���󻯣���С�������»�ԭ������
enum eDCCaptionButton
{
	eCaptionClose = DFCS_CAPTIONCLOSE,		//�ر�
	eCaptionMin = DFCS_CAPTIONMIN,			//��С��
	eCaptionMax = DFCS_CAPTIONMAX,			//���
	eCaptionRestore = DFCS_CAPTIONRESTORE,	//���»�ԭ
	eCaptionHelp = DFCS_CAPTIONHELP			//����
};

//�������ڿ��Ƶ�ǰ��Ԫ�ص�״̬��ö��
enum eDCButtonState
{
	eButtonStateNormal = 0,						//Ĭ�ϸ�ʽ
	eButtonStateChecked = DFCS_CHECKED,			//��ѡ��ѡ��
	eButtonStateFlat = DFCS_FLAT,				//��ť����ƽ��߿�Ч��
	eButtonStateHot = DFCS_HOT,					//������ƹ�ʱ��ť���ȸ���Ч��
	eButtonStateInactive = DFCS_INACTIVE,		//��ť�����ã� ��ɫ ��
	eButtonStateMono = DFCS_MONO,				//��ť���е�ɫ�߿�
	eButtonStatePushed = DFCS_PUSHED,			//��ť������ʱ��Ч��
	eButtonStateTransparent = DFCS_TRANSPARENT	//��ť�ı���͸��
};

//�������ڻ��Ʊ�׼�İ�ť������
//��׼��ť������ͨ�� CommandButton ��ʽ�İ�ť��Ҳ �����Ǹ�ѡ�򡢵�ѡ��
enum eDCButton
{
	eButton3State = DFCS_BUTTON3STATE,			//3 ̬��ť
	eButtonCheck = DFCS_BUTTONCHECK,			//��ѡ��
	eButtonPush = DFCS_BUTTONPUSH,				//��ͨ����ʽ��ť
	eButtonRadio = DFCS_BUTTONRADIO,			//��ѡ��
	eButtonRadioImage = DFCS_BUTTONRADIOIMAGE,	//��ѡ���ͼƬ
	eButtonRadioMask = DFCS_BUTTONRADIOMASK		//��ѡ������Σ� Mask ) ͼƬ
};

//�������ڻ��Ʋ˵���Ԫ������
//����������Ҽ�ͷ�����ͷ���˵�Բ�㡢�Թ�
enum eDCMenuButton
{
	eMenuArrow = DFCS_MENUARROW,			//ָʾ�����Ӳ˵����Ҽ�ͷ
	eMenuArrowRight = DFCS_MENUARROWRIGHT,	//ָʾ�����Ӳ˵�������ļ�ͷ
	eMenuBullet = DFCS_MENUBULLET,			//ָʾ��ѡ��ǵĲ˵�Բ��
	eMenuCheck = DFCS_MENUCHECK				//ָʾ��ѡ��ǵĲ˵� ���Թ���
};

//�������ڻ��ƹ�����Ԫ�ص�����
enum eDCScrollButton
{
	eScrollLCombobox = DFCS_SCROLLCOMBOBOX,				//�����б�� ComboBox ) �����¼�ͷ
	eScrollLDown = DFCS_SCROLLDOWN,						//���������¼�ͷ��ť
	eScrollLLeft = DFCS_SCROLLLEFT,						//�����������ͷ��ť
	eScrollLRight = DFCS_SCROLLRIGHT,					//�L�������Ҽ�ͷ��ť
	eScrollLSizeGrip = DFCS_SCROLLSIZEGRIP,				//λ�ڴ������½ǵ� ��б�ơ�
	eScrollLSizeGripRight = DFCS_SCROLLSIZEGRIPRIGHT,	//λ�ڴ������½ǵķ����� ��б�ơ�
	eScrollLUp = DFCS_SCROLLUP							//�L�������ϼ�ͷ��ť
};

//����ṹ�����ڱ���ͼ���������ε���Ϣ
struct HotRectType 
{
	int Left;					//����������߽�
	int Top;					//���������ϱ߽�
	int Right;					//���������ұ߽�
	int Bottom;					//���������±߽�
	long int RectData;			//�������θ������ݣ���������
	tstring RectString;			//�������εĸ������ݣ��ַ���������Ҫʱ��ʹ��
	long int ReservedData;		//�������ε������������ݣ�������������Ҫʱ��ʹ��
	double ReservedDbl;			//�������ε������������ݣ�˫������������Ҫʱ��ʹ��
	tstring ReservedString;		//�������ε������������ݣ��ַ���������Ҫʱ��ʹ��
};

//////////////////////////////////////////////////////////////////////////
// CBWindGDI ���ʵ��

class CBWindGDI
{
public:
	// =======================================================================
	// �๫�г�Ա�ͳ�Ա����
	//========================================================================

	// ���캯��
	// ���캯�������ֱ��д����Ӧ�ؼ��ľ��
	// ��ôֱ�Ӵ����ڴ滷�����˴�Ĭ�Ͼ������ڴ滷��
	CBWindGDI(HWND hWnd = NULL);

	// ��������
	~CBWindGDI();

	//�����豸�����Ĵ�С
	//����ؼ��Ĵ�С�ı䣬���������Զ��ͷ�֮ǰ���豸���������´���һ����С�Ϳؼ�������豸����
	void UpdateDCRange();

	//���úͲ鿴�ڴ滷���Ĺ����Ŀؼ����ߴ���
	//����û���ʱ�Ѿ�����һ��������ٴ����������ľ���������������Զ���ԭ���ľ�����Ƶ�ͼ��ȫ����ʾ
	//Ȼ���ͷ���Դ������ѡ������µľ����Ϊ�豸���������Ŀؼ��ľ��
	// ����ʵ����һ���������ǰ��󶨶���豸��������������ʹ�õ������
	bool hWndDCWindSet(HWND hWnd);
	HWND hWndDCWind();

	//Ϊ��������Ŀ���չ�ԣ������ṩ�������ڴ滷���ľ���Ľӿ�
	//����ڴ滷���ľ��
	HDC hWndCompatibleDC();
	//����豸�����ľ��
	HDC hWndDC();

	// ���������ݴ��ڴ��豸�����п����� DC ������
	// Ȼ��һ����ʾ������������û�����ִ�д˺����������������Զ�ִ�У�
	void Show();

	//////////////////////////////////////////////////////////////////////////
	// ͳһ���ñʵĸ������ԣ�һ��������ȫ��
	// �ʵģ���ɫ����ȡ���ʽ���˺���������������ȫ���趨������ȱʡ
	// 0 -- ʵ�ߣ���ȴ���1ʱ����ͼ���ڡ���ֱ�����������һ���֣�
	// 1 -- ����
	// 2 -- ������
	// 3 -- �㻮��
	// 4 -- ��㻮��
	// 5 -- ������
	// 6 -- ʵ�ߣ���ȴ���1ʱ����ͼ������չ��
	bool PenSet(COLORREF cPenColor, int iWidth, int iStyle);

	// ���غ����õ�ǰ�ʵ���ɫ
	COLORREF PenColor();
	void PenColorSet(COLORREF cPenColor);

	// ���غ����õ�ǰ�ʵĿ��
	int PenWidth();
	void PenWidthSet(int iWidth);

	// ���غ����õ�ǰ�ʵ���ʽ
	// 0 -- ʵ�ߣ���ȴ���1ʱ����ͼ���ڡ���ֱ�����������һ���֣�
	// 1 -- ����
	// 2 -- ������
	// 3 -- �㻮��
	// 4 -- ��㻮��
	// 5 -- ������
	// 6 -- ʵ�ߣ���ȴ���1ʱ����ͼ������չ��
	int PenStyle();
	void PenStyleSet(int iStyle);

	//////////////////////////////////////////////////////////////////////////
	// ���غ����õ�ǰˢ�ӵ���ɫ
	COLORREF BrushColor();
	void BrushColorSet(COLORREF cBrushColor);

	// ���غ�����ˢ���Ƿ��ǿ�ˢ��
	// ��� fBrushNULL Ϊ false����һ����������ֱ������ˢ�ӵ���ɫ��Ĭ���ǰ�ɫ��ˢ��
	//�����һ������Ϊ true����ô�ڶ��������Զ�����
	bool BrushNULL();
	void BrushNullSet(bool fBrushNULL, COLORREF cBrushColor = 0xffffff);

	//////////////////////////////////////////////////////////////////////////
	// ����һ��λͼ��ʾ��ָ����λ�ã��������趨λͼ�Ĵ�С���Ƿ�͸�����о���д�Ĳ����ӵĵ���������
	// ���ǣ�λͼ���ú󲻿���ɾ����ֻ��ͨ�� InvalidateAll() ������������ſ���
	// idBmp λͼ��ID
	// xDest, yDest �ֱ�Ϊλͼ���õ�λ��
	// iWidth, iHeight �ֱ�Ϊλͼ����λ�õĿ�Ⱥ͸߶ȣ������ -1 ��ʾ���豸���������Ҳ����²�
	// f_Transparent Ϊ�Ƿ�͸����������� false���Զ��������Ĳ������������Ĳ�����Ҫ͸������ɫ
	//f_Stretch ���Ƿ�������Ŵ������Ϊtrue����Ҫ����������������Ϊ���յĿ��������վ�������Ŀ�͸� 
	// xOrigin, yOrigin �ֱ�Ϊԭʼλͼ�Ĳü���λ�ã�Ĭ�ϴ� 0 ��ʼ
	//iWidthSrc, iHeightSrc �ֱ�ΪԴλͼ�ü��Ŀ�Ⱥ͸߶�, -1 �����������µ�λ��
	void BitmapSet(unsigned short idBmp, 
		int xDest=0,int yDest=0,int iDestWidth=-1,int iDestHeight=-1,			// x, y �ֱ�Ϊλͼ���õ�λ�ã�iWidth, iHeight �ֱ�Ϊλͼ����λ�õĿ�Ⱥ͸߶�
		bool f_Transparent=false,										// f_Transparent Ϊ�Ƿ�͸������
		COLORREF c_TransColor=0x000000,									//����� false���Զ��������Ĳ������������Ĳ�����Ҫ͸������ɫ
		double dAngle=0,												//ͼƬ����ת�Ƕȣ����ӹ������ͨ����	
		bool f_Stretch=false,											//f_Stretch ���Ƿ�������Ŵ���
		int xOrigin=0,int yOrigin=0,int iWidthSrc=-1,int iHeightSrc=-1	//���ŵ�ԭʼͼƬ����ʼ�ص��x��y�����Լ���Ⱥ͸߶�
		);
	
	//////////////////////////////////////////////////////////////////////////
	//�򴰿�ָ�������ӡ����
	//szText ����ӡ�ı�����
	void PrintText(LPTSTR szText, int ileft=-1, int itop=-1, int iwidth=-1, int iheight=-1);

	//�����Ƿ�Ϊ������ʾ�ı�����
	void SingleLineSet(bool fSingleLine);
	bool SingleLine();

	//�����ı������ʱ�����ת�Ƕ�
	void FontSpinDegreeSet(double iDegree);
	int FontSpinDegree();

	//�����ı��ĺ�����뷽ʽ
	//0 = ����� 1 = �м���� 2 = �Ҷ���
	void AlignSet(UINT uAlign);
	int Align();

	//�����ı���������뷽ʽ
	//ע��˴���Ҫ�ǵ���ģʽ�����������Զ����������óɵ���ģʽ
	//0 = �������� 1 = �м���� 2 = �ײ�����
	void VAlignSet(UINT uVAlign);
	int VAlign();

	//���ý�Ҫ��ӡ�ı��ķ�Χ
	void SetClip(int ileft=-1, int itop=-1, int iwidth=-1, int iheight=-1);

	//��ú������ı�����ɫ
	void FontColorSet(COLORREF cColor);
	COLORREF FontColor();

	//��ú������ı��ı���ɫ
	//ֻ�е��ı���͸��ʱ����Ч
	void FontBKColorSet(COLORREF cColor);
	COLORREF FontBKColor();

	//��ú������ı������Ƿ�͸��
	void BackTransparentSet(bool fTransparent);
	bool BackTransparent();

	//���غ���������������С
	void FontSizeSet(int iFontSize);
	int FontSize();

	//�������������
	void FontNameSet(LPTSTR szFontName);
	LPTSTR FontName();

	//���������Ƿ���Ҫ�Ӵ�
	void FontBondSet(bool fBond);
	bool FontBond();

	//���������Ƿ���Ҫ���»���
	void FontUnderLineSet(bool fUnderLine);
	bool FontUnderLine();

	//���������Ƿ�Ϊб��
	void FontItalicSet(bool fItalic);
	bool FontItalic();

	//////////////////////////////////////////////////////////////////////////
	//���� 3D Ч���ı߿�
	//iLeft,iTop,iWidth,iHeight �ֱ��ʾҪ���Ƶı߿��λ��
	//eEdgeBump ��ʾҪ���Ƶı߿����ʽ��eEdgeBump = ��Ƕ��eEdgeEtched = ʴ�̣�eEdgeRaised = ̧��eEdgeSunken = ����
	//iFlatMono��0=��ʾ���ǻ���3D�߿� 1=��ʾ����ƽ��Ч�� 2=��ʾ���ǵ�ɫЧ��
	//iBorderType ��ʾ���ǻ��Ƶı߿�����ݣ�1��2��4��8�ֱ��ʾ�����ϡ��ҡ��±߿򣬿����໥��ӻ��������Ҫ���Ƶı߿�����
	//			Ĭ�ϵ��� 15����ȫ������
	//fDiagonal ��ʾ�Ƿ���ƶԽ��ߣ�Ĭ���ǻ��Ƴ������ϵĶԽ��ߣ����iBorderTypeΪ12�����Ƶ��ǳ������µĶԽ���
	void Draw3DBorder(int iLeft=0, int iTop=0, int iWidth=-1, int iHeight=-1, EdgeType eType=eEdgeBump, int iFlatMono=0, UINT iBorderType=15, bool fDiagonal=false);

	//////////////////////////////////////////////////////////////////////////
	//���� Windows ��ťԪ��

	//���Ʊ������İ�ťԪ��
	//eCaptionButton ��������Ҫ���Ƶ������ֵİ�ť
	//eButtonState ���ڿ��ƻ��Ƶİ�ť��״̬��ʲô
	void DrawCaptionButton(eDCCaptionButton eCaptionButton, eDCButtonState eButtonState=eButtonStateNormal, int iLeft=0, int iTop=0, int iWidth=40, int iHeight=20);

	//���Ʊ�׼�İ�ť
	//��׼��ť������ͨ�� CommandButton ��ʽ�İ�ť��Ҳ �����Ǹ�ѡ�򡢵�ѡ��
	void DrawButton(eDCButton eButton, eDCButtonState eButtonState=eButtonStateNormal, int iLeft=0, int iTop=0, int iWidth=40, int iHeight=20);

	//���Ʋ˵�����Դͼ��
	//����������Ҽ�ͷ�����ͷ���˵�Բ�㡢�Թ�
	void DrawMenuButton(eDCMenuButton eMenuButton, eDCButtonState eButtonState=eButtonStateNormal, int iLeft=0, int iTop=0, int iWidth=40, int iHeight=20);

	//���ƹ���������Դͼ��
	void DrawScrollButton(eDCScrollButton eScrollButton, eDCButtonState eButtonState=eButtonStateNormal, int iLeft=0, int iTop=0, int iWidth=40, int iHeight=20);

	//////////////////////////////////////////////////////////////////////////
	// ��տؼ��ڲ���������
	//������ݣ�����򴰿ڷ��� WM_PAINT ��Ϣ
	void InvalidateAll();

	// ��� GDI ���ַ�Χ����Ϊĳһ����ɫ
	// ע����ɫ��Ϊ��ʱ��ˢ�ӵ���ɫ
	void FillRectArea(int iLeft, int iTop, int iRight, int iBottom);

	// ������� GDI �ڴ�����Ϊĳ����ɫ
	// ע����ɫ��Ϊ��ʱ��ˢ�ӵ���ɫ
	void FillRectAll();

private:
	//�ͷ��ڴ���Դ��û�в���
	//����±����������������д��һ�飬����û�ִ�й��������л���Ŀؼ���ô��
	// ��ʱ���ڴ滷����DC�ڴ滷����ͬ�������ģ�����û��������ù����Ŀؼ���
	// ɾ��ԭ���ģ����´���һ������
	void FreeDc();

	//�ͷ���Դ����Ȼʹ�ú� API������ͬ�ķ�ʽ
	//ɾ���´����ıʻ���ˢ�ӻ�����λͼ����Դ�����û�У�ֱ�ӷ��أ�
	// ����ɾ��ϵͳԤ�õĻ��ʻ���ˢ��
	void DeletePenObject();
	void DeleteBrushObject();
	void DeleteFontObject();

private:
	// =======================================================================
	// ��˽�г�Ա
	//========================================================================

	HWND m_hWnd;			//���ڱ����ڴ滷����Ӧ�Ŀؼ����ߴ��ڵľ��

	HDC m_hDc;				//������������Ϊ�ؼ��������ڴ滷��		
	HDC m_hDcMem;			//������������豸���ݵ��ڴ滷��
	HBITMAP m_hBMP;			//����������������µ�λͼ����
	HBITMAP m_hBMPOld;		//���ڱ������ľɵ�λͼ������
	HPEN m_hPen;			//���ڱ��浱ǰ�ıʵ���Ϣ
	HPEN m_hPenOld;			//���ڱ���ԭ���ıʵ���Ϣ����������ͷ���Դ
	HBRUSH m_hBrush;		//���ڱ��浱ǰ��ˢ�ӵ���Ϣ
	HBRUSH m_hBrushOld;		//���ڱ���ԭ����ˢ�ӵ���Ϣ����������ͷ���Դ

	HDC m_hDCBmpSet;		//��ʱ����һ�������ڴ滷�������Է���λͼ������		
	HBITMAP m_hBmpSet;			//���������µ�λͼ�ľ��
	HBITMAP m_hBmpSetOld;		//��������ɵ�λͼ�ľ��

	LOGFONT m_hFontLog;		//���ڱ��浱ǰ���������ݣ�ÿ��ִ�л����ı���ʱ��Ͳ���Ҫ�ٴθ�ֵ��
	HFONT m_hFont;			//���ڴ洢��ǰ���ı��ľ�������û��ʹ��Ϊ NULL
	bool m_fNeedNewFont;	//���ڴ洢��ǰ��ӡ�����Ƿ���Ҫ���´�������

	RECT m_RectClip;		//���ڱ�����������ķ�Χ
	UINT m_uFormatFlag;		//���ڱ����ַ������λ�õı�־����������������л��߶��䷽ʽ

	bool m_fShowStatus;			//Ϊ���ʾִ�й��� Show() ���������ٲ���Ҫ�Զ�ִ�У���Ϊ�٣���ѯ����ʾ���Զ�ִ�� Show() ����
								//רΪ������ƣ�����û��ڻ�ͼ�����Ǵ��ڴ滷��ת����
								//�������û��ִ�� Show() �������Զ�����ִ��

};

//////////////////////////////////////////////////////////////////////////
// BWindGDIPlus ���ʵ��
//////////////////////////////////////////////////////////////////////////


class CBWindGdiPlus
{
public:
	//////////////////////////////////////////////////////////////////////////
	/* GDI+ ���ú���˵����
	* 1.DrawLine ����ֱ��
	* 2.SetSmoothingMode ���û��Ƶ�Ч��Ʒ�ʣ�����ݻ��Ƶ�����
	* 3.FillEllipse ������Բ
	* 4.DrawImage ����ͼƬ
	* 5.DrawString �����ı��ַ���
	* 6.RotateTransform ����ͼ�񣬴����Ͻǽ�����ת
	*/
	//////////////////////////////////////////////////////////////////////////

	//���캯��
	CBWindGdiPlus();
	CBWindGdiPlus(HWND hWnd);

	//��������
	~CBWindGdiPlus();

	//�о��������ǿ������ת��
	//������������˴��������̫ǿ�ˣ��������������ͺܷ����ˣ��ṩ��ǿ������ת���Ľӿ�
	operator Graphics* () { return GetGraphics(); }
	operator Pen* () { return GetPen(); }
	operator Brush* () { return GetBrush(); }
	operator Font* () { return GetFont(); }
	operator FontFamily* () { return GetFontName(); }

	//////////////////////////////////////////////////////////////////////////
	//�ڲ���Ӧ���ڵ��豸����ͻ�����������
	//////////////////////////////////////////////////////////////////////////

	//��ú����ö�Ӧ�Ŀؼ����ߴ��ڵľ��
	void hWindowSet(HWND hWnd);
	HWND hWindow();

	//////////////////////////////////////////////////////////////////////////
	// ��ȡ�ڲ��Ļ�ͼ���ߵĺ���
	//////////////////////////////////////////////////////////////////////////

	//��ö�Ӧ�����ڲ��Ļ��ʵ�ָ��
	//���� gdi+ ���û������Լ��������еĺ�����ʵ�ֹ��ܣ��˴�����Ҫ�Լ��ٱ�д��Ӧ��������
	Pen* GetPen();

	//��ȡ�ڲ��Ļ�ˢ��ָ��
	SolidBrush* GetBrush();

	//��ȡ�ڲ��Ľ��仭ˢ��ָ��
	LinearGradientBrush* GetLinearBrush();

	//��ȡ�ڲ��Ļ�ͼ������ָ��
	Graphics* GetGraphics();

	//��ȡ�����´���ͼƬ�����ָ��
	Image* CreateImage(LPTSTR sImagePath);
	Image* GetImage();

	//////////////////////////////////////////////////////////////////////////
	//���ڻ����ı��ĺ���̫�������ã��˴���������дһ����һ��Ļ����ı��ĺ���
	//////////////////////////////////////////////////////////////////////////

	//��ú���������Ĵ�С
	void FontSizeSet(int iSize = 10);
	int FontSize();

	//��ú��������������
	void FontNameSet(LPCTSTR sFontName = TEXT("΢���ź�"));
	tstring FontName();

	//��ú������Ƿ��������»���
	void FontUnderLineSet(bool fIsUnderline = false);
	bool FontUnderLine();

	//��ú������Ƿ�����Ӵ�
	void FontBoldSet(bool fIsBold = false);
	bool FontBold();

	//��ú������Ƿ�����Ӵ�
	void FontItalicSet(bool fIsItalic = false);
	bool FontItalic();

	//==========================================================
	//Ϊ�˷����������ʹ���ڲ��Ķ������Դ˴�������չ�ӿ�
	//==========================================================

	//���������Ķ�Ӧ�������Ƶ� FontFamily ����ָ��
	FontFamily* GetFontName();

	//���������� Font �����ָ��
	Font* GetFont();

	//������Ķ����ı����Ƶķ�װ���������ԱȽϼ򵥵Ļ����ı�����
	//�ı���ɫΪˢ����ɫ�����Ҫ�����ı���ɫ����Ҫ��ǰ���� Brush ����ɫ
	bool DrawString(LPCTSTR sText, float x, float y);

	///////////////////////////////////////////////////////////////////////////
	// �Զ���Ļ�ͼ�������˴������Զ�����Ǳ�����
	//////////////////////////////////////////////////////////////////////////

	//������̬��,�˴�������̬�ǵ���Ӧ���ڴ滭�����棬���ص��ǻ����ľ�������Ҫ�ڶ���������Ƹ�����
	//��Ҫ���ú������临�Ƶ���Ӧ�Ķ������棬Ĭ�ϵĻ��Ƴ�����ǣ�5:3�����������Ƶı�������������ʧ��
	//ʹ��ʾ����
	/* ���к����������Ƕ�Ӧ��Ҫ���Ƶ��������
	* 	m_gdiPlus.GetGraphics()->DrawImage(
	*				m_graphics->DrawAttitudeInstrument(0,0,0),
	*				0, 0, iWidth, iHeight);
	*/
	//��Ҫ������
	// fPitchAngle �����ǣ���Χ�� -90 �� 90
	// fRollAngle �����ǣ���Χ�� -90 �� 90 
	// fYawAngle ƫ���ǣ���Χ�� -180 �� 180
	//����������
	//��ʱ��ȥ����֪��������ԸĶ��ģ��о������
	Bitmap* DrawAttitudeInstrument(float fPitchAngle, float fRollAngle, float fYawAngle);

	//�����״�ɨ������
	/*
	*/
	//��Ҫ������
	// dMaxDistance ��ʾ����Ȧ�ľ����С��������Χ�����ݽ��޷���Ч�Ļ��Ƴ���
	// fAutoDrawData ��ʾ�Ƿ��Զ��������ݣ�����û���Ҫ�Լ����ƶ�Ӧ�����ݣ��˴�ѡ�� false
	// fScanAngDiff ÿһ��ɨ��䶯�ĽǶȴ�С
	// iScanTimDiff ɨ���ʱ�����Ƕ��٣���̬ˢ����
	Bitmap* DrawRada(double dMaxDistance, bool fAutoDrawData=true, float fScanAngDiff = 3,DWORD iScanTimDiff = 100);

	//Ϊ�״��������ݣ�����ʾһ�κ���Զ�������ݣ���Ҫ���ϲ������ݲ��ܲ��ϵ���ʾ��������
	//dDistance �� dAngle �ֱ��ʾ����ͽǶ�
	//iBmpShape ����û���Ҫ��ʾ�Լ���ͼƬʲô�ģ��˴��������ö�Ӧ�����ݣ���չ������Ĭ��Ϊ�գ�СԲ��
	void AddRadaData(double dDistance,double dAngle, Bitmap* iBmpShape = NULL);

private:
	//gdi+ �ĳ�ʼ���������˴�ֱ���þ�̬����������һ��������
	static void InitialGdiPlus();

	//ж��gdi+
	static void UnInitialGdiplus();

	//����һ�����壬�˺��������ã��Զ��ж϶�Ӧ�ĸ����������ݣ�Ȼ��
	//�Զ��ı��Ӧ�� FontFamily �� Font ָ���Ӧ������
	void CreateMyFont();

private:

	static ULONG_PTR m_gdiplusToken;	//���ڳ�ʼ��gdi��һ������

	//////////////////////////////////////////////////////////////////////////
	// ���������Ķ���
	HWND m_hWnd;						//�ڲ������Ҫ���ƵĿؼ��ľ��
	Graphics* m_pGraphics;				//��ͼ�������豸���
	Graphics* m_pGraphicInstruments;	//���ڻ�ͼ���豸����
	Bitmap* m_pBmpInstruments;				//��bitmap���ڻ����Ǳ���Ķ���
	//������ͼ����
	Pen* m_pPen;						//���ڱ������ڲ���Ӧ�ı�
	SolidBrush* m_pBrush;				//�ڲ���ˢ��
	//�߼���ͼ����
	LinearGradientBrush* m_pLinearBrush;	//���仭ˢ
	//ͼƬ����
	Image* m_pImage;						//ͼƬ�Ķ���
	//������������
	FontFamily* m_pFontName;				//���������
	Font* m_pFont;							//������ֺţ�����ļӴ��»��ߵ�Ч������
	/*
	* �˴�Ϊ�˼��ٲ��ϴ�����ɾ�������˷���Դ���˴�������ʱ���������ֺŵķ�ʽ��
	* ʹ�õ�ʱ���ٴ���������ֺ����ݣ������ӾͿ��Լ��ٴ���������Щ����Ĵ���
	* ���õĽ�ʡϵͳ��Դ��
	*/
	tstring m_sFontName;			//����������Ҫʹ�õ����������
	int m_iFontSize;				//�����������Ӧ��������ֺ�
	int m_iFontStyle;				//����ķ��
	bool m_fFontChange;				//��ʾ�����Ƿ�ı䣬����ı���Զ����´���
};




