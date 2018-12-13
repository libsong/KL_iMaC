#pragma once

#include <QTextCodec>
#include <QColor>

#ifndef u16
typedef unsigned short u16;
#endif
#ifndef u8
typedef unsigned char u8;
#endif
#ifndef u32
typedef unsigned int u32;
#endif
#ifndef s8
typedef char s8;
#endif

//udp组播发现协议结构信息
#define MCUINFOSTRUCT_ROW 13
#pragma pack(1)
typedef struct mcuinfo {
	u16		devType;//设置类型标识，0x0100电源类 0x0200故障类 0x0300高压类
	u16		port;//端口
	s8		devName[32]; //设备名称
	s8		devSN[16]; //设备SN号，暂无，待公司生产正规化后加入	
	s8		mcuType[16]; //芯片类型
	u32		mcuUID[3]; //芯片UID 标识
	u8		mcuSV[3]; //软件版本号，[0]:主版本 [1]:次版本 [2]:debug版本
	u8		mcuIP[4]; //ip
	u8		mcuNetMask[4]; //掩码
	u8		mcuGW[4]; //网关
	u8		mcuMAC[6]; //网络初始化时使用的mac地址
	u8		mcuHV[2];	//母板硬件版本 [0]主版本 [1]次版本
	u8		reserve[1]; //保留后续扩展
}McuInfo_t;
#pragma pack()

typedef enum {
	COMM_CMD_TEST,

	COMM_CMD_RELAY_CONF,			//hvs
	COMM_CMD_RELAY_ACTIVE,			//hvs
	COMM_CMD_RELAY_RESET,			//hvs
	COMM_CMD_RELAY_ADC,				//hvs GET VAL CUR

	COMM_CMD_RELAY_PWR,				//pdo: power distribute

	COMM_CMD_RELAY_FIU_ACT,			//NEW fiu

	COMM_CMD_SP_GAIN,				//Signal process box set gain
	COMM_CMD_SP_STATUS,				//Signal process box query status	

	COMM_CMD_PSB_ACT,				//power simu box

	COMM_CMD_SWB_ACT,				//Siganl switch box

	COMM_CMD_IP = 0xc8,				//ip port gw nm, v1.1.0 fiu modify ip info. by mulcast
	COMM_CMD_UID = 0xc9,			//give mcu uid
	COMM_CMD_SFRST = 0xca,			//mcu soft reset,STM32 mcu
}COMM_CMD;

union  u8Tofloat
{
	float f;
	u8 c[4];
};


extern QString weChinese2LocalCode(char *x);
extern void stringToHtmlFilter(QString &str);
extern void stringToHtml(QString &str, QColor crl);
extern void Delay_MSec_Suspend(unsigned int msec);

