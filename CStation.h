#pragma once
#include "CFrame.h"
#include "def.h"
#include <queue>
#include <random>
using  namespace std;
class CStation
{
public:
	CStation();
	~CStation();
public:
	int m_nID;			// STA��ID
	//int m_nStatus;		//  STA�˿̵�״̬
	//bool m_bIsFullBuffer;   //  buffer�Ƿ�������
	int m_nBackoffNumer;   //  �˱ܼ�����
	bool m_bIsLinkAggreationSuccessful; // sta�Ƿ��Ѿ���·�ۺϳɹ�
	CFrame* m_pFrame;//STA���Դ��������֡
	int m_nAC;// ����
	int m_nSendData;  // Ҫ���͵����ݣ���λ�ֽ�
	queue<int> m_QSendData; //���ڼ�¼Ŀǰ���������
	int CW[4][2]; ////��������min Max
	int allTxData;//�ܴ���������
	int applyforTXOPLen;//��ǰ���������TXOP����
public:
	void init(int id);	  // ��ʼ��STA��״̬
	void backoff();		  //  �˱ܻ���
	void reTransferData();	//  �ش�����
	int transmission(int FrameType); // ����STA����Frame��ʱ��
	bool IsAblelinkAggregation(int TXOPLEN,int BUSYLEN);	//  �Ƿ������·�ۺ�
	int needTXOPLen(int dataSize);//��Ҫ��TXOP����
	void ReBackoff();	// �����˱�
	void ComingDataPerSlot(int num);  // ���߲��ɷֲ����㵱ǰʱ϶��������ݲ��������
	int getSendDataSize();  // ���������Ҫ���͵����ݴ�С ��λ�ֽ�
	int MiscrosecondToSlot(int miscrosecond);  // ΢��ת��Ϊʱ϶
	int TxSpeed();
	//int ComingDataPerSlot();
	bool enableTransmit(); // �жϵ�ǰ����������Ƿ�һ��PPDU�ĳ���
};

/*
*
* ����
id  // ��ʶ���ĸ�STA
STA��״̬��
	�˱�
	�ȴ���backoffΪ0���ȴ�������·�ۺϴ��䡣
	���䣺��������
	���У�û��������
*����
	1���˱�
	2���ȴ�
	3����������
	4����ʼ������

*/