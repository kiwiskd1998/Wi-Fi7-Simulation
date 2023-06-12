// 802.11beSimulation.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "def.h"
#include "CAccessPoint.h"
#include "CStation.h"
#include <iostream>
#include <ctime>
using namespace std;

//相关变量定义
//1个AP和四个STA
CAccessPoint AP;
CAccessPoint AP1;
CStation* STAs[STACOUNT+1] = { NULL };
CStation* otherSTAs[STACOUNT+1] = { NULL };



bool applyforLinkAggregation = false;  // AP是否辅助
bool STA1ApplyAP2AssistSTAaContent = false;  // STA1请求AP2为STAa竞争
bool STAaApplyAP1AssistSTA1Content = false;  // STAa请求AP1为STA1竞争
bool flag_APContentChannel = false;//AP参与竞争信道
bool flag_LinkAggregationSuccessful = false;  // 链路是否聚合成功
bool flag_STA1BegintoBeTXOPHolder = false; // STA1成为TXOPHolder
bool flag_STAaBegintoBeTXOPHolder = false;//STAa开始成为TXOPHolder

int getTXOPHolder(); // BSS1
int getOtherTXOPHolder();// BSS2
int getSendSize(int surplusSlot); // 剩余时隙可以发送的PPDU个数
int AP1AssistSTA1GetTXOPHolder();
int AP2AssistSTAaGetTXOPHolder();

int channelState = 0, otherChannelState = 0;  // 信道状态
int TXOPHolder = -1, otherTXOPHolder = -1;//正在传输的STA
int needConsumeSlot = 0; //目前已经传输的时隙数量
int otherNeedConsumeSlot = 0;


// 实验过程记录
int m_nSTA1ApplyforAPAssist = 0;  // STA1请求AP2为STAa竞争
int m_nSTAaApplyforAPAssist = 0;  // STAa请求AP1为STA1竞争
int linkAggregationSuccessCnt = 0;  // 聚合成功次数
int linkAggregationFailedCnt = 0;  // 聚合失败次数
int linkAggregationBits = 0;  // 通过聚合总的传输业务量

