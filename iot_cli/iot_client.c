/*
	linux物联网平台客户端主程序
*/

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <elog.h>

#define SIZE 1024
int fd;

void init_elog(void)
{

	/* 初始化 EasyLogger */
	elog_init(); // 必须

	/* 设置全局日志过滤级别为 VERBOSE（最低级别） */
	elog_set_filter_lvl(ELOG_LVL_VERBOSE);
	elog_set_output_enabled(true);

	/*设置输出格式*/
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL); // a 所有信息
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO);
	// 自定义日志格式：时间 | 级别 | 标签 | 内容
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO);	// 无TAG PID
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO);	// 无PID TID
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO);	// d 所有信息
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO); // V 所有信息
}

int main(void)
{

	int ret;

	init_elog();

	// 1.买电话
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		perror("socket");
	}
#if 0	
	//2.绑卡
	struct sockaddr_in client; //自己ip+port
	client.sin_family = AF_INET;
	client.sin_port = htons(12346);
	client.sin_addr.s_addr = inet_addr("192.168.10.251");
	ret = bind(fd, (struct sockaddr *)&client, sizeof(client));
	if (ret == -1) {
		perror("bind");
	}
#endif
	// 3.打电话
	struct sockaddr_in server; // 访问服务器ip+port对象
	server.sin_family = AF_INET;
	server.sin_port = htons(12345);
	server.sin_addr.s_addr = inet_addr("172.18.3.79");
	ret = connect(fd, (struct sockaddr *)&server, sizeof(server));
	if (ret == -1)
	{
		perror("connect");
	}
	system("netstat -na | grep 12345"); // 查看tcp连接状态

	// 4.通信
	char buf[SIZE];
	char active = 0;
	int cnt = 0;
	// 1)发送给服务器获取参数请求
	log_v("发送给服务器获取参数请求");
	send(fd, "mobile monitor robin 123", strlen("mobile monitor robin 123"), 0);

	bzero(buf, SIZE);
	recv(fd, buf, sizeof(buf), 0);
	if (strncmp("yes", buf, strlen(buf)) == 0)
		active = 1;

	// 2)发送给服务器参数请求
	while (active)
	{
		send(fd, "mobile monitor_info monitor_info monitor_info",
			 strlen("mobile monitor_info monitor_info monitor_info"), 0);
		bzero(buf, SIZE);
		ret = recv(fd, buf, sizeof(buf), 0);
		if (ret < 0)
		{
			printf("read error");
			exit(1);
		}
		cnt++;
		log_v("%d> %s", cnt, buf);
		sleep(1);
		if (cnt == 10)
			active = 0;
	}

	// 3)发送给服务器结束参数请求
	log_v("发送给服务器结束参数请求");
	send(fd, "mobile stop_moninfo_send stop_moninfo_send  stop_moninfo_send",
		 strlen("mobile stop_moninfo_send stop_moninfo_send  stop_moninfo_send"), 0);
	sleep(1);

	// 5.挂电话
	close(fd);
	return 0;
}
