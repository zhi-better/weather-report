//////////////////////////////////////////////////////////////////////
// BToolBar.h: CBToolBar ��Ķ��壬���ڲ���һ����������¼����⣩
//
// ֧�֣���Ҫ BWindows ģ���֧��
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BWndBase.h"
#include <vector>
#include <list>
using namespace std;

//////////////////////////////////////////////////////////////////////////
//�����������ݵ�ö�ٳ�Ա

//ϵͳ�Զ���Ĺ�����ͼ��λͼ
enum EToolBarSysIcons
{
	eSysIcon_None = -1,							//��ͼ��
	eSysIcon_Cut = STD_CUT,						//����
	eSysIcon_Copy = STD_COPY,					//����
	eSysIcon_Paste = STD_PASTE,					//ճ��
	eSysIcon_Undo = STD_UNDO,					//����
	eSysIcon_Redow = STD_REDOW,					//����
	eSysIcon_Delete = STD_DELETE,				//ɾ��
	eSysIcon_FileNew = STD_FILENEW,				//�½��ļ�
	eSysIcon_FileOpen = STD_FILEOPEN,			//���ļ�
	eSysIcon_FileSave = STD_FILESAVE,			//�����ļ�
	eSysIcon_Printpre = STD_PRINTPRE,			//
	eSysIcon_Properties = STD_PROPERTIES,		//
	eSysIcon_Help = STD_HELP,					//����
	eSysIcon_Find = STD_FIND,					//����
	eSysIcon_Replace = STD_REPLACE,				//�滻
	eSysIcon_Print = STD_PRINT					//��ӡ
};
//��ť�Ĳ�ͬ���
enum ETBBtnStyle  //�볣��ֵ��ͬ
{
	eBtn_Default = TBSTYLE_BUTTON,			//��ȱʡ�ģ���ť����ť��һ���������ѹ��ť��
	eBtn_Check = TBSTYLE_CHECK,				//��ѡ����ť��һ����ѡ��ť�������Ա�ѡ�����߲���ѡ����
	eBtn_ButtonGroup = TBSTYLE_CHECKGROUP,	//��ť�顣ֱ�����ڵ���һ����ť������֮ǰ����ť�����ְ��µ�״̬�����κ�ʱ�̶�ֻ�ܰ�������һ����ť��
	eBtn_Separator = TBSTYLE_SEP,            //�ָ�������ť�Ĺ�������Ϊ�� 8 �����صĹ̶���ȵķָ�����
											//tbtnPlaceholder 4 ռλ������ť����ۺ͹�������ָ����������п����õĿ�ȡ�ռλ��Ҳ�� tbtnSeparator ���ã����轫 bAutoSizeByText ��Ϊ False ,֮���� ButtonWidth ������������
	eBtn_DropDown = TBSTYLE_DROPDOWN         //Creates a drop - down list button.Drop - down buttons send the TBN_DROPDOWN notification.

};
//ˮƽ�ı����뷽ʽ
enum ETBBtnTextAlign
{
	eBtn_TextLeft = DT_LEFT,
	eBtn_TextRight = DT_RIGHT,
	eBtn_TextCenter = DT_CENTER
};
//��ֱ�ı����뷽ʽ
enum ETBBtnTextAlignVert
{
	eBtn_TextTop = DT_TOP,
	eBtn_TextBottom = DT_BOTTOM,
	eBtn_TextVCenter = DT_VCENTER
};
//��������λ��
enum EToolBarPos
{
	eTB_Left = CCS_LEFT,
	eTB_Right = CCS_RIGHT,
	eTB_Top = CCS_TOP,
	eTB_Bottom = CCS_BOTTOM
};

class CBToolBar
{
public:

	//���캯��
	CBToolBar();
	CBToolBar(unsigned int idToolBar);
	//��������
	~CBToolBar();

	//============================= ���ù��������� ==============================

	// ͨ�� id ���ض�Ӧ�ľ������������ڣ����� NULL
	// ���Ҹı����ڲ��� m_hWndToolBar, ������ô���������������ݶ�����Դ˾���Ĺ�����
	HWND SetHwndFromID(unsigned int idToolBar);

	//Ϊ��������һ����������idToolBar ��ʾ��������Ӧ����Դ ID
	// hWndParent ��ʾ���Ǹ����ڵľ������Ҫ��������Ϣ֪ͨ�Ĵ��ھ��
	// fIsRight ��ʾ�����ı�����ʾλ�ã������ true ,�ı�λ�ڰ�ť�ұߣ���Ϊ false, ��ʾ������
	HWND ToolBarAdd(HWND hWndParent, EToolBarPos eTBPos=eTB_Top, bool fTextRight = false);

