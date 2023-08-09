/**************************************************************
 * File Name     : 01main_ver1.c
 * Creator       : 郭文鹏
 * QQ            : 1197930600
 * Email         : 1197930600@qq.com
 * Creat Time    : Tue Jul 18 13:14:47 2023
 * 备注          : 设备2的服务器  开一个线程发送数据
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
#include <time.h>
#define FILE_NAME "shebei2.txt"
/**************************************************************
 * 备注          :  宏定义区域
***************************************************************/
#define MQTT_ADDR "tcp://mqtt.yyzlab.com.cn"    //云平台地址
#define ClientID1 "ubunut_guo_2023012"           //标识id号，在同一个云平台下，id号不能冲突             
#define SUB_TOPIC1 "1688716903572/AIOTSIM2APP"  //订阅、发布主题
#define PUB_TOPIC1 "1688716903572/APP2AIOTSIM"
#define QoS 0                                   //通信质量，0，1，2，
#define TIMEOUT 2000L                           //超时值
/**************************************************************
 * 备注          :  全局变量区域
***************************************************************/
char fan_on[100]="{\"fan\":true,\"id\":0}";   //设备控制命令
char fan_off[100]="{\"fan\":false,\"id\":0}";
char lamp_on[100]="{\"lamp\":true,\"id\":0}";   //设备控制命令
char lamp_off[100]="{\"lamp\":false,\"id\":0}";
char alarm_on[100]="{\"alarm\":true,\"id\":0}";   //设备控制命令
char alarm_off[100]="{\"alarm\":false,\"id\":0}";
char doorLock_on[100]="{\"doorLock\":true,\"id\":0}";   //设备控制命令
char doorLock_off[100]="{\"doorLock\":false,\"id\":0}";
char data_buf3[1024];
char data_buf4[1024];
float tem,hum;
float co2,light;
int infrared,sleet;
int lamp,doorLock;
int alarm1;
int fan;
int zhi;
int zhi2;
int zhi3,zhi4,zhi5;
int ret;
MQTTClient mqtt_client;                     //创建mqtt1
pthread_t tid;                              //创建一个线程号
int sockfd;                                 //创建套接字
pthread_t receive_tid;                      //创建发送数据给客户端的线程号                     
/**************************************************************
 * 备注          :  函数声明区域
***************************************************************/
void *clint(void *arg);
int tcp_server(void);
int tcp_close(void);
int mqtt_pub(MQTTClient mqtt_client,const void *paylaod,const char* topicName);
int recv_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
int mqtt1();
int mqtt2(void *arg);
void *receive_thread(void *arg);
void my_delay(int ms);
/**************************************************************
 * 备注          :  主函数区域
***************************************************************/
int main(void)
{
    struct sockaddr_in  clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    ret=tcp_server();      //创建socket连接，支持多线程
    ret =mqtt1();
    while (1)
    {
		int connfd = accept(sockfd, NULL, NULL);
		if(connfd <=0 )
		{
			perror("accept");
			return 0;
		}
		printf("accept connfd = %d\n", connfd);
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
		pthread_create(&tid, NULL, clint, (void *)&connfd);
    }
	ret=tcp_close();
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
	ser.sin_addr.s_addr = inet_addr("10.0.0.16"); //将IP地址转成网络字节序的IP地址

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
	int connfd = *(int *)arg;
	char recv_buf[1024] = "";
    
    memset(data_buf3, 0, sizeof(data_buf3));
    pthread_create(&receive_tid, NULL, receive_thread, (void *)&connfd);

	while(1)
	{
		memset(recv_buf, 0, sizeof(recv_buf));
		ret = read(connfd, recv_buf, sizeof(recv_buf));      //读取客户端数据，等待中
		if(ret <= 0)
		{
			printf("客户端退出！！！\n");
			break;
        }
           
        if(strcmp(recv_buf, "fan_on")==0)
        {
            mqtt_pub(mqtt_client,fan_on,PUB_TOPIC1);
        }else if(strcmp(recv_buf, "fan_off")==0)
        {
            mqtt_pub(mqtt_client,fan_off,PUB_TOPIC1);
        }else if(strcmp(recv_buf, "lamp_on")==0)
        {
            mqtt_pub(mqtt_client,lamp_on,PUB_TOPIC1);
        }else if(strcmp(recv_buf, "lamp_off")==0)
        {
            mqtt_pub(mqtt_client,lamp_off,PUB_TOPIC1);
        }else if(strcmp(recv_buf, "doorLock_on")==0)
        {
            mqtt_pub(mqtt_client,doorLock_on,PUB_TOPIC1);
        }else if(strcmp(recv_buf, "doorLock_off")==0)
        {
            mqtt_pub(mqtt_client,doorLock_off,PUB_TOPIC1);
        }else if(strcmp(recv_buf, "alarm_on")==0)
        {
            mqtt_pub(mqtt_client,alarm_on,PUB_TOPIC1);
        }else if(strcmp(recv_buf, "alarm_off")==0)
        {
            mqtt_pub(mqtt_client,alarm_off,PUB_TOPIC1);
        }
	}
    pthread_detach(receive_tid);
	return NULL;
}
int tcp_close(void)
{
	close(sockfd);
	pthread_detach(tid);
}
void *receive_thread(void *arg)
{
	int connfd = *(int *)arg;
    while (1) {
		write(connfd, data_buf3, sizeof(data_buf3));
        printf("发送的数据为2：%s",data_buf3);
        my_delay(3000);
    }
}
/**************************************************************
 * 备注          :  mqtt通信
***************************************************************/
int mqtt_pub(MQTTClient mqtt_client,const void *paylaod,const char* topicName)
{
    int ret;
    MQTTClient_deliveryToken token;
    ret=MQTTClient_publish(mqtt_client,topicName,strlen(paylaod),paylaod,QoS,0,&token);
    if (ret!= MQTTCLIENT_SUCCESS)
    {
        printf("1111MQTT发布消息失败, return code %d\n",ret);
        return ret;
    }
    //printf("MQTT发布消息成功\n");
    return 0;
}