int main()
{

	srand((unsigned int)time(NULL));

	// =====================仿真相关参数初始化===========================
	for (int i = 0;i <= STACOUNT;i++) //初始化4个STA
	{
		STAs[i] = new CStation;
		STAs[i]->init(i);
		otherSTAs[i] = new CStation;
		otherSTAs[i]->init(i);
	}
	channelState = CHANNEL_IDLE;  // 初始化信道为空闲状态
	otherChannelState = CHANNEL_IDLE;
	//=========================仿真试验过程========================================

	
	for (int i = 0;i < RUNSLOTCOUNT;i++)
	{

		//记录每个SATs当前时隙到达的数据
		STAs[0]->ComingDataPerSlot(OBJECT_DATA_ARRIVE_RATE /2);
		otherSTAs[0]->ComingDataPerSlot(OBJECT_DATA_ARRIVE_RATE /2);
		for (int j = 1;j < STACOUNT;j++) 
			STAs[j]->ComingDataPerSlot(OTHER_DATA_ARRIVE_RATE);
		for (int j = 1;j < STACOUNT;j++) 
			otherSTAs[j]->ComingDataPerSlot(OTHER_DATA_ARRIVE_RATE);

		//如果当前为空 另外一个也是空
		if (flag_LinkAggregationSuccessful == true) // 如果链路聚合成功
		{
			if (needConsumeSlot > 0)
				needConsumeSlot--;
			if (otherNeedConsumeSlot > 0)
				otherNeedConsumeSlot--;
			if (needConsumeSlot == 0 && otherNeedConsumeSlot == 0) //聚合成功应该两个同步结束
			{
				flag_APContentChannel = false;
				flag_LinkAggregationSuccessful = false;
				channelState = CHANNEL_IDLE;
				otherChannelState = CHANNEL_IDLE;
				STAs[0]->ReBackoff();
				otherSTAs[0]->ReBackoff();
				TXOPHolder = -1;
				otherTXOPHolder = -1;
			}
			continue;
		}
		// 如果两个链路同时到达0则并且都获得TXOP
		// 请求AP辅助监听信道获得TXOP
		if (STA1ApplyAP2AssistSTAaContent ==false&&TXOPHolder == 0 && otherChannelState==CHANNEL_BUSY)//flag_LinkAggregationSuccessful == false
		{
			applyforLinkAggregation = true;
			STA1ApplyAP2AssistSTAaContent = true;
			m_nSTA1ApplyforAPAssist++;
			printf("th%d times STA1 apply for Ap2 Assist for STAa.\nTXOPHolder1 :%d\nchannel State:%d\notherTXOPHolder1 :%d\notherchannel State:%d\n",\
			m_nSTA1ApplyforAPAssist,TXOPHolder,channelState,otherTXOPHolder,otherChannelState);
		}
		else if (STAaApplyAP1AssistSTA1Content ==false&&otherTXOPHolder==0 && channelState==CHANNEL_BUSY)
		{
			applyforLinkAggregation = true;
			STAaApplyAP1AssistSTA1Content = true;
			m_nSTAaApplyforAPAssist++;
			printf("th%d times STAa apply for Ap1 Assist for STA1.\nTXOPHolder1 :%d\nchannel State:%d\notherTXOPHolder1 :%d\notherchannel State:%d\n",\
			m_nSTAaApplyforAPAssist, TXOPHolder, channelState, otherTXOPHolder, otherChannelState);
		}

		

		// BSS1
		if (channelState == CHANNEL_IDLE)
		{
			if (flag_APContentChannel == true) 
				TXOPHolder = AP1AssistSTA1GetTXOPHolder(); // AP1辅助STA1竞争TXOP
			else 
				TXOPHolder = getTXOPHolder();
			if (TXOPHolder == 0)
				flag_STA1BegintoBeTXOPHolder = true;  // STA1传输开始时获得TXOPHolder
			if (-1 == TXOPHolder )
			{
				// 对所有的具有数据的STAs进行退避
				for (int k = 0;k < STACOUNT;k++)
				{
					if (k == 0 && otherTXOPHolder == 0) 
						continue; // STA1传输STAa保持
					if (!STAs[k]->m_QSendData.empty())
						STAs[k]->backoff();
				}
				if (STAaApplyAP1AssistSTA1Content == true) //AP1进行退避
				{
					STAs[STACOUNT]->backoff();
				}
			}
			else { 
			
				// TxopHolder对AP进行数据传输
				if (flag_LinkAggregationSuccessful == true) // 辅助聚合成功时STA发送的数据量有所限制
				{
					needConsumeSlot = otherNeedConsumeSlot;
					int tmpbit = getSendSize(needConsumeSlot);  //获得剩余时隙可以发送的bit数量
					STAs[otherTXOPHolder]->allTxData += tmpbit;  // 总的数据量增加
					AP.m_nSTAReceivedData[0] += tmpbit;  // AP1由STA1接收的数据增加
					printf("th%d successful link Aggregation,send bits:%d\n", linkAggregationSuccessCnt, tmpbit);
				}
				else
				{
					int tmp = STAs[TXOPHolder]->getSendDataSize();
					needConsumeSlot = STAs[TXOPHolder]->needTXOPLen(tmp);
					if (AP.IsSuccsssfulRecevied(TXOPHolder, STAs[TXOPHolder]->m_nSendData))  // 如果AP成功接收数据
					{
						printf("th%d slot\nSTA%d transmiting %d bit data\n apply for %d slot\n", i, TXOPHolder + 1, STAs[TXOPHolder]->m_nSendData, needConsumeSlot);
						for (int i = 0;i < STACOUNT;i++)
						{
							if (i != TXOPHolder)printf("sta%d backoffid is %d\n", i + 1, STAs[i]->m_nBackoffNumer);
						}

						AP.m_nSTAReceivedData[TXOPHolder] += STAs[TXOPHolder]->m_nSendData;	// 记录由TXOPHolder传来的数据	
						//channelState = CHANNEL_BUSY;
					}
					else { //STA重新传输
						STAs[TXOPHolder]->m_QSendData.push(tmp);  // 传输失败的数据放入队列下次继续传输
						STAs[TXOPHolder]->ReBackoff();  // 重置退避计数器 
						needConsumeSlot = 0;//将需要时隙数量置为0
					}
					STAs[TXOPHolder]->m_nSendData = 0;
				}
				channelState = CHANNEL_BUSY;
			}
		}
		else
		{
			needConsumeSlot--;
			if (needConsumeSlot <= 0)
			{
				if (flag_LinkAggregationSuccessful == true) //  如果链路聚合成功
				{
					applyforLinkAggregation = false;
					flag_APContentChannel = false;
				    flag_LinkAggregationSuccessful = false;  //  清除成功状态
					STAs[0]->ReBackoff();  // STA1重置退避计数器
					TXOPHolder = -1;
					STAaApplyAP1AssistSTA1Content = false;// STAa申请AP1辅助清空
				}else {
					if (STAaApplyAP1AssistSTA1Content == true) // STAa请求辅助链路聚合
					{
						if ((otherNeedConsumeSlot - APASSISTLINKAGGREGATION) / (PPDU + SIFS + 6) >= 1 && (otherNeedConsumeSlot - APASSISTLINKAGGREGATION) / (PPDU + SIFS + 6) <= MAXPPDUCOUNT) // 符合条件则进行链路聚合竞争
						{
							flag_APContentChannel = true;
						}
						else
						{
							STAaApplyAP1AssistSTA1Content = false;
							applyforLinkAggregation = false;
						}
					}
					channelState = CHANNEL_IDLE;
					STAs[TXOPHolder]->ReBackoff();  // 重置退避计数器 
					TXOPHolder = -1;
				
				}
				
			}
		}

		// BSS2
		if (otherChannelState == CHANNEL_IDLE)
		{

			if (flag_APContentChannel == true)
				otherTXOPHolder = AP2AssistSTAaGetTXOPHolder(); // AP1辅助STA1竞争TXOP
			else
				otherTXOPHolder = getOtherTXOPHolder();
			if (-1 == otherTXOPHolder)
			{

				// TODO:如果AP辅助则需要判定

				// 对所有的具有数据的otherSTAs进行退避
				for (int k = 0;k < STACOUNT;k++)
				{
					if (k == 0 && TXOPHolder == 0)continue; //STA0传输STA1保持
					if (!otherSTAs[k]->m_QSendData.empty())
						otherSTAs[k]->backoff();
					if (STA1ApplyAP2AssistSTAaContent == true) //AP2进行退避
					{
						otherSTAs[STACOUNT]->backoff();
					}
				}
			}
			else { //TxopHolder对AP进行数据传输
				if (flag_LinkAggregationSuccessful == true)  // AP2获得TXOP 
				{
					otherNeedConsumeSlot = needConsumeSlot;
					int tmpbit = getSendSize(otherNeedConsumeSlot);
					otherSTAs[0]->allTxData += tmpbit;
					AP1.m_nSTAReceivedData[0] += tmpbit;
					printf("th%d successful link Aggregation,send bits:%d\n", linkAggregationSuccessCnt, tmpbit);
					//otherChannelState = CHANNEL_BUSY;
				}
				else 
				{
					int tmp = otherSTAs[otherTXOPHolder]->getSendDataSize();
					otherNeedConsumeSlot = otherSTAs[otherTXOPHolder]->needTXOPLen(tmp);
					if (AP1.IsSuccsssfulRecevied(otherTXOPHolder, otherSTAs[otherTXOPHolder]->m_nSendData))  // 如果AP成功接收数据
					{
						printf("th%d slot\nSTA%d transmiting %d bit data\n apply for %d slot\n", i, otherTXOPHolder + 1, otherSTAs[otherTXOPHolder]->m_nSendData, otherNeedConsumeSlot);
						for (int i = 0;i < STACOUNT;i++)
						{
							if (i != otherTXOPHolder)printf("sta%d backoffid is %d\n", i + 1, otherSTAs[i]->m_nBackoffNumer);
						}
						AP1.m_nSTAReceivedData[otherTXOPHolder] += otherSTAs[otherTXOPHolder]->m_nSendData;	// 记录由TXOPHolder传来的数据
						//otherChannelState = CHANNEL_BUSY;
					}
					else { //STA重新传输
						otherSTAs[otherTXOPHolder]->m_QSendData.push(tmp);  // 传输失败的数据放入队列下次继续传输
						otherSTAs[otherTXOPHolder]->ReBackoff();  // 重置退避计数器 
						otherNeedConsumeSlot = 0;//将需要时隙数量置为0
					}
					otherSTAs[otherTXOPHolder]->m_nSendData = 0;
				}

				otherChannelState = CHANNEL_BUSY;
			}
		}
		else
		{
			otherNeedConsumeSlot--;
			if (otherNeedConsumeSlot <= 0)  //1.判断是否具有请求辅助监听  2.是否完成链路聚合重置 3.普通状态下重置
			{
				
				if (flag_LinkAggregationSuccessful == true) 
				{
					applyforLinkAggregation = false;
					STA1ApplyAP2AssistSTAaContent = false;
					flag_APContentChannel = false;
					flag_LinkAggregationSuccessful = false;
					otherSTAs[TXOPHolder]->ReBackoff();
					otherTXOPHolder = -1;
					channelState = CHANNEL_IDLE;
				}else {
					if (STA1ApplyAP2AssistSTAaContent == true) 
					{
						if ((needConsumeSlot - APASSISTLINKAGGREGATION) / (PPDU + SIFS + 6) >= 1 && (needConsumeSlot - APASSISTLINKAGGREGATION) / (PPDU + SIFS + 6) <= MAXPPDUCOUNT) // 符合条件则进行链路聚合竞争
						{
							flag_APContentChannel = true;

						}
						else // STA1请求了AP2为STAa监听但是不符合条件
						{
							applyforLinkAggregation = false; // 这个好像没什么用
							STA1ApplyAP2AssistSTAaContent = false;

						}
					}
					else  // 没有申请链路聚合
					{
					otherChannelState = CHANNEL_IDLE;
					otherSTAs[otherTXOPHolder]->ReBackoff();
					otherTXOPHolder = -1;
					}
					
				}
			}
		}

		// 对同时为TXOPHolder进行处理
	
	}

	//====================================运行结果 打印输出===========================================
	printf("Lab4:AP assist link aggregation transmit result:\n");
	for (int i = 0;i < STACOUNT;i++)
	{
		int surplus = 0;
		while (!STAs[i]->m_QSendData.empty())
		{
			surplus += STAs[i]->m_QSendData.front();
			STAs[i]->m_QSendData.pop();
		}
		printf("STA%d:\nneed to transmit %d bits\ntransmit to ap %d bits\nsurplus %d bits\nmargin %d bits\n", \
			STAs[i]->m_nID + 1, STAs[i]->allTxData, AP.m_nSTAReceivedData[i], surplus, STAs[i]->allTxData - surplus - AP.m_nSTAReceivedData[i]);

	}
	for (int i = 0;i < STACOUNT;i++)
	{
	
		int surplus = 0;
		while (!otherSTAs[i]->m_QSendData.empty())
		{
			surplus += otherSTAs[i]->m_QSendData.front();
			otherSTAs[i]->m_QSendData.pop();
		}
		printf("STA%c:\nneed to transmit %d bits\ntransmit to ap %d bits\nsurplus %d bits\nmargin %d bits\n", \
			otherSTAs[i]->m_nID + 'a', otherSTAs[i]->allTxData, AP1.m_nSTAReceivedData[i], surplus, otherSTAs[i]->allTxData - surplus - AP1.m_nSTAReceivedData[i]);

	}
	printf("AP Assist Link Aggregation Plan None STR MLD transmit all data is%d\n", AP.m_nSTAReceivedData[0] + AP1.m_nSTAReceivedData[0]);
	printf("Toatl success Link Aggregation count :%d\nSTA1 apply for AP listen link2 count:%d\nSTAa apply for AP listen link1 count:%d\nunder link aggregation transmit bits:%d\nlink Aggregation filed Count:%d", \
		linkAggregationSuccessCnt,m_nSTA1ApplyforAPAssist,m_nSTAaApplyforAPAssist,linkAggregationBits,linkAggregationFailedCnt);
	//====================================程序结束 释放空间==========================================
	for (int i = 0;i < STACOUNT;i++)
	{

		delete STAs[i];
		STAs[i] = NULL;
		delete otherSTAs[i];
		otherSTAs[i] = NULL;
	}
	return 0;
}

