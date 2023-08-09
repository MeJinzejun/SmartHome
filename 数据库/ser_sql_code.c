/**************************************************************
 * File Name     : 01main_ver1.c
 * Creator       : 郭文鹏
 * QQ            : 1197930600
 * Email         : 1197930600@qq.com
 * Creat Time    : Tue Jul 18 13:14:47 2023
 * 备注          : 数据库程序，实现接收客户端发送用户注册信息，以及登录信息。
 * 将注册信息写入sqlite数据库，当用户登录时调用数据库判断是否登录成功
***************************************************************/
#include <stdio.h>                 
#include <string.h>
#include <stdlib.h>                 //基础库文件
#include <unistd.h>
#include "MQTTClient.h"             //mqtt库文件
#include <sys/types.h>                     
#include <sys/socket.h>
#include <netinet/ip.h> 
#include <netinet/in.h>
#include <arpa/inet.h>              //tcp库文件
#include <pthread.h>                //线程库文件
#include "cJSON.h"                  //json库文件
#include <sqlite3.h>
#include <time.h>
#define FILE_NAME "xiaoxi.txt"
/**************************************************************
 * 备注          :  全局变量区域
***************************************************************/
int ret;
pthread_t tid;                              //创建一个线程号
int sockfd;                                 //创建套接字                   
sqlite3 *db;
/**************************************************************
 * 备注          :  函数声明区域
***************************************************************/
void *clint(void *arg);
int tcp_server(void);
int tcp_close(void);
void my_delay(int ms);
void loginUser(sqlite3 *db,char *user,char *pass,void *arg);
static int callback(void *data, int argc, char **argv, char **azColName);
void registerUser(sqlite3 *db,char *user,char *pass,void *arg);
/**************************************************************
 * 备注          :  主函数区域
***************************************************************/
int main(void)
{   
    //int serverSocket, clientSocket;
    struct sockaddr_in  clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int rc;
    // 打开或创建数据库文件
    rc = sqlite3_open("database.db", &db);
    if(rc){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    // 创建用户表
    char *sql = "CREATE TABLE IF NOT EXISTS users ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "username TEXT NOT NULL,"
                "password TEXT NOT NULL);";
    char *errMsg = 0;
    rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    ret=tcp_server();      //创建socket连接，支持多线程
    while (1)
    {
		int connfd = accept(sockfd, NULL, NULL);
		if(connfd <=0 )
		{
			perror("accept");
			return 0;
		}
        // 提取客户端地址和端口
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);
        // 打印客户端地址和端口
        printf("Connected with client: %s:%d\n", clientIP, clientPort);
        time_t current_time;
        struct tm *time_info;
        char time_str[50];
        time(&current_time);
        time_info = localtime(&current_time);
        // 格式化时间字符串
        //strftime(time_str, sizeof(time_str), "%Y年%m月%d日%H时%M分%S秒", time_info);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
        // 打印当前时间
        printf("当前时间：%s\n", time_str);
        FILE *file = fopen(FILE_NAME, "a");
        if (file == NULL) {
            printf("无法打开文件！\n");
            exit(1);
        }
        fprintf(file, "ip地址：%s 端口：%d 时间：%s\n", clientIP, clientPort,time_str);
        fclose(file);

        //printf("注册成功！\n"); 
		pthread_create(&tid, NULL, clint, (void *)&connfd);
    }
	ret=tcp_close();
    sqlite3_close(db);
    return 0;
}
/**************************************************************
 * 备注          :  tcp通信
***************************************************************/
void my_delay(int ms)
{
    for(int i=0;i<ms;i++)
	{
		for(int j=0;j<8900;j++);
	}
}
/**************************************************************
 * 备注          :  tcp通信
***************************************************************/
int tcp_server(void)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket err");                  //打印系统调用函数的错误信息
		return 0;
	}
	//2.绑定
	struct sockaddr_in ser;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(5902);           //将主机序转成网络序
	ser.sin_addr.s_addr = inet_addr("10.0.8.3"); //将IP地址转成网络字节序的IP地址

	int ret = bind(sockfd, (struct sockaddr *)&ser, sizeof(ser));
	if(ret == -1)
	{
		perror("bind err");
		return 0;
	}
	//3.监听
		ret = listen(sockfd, 5);
		if(ret == -1)
		{
			perror("listen err");
			return 0;
		}
		printf("listing-------------\n");
}
void *clint(void *arg)
{
    char recv_buf[1024]="";
    char recv_buf2[1024]="";
    char recv_buf3[1024]="";
    char recv_buf4[1024]="";
    char recv_buf5[1024]="";
	int connfd = *(int *)arg;
	while(1)
	{
		memset(recv_buf, 0, sizeof(recv_buf));
        memset(recv_buf2, 0, sizeof(recv_buf2));
        memset(recv_buf3, 0, sizeof(recv_buf3));
        memset(recv_buf4, 0, sizeof(recv_buf4));
        memset(recv_buf5, 0, sizeof(recv_buf5));
        //printf("等待\n");
		ret = read(connfd, recv_buf, sizeof(recv_buf));      //读取客户端数据，等待中
		if(ret <= 0)
		{
			printf("客户端退出！！！\n");
			break;
		}
        //printf("shuju:%s",recv_buf);
        cJSON* root = cJSON_Parse(recv_buf);
        char* json_str = cJSON_Print(root);
        printf("JSON数据：%s\n", json_str);
        if (root == NULL) {
                cJSON_Delete(root);
            }
        if (cJSON_HasObjectItem(root, "zhuce"))
        {
            //printf("进入\n");
            cJSON * tem_item1 =  cJSON_GetObjectItem(root,"yonghu");
            //char recv_buf2[1024] = tem_item->valuestring;
            strcpy(recv_buf2, tem_item1->valuestring);
            cJSON * hum_item1 =  cJSON_GetObjectItem(root,"mima");
            //char recv_buf3[1024] = hum_item->valuestring;
            strcpy(recv_buf3, hum_item1->valuestring);
            printf("注册shuru1:%smima:%s\n",recv_buf2,recv_buf3);
            registerUser(db,recv_buf2,recv_buf3,(void *)&connfd);
        }
        if (cJSON_HasObjectItem(root, "denglu"))
        {
            //printf("进入\n");
            cJSON * tem_item2 =  cJSON_GetObjectItem(root,"yonghu");
            strcpy(recv_buf4, tem_item2->valuestring);
            //char recv_buf2[1024] = tem_item->valuestring;
            cJSON * hum_item2 =  cJSON_GetObjectItem(root,"mima");
            strcpy(recv_buf5, hum_item2->valuestring);
            printf("登录shuru1:%smima:%s\n",recv_buf4,recv_buf5);
            //char recv_buf3[1024] = hum_item->valuestring;
            loginUser(db,recv_buf4,recv_buf5,(void *)&connfd);
        }
        cJSON_Delete(root);
	}
	return NULL;
}
int tcp_close(void)
{
	close(sockfd);
	pthread_detach(tid);
}
// 回调函数用于处理SQL查询结果
static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

