//////////////////////////////////////////////////////////////////////
// BToolBar.h: CBToolBar 类的定义，用于操作一个工具栏项（事件除外）
//
// 支持：需要 BWindows 模块的支持
//////////////////////////////////////////////////////////////////////

#pragma once

#include "BWndBase.h"
#include <vector>
#include <list>
using namespace std;

//////////////////////////////////////////////////////////////////////////
//工具栏的内容的枚举成员

//系统自定义的工具栏图标位图
enum EToolBarSysIcons
{
	eSysIcon_None = -1,							//无图标
	eSysIcon_Cut = STD_CUT,						//剪切
	eSysIcon_Copy = STD_COPY,					//复制
	eSysIcon_Paste = STD_PASTE,					//粘贴
	eSysIcon_Undo = STD_UNDO,					//撤销
	eSysIcon_Redow = STD_REDOW,					//重做
	eSysIcon_Delete = STD_DELETE,				//删除
	eSysIcon_FileNew = STD_FILENEW,				//新建文件
	eSysIcon_FileOpen = STD_FILEOPEN,			//打开文件
	eSysIcon_FileSave = STD_FILESAVE,			//保存文件
	eSysIcon_Printpre = STD_PRINTPRE,			//
	eSysIcon_Properties = STD_PROPERTIES,		//
	eSysIcon_Help = STD_HELP,					//帮助
	eSysIcon_Find = STD_FIND,					//查找
	eSysIcon_Replace = STD_REPLACE,				//替换
	eSysIcon_Print = STD_PRINT					//打印
};
//按钮的不同风格
enum ETBBtnStyle  //与常量值相同
{
	eBtn_Default = TBSTYLE_BUTTON,			//（缺省的）按钮。按钮是一个规则的下压按钮。
	eBtn_Check = TBSTYLE_CHECK,				//复选。按钮是一个复选按钮，它可以被选定或者不被选定。
	eBtn_ButtonGroup = TBSTYLE_CHECKGROUP,	//按钮组。直到组内的另一个按钮被按下之前，按钮都保持按下的状态。在任何时刻都只能按下组内一个按钮。
	eBtn_Separator = TBSTYLE_SEP,            //分隔符。按钮的功能是作为有 8 个像素的固定宽度的分隔符。
											//tbtnPlaceholder 4 占位符。按钮在外观和功能上象分隔符，但具有可设置的宽度。占位符也用 tbtnSeparator 设置，但需将 bAutoSizeByText 设为 False ,之后用 ButtonWidth 属性设置其宽度
	eBtn_DropDown = TBSTYLE_DROPDOWN         //Creates a drop - down list button.Drop - down buttons send the TBN_DROPDOWN notification.

};
//水平文本对齐方式
enum ETBBtnTextAlign
{
	eBtn_TextLeft = DT_LEFT,
	eBtn_TextRight = DT_RIGHT,
	eBtn_TextCenter = DT_CENTER
};
//竖直文本对齐方式
enum ETBBtnTextAlignVert
{
	eBtn_TextTop = DT_TOP,
	eBtn_TextBottom = DT_BOTTOM,
	eBtn_TextVCenter = DT_VCENTER
};
//工具栏的位置
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

	//构造函数
	CBToolBar();
	CBToolBar(unsigned int idToolBar);
	//析构函数
	~CBToolBar();

	//============================= 设置工具栏函数 ==============================

	// 通过 id 返回对应的句柄，如果不存在，返回 NULL
	// 并且改变类内部的 m_hWndToolBar, 后面调用此类里面的所有内容都是针对此句柄的工具栏
	HWND SetHwndFromID(unsigned int idToolBar);

	//为窗口增加一个工具栏，idToolBar 表示工具栏对应的资源 ID
	// hWndParent 表示的是父窗口的句柄，主要是用于消息通知的窗口句柄
	// fIsRight 表示的是文本的显示位置，如果是 true ,文本位于按钮右边，若为 false, 显示在下面
	HWND ToolBarAdd(HWND hWndParent, EToolBarPos eTBPos=eTB_Top, bool fTextRight = false);

	//为工具栏增加一个按钮
	//iBitmap 表示的是位图的 ID 或者是对应的系统位图的枚举成员
	//szText 表示该按钮的提示文本内容，默认是在下面位置
	//iBtnStyle 表示的是按钮的风格，例如正常按钮，或者是 checked 类型的按钮等
	//iPostoAdd 表示的是增加按钮的位置，从 0 开始左侧插入按钮，如果是 -1，默认增加在最后位置
	//iCommand 为该按钮的命令编号，如果为 -1 表示默认由本函数安排
	//其中重载版本主要是针对系统位图和自定义位图而言，其他参数一致
	bool AddButton(EToolBarSysIcons iBitmap, LPCTSTR szText = L"",
		ETBBtnStyle iBtnStyle = eBtn_Default, DWORD iPostoAdd = -1, int iCommand = -1);
	bool AddButton(unsigned int idBitmap, LPCTSTR szText = L"",
		ETBBtnStyle iBtnStyle = eBtn_Default, DWORD iPostoAdd = -1, int iCommand = -1);

	//为工具栏增加一个位图到系统的图标中，返回的是该图片在系统的位图的索引号
	//并不会增加按钮，仅仅是增加位图，从而改变系统预定义的索引
	//返回值为新增加的位图的索引号数值
	int AddBitmapToSys(unsigned int idBmp);

	//增加一个分割线
	bool AddSeparator(int iCommand = -1);

	//通过按钮的 Command 移除一个按钮
	BOOL RemoveFromCommand(int iCommand);

	//通过按钮的 index 移除一个按钮，位置计算从 0 开始
	BOOL RemoveFromIndex(int iIndex = -1);

	//移动一个按钮到新的位置，位置计算从 0 开始
	BOOL MoveButton(int iIndexOld, int iIndexNew);

	//更新一下所有的按钮的大小，按照按钮内容自动确定大小
	void Resize();

	//如果(x,y)位于一个非分隔线的按钮上，返回所在按钮的 Index （ >0，Index 从 0 开始）
	//如果(x,y)不位于一个非分隔线的按钮上，返回<0的值，绝对值表示分隔线的 Index 或距离最近的非分隔线按钮的 Index
	int HitTest(int x, int y);

	//通过 Command 获得对应的 index
	int CommandToIndex(int iCommand);

	//通过 index 获得对应的 command
	int IndexToCommand(int iIndex);

	//设置第一个按钮前面的像素距离
	void SetFirstIndent(int iIndent);

	//设置按钮中的文本水平对齐方式
	void SetTextAlign(ETBBtnTextAlign iStyle);
	//设置按钮中的文本竖直对齐方式
	void SetTextAlignVert(ETBBtnTextAlignVert iStyle);

	//设置按钮中的文本是否可以多行显示
	void SetTextMultiline(bool fCanMultiline = true);

	//设置按钮的宽度最大值和最小值的范围
	BOOL SetWidthRange(int iMinRange, int iMaxRange);

	//设置工具栏的父窗口的句柄，主要用于消息的接收和处理
	BOOL SetParent(HWND hWnd);

	//^^^^^^^^^^^^^^^^^^^^^^^按钮属性的查看和更改^^^^^^^^^^^^^^^^^^^^^^^^

	//返回或重新修改一个按钮的图标（可设置为枚举成员值外的值，即直接为 ImageList 中的图像索引号）
	int GetButtonImage(int iCommand);
	void SetButtonImage(int iCommand, unsigned int iNewValue);
	void SetButtonImage(int iCommand, EToolBarSysIcons eNewValue);

	//返回和设置一个按钮是否被 Pressed （按下状态且背景色变为其他颜色）
	BOOL GetButtonPressed(int iCommand);
	void SetButtonPressed(int iCommand, bool fNewValue);

	//返回和设置一个按钮是否被 Checked （按下状态，背景色不变）
	BOOL GetButtonChecked(int iCommand);
	void SetButtonChecked(int iCommand, bool fNewValue);

	//返回和设置一个按钮是否被 Enabled
	BOOL GetButtonEnabled(int iCommand);
	void SetButtonEnabled(int iCommand, bool fNewValue);

	//返回或设置一个按钮的 Visible
	BOOL GetButtonVisible(int iCommand);
	void SetButtonVisible(int iCommand, bool fNewValue);

	//返回或设置：是否从一个按钮开始换行
	BOOL GetButtonLineBreak(int iCommand);
	void SetButtonLineBreak(int iCommand, bool fNewValue);

	//返回或设置：是否按钮是否处于灰度状态（灰度状态也有效可接收用户输入）
	BOOL GetButtonGrayed(int iCommand);
	void SetButtonGrayed(int iCommand, bool fNewValue);

	//返回或设置：是否按钮是否处于 Marked 状态
	BOOL GetButtonMarked(int iCommand);
	void SetButtonMarked(int iCommand, bool fNewValue);

	//返回或设置一个按钮的 Command 号，Index 下标从 0 开始
	int GetButtonCommand(int iIndex);
	void SetButtonCommand(int iIndex, int iNewCommand);

	//返回或设置一个按钮的文本
	tstring GetButtonText(int iCommand);
	void SetButtonText(int iCommand, LPCTSTR sNewText);

	//获得按钮的 top, left, height, width
	int GetButtonTop(int iCommand);
	int GetButtonLeft(int iCommand);
	int GetButtonHeight(int iCommand);
	int GetButtonWidth(int iCommand);
	RECT GetButtonRect(int iCommand);

	//设置按钮的 width
	BOOL SetButtonWidth(int iCommand, int iNewWidth);

	//^^^^^^^^^^^^^^^^^ 控件属性的查看和更改 ^^^^^^^^^^^^^^^^^^^^^^^

	//获得和设置工具栏的高度
	//和上面不同的是这个是设置全部的高度
	int GetButtonsHeight();
	void SetButtonsHeight(int iNewValue);

	//获得和设置工具栏的宽度
	//和上面不同的是这个是设置全部的宽度
	int GetButtonsWidth();
	void SetButtonsWidth(int iNewValue);

	//返回和设置当前拥有“热点”的按钮的 Index （下标从 0 开始），属性为 -1 表示无热点按钮
	int GetButtonCurrentHotIndex();
	void SetButtonCurrentHotIndex(int iIndex);

	//只读属性：返回按钮总数
	int ButtonsCount();

	//获得或设置按钮行数（控件必须具有 TBSTYLE_WRAPABLE 风格）
	int GetRows();
	void SetRows(int iNewValue);

	//是否按钮可自动换行
	BOOL GetWrappable();
	void SetWrappable(bool fCanWrappable);

	//返回和设置对应的工具栏的高亮显示的颜色
	COLORREF GetColorBtnHighlight();
	void SetColorBtnHighlight(COLORREF cNewColor);

	//返回和设置按钮对应的阴影的颜色
	COLORREF GetColorBtnShadow();
	void SetColorBtnShadow(COLORREF cNewColor);

	//返回和设置最后一个突出显示的项目将保持突出显示，直到另一个项目突出显示。
	//即使光标离开工具栏控件，也会出现这种情况。
	BOOL GetAnchorHighlight();
	void SetAnchorHighlight(bool fNewValue);

	//==========================================================================

private:

	//========================= 静态成员 =============================
	static CBHashLK m_hashToolBarIDs;		//哈希表：key = 工具栏的资源id，data = 该工具栏的句柄

	//========================= 私有成员 =================================
	//类内部保存的句柄
	HWND m_hWndToolBar;			//一个工具栏控件的句柄
	HWND m_hParentWnd;			//工具栏属于的父窗口的句柄
	unsigned int m_idToolBar;	//当前使用的工具栏的 id
	int m_iCommand;			//用于保存对应的工具栏的控件的 command
	int m_iSepCount;		//获得对应的分割线的数量，主要为了 iCommand 考虑
};








