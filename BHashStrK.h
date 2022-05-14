
// ===============================================================
// CBHashStrK���ַ����� key �Ĺ�ϣ����
// ֧��ÿ��Ԫ������ 1 �� long �͵����ݡ� 2 �� long �͵ĸ������ݡ�
//    1 �� double �͵ĸ������� �� 2 �� �ַ����ĸ�������
//
// �����׳����쳣��
// throw (unsigned char)7;	// �����ڴ�
// throw (unsigned char)5;	// ��Ч�Ĺ��̵��û��������ֵ����
//                          // ����ͼ����Ѵ��ڵ�ͬ����ֵ����Ԫ�أ����ʼ������ڵ�Ԫ��
// throw (unsigned char)9;	// �±�Խ�磺�޷����������ݿռ�
//
// ---- ����ʹ�� Index �ķ�ʽ�������й�ϣ��Ԫ�� ----
//     for (i = 1; i<=hash.Count; i++)
//         cout<<hash.KeyFromIdx(i)
//     
//     for (i = 1; i<=hash.Count; i++)
//         cout<<hash.ItemFromIdx(i)
//     
// ע�� Index ���������ݶ�Ӧ��������ɾ�����ţ����ݵ� Index �����ܻ�仯
// ����ͬһʱ�̣�Index ��ͬ��һ�����ݣ�Key,Data,DataLong,DataString����ͬһ��
// ===============================================================

#pragma once
#pragma warning(disable:4996) // �ر� warning C4996: �� 'wcscpy': This function or variable may be unsafe. Consider using wcscpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.

#include <windows.h>
#include <tchar.h>



class CBHashStrK
{
private:
	typedef struct _MemType
	{
		LPTSTR Key;			// ָ���������� new ���ٵ�һ���ڴ�����ÿ�� mem[] �ڱ�ɾ��ʱ���� delete [] .key
		int Data;
		long DataLong;
		long DataLong2;
		double DataDouble;
		LPTSTR DataStr;
		LPTSTR DataStr2;
		bool Used;
		int Index;		// mArrTable[] ����� mArrTable[index] Ԫ�أ��Ǳ��汾 MemType ����
						//   ���ڵ� mem[] �е��±꣨index>0������ mem2[] �е��±꣨index<0��
						//   mArrTableCount == memUsedCount + memUsedCount2 ʱ�� index !=0 ʱ ��Ч
						//   �� RefreshArrTable �����ô˳�Ա

	} MemType;
	
	static const int	mcIniMemSize;  			// ��ʼ mem[] �Ĵ�С
	static const int	mcMaxItemCount; 		// ���Ԫ�ظ������������ֵ�� long ��ʾ�ķ�Χ֮�ڣ�
	static const float	mcExpandMaxPort;  		// ����Ԫ�ظ������� 0.75*memCount ʱ������ mem[] �Ŀռ�
	static const int	mcExpandCountThres; 	// ���� mem[] �ռ�ʱ���� memCount С�ڴ�ֵ��ÿ������ memCount*2���� memCount ���ڴ�ֵ��ÿ������ Count+Count/2
	static const int	mcExpandCountThresMax;	// ���� mem[] �ռ�ʱ���� memCount �Ѵ��ڴ�ֵ����ÿ�β������� Count+Count/2����ֻ���� Count+mcExpandBigPer
	static const int	mcExpandBigPer; 		// ���� mem[] �ռ�ʱ���� memCount �Ѵ��� mcExpandCountThresMax����ÿ�β������󵽵� Count+Count/2����ֻ���� Count+mcExpandBigPer
	static const int	mcExpandMem2Per; 		// ÿ������ mem2[] �Ĵ�С
	static const int	mcSeqMax; 				// ˳��������ֵ

private:
	MemType * mem;					// ��̬����ָ�룬�����鲻ʹ�� [0] ��Ԫ��
	int memCount, memUsedCount;		// ��̬��������±꣬mem[] �����±�Ϊ [0] �� [memCount]����ϣ������Ԫ�ظ���
	MemType * mem2;					// �ռ��ͻ��Ԫ�صı���ռ䣬˳��ʹ��
	int memCount2, memUsedCount2;	// mem2[] �����±�Ϊ [0] �� [memCount2]������ [0] ���ã���ʹ�ÿռ�Ϊ [1] �� [memUsedCount2]
	int mTravIdxCurr;				// �� NextXXX ���������ĵ�ǰ index����ֵ��ʾ mem[] �е��±꣬��ֵ��ʾ mem2[] �е��±�
	
