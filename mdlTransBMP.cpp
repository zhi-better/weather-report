#include "mdlTransBMP.h"

// TBDrawTransParentBitmap ����͸��λͼ
// �� hDCSrc �е�һ�����ױ����컨ͼ���İ�ɫ������͸�������� hDCDest ��Ϊ��

void TBDrawTransParentBitmap( HDC hDCDest, 
							int destX, 
							int destY, 
							HDC hDCSrc, 
							COLORREF colorTransParent /*= 0*/, 
							int srcX /*= 0*/, 
							int srcY /*= 0*/, 
							int srcWidth /*= -1*/, 
							int srcHeight /*= -1 */ )
{
	// ===================== ȷ�� srcWidth �� srcHeight=========================
	// ��� srcWidth<0 ���� srcHeight<0 ����ʾʹ�� hDCSrc ��λͼ��ʵ�ʵĿ�͸�
	// �Զ���� hDCSrc ��λͼ��ʵ�ʿ�͸�
	if (srcHeight<0 || srcWidth<0)
	{
		// ��õ�ǰ�ı�ѡ��� hDCSrc �е�λͼ�ľ��
		HBITMAP hBmpSrc = (HBITMAP)GetCurrentObject(hDCSrc, OBJ_BITMAP);
		// ��ø�λͼ�����ʵ�ʵĿ�͸�
		BITMAP bmpInfo;
		memset(&bmpInfo, 0, sizeof(bmpInfo));
		GetObject(hBmpSrc, sizeof(BITMAP), &bmpInfo);
		if(srcHeight<0)	srcHeight = bmpInfo.bmHeight;
		if(srcWidth<0)	srcWidth = bmpInfo.bmWidth;
	}

	// ==================== ��������λͼ���ڻ��ױ����� ===============================
	// �������ݵģ���������λͼ���ڴ��豸���� hDCMask
	HDC hDCMask = CreateCompatibleDC(hDCSrc);
	// �� hDCMask �а���һ����ԭͼ��ͬ����С��λͼ��������ͼ�壩
	HBITMAP hBmpMask = CreateCompatibleBitmap(hDCSrc, srcWidth, srcHeight);
	HBITMAP hBmpMaskOld = (HBITMAP)SelectObject(hDCMask, hBmpMask);

	// ------------------------------------------------------------------------------
	// ������ʱ�ģ����ݵģ��������ڵ�ɫλͼ�� �豸���� �� ��ɫλͼ
	// ���±���Ϊ��������λͼ����ʱ��ɫλͼ���豸����
	HDC hDCMono = CreateCompatibleDC(hDCSrc);
	// ������Դͼ��ͬ����С�ĵ�ɫλͼ
	HBITMAP hBmpMono = CreateBitmap(srcWidth, srcHeight, 1, 1, NULL);
	// ����ɫλͼѡ�� hBmpMono
	// �õ�ɫλͼ���� hBmpMono �豸�����ġ���ͼ�塱�����������ͼ�����֧��
	// ��ɫ�������Ժ��� hBmpMono �л��Ƶ�ͼ���ǵ�ɫ��
	HBITMAP hBmpMonoOld = (HBITMAP)SelectObject(hDCMono, hBmpMono);

	// ��Դλͼ�Ը��Ƶķ�ʽ���Ƶ� hBmpMono���� hBmpMono ��
	// �����Դλͼ�ġ���ɫ������
	// Ҫ͸���Ĳ�����ɫ��Ϊ��ɫ���������ֱ�ɺ�ɫ���ڻ��ױ�����
	// �ڸ���֮ǰ��������Դλͼ�ı���ɫ colorTransParent�������Ļ�
	// Դλͼ���ƹ�ȥ colorTransParent ����ɫ���ǰ�ɫ
	COLORREF colorOlkBk = SetBkColor(hDCSrc, colorTransParent);
	BitBlt(hDCMono, 0, 0, srcWidth, srcHeight, hDCSrc, srcX, srcY, SRCCOPY);

	// ^^^^^^^^^^^^^^^^^^^^ ����λͼ�Ѿ��������� hDCMono �� ^^^^^^^^^^^^^^^^^^^^^
	// --------------------------------------------------------------------------

	// ���� hDCMono �е��ǵ�ɫλͼ�����Բ���ֱ�� BitBlt ��Ŀ�껷�� hDCDest ��
	// ����Ӧ�ý� hDCMono �е�λͼ���Ƶ� hDCMask �У�hDCMask ֧�ֲ�ɫλͼ��
	BitBlt(hDCMask, 0, 0, srcWidth, srcHeight, hDCMono, 0, 0, SRCCOPY);
	// ^^^^^^^^^^^^^^^^^^^^ ����λͼ�Ѿ��������� hDCMask �� ^^^^^^^^^^^^^^^^^^^^^
	// --------------------------------------------------------------------------

	// ɾ����ʱ���Ƶ� hDCMono �����е�λͼ
	SelectObject(hDCMono, hBmpMonoOld);
	DeleteObject(hBmpMono);
	DeleteDC(hDCMono);
	// ==========================================================================

	// ==================== ����͸��ɫΪ��ɫ��Դͼ�񣨺컨�ڱ�����===============
	// ���Դλͼ�е�͸������ɫ colorTransParent �������Ǻ�ɫ����ô���ش˴������
	// ������Ǻ�ɫ����Ҫ������Ϊ��ɫ������һ�����ݵ��ڴ��豸���� hDCModi 
	// �����з��á�͸��ɫ����Ϊ��ɫ�ġ�Դͼ��
	HDC hDCModi(NULL);	
	HBITMAP hBmpModi(NULL), hBmpModiOld(NULL);
	if (colorTransParent)	// if(colorTransParent != 0)����Ϊ��ɫ
	{
		// �������ݵ��豸���� hDCModi
		hDCModi = CreateCompatibleDC(hDCSrc);
		// Ϊ�豸���� hDCModi ���á���ͼ�塱����Դͼ��һ�µ�λͼ hBmpModi
		hBmpModi = CreateCompatibleBitmap(hDCSrc, srcWidth, srcHeight);
		hBmpModiOld = (HBITMAP)SelectObject(hDCModi, hBmpModi);

		// ��Դͼ���͸��ɫ��Ϊ��ɫ��ͼ��������δ��ɫ�������浽 hDCModi ��
		// ���Ƚ�����ɰ�ɫ��������Ϊ��ɫ��������ת hDCModi��Ȼ���Դͼ����� ADD����
		// ���ȣ��� hDCMask �еġ��ڻ��ױ�����NOTSRCCOPY �� hDCModi ��Ϊ���ڱ����׻���
		BitBlt(hDCModi, 0, 0, srcWidth, srcHeight, hDCMask, 0, 0, NOTSRCCOPY);
		// ��Դλͼ SRCAND �����ڱ����׻����У�ʹ�� hDCModi Ϊ���ڱ����׻���
		// �� SRCAND �󣬱���ɫ��Ϊ��ɫ��������ɫ�Ͱ�ɫ��֮����Ϊԭ������ɫ��
		BitBlt(hDCModi, 0, 0, srcWidth, srcHeight, hDCSrc, srcX, srcY, SRCAND);
	}
	// =========================================================================

	// ====================== ����͸����ͼ ======================================
	// ���ڣ�hDCMask �б����������λͼ�������ڻ��ױ���
	// hDCModi �б�����ǡ��ڱ����컨��
	// ����͸����ͼ�������ǣ�
	// ��1��Ŀ��� hDCMask ��������
	// ��2���ٺ� hDCModi ��������
	BitBlt(hDCDest, destX, destY, srcWidth, srcHeight, hDCMask, 0, 0, SRCAND);
	if (colorTransParent)
	{
		// ���Ҫ͸������ɫ���Ǻ�ɫ���Ǿ��üӹ���� hDCModi
		BitBlt(hDCDest, destX, destY, srcWidth, srcHeight, hDCModi, 0, 0, SRCPAINT);
	} 
	else
	{
		// ���Ҫ͸������ɫ�Ǻ�ɫ��ֱ���� hDCSrc
		BitBlt(hDCDest, destX, destY, srcWidth, srcHeight, hDCSrc, srcX, srcY, SRCPAINT);
	}
	// ========================================================================

	// ==================== �ͷ���Դ ==========================================
	// �ָ� hDCSrc �ı���ɫ
	SetBkColor(hDCSrc, colorOlkBk);

	//�ͷ���Դ
	SelectObject(hDCMask, hBmpMaskOld);
	DeleteObject(hBmpMask);
	DeleteDC(hDCMask);

	if (colorTransParent)	// if(colorTransParent != 0)����Ϊ��ɫ
	{
		SelectObject(hDCModi, hBmpModiOld);
		DeleteObject(hBmpModi);
		DeleteDC(hDCModi);
	}
}

