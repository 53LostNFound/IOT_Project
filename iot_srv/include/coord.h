
#ifndef __COORD_H__
#define __COORD_H__
//封装协调器发给上位机的数据对象类型
typedef struct data_packet {
	char data[18];

}node_data;

//定义封装环境参数对象
struct moni_msg {
	unsigned char zgno; //节点编号
	unsigned char tem[2]; //温度
	unsigned char hum[2]; //湿度
	unsigned char lux[2]; //光强
};

struct moni_info {
	int zgno; //节点编号
	float temper; //温度
	float humi; //湿度
	unsigned int illum; //光强
	unsigned char dev_stat; //执行机构状态 0关 1开

};

extern char data_arr[13]; //模拟串口发送的13个字节的监控数据


void convert_moni_info(node_data *buf, char *send_buf);

#endif
