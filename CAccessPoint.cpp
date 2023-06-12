#include "CAccessPoint.h"
CAccessPoint::CAccessPoint()
{
	this->m_nSTAReceivedData = new int[STACOUNT];
	for (int i = 0;i < STACOUNT;i++)
		m_nSTAReceivedData[i] = 0;	// 每个STA接收的数据为0
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