void TBRotateAnyAngle(HDC hDCDest, int DestX, int DestY, HDC hDCSrc, double angle/*=0*/, COLORREF cColor/*=RGB(255,255,255)*/, int SrcWidth/*=-1*/,int SrcHeight/*=-1*/)
{
	//��ȡ���˴��룬���Ǻ��þͺ��ˣ�������

	//======================= �Զ���ø߶ȿ�� ===============================
	//����û�û������Դͼ��Ŀ�Ⱥ͸߶ȣ��Զ����
	if (SrcWidth<0 || SrcHeight<0)
	{
		// ��õ�ǰ�ı�ѡ��� hDCSrc �е�λͼ�ľ��
		HBITMAP hBmpSrc = (HBITMAP)GetCurrentObject(hDCSrc, OBJ_BITMAP);
		// ��ø�λͼ�����ʵ�ʵĿ�͸�
		BITMAP bmpInfo;
		memset(&bmpInfo, 0, sizeof(bmpInfo));
		GetObject(hBmpSrc, sizeof(BITMAP), &bmpInfo);
		if(SrcHeight<0)	SrcHeight = bmpInfo.bmHeight;
		if(SrcWidth<0)	SrcWidth = bmpInfo.bmWidth;
	}

	//==================== ��������ͼ��Ŀ�Ⱥ͸߶� ============================
	//һ��Ѽ��㣬���ˣ����뿴��
	double x1,x2,x3;
	double y1,y2,y3;
	double maxWidth, maxHeight, minWidth, minHeight;
	double srcX,srcY;
	double sinA,cosA;
	double DstWidth;
	double DstHeight;
	HDC dcDst;			//��ת����ڴ��豸����
	HBITMAP newBitmap;
	angle = angle/180.0*3.1415926;	//���Ƚ�������ת��Ϊ�Ƕ��Ʋ��У�����ʹ��
	sinA = sin(angle);
	cosA = cos(angle);
	x1 = -SrcHeight * sinA;
	y1 = SrcHeight * cosA;
	x2 = SrcWidth * cosA - SrcHeight * sinA;
	y2 = SrcHeight * cosA + SrcWidth * sinA;
	x3 = SrcWidth * cosA;
	y3 = SrcWidth * sinA;
	minWidth = x3>(x1>x2?x2:x1)?(x1>x2?x2:x1):x3;
	minWidth = minWidth>0?0:minWidth;
	minHeight = y3>(y1>y2?y2:y1)?(y1>y2?y2:y1):y3;
	minHeight = minHeight>0?0:minHeight;
	maxWidth = x3>(x1>x2?x1:x2)?x3:(x1>x2?x1:x2);
	maxWidth = maxWidth>0?maxWidth:0;
	maxHeight = y3>(y1>y2?y1:y2)?y3:(y1>y2?y1:y2);
	maxHeight = maxHeight>0?maxHeight:0;
	DstWidth = maxWidth - minWidth;
	DstHeight = maxHeight - minHeight;

	//====================== ��Ϸ��ʼ������������תλͼ�ĺ��Ĵ��� ============================
	//�����ڴ滷��
	dcDst = CreateCompatibleDC(hDCSrc);
	newBitmap = CreateCompatibleBitmap(hDCSrc,(int)DstWidth,(int)DstHeight);
	SelectObject(dcDst,newBitmap);
	//ѡ�뻭�ʣ�Ȼ����ڻ��Ʊ���ɫ
	HBRUSH hBrush = NULL;	HBRUSH hBrushOld = NULL;
	hBrush = CreateSolidBrush(cColor);
	hBrushOld = (HBRUSH)SelectObject(dcDst, hBrush);
	//��䱳��ɫ��������հף�������������м��п�ȱ
	RECT rc;
	rc.left=0;		rc.top=0;
	rc.right=(int)DstWidth;	rc.bottom=(int)DstHeight;
	FillRect(dcDst, &rc, hBrush);
	for( int I = 0 ;I<DstHeight;I++)
	{
		for(int J = 0 ;J< DstWidth;J++)
		{
			srcX = (J + minWidth) * cosA + (I + minHeight) * sinA;
			srcY = (I + minHeight) * cosA - (J + minWidth) * sinA;
			if( (srcX >= 0) && (srcX <= SrcWidth) &&(srcY >= 0) && (srcY <= SrcHeight))
			{
				BitBlt(dcDst, J, I, 1, 1, hDCSrc,(int)srcX, (int)srcY, SRCCOPY);
			}
		}
	}
	// =================================== ��ʾ��ת���λͼ ===============================
	//���� 1���ͻ������ĺ��ߣ��������м������������
	//�����������ݾ�������㷨�Ǽ������ת������ݷŴ�ŵ�һ����ľ������棬
	//���Ǵ˴��Ϳ���ͨ����������ʼ�ص����������ս�������ݣ��������̫������
	BitBlt(hDCDest,DestX,DestY,(int)SrcWidth,(int)SrcHeight,dcDst,
		int((DstWidth-SrcWidth)/2.0),int((DstHeight-SrcHeight)/2.0),
		SRCCOPY);
	

	//==================================== �ͷ���Դ ====================================
	SelectObject(dcDst, hBrushOld);
	DeleteObject(hBrush);
	DeleteObject(newBitmap);
	DeleteDC(dcDst);
}