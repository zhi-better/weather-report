//////////////////////////////////////////////////////////////////////
// CBHashStrK: �ַ����ͼ�ֵ�Ĺ�ϣ�����ʵ��
//
//////////////////////////////////////////////////////////////////////

#include "BHashStrK.h"
#include <memory.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////
// Static ������ֵ
//////////////////////////////////////////////////////////////////////
const int   CBHashStrK::mcIniMemSize = 7;				// ��ʼ mem[] �Ĵ�С
const int   CBHashStrK::mcMaxItemCount = 100000000;		// ���Ԫ�ظ������������ֵ�� long ��ʾ�ķ�Χ֮�ڣ�
const float CBHashStrK::mcExpandMaxPort = 0.75;			// ����Ԫ�ظ������� 0.75*memCount ʱ������ mem[] �Ŀռ�
const int   CBHashStrK::mcExpandCountThres = 10000;		// ���� mem[] �ռ�ʱ���� memCount С�ڴ�ֵ��ÿ������ memCount*2���� memCount ���ڴ�ֵ��ÿ������ Count+Count/2
const int   CBHashStrK::mcExpandCountThresMax = 10000000;	// ���� mem[] �ռ�ʱ���� memCount �Ѵ��ڴ�ֵ����ÿ�β������� Count+Count/2����ֻ���� Count+mcExpandBigPer
const int   CBHashStrK::mcExpandBigPer = 1000000;			// ���� mem[] �ռ�ʱ���� memCount �Ѵ��� mcExpandCountThresMax����ÿ�β������󵽵� Count+Count/2����ֻ���� Count+mcExpandBigPer
const int   CBHashStrK::mcExpandMem2Per = 10;				// ÿ������ mem2[] �Ĵ�С
const int   CBHashStrK::mcSeqMax = 5;						// ˳��������ֵ


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBHashStrK::CBHashStrK(int memSize/*=0*/)
{
	mArrTable=0; mArrTableCount = -1;	// �޻�������

	memUsedCount = 0; 
	mem2 = 0;
	memCount2 = 0;
    memUsedCount2 = 0;
	KeyCaseSensitive=false;			// �Ƿ� key Ҫ���ִ�Сд��Ĭ�ϲ��ִ�Сд��

	if (memSize)
	{
		// ��ʼ���� memSize ���� mem[] �ռ䣬�� mem2[] �Ŀռ�
		RedimArrMemType(mem, memSize);
		memCount = memSize;
	}
	else
	{
		// ��ʼ���� mcIniMemSize ���� mem[] �ռ䣬�� mem2[] �Ŀռ�
		RedimArrMemType(mem, mcIniMemSize);
		memCount = mcIniMemSize;
	}
}

CBHashStrK::~CBHashStrK()
{
	Clear();

	// Clear() �����У����¿����˳�ʼ��С�� mem[] �ռ䣬�ٽ���ɾ������
	if (mem) delete[] mem;
	memCount = 0;
}



//////////////////////////////////////////////////////////////////////
// ���з���
//////////////////////////////////////////////////////////////////////

void CBHashStrK::AlloMem(int memSize )
{
	/*
	�����ʼ��ʱֻ������ mcIniMemSize ��С�� l_Mem[]���Ժ���ʹ�����Զ��� \
	  �󣻵�������֪���ж�󣬿������ñ����������㹻�������Ժ󲻶� \
	  �Զ������ʱ��ע����ʱҪ��Ԥ�õ�Ԫ�ظ����ඨ��һЩ���������ռ� \
	  ʱ���ռ��ͻ���໹���Զ�����
	�˺���Ҳ�����ڽض� l_Mem[] ����û��ʹ�õĿռ�
      ע��memSize <= memUsedCount ʱ���ܾ����¶��壬��ȷ�����ݲ��ᶪʧ 
	*/

	if (memSize <= memUsedCount || memSize > mcMaxItemCount) return;
	int iPreMemCount;
	iPreMemCount = memCount;

	// ReDim Preserve mem(1 To memSize)
	RedimArrMemType(mem, memSize, memCount, true);
	memCount = memSize;

	if (iPreMemCount <= memCount) ReLocaMem(iPreMemCount); else ReLocaMem(memCount);

	// ��ϣ�������ָ������
	mTravIdxCurr = 0;

	// �� Index ���ʸ�Ԫ�صĻ�����������
    mArrTableCount = -1;
}

bool CBHashStrK::Add( int data, LPCTSTR key/*=0*/, long dataLong/*=0*/, long dataLong2/*=0*/, LPCTSTR dataStr/*=NULL*/, LPCTSTR dataStr2/*=NULL*/, double dataDouble/*=0.0*/, bool raiseErrorIfNotHas/*=true*/ )
{
	int idx;
	
	// ��ϣ���е����ݸ�����಻�ܳ��� mcMaxItemCount
	if (memUsedCount + memUsedCount2 >= mcMaxItemCount)
    {		
		if (raiseErrorIfNotHas)  throw (unsigned char)7;	// �����ڴ�
		return false;
	}
	
	// ��ǰ��ϣ���в�������ͬ�ġ���������
	if (IsKeyExist(key))
	{
		if (raiseErrorIfNotHas)  throw (unsigned char)5;	// ��Ч�Ĺ��̵��û����
		return false;
	}
	
	// ͨ������ AlloMemIndex �������һ�����ÿռ���±꣺idx
	idx = AlloMemIndex(key);
	
	if (idx > 0)
	{
		// ��õ��±�ֵΪ����ʱ��ʹ�� mem[] ����Ŀռ�
		mem[idx].Data = data;
		mem[idx].DataLong = dataLong;
		mem[idx].DataLong2 = dataLong2;
		mem[idx].DataDouble = dataDouble;

		SaveItemString(&(mem[idx].Key), key);		// ���� key �ַ�������̬����һ���ռ������棬�¿ռ��ַ���浽 mem[idx].Key����� mem[idx].Key ����ֵ��Ϊ 0���ú����������� delete mem[idx].Key ����ָ��Ŀռ�Ĺ���
		SaveItemString(&(mem[idx].DataStr), dataStr);
		SaveItemString(&(mem[idx].DataStr2), dataStr2);
		
		mem[idx].Used = true;

		memUsedCount = memUsedCount + 1;
	}
	else if (idx < 0)
	{
		// ��õ��±�ֵΪ����ʱ��ʹ�� mem2[] ����Ŀռ䣬_
		// �ռ��±�Ϊ idx �ľ���ֵ
		mem2[-idx].Data = data;
		mem2[-idx].DataLong = dataLong;
		mem2[-idx].DataLong2 = dataLong2;
		mem2[-idx].DataDouble = dataDouble;

		SaveItemString(&(mem2[-idx].Key), key);		// ���� key �ַ�������̬����һ���ռ������棬�¿ռ��ַ���浽 mem[idx].Key����� mem[idx].Key ����ֵ��Ϊ 0���ú����������� delete mem[idx].Key ����ָ��Ŀռ�Ĺ���
		SaveItemString(&(mem2[-idx].DataStr), dataStr);
		SaveItemString(&(mem2[-idx].DataStr2), dataStr2);

		mem2[-idx].Used = true;

		memUsedCount2 = memUsedCount2 + 1;
	}
	else // idx == 0
	{
		if (raiseErrorIfNotHas)  throw (unsigned char)9;	// �±�Խ�磺�޷����������ݿռ�
	}
	
	// ��ϣ�������ָ������
	mTravIdxCurr = 0;
	
	// �� Index ���ʸ�Ԫ�صĻ�����������
    mArrTableCount = -1;

	// �������سɹ�
    return true;
}