	//Ϊ����������һ����ť
	//iBitmap ��ʾ����λͼ�� ID �����Ƕ�Ӧ��ϵͳλͼ��ö�ٳ�Ա
	//szText ��ʾ�ð�ť����ʾ�ı����ݣ�Ĭ����������λ��
	//iBtnStyle ��ʾ���ǰ�ť�ķ������������ť�������� checked ���͵İ�ť��
	//iPostoAdd ��ʾ�������Ӱ�ť��λ�ã��� 0 ��ʼ�����밴ť������� -1��Ĭ�����������λ��
	//iCommand Ϊ�ð�ť�������ţ����Ϊ -1 ��ʾĬ���ɱ���������
	//�������ذ汾��Ҫ�����ϵͳλͼ���Զ���λͼ���ԣ���������һ��
	bool AddButton(EToolBarSysIcons iBitmap, LPCTSTR szText = L"",
		ETBBtnStyle iBtnStyle = eBtn_Default, DWORD iPostoAdd = -1, int iCommand = -1);
	bool AddButton(unsigned int idBitmap, LPCTSTR szText = L"",
		ETBBtnStyle iBtnStyle = eBtn_Default, DWORD iPostoAdd = -1, int iCommand = -1);

	//Ϊ����������һ��λͼ��ϵͳ��ͼ���У����ص��Ǹ�ͼƬ��ϵͳ��λͼ��������
	//���������Ӱ�ť������������λͼ���Ӷ��ı�ϵͳԤ���������
	//����ֵΪ�����ӵ�λͼ����������ֵ
	int AddBitmapToSys(unsigned int idBmp);

	//����һ���ָ���
	bool AddSeparator(int iCommand = -1);

	//ͨ����ť�� Command �Ƴ�һ����ť
	BOOL RemoveFromCommand(int iCommand);

	//ͨ����ť�� index �Ƴ�һ����ť��λ�ü���� 0 ��ʼ
	BOOL RemoveFromIndex(int iIndex = -1);

	//�ƶ�һ����ť���µ�λ�ã�λ�ü���� 0 ��ʼ
	BOOL MoveButton(int iIndexOld, int iIndexNew);

	//����һ�����еİ�ť�Ĵ�С�����հ�ť�����Զ�ȷ����С
	void Resize();

	//���(x,y)λ��һ���Ƿָ��ߵİ�ť�ϣ��������ڰ�ť�� Index �� >0��Index �� 0 ��ʼ��
	//���(x,y)��λ��һ���Ƿָ��ߵİ�ť�ϣ�����<0��ֵ������ֵ��ʾ�ָ��ߵ� Index ���������ķǷָ��߰�ť�� Index
	int HitTest(int x, int y);

	//ͨ�� Command ��ö�Ӧ�� index
	int CommandToIndex(int iCommand);

	//ͨ�� index ��ö�Ӧ�� command
	int IndexToCommand(int iIndex);

	//���õ�һ����ťǰ������ؾ���
	void SetFirstIndent(int iIndent);

	//���ð�ť�е��ı�ˮƽ���뷽ʽ
	void SetTextAlign(ETBBtnTextAlign iStyle);
	//���ð�ť�е��ı���ֱ���뷽ʽ
	void SetTextAlignVert(ETBBtnTextAlignVert iStyle);

	//���ð�ť�е��ı��Ƿ���Զ�����ʾ
	void SetTextMultiline(bool fCanMultiline = true);

	//���ð�ť�Ŀ�����ֵ����Сֵ�ķ�Χ
	BOOL SetWidthRange(int iMinRange, int iMaxRange);

	//���ù������ĸ����ڵľ������Ҫ������Ϣ�Ľ��պʹ���
	BOOL SetParent(HWND hWnd);

	//^^^^^^^^^^^^^^^^^^^^^^^��ť���ԵĲ鿴�͸���^^^^^^^^^^^^^^^^^^^^^^^^

	//���ػ������޸�һ����ť��ͼ�꣨������Ϊö�ٳ�Աֵ���ֵ����ֱ��Ϊ ImageList �е�ͼ�������ţ�
	int GetButtonImage(int iCommand);
	void SetButtonImage(int iCommand, unsigned int iNewValue);
	void SetButtonImage(int iCommand, EToolBarSysIcons eNewValue);

	//���غ�����һ����ť�Ƿ� Pressed ������״̬�ұ���ɫ��Ϊ������ɫ��
	BOOL GetButtonPressed(int iCommand);
	void SetButtonPressed(int iCommand, bool fNewValue);

