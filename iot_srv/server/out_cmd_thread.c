/*
	从命令队列（pcmdque）中取出控制命令，并模拟执行这些命令（如点亮/熄灭LED）
*/

#include "iot_server.h"


// 模拟设备操作函数，根据参数控制不同LED的开关
void dev_op(int dev, int on)
{
	if (dev == 1)
	{
		if (on == 1)
			printf("led2 green on\n");
		else if (on == 0)
			printf("led2 green off\n");
	}
	if (dev == 2)
	{
		if (on == 1)
			printf("led2 orange on\n");
		else if (on == 0)
			printf("led2 orange off\n");
	}
}

// 从zigbee命令队列出队节点并发送给zigbee模块终端线程
void *out_cmd_thread(void *arg)
{
	int cnt = 0;
	int dev = -1, on = -1;
	char cmdstr[50]; // 保存发给zigbee终端的原始命令字符串
	char devname[30];
	char op[30];
	while (1)
	{
		pthread_mutex_lock(&cmd_queue_mutex);
		pthread_cond_wait(&cmd_queue_cond, &cmd_queue_mutex); // 等待接收命令节点唤醒
		memset(cmdstr, 0, sizeof(cmdstr));
		// 打印出队命令
		if (linkqueue_cmdstr_out(pcmdque, cmdstr))
		{
			printf("********************************\n");
			cnt++;
			printf("$$$命令链表队列节点%d 出队命令字符串:$$$\n ", cnt);
			printf("%s\n", cmdstr);
			printf("********************************\n\n");
		}
		pthread_mutex_unlock(&cmd_queue_mutex);

		// 解析命令字符串
		dev = -1;
		on = -1;
		sscanf(cmdstr, "%s%s", devname, op);
		printf("[%s] [%s]\n", devname, op);
		if (strncmp(devname, "led2_green", strlen(devname)) == 0)
			dev = 1;
		if (strncmp(devname, "led2_orange", strlen(devname)) == 0)
			dev = 2;
		if (strncmp(op, "on", strlen(op)) == 0)
			on = 1;
		if (strncmp(op, "off", strlen(op)) == 0)
			on = 0;

		if ((dev != -1) && (on == -1))
		{
			dev_op(dev, on);
			do_cmds(cmdstr, db);
		}

		// 执行拍照 1张 or 5张
		if ((strncmp(devname, "one", strlen(devname)) == 0) &&
			(strncmp(op, "one", strlen(op)) == 0))
		{
			system("echo one > /tmp/webpipe");
		}

		if ((strncmp(devname, "five", strlen(devname)) == 0) &&
			(strncmp(op, "five", strlen(op)) == 0))
		{
			system("echo five > /tmp/webpipe");
		}
	}

	return NULL;
}