int CBHashStrK::Item( LPCTSTR key, bool raiseErrorIfNotHas/*=true*/ )
{
	int idx;
	idx = GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (idx > 0)
		return mem[idx].Data;
	else if (idx < 0)
		return mem2[-idx].Data;
	else
		return 0;

}


long CBHashStrK::ItemLong( LPCTSTR key, bool raiseErrorIfNotHas/*=true*/ )
{
	int idx;
	idx = GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (idx > 0)
		return mem[idx].DataLong;
	else if (idx < 0)
		return mem2[-idx].DataLong;
	else
		return 0;

}


long CBHashStrK::ItemLong2( LPCTSTR key, bool raiseErrorIfNotHas/*=true*/ )
{
	int idx;
	idx = GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (idx > 0)
		return mem[idx].DataLong2;
	else if (idx < 0)
		return mem2[-idx].DataLong2;
	else
		return 0;

}

double CBHashStrK::ItemDouble( LPCTSTR key, bool raiseErrorIfNotHas/*=true*/ )
{
	int idx;
	idx = GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (idx > 0)
		return mem[idx].DataDouble;
	else if (idx < 0)
		return mem2[-idx].DataDouble;
	else
		return 0;

}

LPTSTR CBHashStrK::ItemStr( LPCTSTR key, bool raiseErrorIfNotHas/*=true*/ )
{
	int idx;
	idx = GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (idx > 0)
		return mem[idx].DataStr;
	else if (idx < 0)
		return mem2[-idx].DataStr;
	else
		return 0;

}

LPTSTR CBHashStrK::ItemStr2( LPCTSTR key, bool raiseErrorIfNotHas/*=true*/ )
{
	int idx;
	idx = GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (idx > 0)
		return mem[idx].DataStr2;
	else if (idx < 0)
		return mem2[-idx].DataStr2;
	else
		return 0;

}


// �ж�һ�� Key �Ƿ��ڵ�ǰ�����д���
bool CBHashStrK::IsKeyExist( LPCTSTR key )
{
	int idx;
	idx = GetMemIndexFromKey(key, false);
    return (idx != 0);
}

bool CBHashStrK::Remove( LPCTSTR key, bool raiseErrorIfNotHas/*=True*/ )
{
	int idx;
	
	// ���� GetMemIndexFromKey ������á�����Ϊ Key ���������ڿռ���±�
	idx = GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (idx == 0)
		return false;
	else if (idx > 0)
	{
		// ��ϣ���С�����Ϊ Key �������� mem[] �����У��±�Ϊ idx
		mem[idx].Used = false;
		SaveItemString(&(mem[idx].Key), 0);	// �ڶ�������Ϊ0��ɾ�� mem[idx].Key ָ��Ŀռ䣬���� mem[idx].Key=0;
		SaveItemString(&(mem[idx].DataStr), 0); 
		SaveItemString(&(mem[idx].DataStr2), 0); 

		memUsedCount = memUsedCount - 1;
	}
	else
	{
		// idx<0 ��ʾ����ϣ���С�����Ϊ Key �������� mem2[] ������ \
		// �±�Ϊ idx �ľ���ֵ
		// ɾ���±�Ϊ��-idx����Ԫ��
		SaveItemString(&(mem[-idx].Key), 0);	// �ڶ�������Ϊ0��ɾ�� mem[-idx].Key ָ��Ŀռ䣬���� mem[-idx].Key=0;
		SaveItemString(&(mem[-idx].DataStr), 0); 
		SaveItemString(&(mem[-idx].DataStr2), 0); 

		for(int i=-idx; i<= memUsedCount2-1; i++)
			mem2[i] = mem2[i+1];
		mem2[memUsedCount2].Key=0;			// ֱ������Ϊ0����˿ռ��ַ�Ѿ����ݸ���һԪ��
		mem2[memUsedCount2].DataStr=0;		// ֱ������Ϊ0����˿ռ��Ѿ����ݸ���һԪ��
		mem2[memUsedCount2].DataStr2=0;		// ֱ������Ϊ0����˿ռ��Ѿ����ݸ���һԪ��

		memUsedCount2 = memUsedCount2 - 1;
	}

	// ��ϣ�������ָ������
	mTravIdxCurr = 0;

	// �� Index ���ʸ�Ԫ�صĻ�����������
    mArrTableCount = -1;

	// �������سɹ�
	return true;
}

void CBHashStrK::StartTraversal()
{
	// ��ʼ�� NextXXX ... ��������
    mTravIdxCurr = 1;
}


int CBHashStrK::NextItem( bool &bRetNotValid )
{
	// ���� StartTraversal ���ô˺������� Data
    // �� bRetNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int idx;
	idx = TraversalGetNextIdx();
	if (idx > 0)
	{
		bRetNotValid = false;
		return mem[idx].Data;
	}
	else if (idx < 0)
	{
		bRetNotValid = false;
		return mem2[-idx].Data;
	}
	else
	{
		bRetNotValid = true;
		return 0;
	}
}

