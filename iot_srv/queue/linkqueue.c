#include "linkqueue.h"


void linkqueue_init(link_pqueue *Q)
{
	//创建封装头尾指针的队列指针对象
	*Q = (link_pqueue)malloc(sizeof(link_queue));
	if (*Q == NULL) {
		perror("malloc");
		exit(1);
	}
	//创建链表队列的头节点让front指向头节点
	(*Q)->front = (list_pnode)malloc(sizeof(list_node));
	if ((*Q)->front == NULL) {
		perror("malloc");
		exit(1);
	}
	//让头节点的后继为NULL,让rear指向头节点
	(*Q)->front->next = NULL;
	(*Q)->rear = (*Q)->front;
	
	
}
bool isempty_linkqueue(link_pqueue pque)
{
	//当front和rear指针指向相同就是空队列
	if (pque->front == pque->rear)
		return true;
	else 
		return false;
}
void linkqueue_in(link_pqueue pque, list_pnode new)
{
	new->next = pque->rear->next;  //将尾节点后继保存到new节点后继
	pque->rear->next = new; //将new保存到尾节点后继
	//pque->rear = pque->rear->next; //将尾指针指向new节点
	pque->rear = new;
}
#if 0
void linkqueue_out(link_pqueue pque, struct wifi_data *data)
{
	list_pnode tmp = NULL;
	//判断队列是否为空
	if (isempty_linkqueue(pque)) {
		printf("队列为空,无法出队!\n");
		return;
	}
	*data = pque->front->next->data;
	tmp = pque->front;
	pque->front = tmp->next;
	free(tmp);

}
#else
bool linkqueue_out(link_pqueue pque, char	args[SIZE])
{
	list_pnode tmp = NULL;
	//判断队列是否为空
	if (isempty_linkqueue(pque)) {
		printf("队列为空,无法出队!\n");
		return false;
	}
	strncpy(args, pque->front->next->mob_str_buf, SIZE);
	//args = pque->front->next->mob_str_buf;
	tmp = pque->front;
	pque->front = tmp->next;
	free(tmp);
	return true;

}

#endif



bool linkqueue_cmdstr_out(link_pqueue pcmdque, char  *cmdstr)
{
	list_pnode tmp = NULL;
	//判断队列是否为空
	if (isempty_linkqueue(pcmdque)) {
		printf("命令队列为空,无法出队!\n");
		return false;
	}
	strncpy(cmdstr,pcmdque->front->next->cmdstr, strlen(pcmdque->front->next->cmdstr));
	tmp = pcmdque->front;
	pcmdque->front = tmp->next;
	free(tmp);
	return true;

}


void linkqueue_show(link_pqueue pque)
{
	list_pnode p;
	printf("----------------------------------------------------\n");
	printf("队头\n");
	for (p = pque->front->next; p != NULL; p = p->next) {
		#if 0
		printf("no:%s led1:%s led2:%s led3:%s jdq:%s motor:%s rgb:%s temper:%f humi:%f\n", 
			p->data.modno, p->data.led1, p->data.led2,p->data.led3, 
			p->data.jdq, p->data.dc_motor, p->data.rgb, 
			p->data.temper, p->data.humi);
		#else
			//printf("modno:%d \n", p->endp_args.data[1]);
		#endif
		usleep(100000);
	}
	printf("队尾\n");
	printf("----------------------------------------------------\n");

}
