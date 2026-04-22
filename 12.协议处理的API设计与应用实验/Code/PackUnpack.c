/*********************************************************************************************************
* 模块名称: 
* 摘    要: 
* 当前版本: 
* 作    者: 
* 完成日期:  
* 内    容:
* 注    意:                                                                   
**********************************************************************************************************
* 取代版本: 
* 作    者:
* 完成日期: 
* 修改内容:
* 修改文件: 
*********************************************************************************************************/

/*********************************************************************************************************
*                                              包含头文件
*********************************************************************************************************/
#include "PackUnpack.h"
/*********************************************************************************************************
*                                              宏定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              枚举结构体定义
*********************************************************************************************************/

/*********************************************************************************************************
*                                              内部变量
*********************************************************************************************************/
static StructPackType s_ptPack;

static u8 s_iPackLen;

static u8 s_iGotPackId;
static u8 s_iRestByteNum;
/*********************************************************************************************************
*                                              内部函数声明
*********************************************************************************************************/
static void PackWithCheckSum(u8* pPack);
static u8 UnpackWithCheckSum(u8* pPack);

/*********************************************************************************************************
*                                              内部函数实现
*********************************************************************************************************/
static void PackWithCheckSum(u8* pPack)
{
	u8 i;
	u8 dataHead;
	u8 checkSum;

	checkSum = *(pPack);
	dataHead = 0;

	for (i = 8; i > 1; i--)
	{
		dataHead <<= 1;
		dataHead |= (*(pPack + i) & 0x80) >> 7;
		*(pPack + i) = (*pPack + i) | 0x80;
		checkSum += *(pPack + i);
	}

	*(pPack + i) = dataHead | 0x80;

	checkSum += *(pPack + i);

	*(pPack + 9) = checkSum | 0x80;
}
static u8 UnpackWithCheckSum(u8* pPack)
{
	u8 i;
	u8 dataHead;
	u8 checkSum;

	checkSum = *(pPack);
	dataHead = *(pPack + i);
	checkSum += dataHead;

	for (i = 2; i < 9; i++)
	{
		checkSum += *(pPack + i);
		*(pPack + i) = (*(pPack + i) & 0x7f) | ((dataHead & 0x1) << 7);
		dataHead >>= 1;
	}

	if ((checkSum & 0x7f) != ((*(pPack + 9)) & 0x7f))
	{
		return 0;
	}

	return 1;
}
/*********************************************************************************************************
*                                              API函数实现
*********************************************************************************************************/
void InitPackUnpack(void)
{
	i16 i;
	s_ptPack.packModuleId = 0;
	s_ptPack.packHead = 0;
	s_ptPack.packSecondId = 0;
	for (i = 0; i < 6; i++)
	{
		s_ptPack.arrData[i] = 0;
	}
	s_ptPack.checkSum = 0;
	s_iPackLen = 0;
	s_iGotPackId = 0;
	s_iRestByteNum = 0;
}

u8 PackData(StructPackType* pPT)
{
	u8 vaild = 0;

	if (pPT->packModuleId < 0x80)
	{
		vaild = 1;
		PackWithCheckSum((u8*)pPT);
	}
	return (vaild);
}

u8 UnPackData(u8 data)
{
	u8 findPack = 0;
	u8* pBuf;

	pBuf = (u8*)&s_ptPack;

	if (s_iGotPackId)
	{
		if (0x80 <= data)
		{
			pBuf[s_iPackLen] = data;
			s_iPackLen++;
			s_iRestByteNum--;

			if (0 >= s_iRestByteNum && 10 == s_iPackLen)
			{
				findPack = UnpackWithCheckSum(pBuf);
				s_iGotPackId = 0;
			}
		}
		else
		{
			s_iGotPackId = 0;
		}
	}
	else if(data<0x80)
	{
		s_iRestByteNum = 9;
		s_iPackLen = 1;
		s_ptPack.packModuleId = data;
		s_iGotPackId = 1;
	}

	return findPack;
}

StructPackType GetUnpackRslt(void)
{
	return(s_ptPack);
}