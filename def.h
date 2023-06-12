#pragma once

// 仿真实验四配置
#define STACOUNT 4  // STA的数量
#define RUNSLOTCOUNT 1000000  // 仿真的运行时隙次数
#define OBJECT_DATA_ARRIVE_RATE  500  //数据到达速率
#define OTHER_DATA_ARRIVE_RATE  100  //数据到达速率
#define TXSPEED 30 // 30Mbps=30*10^6bps
#define DATA_MIN 5000 //  DATA最少480bit才能构成一个PPDDU
#define PPDU 70// 80时隙
#define TXOP 400 // 时隙 4000微秒 =4毫秒#define TXOP 400 // 时隙 4000微秒 =4毫秒
#define MAXPPDUCOUNT (TXOP/(1+6+PPDU)-2)

#define MCS  10
#define SS 10
#define BW 10
#define LENGTH 10



// 信道状态
#define CHANNEL_BUSY 1  // 忙碌
#define CHANNEL_IDLE 2  // 空闲

// STA状态
#define BACKOFF 0
#define SENDINGDATA 1
#define IDLE 3

// 定义PPDU中各个字段传输的时间
#define L_LTF 8  // 单位为微秒
#define L_STF  8
#define L_SIG 4
#define U_SIG 8
#define EHT_SIG 4 
#define EHT_STF 4
#define EHT_LTF 8  //待定
#define VDPES 1  //Variable durations per EHT-LTF symbol
#define HEADER ((L_LTF+L_STF+L_SIG+U_SIG+EHT_SIG+EHT_STF+EHT_LTF*VDPES)/10)
//#define PPDU_LENGTH 100
//TO DO:PPDU中其余字段的长度


// 定义帧间间隔
#define SIFS 1 // 10微秒 = 1个时隙
#define PIFS  2 // 20微秒 = 2个时隙 



// 定义仿真过程中使用的帧类型
#define FRAMEBASE 0
#define CTSTOSELF FRAMEBASE+1
#define BLOCKACK FRAMEBASE+2
#define MURTS FRAMEBASE+3
#define DATA FRAMEBASE+4

// 定义相关业务类型
#define AC_BK 0   // 背景
#define AC_BE 1   // 尽力而为
#define AC_VI 2   // 视频
#define AC_VO 3   // 语音

#define CWmin 0  // 最小竞争窗口
#define CWmax 1  // 最大竞争窗口

// 时隙定义

#define SLOT 10 // 10微秒

#define DATA_LENGTH (int)(SLOT*(PPDU-HEADER)*TXSPEED) //bit



// AP辅助能力判断
#define APASSISTLINKAGGREGATION  18// PIFS+CTSTOSLEF+SIFS+MURTS 2+8+2+6


/*
【程序中需要确定的参数】
//时隙为单位
SIFS
PIFS
PPDU中DATA的字节数  
TXSPEED//传输速率 具体可以根据SS MCS BW等计算  30Mbps
CW 竞争窗口，最小最大退避计数器的设置

单位：时隙
CTSTOSELF=8时隙   
BLOCKACK=6时隙或7时隙
MURTS=(HEADER+16)/SLOT

函数相关：
STA中每个时隙到达的数据速率
int ComingDataPerSlot(); TODO:模拟泊松分布每个时隙到达的数据
帧传输的相关要求（PPDU传输时是否填充满）

*/