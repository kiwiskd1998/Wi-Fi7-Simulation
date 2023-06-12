#include "CStation.h"
#include <stdlib.h>


CStation::CStation()
{
	this->m_pFrame = nullptr;
	this->m_nSendData = 0;
	this->m_nID = 0;
	for (int i = 0;i < 4;i++) 
	{ 
		CW[i][CWmin] = 8; 
		CW[i][CWmax] = 16;
	}
	m_nAC = AC_VO;
	this->m_nBackoffNumer = 0;
	this->m_bIsLinkAggreationSuccessful = false;
	this->allTxData = 0;//�����ܵĵ�������
	applyforTXOPLen = 0;
}

CStation::~CStation()
{
	delete this->m_pFrame;
	this->m_pFrame = nullptr;
	
}

void CStation::init(int id) //�����ʼ���˱ܼ�����
{
	this->m_nBackoffNumer = rand() % 8;  
	this->m_nID = id;
	int tmp = rand() % 50;
	if (tmp > 0) 
	{
		allTxData += tmp;
		this->m_QSendData.push(tmp);//��ʼ����ÿ��STA��ǰ���е�������
	}
	

}
void CStation::backoff()
{
	if (this->m_nBackoffNumer > 0)
		this->m_nBackoffNumer--;
	//else this->ReBackoff();
}
int CStation::transmission(int FrameType)
{
	return this->m_pFrame->getFramelength(FrameType);
}
bool CStation::IsAblelinkAggregation(int TXOPLEN, int BUSYLEN)
{
	/*int twoPPDUTransmissionTime = SIFS + transmission(PPDU_LENGTH) / TXSPEED + SIFS + transmission(BLOCKACK) / TXSPEED + SIFS + transmission(PPDU_LENGTH) / TXSPEED + SIFS + transmission(BLOCKACK) / TXSPEED;
	if ((TXOPLEN - BUSYLEN) > twoPPDUTransmissionTime && (TXOPLEN - BUSYLEN - twoPPDUTransmissionTime) / (transmission(BLOCKACK) / TXSPEED) > 0) return true;*/
	return false;
}

void CStation::reTransferData()
{

}
int CStation::needTXOPLen(int dataSize)	 //���������һ��������С�޶�
{
	if (dataSize < DATA_LENGTH) 
	{
		this->m_nSendData = dataSize;
		this->m_QSendData.pop();
		applyforTXOPLen = HEADER + (dataSize / TXSPEED) / SLOT + SIFS + m_pFrame->getFramelength(BLOCKACK);
		return applyforTXOPLen;
	}
	int cnt = 1;
	while (dataSize > cnt * DATA_LENGTH) 
	{
		if (cnt * (PPDU + SIFS + this->m_pFrame->getFramelength(BLOCKACK)) + (cnt - 1) * SIFS > TXOP)break;
		cnt++;
	}
	if (dataSize <= cnt * DATA_LENGTH) //��TXOP��
	{
		this->m_nSendData = dataSize;
		this->m_QSendData.pop();
		applyforTXOPLen = (cnt - 1) * (PPDU + SIFS + m_pFrame->getFramelength(BLOCKACK)) + (cnt - 2) * SIFS + SIFS + ((HEADER + dataSize - (cnt - 1) * DATA_LENGTH) / TXSPEED) / SLOT;
		return applyforTXOPLen;
	}
	//����TXOP��Χ������TXOP���ҵ�����������
	this->m_nSendData = (cnt - 1)* DATA_LENGTH;
	this->m_QSendData.pop();
	this->m_QSendData.push(dataSize - m_nSendData);//ʣ�����ݷ������
	applyforTXOPLen=(cnt - 1) * (PPDU + SIFS + m_pFrame->getFramelength(BLOCKACK)) + (cnt - 2) * SIFS;
	return applyforTXOPLen;

}


void CStation::ReBackoff()
{
	this->m_nBackoffNumer = rand() %8;
}
void CStation::ComingDataPerSlot(int num)// bit/SLOT
{
	// TODO:��Ҫ����Possion�ֲ����������
	int cnt =rand()%num;
	allTxData += cnt;
	if (cnt > 0) this->m_QSendData.push(cnt);
}

int CStation::getSendDataSize() 
{
	int res = 0;
	while (!this->m_QSendData.empty())
	{
		res += this->m_QSendData.front();
		this->m_QSendData.pop();
	}
	this->m_QSendData.push(res); //������������·������
	return res;
}

int CStation::MiscrosecondToSlot(int miscrosecond)  // ����ת��Ϊʱ϶
{
	return miscrosecond % SLOT==0? miscrosecond / SLOT: miscrosecond / SLOT+1;
}
int CStation::TxSpeed()
{
	switch (this->m_nAC)
	{
	case AC_BE:
		return TXSPEED;
	case AC_BK:
		return TXSPEED;
	case AC_VI:
		return TXSPEED;
	case AC_VO:
		return TXSPEED;
	default:
		break;
	}
	return 1;
}

bool CStation::enableTransmit()  // �жϵ�ǰ�����Ƿ�һ��PPDU�Ĵ���
{
	int tmp = getSendDataSize();
	if (tmp >= DATA_MIN)
		return true;
	return false;
}












