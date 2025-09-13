/*
	接收手机端的各种请求（如登录、参数获取、控制命令、图片列表等），并根据请求类型进行相应的处理和回复。
*/

#include "iot_server.h"
#if defined(LOG_TAG)
#undef LOG_TAG
#define LOG_TAG "recv_mob_thread"
#else
#define LOG_TAG "recv_mob_thread"
#endif

char mob_str[4][20] = {0}; // 解析字符串的二维数组
char uname[20] = {0};	// 当前登录用户名
char pass[20] = {0};	// 当前登录密码
char pics_filelist[N];	 // 存储图片文件列表的字符串

/**
 * 作用：
 * 	遍历 /www/images 目录，将文件名用#拼接成一个字符串，统计图片文件数量
 * 
 * 参数:
 *   pcnt: 指向一个整数的指针，用于存储找到的图片文件数量
 */
void get_pics_fileList(int *pcnt)
{
	// 用于存储目录遍历结果的结构体指针数组
	struct dirent **namelist;
	// m用于遍历文件列表，n存储扫描到的文件数量
	int m, n;
	// 标记是否是第一个文件，用于控制文件名之间的分隔符添加
	char isFist = 1;
	// char list_cnt[10];
	*pcnt = 0;

	// 打印所有文件列表的标题
	printf("all file list:\n");
	bzero(pics_filelist, N);
	// 遍历指定目录，并对文件名排序输出
	n = scandir("/www/images", &namelist, NULL, alphasort);
	if (n < 0)
		perror("scandir error");
	else
	{
		for (m = 0; m < n; ++m)
		{
			// 排除隐藏文件和特定文件
			if (strncmp(".", namelist[m]->d_name, 1) == 0)
			{
				free(namelist[m]);
				continue;
			}
			if (strncmp("..", namelist[m]->d_name, 2) == 0)
			{
				free(namelist[m]);
				continue;
			}
			if (strncmp("p.jpg", namelist[m]->d_name, 5) == 0)
			{
				free(namelist[m]);
				continue;
			}
			// 根据是否是第一个文件，决定是否添加分隔符
			if (!isFist)
				strncat(pics_filelist, "#", N);
			else
				isFist = 0;
			// 将文件名拼接到文件列表字符串中
			strncat(pics_filelist, namelist[m]->d_name, N);
			(*pcnt)++;							 // 文件计数器增加
			printf("%s\n", namelist[m]->d_name); // 打印当前文件名
		}
		free(namelist);
	}
	// 将统计的文件个数转换为字符串
	// snprintf(list_cnt, 10, "#%d", *pcnt);
	// 在文件列表字符串后面链接文件个数字符串 zzz.jpg#yyy.jpg#XXX.jpg#100
	// strncat(picsFileList, list_cnt, 10);
}