long CBHashStrK::NextItemLong( bool &bRetNotValid )
{
	// ���� StartTraversal ���ô˺������� DataLong
    // �� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int idx;
	idx = TraversalGetNextIdx();
	if (idx > 0)
	{
		bRetNotValid = false;
		return mem[idx].DataLong;
	}
	else if (idx < 0)
	{
		bRetNotValid = false;
		return mem2[-idx].DataLong;
	}
	else
	{
		bRetNotValid = true;
		return 0;
	}
}



long CBHashStrK::NextItemLong2( bool &bRetNotValid )
{
	// ���� StartTraversal ���ô˺������� DataLong
    // �� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int idx;
	idx = TraversalGetNextIdx();
	if (idx > 0)
	{
		bRetNotValid = false;
		return mem[idx].DataLong2;
	}
	else if (idx < 0)
	{
		bRetNotValid = false;
		return mem2[-idx].DataLong2;
	}
	else
	{
		bRetNotValid = true;
		return 0;
	}
}

double CBHashStrK::NextItemDouble( bool &bRetNotValid )
{
	// ���� StartTraversal ���ô˺������� DataLong
    // �� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int idx;
	idx = TraversalGetNextIdx();
	if (idx > 0)
	{
		bRetNotValid = false;
		return mem[idx].DataDouble;
	}
	else if (idx < 0)
	{
		bRetNotValid = false;
		return mem2[-idx].DataDouble;
	}
	else
	{
		bRetNotValid = true;
		return 0;
	}
}

LPTSTR CBHashStrK::NextItemStr( bool &bRetNotValid )
{
	// ���� StartTraversal ���ô˺������� DataLong
    // �� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int idx;
	idx = TraversalGetNextIdx();
	if (idx > 0)
	{
		bRetNotValid = false;
		return mem[idx].DataStr;
	}
	else if (idx < 0)
	{
		bRetNotValid = false;
		return mem2[-idx].DataStr;
	}
	else
	{
		bRetNotValid = true;
		return 0;
	}
}

LPTSTR CBHashStrK::NextItemStr2( bool &bRetNotValid )
{
	// ���� StartTraversal ���ô˺������� DataLong
    // �� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int idx;
	idx = TraversalGetNextIdx();
	if (idx > 0)
	{
		bRetNotValid = false;
		return mem[idx].DataStr2;
	}
	else if (idx < 0)
	{
		bRetNotValid = false;
		return mem2[-idx].DataStr2;
	}
	else
	{
		bRetNotValid = true;
		return 0;
	}
}


// ���ص��ַ���ָ�룬ֱ��ָ�����ڲ��� key ���ݣ����ע�⣺�����޸��˸�ָ��ָ������ݣ�
//   ���������ݵ� key Ҳͬʱ�����仯
LPTSTR CBHashStrK::NextKey( bool &bRetNotValid )
{
	// ���� StartTraversal ���ô˺������� String
    // �� bRetEndNotValid ���� True����˴α����ѽ�������ʱ��������ֵҲ��Ч��
	int idx;
	idx = TraversalGetNextIdx();
	if (idx > 0)
	{
		bRetNotValid = false;
		return mem[idx].Key;
	}
	else if (idx < 0)
	{
		bRetNotValid = false;
		return mem2[-idx].Key;
	}
	else
	{
		bRetNotValid = true;
		return 0;
	}
}

// �������Ԫ�أ��ض��� mcIniMemSize ���洢�ռ�
void CBHashStrK::Clear( void )
{
	// ���	�� Index ���ʸ�Ԫ�صĻ�������
	if (mArrTable) {delete []mArrTable; mArrTable=0;}
	mArrTableCount=-1;

	// ɾ�� mem[] �� mem2[] �е�ÿ��Ԫ�ص� key �� �ַ������� ָ��Ŀռ�
	int i;
	for (i=1; i<=memCount; i++)
	{
		if (mem[i].Key) {delete [] mem[i].Key; mem[i].Key=0; }
		if (mem[i].DataStr) {delete [] mem[i].DataStr; mem[i].DataStr=0; }
		if (mem[i].DataStr2) {delete [] mem[i].DataStr2; mem[i].DataStr2=0; }
	}
	for (i=1; i<=memCount2; i++)
	{
		if (mem2[i].Key) {delete [] mem2[i].Key; mem2[i].Key=0; }
		if (mem2[i].DataStr) {delete [] mem2[i].DataStr; mem2[i].DataStr=0; }
		if (mem2[i].DataStr2) {delete [] mem2[i].DataStr2; mem2[i].DataStr2=0; }
	}

	// ɾ�� mem[] �� mem2[] �Ŀռ�
	delete [] mem; mem=0;
	delete [] mem2; mem2=0;
	memCount = 0; memUsedCount = 0;
	memCount2 = 0; memUsedCount2 = 0;

	// ���¿��ٿռ�
	RedimArrMemType(mem, mcIniMemSize, memCount, false);
	memCount = mcIniMemSize;

	mTravIdxCurr = 0;
}


// ���ع���Ԫ�ظ���
int CBHashStrK::Count( void )
{
	return memUsedCount + memUsedCount2;
}




//////////////////////////////////////////////////////////////////////
// ˽�з���
//////////////////////////////////////////////////////////////////////