// 注册函数
void registerUser(sqlite3 *db,char *user,char *pass,void *arg){
    // 构建插入数据的SQL语句
    char sql[100];
    char data_buf3[1024];
    char data_buf4[1024];
    int connfd = *(int *)arg;
    sprintf(sql, "INSERT INTO users (username, password) VALUES ('%s', '%s')", user, pass);
    // 执行插入数据的SQL语句
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if(rc != SQLITE_OK){
        printf("Registration failed: %s\n", errMsg);
        sprintf(data_buf3, "{\"denglu\":\"%s\"}", "mmmm");
        write(connfd, data_buf3, sizeof(data_buf3));

    } else {
        printf("Registration successful!\n");
        sprintf(data_buf4, "{\"denglu\":\"%s\"}", "kkkk");
        write(connfd, data_buf4, sizeof(data_buf4));

    }
}

void loginUser(sqlite3 *db,char *user,char *pass,void *arg){
    char sql[100];
    char data_buf3[1024];
    char data_buf4[1024];
    int connfd = *(int *)arg;
    sprintf(sql, "SELECT * FROM users WHERE username='%s' AND password='%s'", user, pass);
    // 执行查询数据的SQL语句
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if(rc != SQLITE_OK){
        printf("Login failed: %s\n", errMsg);
    } else {
        // 获取查询结果的行数
        int count = 0;
        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            count++;
        }
        sqlite3_finalize(stmt);
        if(count > 0){
            sprintf(data_buf3, "{\"denglu\":\"%s\"}", "aaaa");
            write(connfd, data_buf3, sizeof(data_buf3));
            printf("Login successful!\n");
        } else {
            sprintf(data_buf4, "{\"denglu\":\"%s\"}", "bbbb");
            write(connfd, data_buf4, sizeof(data_buf4));
            printf("Invalid username or password\n");
        }
    }
}
