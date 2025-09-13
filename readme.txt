增加了 

队列链表
queue目录
	linkqueue.c
	修改了linkqueue.h

命令出队线程
out_cmd_thread.c

修改了 Makefile


客户端没修改


n@ubuntu:~/share/iot/12_cmd/iot_srv$ find . -type f -name "*.c" -exec wc -l {} + | tail -n +2 | awk '{s+=$1} END {print s}'
4924
robin@ubuntu:~/share/iot/12_cmd/iot_srv$ find . -type f -name "Makefile" -exec wc -l {} + | tail -n +2 | awk '{s+=$1} END {print s}'
205
robin@ubuntu:~/share/iot/12_cmd/iot_srv$ find . -type f -name "*.h" -exec wc -l {} + | tail -n +2 | awk '{s+=$1} END {print s}'
1074




