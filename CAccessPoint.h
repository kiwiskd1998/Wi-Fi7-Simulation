#pragma once
#include "def.h"
class CAccessPoint
{
public:
	int* m_nSTAReceivedData; // ����һ������ָ�����ڼ���AP���յ�ÿ��STA�ɹ����͵�����
public:
	CAccessPoint();
	~CAccessPoint();
	bool IsSuccsssfulRecevied(int staID,int data);
};