void CBHashStrK::ReLocaMem( int preMemCountTo )
{
	/*
	���·��� mem[], mem2[] �ĸ�Ԫ�صĿռ䣬mem2[] ��ĳЩԪ�ؿ��ܱ� \
	�����ƶ��� mem
	���޸� memUsedCount,memUsedCount2, memCount2, mem2[] ��ֵ
	preMemCountTo ֻ���� mem[1 to preMemCountTo]��preMemCountTo �Ժ��Ԫ�ر���Ϊ \
	δ�ã������ǣ���������ζ����� mem2[] �е�����Ԫ��
	*/
	
	// �� mem[] �е���ʹ��Ԫ�غ� mem2[] �е�����Ԫ���ȷ��� memUsed[] �У� \
	// �� memUsed[] ����Ϊ�㹻��ʵ�� memUsed[] ֻʹ���� lngUsedCount ��Ԫ��
	MemType * memUsed;
	RedimArrMemType(memUsed, preMemCountTo + memUsedCount2);

	int iUsedCount=0;		
	int i;

	// �� mem[] ����ʹ�õ�Ԫ�ش��� memUsed[]
	for (i=1; i<=preMemCountTo; i++)
		if (mem[i].Used)
		{
			iUsedCount = iUsedCount + 1;
			memUsed[iUsedCount] = mem[i];
		}

	// �� mem2[] �е�����Ԫ�ش��� memUsed[]
	for (i=1; i<=memUsedCount2; i++)
	{
		iUsedCount = iUsedCount + 1;
		memUsed[iUsedCount] = mem2[i];
	}


	/*
	��ʱ memUsed[1 To lngUsedCount] ��Ϊ���� mem[] \
	�е���ʹ��Ԫ�� �� mem2[] �е�����Ԫ��
	*/

	// ��� mem��Ҳ��������� Used ��
	RedimArrMemType(mem, memCount, memCount, false); 
	memUsedCount=0;  // memUsedCount ��0���������ƶ���+1

	// ��� mem2��Ҳ��������� Used ��
	RedimArrMemType(mem2, -1, memCount2, false);
	memCount2 = 0;
	memUsedCount2 = 0; 

	// ����� memUsed[1 To lngUsedCount] �е�Ԫ�ذ��������Сӳ���±���� mem[]
	int idx;
	for (i=1; i<=iUsedCount; i++)
	{
		idx = AlloMemIndex(memUsed[i].Key, false);
		if (idx > 0)
		{
			mem[idx] = memUsed[i];
			mem[idx].Used = 1;
			memUsedCount = memUsedCount + 1;
		}
		else
		{
			mem2[-idx] = memUsed[i];
			mem2[-idx].Used = 1;
			memUsedCount2 = memUsedCount2 + 1;
		}
	}

	// ɾ����ʱ�ռ� memUsed
	delete [] memUsed; memUsed=0;

	// ��ϣ�������ָ������
	mTravIdxCurr = 0;
	
	// �� Index ���ʸ�Ԫ�صĻ�����������
    mArrTableCount = -1;
}


// �ض��� mem[] �����С������ mem[] �Ŀռ�
void CBHashStrK::ExpandMem( void )
{
	int iCount, iPreMemCount;

	// �����ϣ���й�����������
	iCount = memUsedCount + memUsedCount2;

	// ȡ�����������������͡���ǰ mem[] �Ŀռ����������ߵĽϴ�ֵ
	if (iCount < memCount) iCount = memCount;

	// ���������ռ�֮ǰ�ġ�ԭ���� mem[] �Ŀռ�����
	iPreMemCount = memCount;

	if (iCount<1) iCount=1;		// ���� iCount Ϊ0ʱ���޷�����ռ�
	if (iCount < mcExpandCountThres)
	{
		// ��������������Ƚ��١����������ռ�Ϊԭ����2��
		iCount = iCount * 2;

	}
	else if (iCount < mcExpandCountThresMax)
	{
		// ������������Ѿ����е�ࡱ���������ռ�Ϊԭ����1.5��
		iCount = iCount * 3 / 2;
	}
	else
	{
		// ��������������ܶࡱ�������� mcExpandBigPer ���ռ�
		iCount = iCount + mcExpandBigPer;
	}

	// �ض��������С
	// ReDim Preserve mem(1 To lngCount)
	RedimArrMemType(mem, iCount, memCount, true);
	memCount = iCount;

	// ���������С�����°��������������ݵ���λ�ã�������Ҫ����
	// �����ռ�֮ǰ�ġ�ԭ���� mem[] �Ŀռ�����
	ReLocaMem(iPreMemCount);

	// ��ϣ�������ָ������
	mTravIdxCurr = 0;
	
	// �� Index ���ʸ�Ԫ�صĻ�����������
    mArrTableCount = -1;
}


// ������ϣ�������ݴ��� mArrTable()������ mArrTableCount Ϊ���ݸ��������سɹ���ʧ�ܣ�
bool CBHashStrK::RefreshArrTable()
{
	int iCount;
	int i,j;
	
	// �����ϣ���й�����������
	iCount = memUsedCount + memUsedCount2;
	
	mArrTableCount=iCount;
	if (mArrTableCount<=0) return false;
	
	if (mArrTable) {delete []mArrTable; mArrTable=0;}
	mArrTable=new int [iCount+1];	// ʹ�����±��1��ʼ
	memset(mArrTable, 0, sizeof(int)*(iCount+1));
	
	j=1;
	for (i=1; i<=memCount; i++)
	{
		if (mem[i].Used)
		{
			if (j > iCount) return false;
			
			mArrTable[j] = i;		// �� ��ϣ��ı��������ڵ� mem[] ���±� i ���� mArrTable[j]
			mem[i].Index = j;		// �� ��ϣ��ı����� �� Index ��Ա�м�¼ mArrTable ���±� j
			
			j=j+1;
		}
	}
	
	for (i=1; i<=memUsedCount2; i++)
	{
		if (mem2[i].Used)
		{
			if (j > iCount) return false;
			
			mArrTable[j] = -i;		// �� ��ϣ��ı��������ڵ� mem2[] ���±� i ��ȡ�������� mArrTable[j]
			mem[i].Index = j;		// �� ��ϣ��ı����� �� Index ��Ա�м�¼ mArrTable ���±� j
			
			j=j+1;
		}
	}
	
	return true;
}


int CBHashStrK::AlloSeqIdx( int fromIndex, int toIndex )
{
    /*
	 �� mem[] ��һ��ûʹ�õĿռ䣬�� fromIndex ��ʼ�� \
		�� toIndex ����
	 ���� mem[] ��һ��ûʹ��Ԫ�ص��±꣬û�ҵ����� 0
	*/
	int i;
	if (fromIndex <= 0)  fromIndex = 1;
	if (toIndex > memCount) toIndex = memCount;
	
	for (i=fromIndex; i<=toIndex; i++)
		if (! mem[i].Used) return i; 

	return 0;
}

