#pragma once
#include "def.h"
class CAccessPoint
{
public:
	int* m_nSTAReceivedData; // 定义一个数组指针用于计算AP接收到每个STA成功发送的数据
public:
	CAccessPoint();
	~CAccessPoint();
	bool IsSuccsssfulRecevied(int staID,int data);
};

