

#ifndef __IOT_SERVER_H__
#define __IOT_SERVER__
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elog.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>			/* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sqlite3.h>
#include <signal.h>

//获取视频图像文件
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#define N 4096





#include "linkqueue.h"

#define SIZE 1024
#define LEN_MONI 7 //监控数据的字节数为7

//实现和移动端通信用的标志
#define DEV_ID_STR  			"peripheral"
#define MOBL_ID_STR  		 	"mobile"
#define SER_ACK_YES             "yes"
#define SER_ACK_NO              "no"
#define SER_ACK_OK              "ok"
#define SER_REQ_LOGIN_HEAD  	"login"
#define SER_REQ_REG_HEAD      	"register"
#define SER_REQ_CMD_HEAD      	"control"
#define SER_REQ_BIND_HEAD     	"bind"
#define SER_REQ_MONI_HEAD		"monitor"
#define SER_REQ_MONI_INFO		"monitor_info"
#define SER_REQ_MONI_END		"stop_moninfo_send"
#define DATABASE				"../iot.db"

#define SER_REQ_PICLIST_HEAD	"picslist" //获取拍照的文件列表请求
#define SER_REQ_MONI_LIST		"monitor_list"




extern char send_mob_buf[SIZE]; //发送监控数据字符串给移动端的缓冲区

extern link_pqueue pque; //定义全局的封装环境参数队列指针的对象
extern link_pqueue pcmdque; 	//定义全局的封装zigbee命令队列指针的对象

extern pthread_mutex_t queue_mutex; //创建环境参数队列用的互斥锁 
extern pthread_mutex_t zigbee_mob_mutex; //创建物联网环境参数字符串给移动端用的互斥锁
extern pthread_mutex_t cmd_queue_mutex;	//创建命令队列用的互斥锁 

//extern pthread_mutex_t coord_mutex;//创建互斥锁

extern pthread_cond_t queue_cond; 		//创建队列用条件变量
extern pthread_cond_t cmd_queue_cond; 	//创建命令队列用的条件变量


extern pthread_t tid_recv_mob_thread;
extern pthread_t tid_recv_coord_thread;
extern pthread_t tid_out_queue_thread;

extern sqlite3 *db;

extern char uname[20]; //保存登录成功的用户名
extern char pass[20];


void* recv_coord_thread(void *arg);
void* recv_mob_thread(void *arg);
void * out_queue_thread(void *arg);
void * out_cmd_thread(void *arg);

int do_login(char *uname, char *pass, sqlite3 *db);
int  do_args(char   *args, sqlite3 *db);
int  do_args(char   *args, sqlite3 *db);

void init_db(sqlite3 **pdb);




#endif