int CBHashStrK::AlloMemIndex( LPCTSTR key, bool CanExpandMem/*=true */ )
{
	/* 
	  ���� Key ����һ�� mem[] �е�δ�ô洢�ռ䣬���� mem[] �����±�
		���ظ�ֵ������ mem[] ���ҵ��ռ䣺����ֵ�ľ���ֵΪ mem2[] �� \
		��һ�������±�ռ䣨mem2[]�Զ�Redim�����Դ��� mem2[]
	  ������ȷ������һ����ʹ�õĿռ䣬���������� mem2[] �еĿռ�
	  �����������޸� memUsedCount2 ��ֵ���� redim mem2[]
		CanExpandMem=true ʱ�����������Զ����� mem[]�����򲻻��Զ�����
		
		���������ַ����� key ת��Ϊ���������Ϊ������ȡ����ֵ��Ȼ��
		1. ���� Key Mod memCount + 1���� Index -> idxMod
		2. �������Ԫ���Ѿ�ʹ�ã���Key�Ƿ� < cMaxNumForSquare (sqr(2^31)=46340) \
		�� <����ƽ�� Key��Ȼ�� mod memCount + 1�� \
		�� >=�����ð�λ����λ���㣬����� key �ĸ������֣�Ȼ�� mod memCount + 1
		��������������˲� Index -> idxSq
		3. �� memCount-idxMod+1 -> idxModRev
		4. �� memCount-idxSq+1 -> idxSqRev
		5. �������ҵ��� Index ����ʹ���ˣ��� Count �Ƿ� > \
		mcExpandMaxPort*Count�����ǣ��� CanExpandMem=true�� \
		������ mem[] �Ĵ洢�ռ䣬Ȼ��ݹ鱾���̣��ظ� 1-4 ��
		6. �� idxMod+1,+2,...,+mcSeqMax���� idxMod-1,-2,...,-mcSeqMax
		7. ��û�У����ظ�ֵ������ֵΪ mem2[] ����һ�����ÿռ䣬�Դ��� mem2[]
	*/


	const int cMaxNumForSquare = 46340;
	
	int idxMod=0, idxSq=0;
    int idxModRev=0, idxSqRev=0;
    int iCount=0;
    long keyToCalc = KeyStringToLong(key); // ������ Key����ԶΪ>0����
    if (keyToCalc < 0) keyToCalc = 0 - keyToCalc;	// ��� Key �Ǹ�ֵ����ת��Ϊ�����������Ĵ洢�ռ�
    iCount = memUsedCount + memUsedCount2;
    
	if (memCount) 
	{	
		// 1: ���� Key Mod memCount + 1���� Index -> idxMod
		idxMod = keyToCalc % memCount + 1;
		if (! mem[idxMod].Used) return idxMod;
    
		// 2: �� ƽ��Key ���ٳ���ȡ�࣬�� Index -> idxSq
		if (keyToCalc <= cMaxNumForSquare)
		{
			idxSq = (keyToCalc * keyToCalc) % memCount + 1;
		}
		else
		{
			int kBitSum=0;
			kBitSum = (keyToCalc & 0xFFFF0000)>>16;
			kBitSum += (keyToCalc & 0xFF00)>>8;
			kBitSum += (keyToCalc & 0xF0)>>4;
			kBitSum += (keyToCalc & 0xF);
			idxSq = kBitSum % memCount + 1;
		}
		if (! mem[idxSq].Used) return idxSq;

		// 3: ���Ե����� idxMod ���ռ� -> idxModRev
		idxModRev = memCount - idxMod + 1; 
		if (! mem[idxModRev].Used) return idxModRev;
    
		// 4: ���Ե����� idxSq ���ռ� -> idxSqRev
		idxSqRev = memCount - idxSq + 1;
		if (! mem[idxSqRev].Used) return idxSqRev;
	}
    
    // 5: ����ռ�ʹ�ðٷֱȳ�����ֵ�������� mem[] �� �ռ�
    if (CanExpandMem && iCount > mcExpandMaxPort * memCount)
	{
		ExpandMem();  // ���� mem[] �Ŀռ�
		return AlloMemIndex(key, CanExpandMem); // �ݹ飬�ظ�1-4��
	}
        
    
    int lngRetIdx;
    
    // 6: �� idxMod ��ʼ��ǰ������������� mcSeqMax ���ռ�
    int idxMdSta, idxMdEnd; 
    idxMdSta = idxMod - mcSeqMax; idxMdEnd = idxMod + mcSeqMax;
    lngRetIdx = AlloSeqIdx(idxMdSta, idxMod - 1); 
    if (lngRetIdx > 0) return lngRetIdx;
    lngRetIdx = AlloSeqIdx(idxMod + 1, idxMdEnd);
    if (lngRetIdx > 0) return lngRetIdx; 
    

    // 7: ���ظ�ֵ������ֵΪ mem2[] ����һ��Ԫ�أ��Դ��� mem2[]
    if (memUsedCount2 + 1 > memCount2)
    {    
        // ReDim Preserve mem2(1 To mcExpandMem2Per)
		RedimArrMemType(mem2, memCount2 + mcExpandMem2Per, memCount2, true);
		memCount2 = memCount2 + mcExpandMem2Per;
	}

    return -(memUsedCount2 + 1);
}


int CBHashStrK::FindSeqIdx( LPCTSTR key, int fromIndex, int toIndex )
{
	
    // �� mem[] �м�ΪKey��Ԫ���±꣬�� fromIndex ��ʼ�� \
	//	�� toIndex ����
	//	���� mem[] ���ҵ������±꣨>0����û�ҵ����� 0

	int i;
	if (fromIndex < 1) fromIndex = 1;
	if (toIndex > memCount) toIndex = memCount;

	for (i=fromIndex; i<=toIndex; i++)
		if ((mem[i].Used) && CompareKey(mem[i].Key, key) )
			return i;
	
	return 0;
}