int AP2AssistSTAaGetTXOPHolder() 
{
	int arr[5] = { 0 };
	int cnt = 0;
	int holder = 0;
	for (int i = 0;i <= STACOUNT;i++)
	{
		if (otherSTAs[i]->m_nBackoffNumer == 0)
		{
			
			if (otherSTAs[i]->enableTransmit() == true || otherSTAs[i]->m_nID == 4) // 4代表AP
				arr[cnt++] = i;
			else otherSTAs[i]->ReBackoff();  // 只要退避到0，就重置计数器
		}
	}
	if (cnt >= 2)
		holder = arr[rand() % cnt]; // 随机选择一个STA进行传输数据
	else if (cnt == 1)
		holder = arr[0];
	else holder = -1;
	if (holder == 4)  // AP1辅助竞争到信道 
	{
		holder = 0;
		otherTXOPHolder= 0;
		linkAggregationSuccessCnt++;  // 成功次数＋1
		otherChannelState = CHANNEL_BUSY;
		flag_LinkAggregationSuccessful = true;
		// 聚合成功修改时隙
		otherNeedConsumeSlot=needConsumeSlot;
		int tmpbit = getSendSize(needConsumeSlot);
		otherSTAs[otherTXOPHolder]->allTxData += tmpbit;  // 总的数据量增加
		AP1.m_nSTAReceivedData[0] += tmpbit;  // AP1由STA1接收的数据增加
		linkAggregationBits += tmpbit;
		applyforLinkAggregation = true;  // 重置请求申请监听
		
	}
	else {
		linkAggregationFailedCnt++;  // 失败
		flag_APContentChannel = false; // AP1辅助STA2竞争失败
		STA1ApplyAP2AssistSTAaContent = false;
	}
	return holder;
}
int getTXOPHolder()
{
	int arr[4] = { 0 };
	int cnt = 0;
	int holder = 0;
	for (int i = 0;i < STACOUNT;i++)
	{
		if (STAs[i]->m_nBackoffNumer == 0)
		{
			//if (flag_STAaBegintoBeTXOPHolder == true && i == 0)   // STAa刚开始成为TXOPHolder并且STA1到0
			//{
			//	arr[cnt++] = i;
			//	continue;
			//}
			if (i == 0 && otherTXOPHolder == 0 && otherChannelState == CHANNEL_BUSY)
			{
				STAs[i]->ReBackoff();
				continue;
			}
			if (STAs[i]->enableTransmit() == true)
				arr[cnt++] = i;
			else STAs[i]->ReBackoff();  // 只要退避到0，就重置计数器
		}
	}
	if (cnt >= 2)
		holder = arr[rand() % cnt]; // 随机选择一个STA进行传输数据
	else if (cnt == 1)
		holder = arr[0];
	else holder = -1;
	//if (flag_STAaBegintoBeTXOPHolder == true && holder == 0)//同时获得则进行链路聚合传输
	//	flag_LinkAggregationSuccessful = true;

	return holder;

}
int AP1AssistSTA1GetTXOPHolder()   //  AP1辅助链路1去竞争
{
	int arr[5] = { 0 };
	int cnt = 0;
	int holder = 0;
	for (int i = 0;i <= STACOUNT;i++)
	{
		if (STAs[i]->m_nBackoffNumer == 0)
		{

			if (STAs[i]->enableTransmit() == true || STAs[i]->m_nID == 4)   // =认为STA1 FULL BUFFER
				arr[cnt++] = i;
			else
				STAs[i]->ReBackoff();  // 只要退避到0，就重置计数器
		}
	}
	if (cnt >= 2)
		holder = arr[rand() % cnt]; // 随机选择一个STA进行传输数据
	else if (cnt == 1)
		holder = arr[0];
	else holder = -1;
	if (holder == 4)  // AP1辅助竞争到信道信道 
	{
		holder = 0;
		TXOPHolder = 0;
		channelState = CHANNEL_BUSY;
		linkAggregationSuccessCnt++;  // 成功次数＋1
		flag_LinkAggregationSuccessful = true;
		needConsumeSlot=otherNeedConsumeSlot;
		int tmpbit = getSendSize(otherNeedConsumeSlot);
		STAs[otherTXOPHolder]->allTxData += tmpbit;
		AP.m_nSTAReceivedData[0] += tmpbit;
		linkAggregationBits += tmpbit;
		applyforLinkAggregation = true;  // 重置请求申请监听
	}
	else {
		flag_APContentChannel = false; // AP1辅助STA2竞争失败
		linkAggregationFailedCnt++;  // 失败
		STAaApplyAP1AssistSTA1Content = false;
	}
	return holder;
}
int getOtherTXOPHolder()// BSS2
{
		int arr[4] = { 0 };
		int cnt = 0;
		int holder = 0;
		for (int i = 0;i < STACOUNT;i++)
		{
			if (otherSTAs[i]->m_nBackoffNumer == 0)
			{
				//if (i == 0 && flag_STA1BegintoBeTXOPHolder == true)   // STA1刚开始传输
				//{
				//	arr[cnt++] = i;
				//	continue;
				//}
				if (i == 0 && TXOPHolder == 0 && channelState == CHANNEL_BUSY)
				{
					otherSTAs[i]->ReBackoff();
					continue;
				}
				if (otherSTAs[i]->enableTransmit() == true)
					arr[cnt++] = i;
				else otherSTAs[i]->ReBackoff();  // 只要退避到0，就重置计数器
			}
		}
		if (cnt >= 2)
			holder = arr[rand() % cnt]; // 随机选择一个STA进行传输数据
		else if (cnt == 1)
			holder = arr[0];
		else holder = -1;
		//if (flag_STA1BegintoBeTXOPHolder == true)//同时获得则进行链路聚合传输
		//{
		//	if (holder == 0)
		//		flag_LinkAggregationSuccessful = true;
		//	else 
		//		flag_LinkAggregationSuccessful = false;
		//}	

		return holder;
	
}
int getSendSize(int surplusSlot) 
{
	if (surplusSlot / (PPDU + SIFS + 6) >= 1) 
	{
		return (surplusSlot / (PPDU + SIFS + 6)) * DATA_LENGTH;//可以发送的数据
	
	}
	return 0;

}