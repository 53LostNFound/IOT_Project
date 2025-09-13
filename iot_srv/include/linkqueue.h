#ifndef _LINKQUEUE_H_
#define _LINKQUEUE_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#define SIZE 1024
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//定义链表队列中节点类型
typedef struct listnode {
	char mob_str_buf[SIZE];
	char cmdstr[50]; 
	struct listnode *next; //节点的后继指针
}list_node, *list_pnode;

//定义链表队列中封装头指针和尾指针的队列指针对象类型
typedef struct linkqueue {
	list_pnode front, rear;
}link_queue, *link_pqueue;

void linkqueue_init(link_pqueue *Q);
bool isempty_linkqueue(link_pqueue pque);
void linkqueue_in(link_pqueue pque, list_pnode new);
bool linkqueue_out(link_pqueue pque, char mob_str_buf[SIZE]);
bool linkqueue_cmdstr_out(link_pqueue pcmdque, char  *cmdstr);

void isfull_linkqueue(link_pqueue queue);
void linkqueue_show(link_pqueue queue);



#endif 