int CBHashStrK::TraversalGetNextIdx( void )
{
	// �� NextXXX ��������ʱ��������һ����Next���� mem[]�±꣨����ֵ>0���� \
	// �� mem2[] ���±꣨����ֵ<0�������ѱ�������������ֵ=0��
	
	int iRetIdx;
	
	if (mTravIdxCurr > memCount ||
		-mTravIdxCurr > memCount2 ||
		mTravIdxCurr == 0) return 0;
	
	if (mTravIdxCurr > 0)
	{
		//////////// �� mem[] ���� ////////////
		while (! mem[mTravIdxCurr].Used)
		{
			mTravIdxCurr = mTravIdxCurr + 1;
			if (mTravIdxCurr > memCount) break;
		}
		
		if (mTravIdxCurr > memCount)
		{
			//// �ѱ������������� mem2[] �л������ݼ������� mem2[] ////
			if (memCount2 > 0)
			{
				// �����´α��� mem2[] �����ݵ��±�ĸ���
				mTravIdxCurr = -1;
				// ִ������� if mTravIdxCurr < 0 Then 
			}
			else
			{
				// ���ؽ���
				iRetIdx = 0;
				return iRetIdx;
			}
		}
		else
		{
			//// ���� mTravIdxCurr ////
			iRetIdx = mTravIdxCurr;
			// �����´α���ָ�� ָ����һ��λ�ã����� mem[] ����һ���� \
			// ���� mem2[] ����ʼ��
			mTravIdxCurr = mTravIdxCurr + 1;
			if (mTravIdxCurr > memCount) if (memCount2 > 0) mTravIdxCurr = -1;
			return iRetIdx;
		}
	}
	
	if (mTravIdxCurr < 0)
	{
		//////////// �� mem2[] ���� ////////////
		while (! mem2[-mTravIdxCurr].Used)
		{	
			mTravIdxCurr = mTravIdxCurr - 1;
			if (-mTravIdxCurr > memCount2) break;
		}
		
		if (-mTravIdxCurr > memCount2)
		{
			//// �ѱ������� ////
			// ���ؽ���
			iRetIdx = 0; 
		}
		else
		{
			// ���ظ�ֵ�� mTravIdxCurr
			iRetIdx = mTravIdxCurr;
			// ���� mTravIdxCurr ��ָ��
			mTravIdxCurr = mTravIdxCurr - 1;
		}
		return iRetIdx;
	}
	
	return 0;
}




// �ض��� һ�� MemType ���͵����飨������� mem[] �� mem2[]���Ĵ�С���¶���ռ��Զ�����
// arr��Ϊ����ָ�룬�ɴ��ݣ�mem �� mem2�����������޸Ĵ�ָ���ָ��
// toUBound��ΪҪ�ض����������Ͻ磬����Ϊ��[0] to [toUBound]��Ϊ -1 ʱ�����ٿռ䣬������ɾ��ԭ
//	 �ռ䣬�� arr �ᱻ��Ϊ0
// uboundCurrent��Ϊ�ض���ǰ������Ͻ� [0] to [uboundCurrent]��Ϊ -1 ��ʾ��δ���ٹ��ռ�Ϊ��һ�ε���
// preserve����������ԭʼ���ݷ��򲻱���
// �����¿ռ��ϱ꣬�� toUBound
int CBHashStrK::RedimArrMemType( MemType * &arr, int toUBound/*=-1*/, int uboundCurrent/*=-1*/, bool preserve/*=false*/ )
{
	// �����¿ռ䣺[0] to [toUBound]
	if (toUBound >= 0)
	{
		MemType * ptrNew = new MemType [toUBound + 1];		// +1 Ϊʹ�����±���� toUBound
		// �¿ռ�����
		memset(ptrNew, 0, sizeof(MemType)*(toUBound + 1));
		
		// ��ԭ�пռ����ݿ������¿ռ�
		if (preserve && arr!=0 && uboundCurrent>=0)
		{
			int ctToCpy;										// ����ԭ�����ݣ���Ҫ�����ڴ�� MemType Ԫ�ظ���
			ctToCpy = uboundCurrent;
			if (uboundCurrent>toUBound) ctToCpy = toUBound;		// ȡ uboundCurrent �� toUBound ����Сֵ
			ctToCpy = ctToCpy + 1;								// ���� +1����Ϊ uboundCurrent �� toUBound ���������Ͻ�
			memcpy(ptrNew, arr, sizeof(MemType)*ctToCpy); 
		}

		// ɾ��ԭ�пռ�
		if (arr!=0 && uboundCurrent>=0) delete [] arr;

		// ָ��ָ���¿ռ�
		arr = ptrNew;
		return toUBound;
	}
	else		// if (toUBound < 0)�������ٿռ䣬ɾ��ԭ�пռ�
	{
		if(arr!=0 && uboundCurrent>=0) delete [] arr;
		arr = 0;
		return 0;
	}
	
}


// �� new �������ַ����ռ䣬�� ptrNewString ָ����ַ����������¿ռ䣻
//   ptrSaveTo ��һ�������ַ�����ַ��ָ������ĵ�ַ����ָ���ָ�����������
//   ���� new ���ٵ����ַ����ռ�ĵ�ַ�������� ��*ptrSaveTo = �¿ռ��ַ��
// �����ͷš�*ptrSaveTo����ָ��Ŀռ�Ĺ���
//   �� ptrSaveTo �����ɱ����� &(mem[i].key) ��ָ���ָ�룻ptrNewString �ɱ������µ� key
//   ����ɡ�mem[i].key=key���Ĳ������������޸� mem[i].key �����ݣ�
//   ��ɾ������ָ��Ŀռ䣬������ָ���¿ռ�
// ��� key Ϊ��ָ�룬���ͷš�*ptrSaveTo����ָ��Ŀռ�
void CBHashStrK::SaveItemString( TCHAR ** ptrSaveTo, LPCTSTR ptrNewString )
{
	// ע�� ptrSaveTo �Ǹ�����ָ��
	if (ptrSaveTo==0) return;  // û�б����λ��
	
	// ��� ptrSaveTo ָ���ָ�������Ϊ����ָ�롱����ʾҪ����֮������
	//   ������һ����ǰ���ٵĿռ��ַ��Ӧ��ɾ����ǰ���ٵĿռ�
	if (*ptrSaveTo != 0) {delete [] (*ptrSaveTo); *ptrSaveTo=0; }

	if (ptrNewString)
	{
		// �����¿ռ䣬���� ptrNewString ����ַ������¿ռ�
		TCHAR * p = new TCHAR [_tcslen(ptrNewString)+1];
		_tcscpy(p, ptrNewString);
		
		// ʹ *ptrSaveTo ָ���¿ռ�
		*ptrSaveTo = p;
	}
}