	// ֧��ͨ�������±� Index ����һ����ϣ�����ݣ�mArrTable ָ��̬���飬
	//   ����Ԫ�ر��棺���й�ϣ������������˳�����ڵ� mem[] �е��±꣨>0��
	//   �� mem2[] �е��±ꡣ
	// ����һ�Σ������й�ϣ������ mem[] �� mem2[] ���±���ڴ����飬
	//   �Ժ����ظ�������ֱ��ͨ�������±� Index ����һ����ϣ�����ݡ�
	//   mArrTableCount != memUsedCount + memUsedCount2 Ϊ��־��
	//   �� !=����־Ҫ����ˢ�±�����ע�����±�����Ԫ��˳����ܻ����š�
	//   ����ϣ�����ݵ� Index ������һֱ�����
	int * mArrTable;
	int mArrTableCount;

public:
	bool KeyCaseSensitive;			// �Ƿ� key Ҫ���ִ�Сд��Ĭ�ϲ��ִ�Сд��

public:
	CBHashStrK(int memSize=0);		// memSize=0 �򿪱ٳ�ʼ mcIniMemSize ���ռ䣬���򿪱� memSize ���ռ䣬memSize Ӧ��ʵ�����ݸ�����һЩ
	~CBHashStrK();
	void AlloMem(int memSize);		// ���ȿ��ô˺��������㹻��Ŀռ䣬�Լ����Ժ��Զ�����ռ�Ĵ��������Ч��
	bool Add(int data, LPCTSTR key=0, long dataLong=0, long dataLong2=0, LPCTSTR dataStr=NULL, LPCTSTR dataStr2=NULL, double dataDouble=0.0, bool raiseErrorIfNotHas=true);	// ���Ԫ��
	bool Remove(LPCTSTR key, bool raiseErrorIfNotHas=true);					// ɾ��Ԫ��
	
	// ���� key ���Ԫ�ء���������
	int Item(LPCTSTR key, bool raiseErrorIfNotHas=true);
	long ItemLong(LPCTSTR key, bool raiseErrorIfNotHas=true);
	long ItemLong2(LPCTSTR key, bool raiseErrorIfNotHas=true);
	double ItemDouble(LPCTSTR key, bool raiseErrorIfNotHas=true);
	LPTSTR ItemStr(LPCTSTR key, bool raiseErrorIfNotHas=true);
	LPTSTR ItemStr2(LPCTSTR key, bool raiseErrorIfNotHas=true);

