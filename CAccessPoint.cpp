#include "CAccessPoint.h"
CAccessPoint::CAccessPoint()
{
	this->m_nSTAReceivedData = new int[STACOUNT];
	for (int i = 0;i < STACOUNT;i++)
		m_nSTAReceivedData[i] = 0;	// ÿ��STA���յ�����Ϊ0
}
CAccessPoint::~CAccessPoint()
{
	delete this->m_nSTAReceivedData;
	this->m_nSTAReceivedData = 0;
}
bool CAccessPoint::IsSuccsssfulRecevied(int staID, int data)
{
	
	
	return true;
}