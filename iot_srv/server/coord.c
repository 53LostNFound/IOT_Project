/*
	将底层 Zigbee 节点采集到的原始监控数据，转换为移动端易于解析的字符串格式。
*/

#include "iot_server.h"
#include "coord.h"
char data_arr[13]; //模拟串口发送的13个字节的监控数据
char send_mob_buf[SIZE]; //发送监控数据字符串给移动端的缓冲区

#if defined(LOG_TAG)
#undef LOG_TAG 
#define LOG_TAG    "coord"
#else 
#define LOG_TAG    "coord"
#endif

void convert_moni_info(node_data *buf, char *send_buf)
{
	//node_data-->moni_msg-->moni_info
	struct moni_msg msg;
	//node_data-->moni_msg
	memcpy(&msg, buf->data, sizeof(msg)); //只前面7个字节
	//moni_msg-->moni_info
	struct moni_info temp;
	temp.zgno = msg.zgno; //节点编号
	temp.temper = msg.tem[1]; //温度
	temp.humi = msg.hum[1]; //湿度
	temp.illum = (unsigned int)(msg.lux[1]<<8 | msg.lux[0]);
	temp.dev_stat = 0; //设备未开启
	//格式化字符串
	sprintf(send_buf, "%0.1f#%0.1f#%u#%u", temp.temper, temp.humi, temp.illum, temp.dev_stat);
//	log_v("发送给移动端的字符串:%s",send_buf);
}