int recv_message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    cJSON* root = cJSON_Parse((char *)message->payload);
    if (root == NULL) {
        cJSON_Delete(root);
        return 0;
    }

    cJSON *tem_item = cJSON_GetObjectItem(root, "tem");
    cJSON *hum_item = cJSON_GetObjectItem(root, "hum");
    cJSON *light_item = cJSON_GetObjectItem(root, "light");
    cJSON *co2_item = cJSON_GetObjectItem(root, "co2");
    cJSON *infrared_item = cJSON_GetObjectItem(root, "infrared");
    cJSON *sleet_item = cJSON_GetObjectItem(root, "sleet");
    cJSON *fan_item = cJSON_GetObjectItem(root, "fan");
    cJSON *lamp_item = cJSON_GetObjectItem(root, "lamp");
    cJSON *doorLock_item = cJSON_GetObjectItem(root, "doorLock");
    cJSON *alarm_item = cJSON_GetObjectItem(root, "alarm");

    if (tem_item != NULL && hum_item != NULL) {
        tem = tem_item->valuedouble;
        hum = hum_item->valuedouble;
        zhi = cJSON_GetObjectItem(root, "id")->valueint;
    }
    if (light_item != NULL) {

    light = light_item->valuedouble;
    }
    if (co2_item != NULL) {
    co2 = co2_item->valuedouble;
    }
    if (infrared_item != NULL) {
    infrared = cJSON_IsTrue(infrared_item);
    }
    if (sleet_item != NULL) {
    sleet = cJSON_IsTrue(sleet_item);
    }
    if (fan_item != NULL) {
    fan = cJSON_IsTrue(fan_item);
    }
    if (alarm_item != NULL) {
    alarm1 = cJSON_IsTrue(alarm_item);
    }

    if (lamp_item != NULL) {
    lamp = cJSON_IsTrue(lamp_item);
    }
    if (doorLock_item != NULL) {
    doorLock = cJSON_IsTrue(doorLock_item);
    }
    sprintf(data_buf3, "{\"tem\":%f,\"hum\":%f,\"light\":%f,\"co2\":%f,\"infrared\":%d,\"sleet\":%d,\"fan\":%d,\"lamp\":%d,\"doorLock\":%d,\"alarm\":%d}", tem, hum, light, co2, infrared, sleet, fan, lamp, doorLock, alarm1);
    printf("获取mqtt的值2：%f,%f,%f,%f,%d,%d,%d,%d,%d,%d\n", tem, hum, light, co2, infrared, sleet, fan, lamp, doorLock, alarm1);
    cJSON_Delete(root);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int mqtt1(void)
{
    //int connfd = *(int *)arg;
    // 创建MQTTClient
    ret = MQTTClient_create(&mqtt_client, MQTT_ADDR, ClientID1, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (ret != MQTTCLIENT_SUCCESS)
    {
        printf("MQT111TClient_create failed\n");
        return -1;
    }
    // 设置回调函数（用于接受数据）
    ret = MQTTClient_setCallbacks(mqtt_client,NULL, NULL, recv_message,NULL);
    if (ret != MQTTCLIENT_SUCCESS)
    {
        printf("111MQTTClient_setCallbacks failed\n");
        return -1;
    }
    // 连接到mqtt代理服务器
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    ret = MQTTClient_connect(mqtt_client, &conn_opts);
    if (ret != MQTTCLIENT_SUCCESS)
    {
        printf("11MQTTClient_connect failed\n");
        return -1;
    }
    // 订阅
    ret = MQTTClient_subscribe(mqtt_client, SUB_TOPIC1, QoS);
    if (ret != MQTTCLIENT_SUCCESS)
    {
        printf("111MQTTClient_subscribe failed\n");
        return -1;
    }
    return 0;
}