	//���غ�����һ����ť�Ƿ� Checked ������״̬������ɫ���䣩
	BOOL GetButtonChecked(int iCommand);
	void SetButtonChecked(int iCommand, bool fNewValue);

	//���غ�����һ����ť�Ƿ� Enabled
	BOOL GetButtonEnabled(int iCommand);
	void SetButtonEnabled(int iCommand, bool fNewValue);

	//���ػ�����һ����ť�� Visible
	BOOL GetButtonVisible(int iCommand);
	void SetButtonVisible(int iCommand, bool fNewValue);

	//���ػ����ã��Ƿ��һ����ť��ʼ����
	BOOL GetButtonLineBreak(int iCommand);
	void SetButtonLineBreak(int iCommand, bool fNewValue);

	//���ػ����ã��Ƿ�ť�Ƿ��ڻҶ�״̬���Ҷ�״̬Ҳ��Ч�ɽ����û����룩
	BOOL GetButtonGrayed(int iCommand);
	void SetButtonGrayed(int iCommand, bool fNewValue);

	//���ػ����ã��Ƿ�ť�Ƿ��� Marked ״̬
	BOOL GetButtonMarked(int iCommand);
	void SetButtonMarked(int iCommand, bool fNewValue);

	//���ػ�����һ����ť�� Command �ţ�Index �±�� 0 ��ʼ
	int GetButtonCommand(int iIndex);
	void SetButtonCommand(int iIndex, int iNewCommand);

	//���ػ�����һ����ť���ı�
	tstring GetButtonText(int iCommand);
	void SetButtonText(int iCommand, LPCTSTR sNewText);

	//��ð�ť�� top, left, height, width
	int GetButtonTop(int iCommand);
	int GetButtonLeft(int iCommand);
	int GetButtonHeight(int iCommand);
	int GetButtonWidth(int iCommand);
	RECT GetButtonRect(int iCommand);

	//���ð�ť�� width
	BOOL SetButtonWidth(int iCommand, int iNewWidth);

	//^^^^^^^^^^^^^^^^^ �ؼ����ԵĲ鿴�͸��� ^^^^^^^^^^^^^^^^^^^^^^^

	//��ú����ù������ĸ߶�
	//�����治ͬ�������������ȫ���ĸ߶�
	int GetButtonsHeight();
	void SetButtonsHeight(int iNewValue);

	//��ú����ù������Ŀ��
	//�����治ͬ�������������ȫ���Ŀ��
	int GetButtonsWidth();
	void SetButtonsWidth(int iNewValue);

	//���غ����õ�ǰӵ�С��ȵ㡱�İ�ť�� Index ���±�� 0 ��ʼ��������Ϊ -1 ��ʾ���ȵ㰴ť
	int GetButtonCurrentHotIndex();
	void SetButtonCurrentHotIndex(int iIndex);

	//ֻ�����ԣ����ذ�ť����
	int ButtonsCount();

	//��û����ð�ť�������ؼ�������� TBSTYLE_WRAPABLE ���
	int GetRows();
	void SetRows(int iNewValue);

	//�Ƿ�ť���Զ�����
	BOOL GetWrappable();
	void SetWrappable(bool fCanWrappable);

	//���غ����ö�Ӧ�Ĺ������ĸ�����ʾ����ɫ
	COLORREF GetColorBtnHighlight();
	void SetColorBtnHighlight(COLORREF cNewColor);

	//���غ����ð�ť��Ӧ����Ӱ����ɫ
	COLORREF GetColorBtnShadow();
	void SetColorBtnShadow(COLORREF cNewColor);

	//���غ��������һ��ͻ����ʾ����Ŀ������ͻ����ʾ��ֱ����һ����Ŀͻ����ʾ��
	//��ʹ����뿪�������ؼ���Ҳ��������������
	BOOL GetAnchorHighlight();
	void SetAnchorHighlight(bool fNewValue);

	//==========================================================================

private:

	//========================= ��̬��Ա =============================
	static CBHashLK m_hashToolBarIDs;		//��ϣ��key = ����������Դid��data = �ù������ľ��

	//========================= ˽�г�Ա =================================
	//���ڲ�����ľ��
	HWND m_hWndToolBar;			//һ���������ؼ��ľ��
	HWND m_hParentWnd;			//���������ڵĸ����ڵľ��
	unsigned int m_idToolBar;	//��ǰʹ�õĹ������� id
	int m_iCommand;			//���ڱ����Ӧ�Ĺ������Ŀؼ��� command
	int m_iSepCount;		//��ö�Ӧ�ķָ��ߵ���������ҪΪ�� iCommand ����
};








