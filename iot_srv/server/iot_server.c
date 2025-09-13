/*
	linux物联网平台服务器端主程序

*/

#include "iot_server.h"

link_pqueue pque; 		//定义全局的封装环境参数队列指针的对象
link_pqueue pcmdque; 	//定义全局的封装zigbee命令队列指针的对象


pthread_mutex_t queue_mutex; //创建环境参数队列用的互斥锁 
//pthread_mutex_t coord_mutex;//创建互斥锁
pthread_mutex_t zigbee_mob_mutex; //创建物联网环境参数字符串给移动端用的互斥锁
pthread_mutex_t cmd_queue_mutex; //创建命令队列用的互斥锁 

pthread_cond_t queue_cond; 	    //创建环境参数队列用条件变量
pthread_cond_t cmd_queue_cond; 	//创建命令队列用的条件变量


pthread_t tid_recv_mob_thread;
pthread_t tid_recv_coord_thread;
pthread_t tid_out_queue_thread;
pthread_t tid_out_cmd_thread;

sqlite3 *db;


#if defined(LOG_TAG)
#undef LOG_TAG 
#define LOG_TAG    "iot_server"
#else 
#define LOG_TAG    "iot_server"
#endif


// 初始化 EasyLogger 日志系统，设置日志级别和输出格式，方便调试和运行时追踪。
void init_elog(void)
{
	
	/* 初始化 EasyLogger */
	elog_init(); //必须

	/* 设置全局日志过滤级别为 VERBOSE（最低级别） */
	elog_set_filter_lvl(ELOG_LVL_VERBOSE);
	elog_set_output_enabled(true);

	/*设置输出格式*/
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL); //a 所有信息
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL&~ELOG_FMT_P_INFO&~ELOG_FMT_T_INFO);
	// 自定义日志格式：时间 | 级别 | 标签 | 内容 
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_ALL&~ELOG_FMT_P_INFO&~ELOG_FMT_T_INFO ); //无TAG PID
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_ALL&~ELOG_FMT_P_INFO&~ELOG_FMT_T_INFO ); //无PID TID
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL&~ELOG_FMT_P_INFO&~ELOG_FMT_T_INFO ); //d 所有信息
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL&~ELOG_FMT_P_INFO&~ELOG_FMT_T_INFO ); //V 所有信息

}


void start_tcp_server(void)
{
	int ret = -1;
	int newfd ;
	//recv_coord_fun();
	//1.买电话
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		log_e("socket %s", strerror(errno));
	}
	//重用端口
	int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	
	//2.绑卡 ip+port
	struct sockaddr_in server; //设置服务端ip+port
	server.sin_family = AF_INET;
	server.sin_port = htons(12345);
	server.sin_addr.s_addr = inet_addr("172.18.3.79");
	ret = bind(fd, (struct sockaddr *)&server, sizeof(server));
	if (ret == -1) {
		log_e("bind %s", strerror(errno));
	}
	
	//3.监听
	listen(fd, 5);

	//4.接听
	struct sockaddr_in client; //接收客户端ip+port对象
	socklen_t len = sizeof(client);

	log_d("tcp server start... \n");
	system("netstat -na | grep 12345"); //查看tcp连接状态
	

	// 接收客户请求并建立和服务器端连接 并发服务器
	while (1) {
		newfd = accept(fd, (struct sockaddr *)&client, &len); //阻塞函数
		//没有客户端请求就让进程暂停,当有请求成功,建立连接返回
		if (newfd == -1) {
			perror("accept");
		}
		log_v("client ip=%s\n", inet_ntoa(client.sin_addr));
		log_v("client port=%d\n", ntohs(client.sin_port));
		log_v("客户端 fd=%d 上线!\n", newfd);
		

		//5.通信
		//创建接收线程
		pthread_create(&tid_recv_mob_thread, NULL, recv_mob_thread, &newfd);
	}
	//6.挂电话
	close(fd);
}

//当ctrl+c 或 ctrl+\  给进程发信号,内核就调用信号处理函数main_exit
// 释放项目的资源  2个线程,锁对象

void main_exit(int signo)
{
	pthread_cancel(tid_recv_mob_thread);
	pthread_cancel(tid_recv_coord_thread);
	pthread_cancel(tid_out_queue_thread);
	pthread_cancel(tid_out_cmd_thread);
//	pthread_mutex_destroy(&coord_mutex);
	pthread_mutex_destroy(&zigbee_mob_mutex);
	pthread_mutex_destroy(&queue_mutex);
	pthread_mutex_destroy(&cmd_queue_mutex);
	pthread_cond_destroy(&queue_cond);
	pthread_cond_destroy(&cmd_queue_cond);
	sqlite3_close(db);       //关闭数据库

	log_e("main thread quit, release all resource\n");
	exit(0);
}


int main(void)
{

	init_elog();
	
	log_a("linux 物联网中控");
	
	//注册信号处理函数
	signal(SIGINT, main_exit);
	signal(SIGQUIT, main_exit);

	
	//初始化
	linkqueue_init(&pque); //初始化zigbee终端环境参数队列
	linkqueue_init(&pcmdque); //初始化zigbee命令队列

	
	pthread_mutex_init(&queue_mutex, NULL);
	pthread_mutex_init(&zigbee_mob_mutex, NULL);
	pthread_mutex_init(&cmd_queue_mutex, NULL);
//	pthread_mutex_init(&coord_mutex, NULL);
	
	//初始化链表条件变量
	pthread_cond_init(&queue_cond, NULL);
	pthread_cond_init(&cmd_queue_cond, NULL);

	init_db(&db);
	
	// 接收协调器数据线程
	pthread_create(&tid_recv_coord_thread, NULL, recv_coord_thread, NULL);

	// 创建获取zigbee终端环境参数队列数据节点的线程
	pthread_create(&tid_out_queue_thread, NULL, out_queue_thread, NULL);

	//创建获取zigbee命令链表队列数据节点的线程
	pthread_create(&tid_out_cmd_thread, NULL, out_cmd_thread, NULL);


	start_tcp_server();

	

	

	return 0;
}
