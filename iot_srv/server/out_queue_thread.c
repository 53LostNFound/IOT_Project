/*
	从环境参数队列（pque）中取出由协调器采集到的监控数据，并进行进一步处理（如写入全局缓冲区、数据库等）
*/

#include "iot_server.h"
#if defined(LOG_TAG)
#undef LOG_TAG
#define LOG_TAG "out_queue_thread"
#else
#define LOG_TAG "out_queue_thread"
#endif

void *out_queue_thread(void *arg)
{
	int cnt = 0;
	char args[SIZE];
	memset(args, 0, SIZE);
	printf("call %s\n", __FUNCTION__);
	while (1)
	{
		// ===================================================================================
		pthread_mutex_lock(&queue_mutex);
		 // 等待接收协调器数据线程唤醒，保证只有在有新数据时才会继续执行，避免无谓的CPU占用
		pthread_cond_wait(&queue_cond, &queue_mutex);
		memset(&args, 0, SIZE);						  // arg临时保存每个终端发来的环境参数,先清0

		if (linkqueue_out(pque, args))
		{
#if 0
			cnt++;
			log_w("出队节点编号为:%d %s\n", cnt, args);
#endif
		}
		pthread_mutex_unlock(&queue_mutex);
		// ===================================================================================
		// 将读取节点的数据写入全局数组的缓冲区中
		// zigbee_mob_mutex 用于保护全局缓冲区send_mob_buf，防止并发写入导致数据错乱。
		pthread_mutex_lock(&zigbee_mob_mutex); // 上锁
		if (strlen(args) > 0)
		{
			strncpy(send_mob_buf, args, strlen(args));
			do_args(send_mob_buf, db);
			bzero(args, SIZE);
		}
		pthread_mutex_unlock(&zigbee_mob_mutex); // 解锁
	}

	return NULL;
}