// �� Key ��������� mem[] �е��±꣨����ֵ>0������ mem2[] �е��±꣨����ֵ<0���������� 0
int CBHashStrK::GetMemIndexFromKey( LPCTSTR key, bool raiseErrorIfNotHas/*=true*/ )
{
	const long cMaxNumForSquare = 46340;  // sqrt(2^31)=46340
    
    int idxMod=0, idxSq=0;
    int idxModRev=0, idxSqRev=0;
    long keyToCalc = KeyStringToLong(key); // ������ Key����ԶΪ>=0����
    if (keyToCalc < 0) keyToCalc = 0 - keyToCalc;

	if (memCount)
	{
		// 1: ���� Key Mod memCount + 1���� Index -> idxMod
		idxMod = keyToCalc % memCount + 1;
		if (mem[idxMod].Used && CompareKey(mem[idxMod].Key, key)) 
			return idxMod;
    
		// 2: �� ƽ��Key���ٳ���ȡ�࣬�� Index -> idxSq
		if (keyToCalc <= cMaxNumForSquare)
		{
			idxSq = (keyToCalc * keyToCalc) % memCount + 1;
		}
		else
		{
			int kBitSum=0;
			kBitSum = (keyToCalc & 0xFFFF0000)>>16;
			kBitSum += (keyToCalc & 0xFF00)>>8;
			kBitSum += (keyToCalc & 0xF0)>>4;
			kBitSum += (keyToCalc & 0xF);
			idxSq = kBitSum % memCount + 1;
		}
		if (mem[idxSq].Used && CompareKey(mem[idxSq].Key, key))
			return idxSq;
    
		// 3: ���Ե����� idxMod ���ռ� -> idxModRev
		idxModRev = memCount - idxMod + 1;
		if (mem[idxModRev].Used && CompareKey(mem[idxModRev].Key, key))
		   return idxModRev;
    
		// 4: ���Ե����� idxSq ���ռ� -> idxSqRev
		idxSqRev = memCount - idxSq + 1;
		if (mem[idxSqRev].Used && CompareKey(mem[idxSqRev].Key, key))
		   return idxSqRev;
	}

    int lngRetIdx;
    
    // 6: �� idxMod ��ʼ��ǰ������������� mcSeqMax ���ռ�
    int idxMdSta, idxMdEnd;
    idxMdSta = idxMod - mcSeqMax; idxMdEnd = idxMod + mcSeqMax;
    lngRetIdx = FindSeqIdx(key, idxMdSta, idxMod - 1);
    if (lngRetIdx > 0)  return  lngRetIdx;
    lngRetIdx = FindSeqIdx(key, idxMod + 1, idxMdEnd);
    if (lngRetIdx > 0)  return  lngRetIdx;

    // 7: �ٲ鿴 mem2[] �е�Ԫ����û��
    for (int i=1; i<=memUsedCount2; i++)
        if (mem2[i].Used && CompareKey(mem2[i].Key, key)) return -i;
	
	if (raiseErrorIfNotHas) throw (unsigned char)5;	// ��Ч�Ĺ��̵��û����
	return 0;
}

// �� index ��������� mem[] �е��±꣨����ֵ>0������ mem2[] �е��±꣨����ֵ<0���������� 0
int CBHashStrK::GetMemIndexFromIndex( int index, bool raiseErrorIfNotHas/*=true*/ )
{
	if (mArrTableCount != memUsedCount + memUsedCount2) RefreshArrTable(); // ˢ�����黺��
	if (index<1 || index>mArrTableCount)
	{
		if (raiseErrorIfNotHas)  throw (unsigned char)5;	// ��Ч�Ĺ��̵��û����
		return 0;
	}
	
	int idx=mArrTable[index];
	if (idx==0) 
	{
		if (raiseErrorIfNotHas)  throw (unsigned char)7;	// �����ڴ棨mArrTable[index]����Ϊ0��
		return 0;
	}
	else
		return idx;
}


// ��һ�� �ַ������͵� key ת��Ϊһ��������
// �����ǣ�ȡǰ16���ַ�����16���ַ�������32���ַ���ASCII�������ͷ���
long CBHashStrK::KeyStringToLong( LPCTSTR key )
{
	// �����ǣ�ȡǰ16���ַ�����16���ַ�������32���ַ���ASCII�������ͷ���
    const int c_CalcCharNumHead = 16;
    const int c_CalcCharNumTail = 16;

	long lResult=0;
	int iHeadToPos=0;	// ʵ��Ҫȡǰ�����ַ�
	int iTailToPos=0;	// ʵ��Ҫȡ�󼸸��ַ�
	int i;

	TCHAR * keyLoc = new TCHAR[_tcslen(key)+1];		// ������ key
	_tcscpy(keyLoc, key);							// ���� key �ַ������¿��ٵĿռ����������� key ����Ҫ������ KeyCaseSensitive ����ת����Сд
	if (! KeyCaseSensitive)  _tcsupr(keyLoc);		// ��������ִ�Сд���� key ת��Ϊ ��д
	
	// ��ʼ���ۼ� ASCII ��Ľ��
    lResult = 0;

	// �ۼ�ǰ c_CalcCharNumHead ���ַ��� ASCII �루���� c_CalcCharNumHead �������ַ�������Ϊֹ��
	iHeadToPos = c_CalcCharNumHead;
	if ((int)_tcslen(keyLoc) < iHeadToPos) iHeadToPos = _tcslen(keyLoc);
	for(i=0; i<iHeadToPos; i++)
		lResult += *(keyLoc+i);

	// �ۼӺ� c_CalcCharNumTail ���ַ��� ASCII �루���� c_CalcCharNumTail �������ַ�������Ϊֹ��
	iTailToPos = c_CalcCharNumTail;
	if ((int)_tcslen(keyLoc) - iHeadToPos < iTailToPos) iTailToPos = _tcslen(keyLoc) - iHeadToPos;
	for(i = (int)_tcslen(keyLoc) - iTailToPos; i<(int)_tcslen(keyLoc); i++)
		lResult += *(keyLoc+i);

	// �ͷſռ�
	delete []keyLoc;


	// ���ؽ�����ۼӵ� ASCII ��
	return lResult;
}