	// ���� key ����Ԫ�ء���������
	bool ItemSet(LPCTSTR key, int vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemLongSet(LPCTSTR key, long vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemLong2Set(LPCTSTR key, long vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemDoubleSet(LPCTSTR key, double vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemStrSet(LPCTSTR key, LPCTSTR vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemStr2Set(LPCTSTR key, LPCTSTR vNewValue, bool raiseErrorIfNotHas=true);


	// ���� index ���Ԫ�ء���������
	int ItemFromIndex(int index, bool raiseErrorIfNotHas=true);	
	long ItemLongFromIndex(int index, bool raiseErrorIfNotHas=true);
	long ItemLong2FromIndex(int index, bool raiseErrorIfNotHas=true);
	double ItemDoubleFromIndex(int index, bool raiseErrorIfNotHas=true);
	LPTSTR ItemStrFromIndex(int index, bool raiseErrorIfNotHas=true);
	LPTSTR ItemStr2FromIndex(int index, bool raiseErrorIfNotHas=true);

	// ���� index ����Ԫ�ء��������ݣ����������� Key��KeyΪֻ����
	bool ItemFromIndexSet(int index, int vNewValue, bool raiseErrorIfNotHas=true);	
	bool ItemLongFromIndexSet(int index, long vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemLong2FromIndexSet(int index, long vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemDoubleFromIndexSet(int index, double vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemStrFromIndexSet(int index, LPCTSTR vNewValue, bool raiseErrorIfNotHas=true);
	bool ItemStr2FromIndexSet(int index, LPCTSTR vNewValue, bool raiseErrorIfNotHas=true);

	LPTSTR IndexToKey(int index, bool raiseErrorIfNotHas=true);
	int KeyToIndex(LPCTSTR key, bool raiseErrorIfNotHas=true);


	bool IsKeyExist(LPCTSTR key);			// �ж�ĳ�� key ��Ԫ���Ƿ����
	void Clear(void);						// �������Ԫ�أ��ض��� mcIniMemSize ���洢�ռ�
	
	void StartTraversal();					// ��ʼһ����������
	int NextItem(bool &bRetNotValid);		// �������̿�ʼ�󣬲��ϵ��ô˺��������ÿ��Ԫ�أ�ֱ�� bRetNotValid ���� true
	long NextItemLong(bool &bRetNotValid);	// �������̿�ʼ�󣬲��ϵ��ô˺��������ÿ��Ԫ�صĸ������ݣ�ֱ�� bRetNotValid ���� true
	long NextItemLong2(bool &bRetNotValid);
	double NextItemDouble(bool &bRetNotValid);
	LPTSTR NextItemStr(bool &bRetNotValid);
	LPTSTR NextItemStr2(bool &bRetNotValid);

	LPTSTR NextKey(bool &bRetNotValid);		// �������̿�ʼ�󣬲��ϵ��ô˺��������ÿ��Ԫ�ص� key��ֱ�� bRetNotValid ���� true

	int Count(void);						// ���ع���Ԫ�ظ���


private:
	int AlloMemIndex(LPCTSTR Key, bool CanExpandMem=true );		// ���� Key ����һ�� mem[] �е�δ�ô洢�ռ䣬���� mem[] �����±�
	int FindSeqIdx(LPCTSTR key, int fromIndex, int toIndex);	// �� mem[] �м�Ϊ key ��Ԫ���±꣬�����ҿռ��±�Ϊ�� fromIndex ��ʼ���� toIndex �����Ŀռ�
	void ReLocaMem(int preMemCountTo);							// ���·��� mem[], mem2[] �ĸ�Ԫ�صĵ�ַ��mem2[] ��ĳЩԪ�ؿ��ܱ������ƶ��� mem[]
	void ExpandMem(void);										// �ض��� mem[] �����С������ mem[] �Ŀռ�
	int TraversalGetNextIdx(void);								// �� NextXXX ��������ʱ��������һ����Next���� mem[]�±꣨����ֵ>0������ mem2[] ���±꣨����ֵ<0�������ѱ�������������ֵ=0��
	int AlloSeqIdx(int fromIndex, int toIndex);					// �� mem[] ��һ��ûʹ�õĿռ䣬�����ҿռ��±�Ϊ�� fromIndex ��ʼ���� toIndex �����Ŀռ�
	bool RefreshArrTable();										// ������ϣ���������±���� mArrTable[]������ mArrTableCount Ϊ���ݸ��������سɹ���ʧ�ܣ�

	int RedimArrMemType(MemType * &arr, int toUBound=-1, int uboundCurrent=-1, bool preserve=false);	// �ض��� һ�� MemType ���͵����飨������� lMem[] �� lMem2[]���Ĵ�С���¶���ռ��Զ�����

	int GetMemIndexFromKey(LPCTSTR key, bool raiseErrorIfNotHas=true);	// �� Key ��������� mem[] �е��±꣨����ֵ>0������ mem2[] �е��±꣨����ֵ<0���������� 0
	int GetMemIndexFromIndex(int index, bool raiseErrorIfNotHas=true);	// �� index ��������� mem[] �е��±꣨����ֵ>0������ mem2[] �е��±꣨����ֵ<0���������� 0

	long KeyStringToLong(LPCTSTR key);		// ��һ�� �ַ������͵� key ת��Ϊһ��������
	void SaveItemString(TCHAR ** ptrSaveTo, LPCTSTR ptrNewString);			// �� new �������ַ����ռ䣬�� key ָ����ַ����������¿ռ䣻ptrSaveTo ��һ�������ַ�����ַ��ָ������ĵ�ַ����ָ���ָ����������桰�� new ���ٵ����ַ����ռ�ĵ�ַ�������� ��*ptrSaveTo = �¿ռ��ַ��
	bool CompareKey(LPCTSTR key1, LPCTSTR key2);		// ���� KeyCaseSensitive ���ԣ��Ƚ��� key ���ַ����Ƿ���ȣ���ȷ���True�����ȷ���False
};

