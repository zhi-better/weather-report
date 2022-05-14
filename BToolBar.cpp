//////////////////////////////////////////////////////////////////////////
//CBToolBar ���ʵ��
//���ڣ�2021/1/19
//��Ҫ���ڲ���������������
//////////////////////////////////////////////////////////////////////////

#include "BToolBar.h"


CBToolBar::CBToolBar()
{
	m_hWndToolBar = NULL;
	m_hParentWnd = NULL;
	m_idToolBar = 0;
	m_iCommand = 1;
	m_iSepCount = 0;
}

CBToolBar::CBToolBar(unsigned int idToolBar)
{
	CBToolBar();
	SetHwndFromID(idToolBar);
}

CBToolBar::~CBToolBar()
{

}

HWND CBToolBar::SetHwndFromID(unsigned int idToolBar)
{
	HWND hWnd;
	hWnd = (HWND)m_hashToolBarIDs.Item(idToolBar, false);
	if (hWnd)
	{
		m_hWndToolBar = hWnd;
		m_iCommand = (int)m_hashToolBarIDs.ItemDouble(idToolBar);
		return hWnd;
	}
	else
	{
		m_hWndToolBar = NULL;
		m_idToolBar = idToolBar;	// ���ñ�������Ĺ����� id 
		m_iCommand = 1;
		return NULL;
	}
}

