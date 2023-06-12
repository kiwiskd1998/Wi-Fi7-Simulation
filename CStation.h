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
	int m_nID;			// STA的ID
	//int m_nStatus;		//  STA此刻的状态
	//bool m_bIsFullBuffer;   //  buffer是否有数据
	int m_nBackoffNumer;   //  退避计数器
	bool m_bIsLinkAggreationSuccessful; // sta是否已经链路聚合成功
	CFrame* m_pFrame;//STA可以传输的所有帧
	int m_nAC;// 类型
	int m_nSendData;  // 要发送的数据，单位字节
	queue<int> m_QSendData; //用于记录目前到达的数据
	int CW[4][2]; ////竞争窗口min Max
	int allTxData;//总传输数据量
	int applyforTXOPLen;//当前传输申请的TXOP长度
public:
	void init(int id);	  // 初始化STA的状态
	void backoff();		  //  退避机制
	void reTransferData();	//  重传机制
	int transmission(int FrameType); // 返回STA传输Frame的时间
	bool IsAblelinkAggregation(int TXOPLEN,int BUSYLEN);	//  是否可以链路聚合
	int needTXOPLen(int dataSize);//需要的TXOP长度
	void ReBackoff();	// 重置退避
	void ComingDataPerSlot(int num);  // 更具泊松分布计算当前时隙到达的数据并放入队列
	int getSendDataSize();  // 计算队列中要发送的数据大小 单位字节
	int MiscrosecondToSlot(int miscrosecond);  // 微秒转换为时隙
	int TxSpeed();
	//int ComingDataPerSlot();
	bool enableTransmit(); // 判断当前到达的数据是否够一个PPDU的长度
};

/*
*
* 属性
id  // 标识是哪个STA
STA的状态：
	退避
	等待：backoff为0，等待进行链路聚合传输。
	传输：发送数据
	空闲：没传输任务
*函数
	1）退避
	2）等待
	3）传输数据
	4）初始化函数

*/