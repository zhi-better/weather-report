#include <Windows.h>
#include <math.h>

// TBDrawTransParentBitmap ����͸��λͼ
// �� hDCSrc �е�һ�����ױ����컨ͼ���İ�ɫ������͸�������� hDCDest ��Ϊ��
// hDCDest ��ͼ��Ҫ���õ��ڴ滷���ľ��
// destX ��ͼҪ���õ� x ����
// destY ��ͼҪ���õ� y ����
// hDCSrc ��ͼ���ڴ滷���ľ��
// colorTransParent Ҫ����͸���������ͼ����ɫ
// srcX ����Դͼ����ĸ� X ���ֽ��н�ȡ
// srcY ����Դͼ����ĸ� Y ���ֽ��н�ȡ
// srcWidth Ҫ͸�������λͼ�Ŀ��
// srcHeight Ҫ͸����λͼ�ĸ߶�
void TBDrawTransParentBitmap( HDC hDCDest,
	int destX,
	int destY,
	HDC hDCSrc,
	COLORREF colorTransParent = 0,
	int srcX = 0,
	int srcY = 0,
	int srcWidth = -1,
	int srcHeight = -1
	);

// ��תλͼָ���ĽǶ�
// hDCDest ��ʾ��ͼ��Ҫ���õ��ڴ滷���ľ��
// destX ��ͼҪ���õ� x ����
// destY ��ͼҪ���õ� y ����
// hDCSrc ��ʾҪ��ת��λͼ���ڵ��豸�����ľ��
// angle ��ʾҪ��ת�ĽǶȣ�Ĭ���� �Ƕ��� ��
// cColor ָ���������ת���м�Ŀ�ȱ��ʲô��ɫ�����
// SrcWidth��SrcHeight Ҫ��ת��Դλͼ�Ŀ�Ⱥ͸߶�
// Ϊ�˸��ӷ���ʹ�ã��˴������������һ���ķ�װ��������ת����Բ�ķ�ʽ��ת��������������ת�ᵼ�������������
void TBRotateAnyAngle(HDC hDCDest, int DestX, int DestY, HDC hDCSrc, double angle=0, COLORREF cColor=RGB(255,255,255), int SrcWidth=-1, int SrcHeight=-1);
