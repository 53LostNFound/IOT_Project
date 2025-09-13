/*
	服务端将从协调器接收到的数据转换为字符串格式，并保存到链表节点中。
*/

#include "coord.h"
#include "iot_server.h"

#if defined(LOG_TAG)
#undef LOG_TAG
#define LOG_TAG "recv_coord_thread"
#else
#define LOG_TAG "recv_coord_thread"
#endif

char mob_str_buf[SIZE]; // 发送监控数据字符串给移动端的缓冲区

void *recv_coord_thread(void *arg)
{
	int i;
	//	int check = 0; //未同步
	char vbuf[SIZE];
	node_data buf;
	int ret = 0;
	int in_cnt = 0;
	int line = 0;
	char byte;		 // 用来保存串口一个字节的变量
	char syncok = 0; // 是否同步成功的标志
	// 打开串口设备文件，读取 Zigbee 节点数据
	int fd = open("../uart_data.dat", O_RDONLY);
	lseek(fd, 5, SEEK_SET);
	log_d("连接串口设备,正在同步...");
	while (1)
	{
		bzero(vbuf, SIZE);
		// 循环读取一个数据包
		while (1)
		{
			byte = 0;
			ret = read(fd, &byte, 1); // 随机读取一个字节
			if (ret == 0)
				goto repeat;
			usleep(80000);
			// cnt++;
			// 依次判断是否为 "##IDTH" 头
			if (byte == '#')
			{
				byte = 0;
				ret = read(fd, &byte, 1);
				usleep(80000);
				if (ret == 0)
					goto repeat;
				if (byte == '#')
				{
					byte = 0;
					ret = read(fd, &byte, 1);
					usleep(80000);
					if (ret == 0)
						goto repeat;
					if (byte == 'I')
					{
						byte = 0;
						ret = read(fd, &byte, 1);
						usleep(80000);
						if (ret == 0)
							goto repeat;
						if (byte == 'D')
						{
							byte = 0;
							ret = read(fd, &byte, 1);
							usleep(80000);
							if (ret == 0)
								goto repeat;
							if (byte == 'T')
							{
								byte = 0;
								ret = read(fd, &byte, 1);
								usleep(80000);
								if (ret == 0)
									goto repeat;
								if (byte == 'H')
								{
									// ##IDTH全部找到，确保数据包的起始位置正确
									// 读取后面7个字节，一共13个字节为一个数据包
									for (i = 0; i < LEN_MONI; i++)
									{
										ret = read(fd, &byte, 1);
										if (ret == 0)
											goto repeat;
										// 将读取到的7字节监控数据存储到 buf.data 数组中
										buf.data[i] = byte;
										usleep(80000);
									}
									if (i == LEN_MONI)
									{
										syncok = 1;
										line++;

										//									for (i = 0; i < LEN_MONI; i++) {
										//										printf("%02x ", buf.data[i]);
										//									}
										/*log_d("==%d== %02x %02x %02x %02x %02x %02x %02x", line, \
											buf.data[0], buf.data[1], buf.data[2], buf.data[3],\
											buf.data[4], buf.data[5], buf.data[6]);*/
									}
								}
							}
						}
					}
				}
			}

			if (syncok == 1)
			{

#if 0
				pthread_mutex_lock(&coord_mutex);//上锁
				bzero(send_mob_buf, SIZE);
				convert_moni_info(&buf, send_mob_buf);
				pthread_mutex_unlock(&coord_mutex);//释放锁
#endif
				// 将原始数据buf转换成字符串格式mob_str_buf
				bzero(mob_str_buf, SIZE);
				convert_moni_info(&buf, mob_str_buf);

				// 创建一个链表节点，存储格式化后的字符串
				list_pnode new;

				// 将一个终端节点数据保存到一个链表节点中
				new = (list_pnode)malloc(sizeof(list_node));
				if (NULL == new)
				{
					perror("malloc");
					return NULL;
				}
				memset(new, 0, sizeof(list_node)); // 新节点清0
				memcpy(new->mob_str_buf, mob_str_buf, SIZE);
				// 加锁，保证链表操作安全，此时只能入队列不可以取出队列
				pthread_mutex_lock(&queue_mutex);
				linkqueue_in(pque, new);
				in_cnt++;
#if 0
				log_i("入队节点编号:%d   %s\n",in_cnt, new->mob_str_buf		);
#endif
				pthread_mutex_unlock(&queue_mutex);
				// 配合条件变量，通知其他等待队列的线程（比如数据处理线程），有新数据可以处理
				pthread_cond_signal(&queue_cond); 

				syncok = 0;
			}
		}
		if (ret > 0)
			continue;
	repeat:
		lseek(fd, 0, SEEK_SET);
		line = 0;
	}

	return NULL;
}