// ���� lKeyCaseSensitive ���ԣ��Ƚ��� key ���ַ����Ƿ����
// ��ȷ���True�����ȷ���False
bool CBHashStrK::CompareKey(LPCTSTR key1, LPCTSTR key2 )
{
	if (key1==0 || key2==0) return false;
	if (KeyCaseSensitive)
		return lstrcmp(key1, key2)==0;
	else
	{
		return lstrcmpi(key1, key2)==0;
	}
}


// ---------------- �� Index ������������(����Key����KeyΪֻ��) ----------------
// ע������������ɾ��Index ���ܻ�仯��ĳ���ݵ� Index ����������һһ��Ӧ

int CBHashStrK::ItemFromIndex( int index, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii > 0)
		return mem[ii].Data;
	else if (ii < 0)
		return mem2[-ii].Data;
	else
		return 0;
}

long CBHashStrK::ItemLongFromIndex( int index, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii > 0)
		return mem[ii].DataLong;
	else if (ii < 0)
		return mem2[-ii].DataLong;
	else
		return 0;
}

long CBHashStrK::ItemLong2FromIndex( int index, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii > 0)
		return mem[ii].DataLong2;
	else if (ii < 0)
		return mem2[-ii].DataLong2;
	else
		return 0;		
}

double CBHashStrK::ItemDoubleFromIndex( int index, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii > 0)
		return mem[ii].DataDouble;
	else if (ii < 0)
		return mem2[-ii].DataDouble;
	else
		return 0;
}

LPTSTR CBHashStrK::ItemStrFromIndex( int index, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii > 0)
		return mem[ii].DataStr;
	else if (ii < 0)
		return mem2[-ii].DataStr;
	else
		return 0;	
}

LPTSTR CBHashStrK::ItemStr2FromIndex( int index, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii > 0)
		return mem[ii].DataStr2;
	else if (ii < 0)
		return mem2[-ii].DataStr2;
	else
		return 0;
}

LPTSTR CBHashStrK::IndexToKey( int index, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii > 0)
		return mem[ii].Key;
	else if (ii < 0)
		return mem2[-ii].Key;
	else
		return 0;		
}


int CBHashStrK::KeyToIndex( LPCTSTR key, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	if (mArrTableCount != memUsedCount + memUsedCount2) RefreshArrTable(); // ˢ�����黺��
	ii=GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (ii > 0)
		return mem[ii].Index;
	else if (ii < 0)
		return mem2[-ii].Index;
	else
		return 0;		
}



// ---------------- �� Key ������������ ----------------

bool CBHashStrK::ItemSet( LPCTSTR key, int vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		mem[ii].Data = vNewValue;
	else if (ii < 0)
		mem2[-ii].Data = vNewValue;
	return true;	
}

bool CBHashStrK::ItemLong2Set( LPCTSTR key, long vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		mem[ii].DataLong2 = vNewValue;
	else if (ii < 0)
		mem2[-ii].DataLong2 = vNewValue;
	return true;	
}

bool CBHashStrK::ItemLongSet( LPCTSTR key, long vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		mem[ii].DataLong = vNewValue;
	else if (ii < 0)
		mem2[-ii].DataLong = vNewValue;
	return true;	
}

bool CBHashStrK::ItemDoubleSet( LPCTSTR key, double vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		mem[ii].DataDouble = vNewValue;
	else if (ii < 0)
		mem2[-ii].DataDouble = vNewValue;
	return true;	
}

bool CBHashStrK::ItemStrSet( LPCTSTR key, LPCTSTR vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		SaveItemString(&(mem[ii].DataStr), vNewValue);
	else if (ii < 0)
		SaveItemString(&(mem2[-ii].DataStr), vNewValue);
	return true;	
}

bool CBHashStrK::ItemStr2Set( LPCTSTR key, LPCTSTR vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromKey(key, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		SaveItemString(&(mem[ii].DataStr2), vNewValue);
	else if (ii < 0)
		SaveItemString(&(mem2[-ii].DataStr2), vNewValue);
	return true;	
}


// ---------------- �� Index ������������(KeyΪֻ����������Key) ----------------
// ע������������ɾ��Index ���ܻ�仯��ĳ���ݵ� Index ����������һһ��Ӧ

bool CBHashStrK::ItemFromIndexSet( int index, int vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		mem[ii].Data = vNewValue;
	else if (ii < 0)
		mem2[-ii].Data = vNewValue;
	return true;	
}

bool CBHashStrK::ItemLongFromIndexSet( int index, long vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		mem[ii].DataLong = vNewValue;
	else if (ii < 0)
		mem2[-ii].DataLong = vNewValue;
	return true;	
}

bool CBHashStrK::ItemLong2FromIndexSet( int index, long vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		mem[ii].DataLong2 = vNewValue;
	else if (ii < 0)
		mem2[-ii].DataLong2 = vNewValue;
	return true;	
}

bool CBHashStrK::ItemDoubleFromIndexSet( int index, double vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		mem[ii].DataDouble = vNewValue;
	else if (ii < 0)
		mem2[-ii].DataDouble = vNewValue;
	return true;	
}

bool CBHashStrK::ItemStrFromIndexSet( int index, LPCTSTR vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		SaveItemString(&(mem[ii].DataStr), vNewValue);
	else if (ii < 0)
		SaveItemString(&(mem2[-ii].DataStr), vNewValue);
	return true;	
}

bool CBHashStrK::ItemStr2FromIndexSet( int index, LPCTSTR vNewValue, bool raiseErrorIfNotHas/*=true*/ )
{
	int ii;
	ii=GetMemIndexFromIndex(index, raiseErrorIfNotHas);
	if (ii == 0)
		return false;
	else if (ii > 0)
		SaveItemString(&(mem[ii].DataStr2), vNewValue);
	else if (ii < 0)
		SaveItemString(&(mem2[-ii].DataStr2), vNewValue);
	return true;
}





