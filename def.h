#pragma once

// ����ʵ��������
#define STACOUNT 4  // STA������
#define RUNSLOTCOUNT 1000000  // ���������ʱ϶����
#define OBJECT_DATA_ARRIVE_RATE  500  //���ݵ�������
#define OTHER_DATA_ARRIVE_RATE  100  //���ݵ�������
#define TXSPEED 30 // 30Mbps=30*10^6bps
#define DATA_MIN 5000 //  DATA����480bit���ܹ���һ��PPDDU
#define PPDU 70// 80ʱ϶
#define TXOP 400 // ʱ϶ 4000΢�� =4����#define TXOP 400 // ʱ϶ 4000΢�� =4����
#define MAXPPDUCOUNT (TXOP/(1+6+PPDU)-2)

#define MCS  10
#define SS 10
#define BW 10
#define LENGTH 10



// �ŵ�״̬
#define CHANNEL_BUSY 1  // æµ
#define CHANNEL_IDLE 2  // ����

// STA״̬
#define BACKOFF 0
#define SENDINGDATA 1
#define IDLE 3

// ����PPDU�и����ֶδ����ʱ��
#define L_LTF 8  // ��λΪ΢��
#define L_STF  8
#define L_SIG 4
#define U_SIG 8
#define EHT_SIG 4 
#define EHT_STF 4
#define EHT_LTF 8  //����
#define VDPES 1  //Variable durations per EHT-LTF symbol
#define HEADER ((L_LTF+L_STF+L_SIG+U_SIG+EHT_SIG+EHT_STF+EHT_LTF*VDPES)/10)
//#define PPDU_LENGTH 100
//TO DO:PPDU�������ֶεĳ���


// ����֡����
#define SIFS 1 // 10΢�� = 1��ʱ϶
#define PIFS  2 // 20΢�� = 2��ʱ϶ 



// ������������ʹ�õ�֡����
#define FRAMEBASE 0
#define CTSTOSELF FRAMEBASE+1
#define BLOCKACK FRAMEBASE+2
#define MURTS FRAMEBASE+3
#define DATA FRAMEBASE+4

// �������ҵ������
#define AC_BK 0   // ����
#define AC_BE 1   // ������Ϊ
#define AC_VI 2   // ��Ƶ
#define AC_VO 3   // ����

#define CWmin 0  // ��С��������
#define CWmax 1  // ���������

// ʱ϶����

#define SLOT 10 // 10΢��

#define DATA_LENGTH (int)(SLOT*(PPDU-HEADER)*TXSPEED) //bit



// AP���������ж�
#define APASSISTLINKAGGREGATION  18// PIFS+CTSTOSLEF+SIFS+MURTS 2+8+2+6


/*
����������Ҫȷ���Ĳ�����
//ʱ϶Ϊ��λ
SIFS
PIFS
PPDU��DATA���ֽ���  
TXSPEED//�������� ������Ը���SS MCS BW�ȼ���  30Mbps
CW �������ڣ���С����˱ܼ�����������

��λ��ʱ϶
CTSTOSELF=8ʱ϶   
BLOCKACK=6ʱ϶��7ʱ϶
MURTS=(HEADER+16)/SLOT

������أ�
STA��ÿ��ʱ϶�������������
int ComingDataPerSlot(); TODO:ģ�Ⲵ�ɷֲ�ÿ��ʱ϶���������
֡��������Ҫ��PPDU����ʱ�Ƿ��������

*/