void *recv_mob_thread(void *arg)
{
	int ret;
	char buf[SIZE];
	int newfd = *((int *)arg);
	char cmdstr[50];
	int cmdcnt = 0;
	int pics_list_cnt = 0;
	char list_cnt[10];

	while (1)
	{
		bzero(buf, SIZE);
		ret = recv(newfd, buf, sizeof(buf), 0); // 类似read  阻塞
		if (ret < 0)
			log_e("recv fail");

		log_e("[%d]>收到手机端发送的字符串:%s", __LINE__, buf);

		// 将socket中接收数据缓冲区解析（解析成4个字符串）
		bzero(mob_str[0], strlen(mob_str[0]));
		bzero(mob_str[1], strlen(mob_str[1]));
		bzero(mob_str[2], strlen(mob_str[2]));
		bzero(mob_str[3], strlen(mob_str[3]));
		sscanf(buf, "%s%s%s%s", mob_str[0], mob_str[1], mob_str[2], mob_str[3]);

		// （a）登录请求 ------------------------->  "mobile login 用户名 密码"
		if ((strncmp(mob_str[1], SER_REQ_LOGIN_HEAD, strlen(mob_str[1])) == 0) &&
			(strlen(mob_str[1]) > 0))
		{
			log_e("[%d]>收到手机端登录请求", __LINE__);
			log_e("mob_str[2]=%s", mob_str[2]);
			log_e("mob_str[3]=%s", mob_str[3]);
			// 调用 do_login 验证用户名密码，返回不同的应答（YES/NO）
			ret = do_login(mob_str[2], mob_str[3], db);
			log_e("ret=%d", ret);
			if (ret == 9)
				send(newfd, SER_ACK_NO, strlen(SER_ACK_NO), 0);
			else if (ret == 1)
			{
				// 登录成功后，保存用户名和密码到全局变量
				strncpy(uname, mob_str[2], strlen(mob_str[2]));
				strncpy(pass, mob_str[3], strlen(mob_str[3]));
				send(newfd, SER_ACK_YES, strlen(SER_ACK_YES), 0);
			}
			else
				send(newfd, SER_ACK_NO, strlen(SER_ACK_NO), 0);
		}
		// （b）控制请求 ------------------------->  "control mac led2_green|orange on|off"
		else if ((strncmp(mob_str[0], SER_REQ_CMD_HEAD, strlen(mob_str[0])) == 0) &&
				 (strlen(mob_str[0]) > 0))
		{
			log_e("[%d]>req head:%s\n devmac:%s\nhwName:%s\ncmd:%s", __LINE__, mob_str[0], mob_str[1], mob_str[2], mob_str[3]);
			log_e("[%d]>接收到移动端的控制请求.", __LINE__);
			log_e("[%d]>目标设备的MAC地址:%s", __LINE__, mob_str[1]);
			log_e("[%d]>指令:%s %s\n", __LINE__, mob_str[2], mob_str[3]);

			snprintf(cmdstr, 50, "%s %s", mob_str[2], mob_str[3]);

			// 组装命令字符串，封装为链表节点，入命令队列 pcmdque
			list_pnode new = (list_pnode)malloc(sizeof(list_node));
			if (NULL == new)
			{
				perror("malloc");
				return NULL;
			}
			memset(new, 0, sizeof(list_node));
			strncpy(new->cmdstr, cmdstr, strlen(cmdstr));

			pthread_mutex_lock(&cmd_queue_mutex);
			linkqueue_in(pcmdque, new);
#if 1
			printf("===============================\n");
			cmdcnt++;
			printf("zigbee命令节点%d 入队链表队列....\n", cmdcnt);
			printf("%s\n", new->cmdstr);
			printf("===============================\n\n");
#endif
			pthread_mutex_unlock(&cmd_queue_mutex);
			// 唤醒获取列队命令的线程out_cmd_thread
			pthread_cond_signal(&cmd_queue_cond); 
		}
		// （c）获取参数请求 ------------------------->  "mobile monitor 用户名 密码"
		else if (strncmp(mob_str[1], SER_REQ_MONI_HEAD, strlen(mob_str[1])) == 0)
		{
			log_e("[%d]>收到手机端获取参数请求\n", __LINE__);
			if ((strncmp(uname, mob_str[2], strlen(uname)) == 0) &&
				(strncmp(pass, mob_str[3], strlen(pass)) == 0))
			{
				send(newfd, SER_ACK_YES, strlen(SER_ACK_YES), 0);
				log_a("参数请求响应=================%s", SER_ACK_YES);
			}
			else
			{
				send(newfd, SER_ACK_NO, strlen(SER_ACK_NO), 0);
				log_a("参数请求响应=================%s", SER_ACK_NO);
			}
		}
		// （d）获取参数信息 ------------------------->  "mobile monitor_info 用户名 密码"
		else if (strncmp(mob_str[1], SER_REQ_MONI_INFO, strlen(mob_str[1])) == 0)
		{
			// 读全局缓冲区数据，将最新监控数据发送给手机端
			pthread_mutex_lock(&zigbee_mob_mutex); // 上锁
			send(newfd, send_mob_buf, strlen(send_mob_buf), 0);
			pthread_mutex_unlock(&zigbee_mob_mutex); // 释放锁
			printf("[%d]>发送给手机端的参数: %s\n", __LINE__, send_mob_buf);
		}
		// （e）结束参数获取 -------------------------> 仅打印日志，实际可扩展为资源释放等操作
		else if (strncmp(mob_str[1], SER_REQ_MONI_END, strlen(mob_str[1])) == 0)
		{
			printf("[%d]>收到手机端获取参数结束请求\n", __LINE__);

			// 获取视频图像文件列表
		}
		// （f）获取视频图像文件列表 ------------------------->  "mobile picslist 用户名 密码"
		else if (strncmp(mob_str[1], SER_REQ_PICLIST_HEAD, strlen(mob_str[1])) == 0)
		{
			printf("[%d]>收到手机端获取视频图像文件请求\n", __LINE__);
			// 校验用户名密码，获取图片文件列表和数量，分别发送给手机端
			if ((strncmp("robin", mob_str[2], strlen("robin")) == 0) &&
				(strncmp("123", mob_str[3], strlen("123")) == 0))
			{
				get_pics_fileList(&pics_list_cnt); 
				snprintf(list_cnt, 10, "%d", pics_list_cnt);
				send(newfd, list_cnt, strlen(list_cnt), 0); // 照片数量
				send(newfd, pics_filelist, strlen(pics_filelist), 0); // 照片文件名列表
			}
			else
			{
				send(newfd, SER_ACK_NO, strlen(SER_ACK_NO), 0);
			}
		}

		if (ret == 0)
		{
			log_a("client fd=%d offline", newfd);
			break;
		}
	}
	return NULL;
}