HWND CBToolBar::ToolBarAdd(HWND hWndParent, EToolBarPos eTBPos /*= eTB_Top*/, bool fTextRight /*= false*/ )
{
	if (!m_idToolBar)	//����˴��Ĺ����� id �� 0 ��ʾû�й��������˴�ֱ�ӷ��� NULL
		return NULL;
	
	int iFlag = TBSTYLE_WRAPABLE | TBSTYLE_FLAT | WS_CHILD | WS_VISIBLE | WS_BORDER | eTBPos;
	if (fTextRight)
		iFlag = iFlag | TBSTYLE_LIST;
	//�����´���һ�� toolbar �ؼ��������� m_hashToolBarIDs ����洢�ù����� id �Ͷ�Ӧ�ľ��
	m_hWndToolBar = CreateWindowEx(0L, TOOLBARCLASSNAME, NULL,	//������������Ĭ�ϲ���
		iFlag,									//���ù������ķ��
		0, 0, 0, 0,								//���ù������Ĵ�С
		hWndParent,								//�����ڣ�������Ϣ�ص�
		(HMENU)m_idToolBar,						//��ʾ���ǹ������� ID ����
		pApp->hInstance, NULL);

	// ��������ɹ�����ô�ڹ�ϣ���б����Ӧ�����ݣ����򷵻� NULL����ʾ����ʧ��
	if (m_hWndToolBar)
	{
		m_hashToolBarIDs.Add((int)m_hWndToolBar, m_idToolBar);
		m_hashToolBarIDs.ItemDoubleSet(m_idToolBar, m_iCommand);
		m_hParentWnd = hWndParent;
	}
	else
		return NULL;
	//ʹ�� CreateWindowEx ����һ��toolbar�ؼ������뷢�������Ϣ����ӿؼ�֮ǰ
	SendMessage(m_hWndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	//���ϵͳԤ�õ�λͼ���ؼ��У���������ֱ�ӵ�����
	TBADDBITMAP tbBitmap;
	tbBitmap.hInst = HINST_COMMCTRL;
	tbBitmap.nID = IDB_STD_SMALL_COLOR;
	SendMessage(m_hWndToolBar, TB_ADDBITMAP, 0, (LPARAM)&tbBitmap);

	return m_hWndToolBar;
}

bool CBToolBar::AddButton(EToolBarSysIcons iBitmap, LPCTSTR szText /*= L""*/, ETBBtnStyle iBtnStyle /*= eBtn_Default*/, DWORD iPostoAdd/*=-1*/, int iCommand/*=-1*/)
{
	if (!m_hWndToolBar)		//���û�з��� false
		return false;

	//Ĭ������һ����ť
	TBBUTTON tbb = { 0 };			//��ť

	tbb.iBitmap = iBitmap;				//�½���λͼ
	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = iBtnStyle;
	tbb.iString = szText == L"" ? 0 : (INT_PTR)szText;

	//��Ӧ������Ƿָ��߰�ť��command ���� 0
	if (iCommand == -1 )
	{
		tbb.idCommand = m_iCommand++;
		m_hashToolBarIDs.ItemDoubleSet(m_idToolBar, m_iCommand);
		if (iBtnStyle == eBtn_Separator)
			m_iSepCount++;
	}
	else
		tbb.idCommand = iCommand;
	tbb.dwData = 0;						//ͦ��Ҫ�ģ����洦��

	//��Ӱ�ť��������
	if (iPostoAdd == -1)
		SendMessage(m_hWndToolBar, TB_ADDBUTTONS, (WPARAM)1/*Ĭ������һ����ť*/, (LPARAM)&tbb);
	else
		SendMessage(m_hWndToolBar, TB_INSERTBUTTON, iPostoAdd, (LPARAM)&tbb);

	return true;
}

bool CBToolBar::AddButton(unsigned int idBitmap, LPCTSTR szText /*= L""*/, ETBBtnStyle iBtnStyle /*= eBtn_Default*/, DWORD iPostoAdd /*= -1*/, int iCommand /*= -1*/)
{
	if (!m_hWndToolBar)		//���û�з��� false
		return false;

	//Ĭ������һ����ť
	TBBUTTON tbb = { 0 };			//��ť

	tbb.iBitmap = AddBitmapToSys(idBitmap);		//�½���λͼ����ö�Ӧ��������
	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = iBtnStyle;
	tbb.iString = (szText == L"") ? 0 : (INT_PTR)szText;
	//��Ӧ������Ƿָ��߰�ť��command ���� 0
	if (iCommand == -1)
	{
		tbb.idCommand = m_iCommand++;
		m_hashToolBarIDs.ItemDoubleSet(m_idToolBar, m_iCommand);
		if (iBtnStyle == eBtn_Separator)
			m_iSepCount++;
	}
	else
		tbb.idCommand = iCommand;
	tbb.dwData = 0;						//ͦ��Ҫ�ģ����洦��

	//��Ӱ�ť��������  
	if (iPostoAdd == -1)
		SendMessage(m_hWndToolBar, TB_ADDBUTTONS, (WPARAM)1/*Ĭ������һ����ť*/, (LPARAM)&tbb);
	else
		SendMessage(m_hWndToolBar, TB_INSERTBUTTON, iPostoAdd, (LPARAM)&tbb);

	return true;
}

int CBToolBar::AddBitmapToSys(unsigned int idBmp)
{
	//����ط��ò���һ�ַ�ʽ�ܹ�����ס�Ѿ���ӹ��� id �Ķ�Ӧ���������������׻����
	static vector<unsigned int> sidCtrls;
	static vector<int> siIndex;
	int i = 0;
	int iIndex = 0;

	//ͨ��ѭ���ж��ڲ��Ƿ��Ѿ���ӹ���ͼƬ������Ѿ����ˣ����ض�Ӧ��������
	for (; i < (int)sidCtrls.size(); i++)
	{
		if (sidCtrls[i] == idBmp)
			break;
	}
	if (i < (int)sidCtrls.size() && (int)sidCtrls.size() != 0)	//����ҵ��ˣ�ֱ�ӷ��ض�Ӧ��������
		return siIndex[i];
	else
	{
		//�������ӣ����Ҹı����е�ԭ������������
		TBADDBITMAP iNewBmp;
		iNewBmp.hInst = pApp->hInstance;
		iNewBmp.nID = idBmp;
		iIndex = SendMessage(m_hWndToolBar, TB_ADDBITMAP, 1, (LPARAM)&iNewBmp);
		if (iIndex != -1)
		{
			//��չԭ�������еĿռ䣬�����µ�λͼ������ id
			siIndex.push_back(iIndex);
			sidCtrls.push_back(idBmp);
		}
	}

	return iIndex;
}

bool CBToolBar::AddSeparator(int iCommand /*= -1*/)
{
	return AddButton(eSysIcon_None, NULL, eBtn_Separator, -1, -1);
}

BOOL CBToolBar::RemoveFromCommand(int iCommand)
{
	if (iCommand == 0)	m_iSepCount--;
	int idxBtn = SendMessage(m_hWndToolBar, TB_COMMANDTOINDEX, iCommand, 0);
	return SendMessage(m_hWndToolBar, TB_DELETEBUTTON, idxBtn, 0);
}

BOOL CBToolBar::RemoveFromIndex(int iIndex /*= -1*/)
{
	return RemoveFromCommand(IndexToCommand(iIndex));
}

BOOL CBToolBar::MoveButton(int iIndexOld, int iIndexNew)
{
	return SendMessage(m_hWndToolBar, TB_MOVEBUTTON, iIndexOld, iIndexNew);
}

void CBToolBar::Resize()
{
	SendMessage(m_hWndToolBar, TB_AUTOSIZE, 0, 0);
}

int CBToolBar::HitTest(int x, int y)
{
	POINT pt;
	int r;
	pt.x = x;
	pt.y = y;
	r = SendMessage(m_hWndToolBar, TB_HITTEST, 0, (LPARAM)&pt);

	return r;
}

int CBToolBar::CommandToIndex(int iCommand)
{
	return SendMessage(m_hWndToolBar, TB_COMMANDTOINDEX, iCommand, 0);
}

int CBToolBar::IndexToCommand(int iIndex)
{
	TBBUTTON btn;
	SendMessage(m_hWndToolBar, TB_GETBUTTON, iIndex, (LPARAM)&btn);
	return btn.idCommand;
}

void CBToolBar::SetFirstIndent(int iIndent)
{
	SendMessage(m_hWndToolBar, TB_SETINDENT, iIndent, 0);
	Resize();
}

void CBToolBar::SetTextAlign(ETBBtnTextAlign iStyle)
{
	SendMessage(m_hWndToolBar, TB_SETDRAWTEXTFLAGS, -1, iStyle);
	Resize();
}

void CBToolBar::SetTextAlignVert(ETBBtnTextAlignVert iStyle)
{
	SendMessage(m_hWndToolBar, TB_SETDRAWTEXTFLAGS, -1, iStyle);
	Resize();
}

void CBToolBar::SetTextMultiline(bool fCanMultiline /*= true*/)
{
	SendMessage(m_hWndToolBar, TB_SETDRAWTEXTFLAGS, DT_SINGLELINE, fCanMultiline ? DT_SINGLELINE : 0);
}

BOOL CBToolBar::SetWidthRange(int iMinRange, int iMaxRange)
{
	return SendMessage(m_hWndToolBar, TB_SETBUTTONWIDTH, 0, iMaxRange * 65536 + iMinRange);
}

BOOL CBToolBar::SetParent(HWND hWnd)
{
	return SendMessage(m_hWndToolBar, TB_SETPARENT, (WPARAM)hWnd, 0);
}

int CBToolBar::GetButtonImage(int iCommand)
{
	return SendMessage(m_hWndToolBar, TB_GETBITMAP, iCommand, 0);
}

void CBToolBar::SetButtonImage(int iCommand, unsigned int iNewValue)
{
	SendMessage(m_hWndToolBar, TB_CHANGEBITMAP, iCommand, AddBitmapToSys(iNewValue));
}

void CBToolBar::SetButtonImage(int iCommand, EToolBarSysIcons eNewValue)
{
	SendMessage(m_hWndToolBar, TB_CHANGEBITMAP, iCommand, eNewValue);
}

BOOL CBToolBar::GetButtonPressed(int iCommand)
{
	int iState = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	return (iState & TBSTATE_PRESSED);
}

void CBToolBar::SetButtonPressed(int iCommand, bool fNewValue)
{
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	if (fNewValue)
		state = state | TBSTATE_PRESSED;
	else
		state = state & ~TBSTATE_PRESSED;
	SendMessage(m_hWndToolBar, TB_SETSTATE, iCommand, state);
}

BOOL CBToolBar::GetButtonChecked(int iCommand)
{

	
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	return (state & TBSTATE_CHECKED);
}

void CBToolBar::SetButtonChecked(int iCommand, bool fNewValue)
{

	
	SendMessage(m_hWndToolBar, TB_CHECKBUTTON, iCommand, fNewValue);
}

BOOL CBToolBar::GetButtonEnabled(int iCommand)
{

	
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	return (state & TBSTATE_ENABLED);
}

void CBToolBar::SetButtonEnabled(int iCommand, bool fNewValue)
{

	
	SendMessage(m_hWndToolBar, TB_ENABLEBUTTON, iCommand, fNewValue);
}

BOOL CBToolBar::GetButtonVisible(int iCommand)
{

	
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	return (state & TBSTATE_HIDDEN);
}

void CBToolBar::SetButtonVisible(int iCommand, bool fNewValue)
{

	
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	if (!fNewValue)
		state = state | TBSTATE_HIDDEN;
	else
		state = state & ~TBSTATE_HIDDEN;
	SendMessage(m_hWndToolBar, TB_SETSTATE, iCommand, state);
}

BOOL CBToolBar::GetButtonLineBreak(int iCommand)
{
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	return (state & TBSTATE_WRAP);
}

void CBToolBar::SetButtonLineBreak(int iCommand, bool fNewValue)
{
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	if (fNewValue)
		state = state | TBSTATE_WRAP;
	else
		state = state & ~TBSTATE_WRAP;
	SendMessage(m_hWndToolBar, TB_SETSTATE, iCommand, MAKELONG(state, 0));
}

BOOL CBToolBar::GetButtonGrayed(int iCommand)
{
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	return (state & TBSTATE_INDETERMINATE);
}

void CBToolBar::SetButtonGrayed(int iCommand, bool fNewValue)
{
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	if (fNewValue)
		state = state | TBSTATE_INDETERMINATE;
	else
		state = state & ~TBSTATE_INDETERMINATE;
	SendMessage(m_hWndToolBar, TB_SETSTATE, iCommand, state);
}

BOOL CBToolBar::GetButtonMarked(int iCommand)
{
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	return (state & TBSTATE_MARKED);
}

void CBToolBar::SetButtonMarked(int iCommand, bool fNewValue)
{
	int state = SendMessage(m_hWndToolBar, TB_GETSTATE, iCommand, 0);
	if (fNewValue)
		state = state | TBSTATE_MARKED;
	else
		state = state & ~TBSTATE_MARKED;
	SendMessage(m_hWndToolBar, TB_SETSTATE, iCommand, state);
}

int CBToolBar::GetButtonCommand(int iIndex)
{
	return IndexToCommand(iIndex);
}

void CBToolBar::SetButtonCommand(int iIndex, int iNewCommand)
{
	SendMessage(m_hWndToolBar, TB_SETCMDID, iIndex, iNewCommand);
}

tstring CBToolBar::GetButtonText(int iCommand)
{
	tstring sText;
	sText.resize(1024);
	SendMessage(m_hWndToolBar, TB_GETBUTTONTEXT, iCommand, (LPARAM)sText.c_str());
	return sText;
}

void CBToolBar::SetButtonText(int iCommand, LPCTSTR sNewText)
{
	TBBUTTONINFO btnInfo;
	tstring sTextNew = sNewText;

	btnInfo.cbSize = sizeof(TBBUTTONINFO);
	btnInfo.dwMask = TBIF_TEXT;
	btnInfo.pszText = (LPTSTR)sTextNew.c_str();
	btnInfo.cchText = sTextNew.length();

	SendMessage(m_hWndToolBar, TB_SETBUTTONINFO, iCommand, (LPARAM)&btnInfo);
}

int CBToolBar::GetButtonTop(int iCommand)
{
	return GetButtonRect(iCommand).top;
}

int CBToolBar::GetButtonLeft(int iCommand)
{
	return GetButtonRect(iCommand).left;
}

int CBToolBar::GetButtonHeight(int iCommand)
{
	RECT rc = GetButtonRect(iCommand);
	return rc.bottom - rc.top;
}

int CBToolBar::GetButtonWidth(int iCommand)
{
	RECT rc = GetButtonRect(iCommand);
	return rc.right - rc.left;
}

RECT CBToolBar::GetButtonRect(int iCommand)
{
	RECT rc = { 0,0,0,0 };
	SendMessage(m_hWndToolBar, TB_GETRECT, iCommand, (LPARAM)&rc);
	return rc;
}

BOOL CBToolBar::SetButtonWidth(int iCommand, int iNewWidth)
{
	TBBUTTONINFO btnInfo;

	btnInfo.cbSize = sizeof(TBBUTTONINFO);
	btnInfo.dwMask = TBIF_SIZE;
	btnInfo.cx = iNewWidth;

	return SendMessage(m_hWndToolBar, TB_SETBUTTONINFO, iCommand, (LPARAM)&btnInfo);
}

int CBToolBar::GetButtonsHeight()
{
	//�˴��Ƿ���һ�� DWORD ���ݣ����еĸ�λ��ʾ���Ǹ߶ȣ���λ��ʾ���ǿ��
	return SendMessage(m_hWndToolBar, TB_GETBUTTONSIZE, 0, 0) / 65536;
}

void CBToolBar::SetButtonsHeight(int iNewValue)
{
	int iWidth = GetButtonsWidth();
	SendMessage(m_hWndToolBar, TB_SETBUTTONSIZE, 0, iNewValue * 65536 + iWidth);
}

int CBToolBar::GetButtonsWidth()
{
	//�˴��Ƿ���һ�� DWORD ���ݣ����еĸ�λ��ʾ���Ǹ߶ȣ���λ��ʾ���ǿ��
	return SendMessage(m_hWndToolBar, TB_GETBUTTONSIZE, 0, 0) % 65536;
}

void CBToolBar::SetButtonsWidth(int iNewValue)
{
	int iHeight = GetButtonsHeight();
	SendMessage(m_hWndToolBar, TB_SETBUTTONSIZE, 0, iHeight * 65536 + iNewValue);
}

int CBToolBar::GetButtonCurrentHotIndex()
{
	return SendMessage(m_hWndToolBar, TB_GETHOTITEM, 0, 0);
}

void CBToolBar::SetButtonCurrentHotIndex(int iIndex)
{
	SendMessage(m_hWndToolBar, TB_SETHOTITEM, iIndex, 0);
}

int CBToolBar::ButtonsCount()
{
	return SendMessage(m_hWndToolBar, TB_BUTTONCOUNT, 0, 0);
}

int CBToolBar::GetRows()
{
	return SendMessage(m_hWndToolBar, TB_GETROWS, 0, 0);
}

void CBToolBar::SetRows(int iNewValue)
{
	RECT rc;	//���ڽ��մ����µ�һ��֮���µı߿�Χ
	SendMessage(m_hWndToolBar, TB_SETROWS, iNewValue, (LPARAM)&rc);
}

BOOL CBToolBar::GetWrappable()
{
	unsigned long r;
	r = (unsigned long)GetWindowLong(m_hWndToolBar, GWL_EXSTYLE);

	return (r & TBSTYLE_WRAPABLE);
}

void CBToolBar::SetWrappable(bool fCanWrappable)
{
	unsigned long r;
	r = (unsigned long)GetWindowLong(m_hWndToolBar, GWL_EXSTYLE);
	if (fCanWrappable)
		r = r | TBSTYLE_WRAPABLE;
	else
		r = r & ~TBSTYLE_WRAPABLE;

	SetWindowLong(m_hWndToolBar, GWL_EXSTYLE, (LONG)r);
}

COLORREF CBToolBar::GetColorBtnHighlight()
{

	
	COLORSCHEME colorInfo;
	colorInfo.dwSize = sizeof(COLORSCHEME);
	SendMessage(m_hWndToolBar, TB_GETCOLORSCHEME, 0, (LPARAM)&colorInfo);
	return colorInfo.clrBtnHighlight;
}

void CBToolBar::SetColorBtnHighlight(COLORREF cNewColor)
{

	
	COLORSCHEME colorInfo;
	colorInfo.dwSize = sizeof(COLORSCHEME);
	colorInfo.clrBtnHighlight = cNewColor;
	colorInfo.clrBtnShadow = GetColorBtnShadow();
	SendMessage(m_hWndToolBar, TB_SETCOLORSCHEME, 0, (LPARAM)&colorInfo);
}

COLORREF CBToolBar::GetColorBtnShadow()
{

	
	COLORSCHEME colorInfo;
	colorInfo.dwSize = sizeof(COLORSCHEME);
	SendMessage(m_hWndToolBar, TB_GETCOLORSCHEME, 0, (LPARAM)&colorInfo);
	return colorInfo.clrBtnShadow;
}

void CBToolBar::SetColorBtnShadow(COLORREF cNewColor)
{

	
	COLORSCHEME colorInfo;
	colorInfo.dwSize = sizeof(COLORSCHEME);
	colorInfo.clrBtnHighlight = GetColorBtnHighlight();
	colorInfo.clrBtnShadow = cNewColor;
	SendMessage(m_hWndToolBar, TB_SETCOLORSCHEME, 0, (LPARAM)&colorInfo);
}

BOOL CBToolBar::GetAnchorHighlight()
{

	
	return SendMessage(m_hWndToolBar, TB_GETANCHORHIGHLIGHT, 0, 0);
}

void CBToolBar::SetAnchorHighlight(bool fNewValue)
{

	
	SendMessage(m_hWndToolBar, TB_SETANCHORHIGHLIGHT, fNewValue, 0);
}

CBHashLK CBToolBar::m_hashToolBarIDs;










