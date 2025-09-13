/*
	sqlite3_op.c 主要负责服务器端与 SQLite 数据库的交互，包括用户登录验证、环境参数存储、命令存储
	以及数据库的初始化和表结构创建。它是整个物联网服务器数据持久化的核心模块。
*/

#include "iot_server.h"

#if defined(LOG_TAG)
#undef LOG_TAG
#define LOG_TAG "sqlite3_op"
#else
#define LOG_TAG "sqlite3_op"
#endif


// 验证用户名和密码是否存在于 users 表中，实现登录认证。
int do_login(char *uname, char *pass, sqlite3 *db)
{

	char sql[SIZE] = {0};
	char *errmsg;
	char **result;
	int nrow;
	int ncolumn;
	int ret;

	sprintf(sql, "select * from users where uname = \"%s\" and  pass = \"%s\";", uname, pass);
	log_e("sql=%s\n", sql);
	// 查询数据库
	if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != SQLITE_OK) // 如果失败，将出错信息打印，并将失败提示传给客户端。只需要查找是否存在即可。
	{
		ret = 9;
	}
	else
	{
		if (nrow == 0) // 查询到零条也代表查询失败，此用户未注册，返回给客户端
			ret = 9;
		else
			ret = 1; // 查询到符合条件的数据，传给客户端。
	}
	sqlite3_free_table(result); // 释放查询的信息。

	return ret;
}


// 添加环境参数进表，结构体type代表种类，另两个值代表要存入的用户名与密码
int do_args(char *args, sqlite3 *db) 
{
	// 解析参数字符串（格式如 "23.5#45.2#1234#0"）
	char sql[SIZE] = {0};
	char *errmsg;
	char buf[SIZE];
	char arg_str[4][50] = {0};
	strncpy(buf, args, SIZE);
	char *p = strtok(buf, "#");
	bzero(arg_str[0], 50);
	strncpy(arg_str[0], p, strlen(p));
	p = strtok(NULL, "#");
	bzero(arg_str[1], 50);
	strncpy(arg_str[1], p, strlen(p));
	p = strtok(NULL, "#");
	bzero(arg_str[2], 50);
	strncpy(arg_str[2], p, strlen(p));
	p = strtok(NULL, "#");
	bzero(arg_str[3], 50);
	strncpy(arg_str[3], p, strlen(p));
	// 添加到user表，存放信息。
	sprintf(sql, "insert into args(zigno, tem, hum, lux, dev_state) values(1, %s, %s, %s, %s);",
			arg_str[0], arg_str[1], arg_str[2], arg_str[3]); 
	//	log_w(sql);
	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{

		log_e("%s", errmsg); // 失败，打印错误信息，并将提示写入结构体返回给客户端看
		return 1;
	}
	else
		return 0; // 成功，将成功信息写入结构体，返回给客户端
}


// 添加命令进表，结构体type代表种类，另两个值代表要存入的用户名与密码
int do_cmds(char *args, sqlite3 *db) 
{
	char sql[SIZE] = {0};
	char *errmsg;
	char devname[30];
	char op[30];
	// args:
	sscanf(args, "%s%s", devname, op);
	// 添加到user表，存放信息。
	sprintf(sql, "insert into cmds(uname, devname, op) values('%s', '%s', '%s');",
			uname, devname, op);
	log_w(sql);
	if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{

		log_e("%s", errmsg); // 失败，打印错误信息，并将提示写入结构体返回给客户端看
		return 1;
	}
	else
		return 0; // 成功，将成功信息写入结构体，返回给客户端
}

// 单次打开+长连接的方案在性能上显著优于频繁开关，
void init_db(sqlite3 **pdb)
{
	char *errmsg;
	if (sqlite3_open(DATABASE, pdb) != SQLITE_OK)
	{ // 打开数据库
		log_e("%s\n", sqlite3_errmsg(*pdb));
		exit(1);
	}

	if (sqlite3_exec(*pdb, "create table if not exists users (uname char(30) primary key, pass char(30));", NULL, NULL, &errmsg) != SQLITE_OK)
	{
		log_e("%s", errmsg);
	}
	else
	{
		log_a("create or open table users success\n");
	}

	if (sqlite3_exec(*pdb,
					 "create table args(id integer primary key autoincrement, zigno integer, tem integer, hum integer, lux integer, dev_state integer, insert_date TimeStamp default (date('now', 'localtime')), insert_time timeStamp default (time('now', 'localtime')) );", NULL, NULL, &errmsg) != SQLITE_OK)
	{
		log_a("%s", errmsg);
	}
	else
	{
		printf("create or open table args success\n");
	}

	if (sqlite3_exec(*pdb,
					 "create table cmds(id integer primary key autoincrement, uname  char(30), devname char(100), op char(100), insert_date TimeStamp default (date('now', 'localtime')), insert_time timeStamp default (time('now', 'localtime')) );", NULL, NULL, &errmsg) != SQLITE_OK)
	{
		log_a("%s", errmsg);
	}
	else
	{
		printf("create or open table cmds success\n");
	}
}
