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
#define FILE_NAME "shebei3.txt"
/**************************************************************
 * 备注          :  宏定义区域
***************************************************************/
#define MQTT_ADDR "tcp://mqtt.yyzlab.com.cn"    //云平台地址
#define ClientID1 "ubunut_guo_202301567774"           //标识id号，在同一个云平台下，id号不能冲突  
          
#define SUB_TOPIC1 "1688716903573/AIOTSIM2APP"  //订阅、发布主题
#define PUB_TOPIC1 "1688716903573/APP2AIOTSIM"

#define QoS 0                                   //通信质量，0，1，2，
#define TIMEOUT 2000L                           //超时值
/**************************************************************
 * 备注          :  全局变量区域
***************************************************************/
// char fan_on[100]="{\"fan\":true,\"id\":0}";   //设备控制命令
// char fan_off[100]="{\"fan\":false,\"id\":0}";
// char lamp_on[100]="{\"lamp\":true,\"id\":0}";   //设备控制命令
// char lamp_off[100]="{\"lamp\":false,\"id\":0}";
// char alarm_on[100]="{\"alarm\":true,\"id\":0}";   //设备控制命令
// char alarm_off[100]="{\"alarm\":false,\"id\":0}";
// char doorLock_on[100]="{\"doorLock\":true,\"id\":0}";   //设备控制命令
// char doorLock_off[100]="{\"doorLock\":false,\"id\":0}";
// char feng_on[100]="{\"feng\":true,\"id\":0}";   //设备控制命令
// char feng_off[100]="{\"feng\":false,\"id\":0}";
char fan_on[100]="fan_on";   //设备控制命令
char fan_off[100]="fan_off";
char lamp_on[100]="lamp_on";   //设备控制命令
char lamp_off[100]="lamp_off";
// char alarm_on[100]="lamp_o";   //设备控制命令
// char alarm_off[100]="{\"alarm\":false,\"id\":0}";
// char doorLock_on[100]="{\"doorLock\":true,\"id\":0}";   //设备控制命令
// char doorLock_off[100]="{\"doorLock\":false,\"id\":0}";
char feng_on[100]="feng_on";   //设备控制命令
char feng_off[100]="feng_off";
char data_buf3[1024];
char data_buf4[1024];
float tem=0,hum=0;
float co2=0,light=0;
int infrared=0,sleet=0;
int lamp=0,doorLock=0;
int alarm1=0;
int feng=0;
int fan=0;
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
	ser.sin_port = htons(5903);           //将主机序转成网络序
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
    if (pthread_create(&receive_tid, NULL, receive_thread, (void *)&connfd) != 0)     
        {                                             //判断创建写数据线程
            perror("thread creation failed");
            exit(EXIT_FAILURE);
        }
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
        }else if(strcmp(recv_buf, "feng_on")==0)
        {
            mqtt_pub(mqtt_client,feng_on,PUB_TOPIC1);
        }else if(strcmp(recv_buf, "feng_off")==0)
        {
            mqtt_pub(mqtt_client,feng_off,PUB_TOPIC1);
        }
	}
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
        printf("设备3发送的数据%s",data_buf3);
        //memset(data_buf3, 0, sizeof(data_buf3));
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
        printf("MQTT发布消息失败, return code %d\n",ret);
        return ret;
    }
    //printf("MQTT发布消息成功\n");
    return 0;
}
// 用于接受消息
// int recv_message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
// {
//     cJSON* root = cJSON_Parse((char *)message->payload);
//     if (root == NULL) {
//             cJSON_Delete(root);
//         }
//     if (cJSON_HasObjectItem(root, "tem"))
//     {
//         cJSON * tem_item =  cJSON_GetObjectItem(root,"tem");
//         tem = tem_item->valuedouble;
//         cJSON * hum_item =  cJSON_GetObjectItem(root,"hum");
//         hum = hum_item->valuedouble;
//         cJSON * id_item =  cJSON_GetObjectItem(root,"id");
//         zhi = id_item->valueint;
//     }
//     if (cJSON_HasObjectItem(root, "light"))
//     {
//         cJSON * light_item =  cJSON_GetObjectItem(root,"light");
//         light = light_item->valuedouble;
//     }
//     if (cJSON_HasObjectItem(root, "co2"))
//     {
//         cJSON * co2_item =  cJSON_GetObjectItem(root,"co2");
//         co2 = co2_item->valuedouble;
//     }
//     if (cJSON_HasObjectItem(root, "infrared"))   //人体
//     {
//         cJSON * infrared_item =  cJSON_GetObjectItem(root,"infrared");
//         infrared = cJSON_IsTrue(infrared_item);
//     }
//     if (cJSON_HasObjectItem(root, "sleet"))
//     {
//         cJSON * sleet_item =  cJSON_GetObjectItem(root,"sleet");   //雨雪
//         sleet = cJSON_IsTrue(sleet_item);
//     }
//     if (cJSON_HasObjectItem(root, "fan"))
//     {
//         cJSON * fan_item =  cJSON_GetObjectItem(root,"fan");
//         fan = cJSON_IsTrue(fan_item);
//     }
//     if (cJSON_HasObjectItem(root, "alarm"))
//     {
//         cJSON * alarm_item =  cJSON_GetObjectItem(root,"alarm");
//         alarm1 = cJSON_IsTrue(alarm_item);
//     }
//     if (cJSON_HasObjectItem(root, "lamp"))
//     {
//         cJSON * lamp_item =  cJSON_GetObjectItem(root,"lamp");
//         lamp = cJSON_IsTrue(lamp_item);
//     }
//     if (cJSON_HasObjectItem(root, "doorLock"))
//     {
//         cJSON * doorLock_item =  cJSON_GetObjectItem(root,"doorLock");
//         doorLock = cJSON_IsTrue(doorLock_item);
//     }
// 	//sprintf(data_buf3, "{\"tem\":%f,\"hum\":%f,\"id\":%d,\"fan\":%d,\"flag2\":%d}", tem, hum, zhi,fan,flag2);
//     sprintf(data_buf3, "{\"tem\":%f,\"hum\":%f,\"light\":%f,\"co2\":%f,\"infrared\":%d,\"sleet\":%d,\"fan\":%d,\"lamp\":%d,\"doorLock\":%d,\"alarm\":%d}", tem, hum, light, co2,  infrared, sleet, fan, lamp, doorLock, alarm1);
//     printf("获取的值1：%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d\n",tem, hum, light, co2,infrared, sleet, fan, lamp, doorLock, alarm1, flag2);
//     //write(connfd, data_buf3, sizeof(data_buf3));
//     //my_delay(3000);
//     //memset(data_buf3, 0, sizeof(data_buf3));
//     cJSON_Delete(root);
//     MQTTClient_freeMessage(&message);
//     MQTTClient_free(topicName);
//     return 1;
// }
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
   // cJSON *co2_item = cJSON_GetObjectItem(root, "co2");
 //   cJSON *infrared_item = cJSON_GetObjectItem(root, "infrared");

    cJSON *sleet_item = cJSON_GetObjectItem(root, "sleet");
    cJSON *fan_item = cJSON_GetObjectItem(root, "fan");
    cJSON *lamp_item = cJSON_GetObjectItem(root, "lamp");
   // cJSON *doorLock_item = cJSON_GetObjectItem(root, "doorLock");
   // cJSON *alarm_item = cJSON_GetObjectItem(root, "alarm");

    cJSON *feng_item = cJSON_GetObjectItem(root, "feng");
   // cJSON *alarm_item = cJSON_GetObjectItem(root, "alarm");
   // cJSON *alarm_item = cJSON_GetObjectItem(root, "alarm");


    if (tem_item != NULL && hum_item != NULL) {
        tem = tem_item->valuedouble;
        hum = hum_item->valuedouble;
        //zhi = cJSON_GetObjectItem(root, "id")->valueint;
    }
    if (light_item != NULL) {

    light = light_item->valuedouble;
    }
    // if (co2_item != NULL) {
    // co2 = co2_item->valuedouble;
    // }
    if (feng_item != NULL) {
    //feng = cJSON_IsTrue(feng_item);
    feng=feng_item->valueint;
    }
    // if (infrared_item != NULL) {
    // infrared = cJSON_IsTrue(infrared_item);
    // }
    if (sleet_item != NULL) {
    sleet =sleet_item->valueint;
    //sleet = cJSON_IsTrue(sleet_item);
    }
    if (fan_item != NULL) {
        fan=fan_item->valueint;
    //fan = cJSON_IsTrue(fan_item);
    }
    // if (alarm_item != NULL) {
    // alarm1 = cJSON_IsTrue(alarm_item);
    // }

    if (lamp_item != NULL) {
        lamp=lamp_item->valueint;
    //lamp = cJSON_IsTrue(lamp_item);
    }
    // if (doorLock_item != NULL) {
    // doorLock = cJSON_IsTrue(doorLock_item);
    // }
    //sprintf(data_buf3, "{\"tem\":%f,\"hum\":%f,\"light\":%f,\"co2\":%f,\"infrared\":%d,\"sleet\":%d,\"fan\":%d,\"lamp\":%d,\"doorLock\":%d,\"alarm\":%d}", tem, hum, light, co2, infrared, sleet, fan, lamp, doorLock, alarm1);
    sprintf(data_buf3, "{\"tem\":%f,\"hum\":%f,\"light\":%f,\"sleet\":%d,\"fan\":%d,\"lamp\":%d,\"feng\":%d}", tem, hum, light,  sleet, fan, lamp,feng);
    printf("获取的mqtt值3：%f,%f,%f,%d,%d,%d,%d\n", tem, hum, light,  sleet, fan, lamp,feng);
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
        printf("MQTTClient_create failed\n");
        return -1;
    }
    // 设置回调函数（用于接受数据）
    ret = MQTTClient_setCallbacks(mqtt_client,NULL, NULL, recv_message,NULL);
    if (ret != MQTTCLIENT_SUCCESS)
    {
        printf("MQTTClient_setCallbacks failed\n");
        return -1;
    }
    // 连接到mqtt代理服务器
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    ret = MQTTClient_connect(mqtt_client, &conn_opts);
    if (ret != MQTTCLIENT_SUCCESS)
    {
        printf("MQTTClient_connect failed\n");
        return -1;
    }
    // 订阅
    ret = MQTTClient_subscribe(mqtt_client, SUB_TOPIC1, QoS);
    if (ret != MQTTCLIENT_SUCCESS)
    {
        printf("MQTTClient_subscribe failed\n");
        return -1;
    }
    return 0;
}




// /**************************************************************
//  * File Name     : 01main_ver1.c
//  * Creator       : 郭文鹏
//  * QQ            : 1197930600
//  * Email         : 1197930600@qq.com
//  * Creat Time    : Tue Jul 18 13:14:47 2023
//  * 备注          : 设备1的服务器    发送数据在回调函数中
// ***************************************************************/
// #include <stdio.h>                 
// #include <string.h>
// #include <stdlib.h>                 //基础库文件
// #include <unistd.h>
// #include "MQTTClient.h"             //mqtt库文件
// #include <sys/types.h>                     
// #include <sys/socket.h>
// #include <netinet/ip.h> 
// #include <netinet/in.h>
// #include <arpa/inet.h>              //tcp库文件
// #include <pthread.h>                //线程库文件
// #include "cJSON.h"                  //json库文件
// #include <time.h>
// #define FILE_NAME "shebei1.txt"
// /**************************************************************
//  * 备注          :  宏定义区域
// ***************************************************************/
// #define MQTT_ADDR "tcp://mqtt.yyzlab.com.cn"    //云平台地址
// #define ClientID1 "ubunut_guo_202301"           //标识id号，在同一个云平台下，id号不能冲突  
// #define ClientID2 "ubunut_guo_202302"
// #define ClientID3 "ubunut_guo_202303"
// #define ClientID4 "ubunut_guo_202304"           
// #define SUB_TOPIC1 "1688716903571/AIOTSIM2APP"  //订阅、发布主题
// #define PUB_TOPIC1 "1688716903571/APP2AIOTSIM"
// #define SUB_TOPIC2 "1688716903572/AIOTSIM2APP"
// #define PUB_TOPIC2 "1688716903572/APP2AIOTSIM"
// #define SUB_TOPIC3 "1688716903573/AIOTSIM2APP"
// #define PUB_TOPIC3 "1688716903573/APP2AIOTSIM"
// #define SUB_TOPIC4 "1688716903574/AIOTSIM2APP"
// #define PUB_TOPIC4 "1688716903574/APP2AIOTSIM"
// #define QoS 0                                   //通信质量，0，1，2，
// #define TIMEOUT 2000L                           //超时值
// /**************************************************************
//  * 备注          :  全局变量区域
// ***************************************************************/
// char fan_on[100]="{\"fan\":true,\"id\":0}";   //设备控制命令
// char fan_off[100]="{\"fan\":false,\"id\":0}";
// char lamp_on[100]="{\"lamp\":true,\"id\":0}";   //设备控制命令
// char lamp_off[100]="{\"lamp\":false,\"id\":0}";
// char alarm_on[100]="{\"alarm\":true,\"id\":0}";   //设备控制命令
// char alarm_off[100]="{\"alarm\":false,\"id\":0}";
// char doorLock_on[100]="{\"doorLock\":true,\"id\":0}";   //设备控制命令
// char doorLock_off[100]="{\"doorLock\":false,\"id\":0}";
// char data_buf3[1024];
// char data_buf4[1024];
// float tem,hum;
// float co2,light;
// int infrared,sleet;
// int lamp,doorLock;
// int alarm1;
// int fan;
// int zhi;
// int zhi2;
// int zhi3,zhi4,zhi5;
// int ret;
// MQTTClient mqtt_client;                     //创建mqtt1
// MQTTClient mqtt_client2;                    //创建mqtt2
// MQTTClient mqtt_client3;                     //创建mqtt1
// MQTTClient mqtt_client4;                    //创建mqtt2
// pthread_t tid;                              //创建一个线程号
// int sockfd;                                 //创建套接字
// pthread_t receive_tid;                      //创建发送数据给客户端的线程号                     
// int flag1=1;                                //离线、在线状态
// int flag2=0;                                //控制设备    注意这里不能是全局变量，不然会出现，客户端1的控制，影响客户端2
// int flag3=0;                                //发送数据给客户端线程是否开启
// int flag4=0;                                //发送数据给客户端线程是否开启
// /**************************************************************
//  * 备注          :  函数声明区域
// ***************************************************************/
// void *clint(void *arg);
// int tcp_server(void);
// int tcp_close(void);
// int mqtt_pub(MQTTClient mqtt_client,const void *paylaod,const char* topicName);
// int recv_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
// int recv_message2(void *context, char *topicName, int topicLen, MQTTClient_message *message);
// int mqtt1(void *arg);
// int mqtt2(void *arg);
// void *receive_thread(void *arg);
// void my_delay(int ms);
// /**************************************************************
//  * 备注          :  主函数区域
// ***************************************************************/
// int main(void)
// {
//     struct sockaddr_in  clientAddr;
//     socklen_t addrLen = sizeof(clientAddr);
//     ret=tcp_server();      //创建socket连接，支持多线程
//     while (1)
//     {
// 		int connfd = accept(sockfd, NULL, NULL);
// 		if(connfd <=0 )
// 		{
// 			perror("accept");
// 			return 0;
// 		}
// 		printf("accept connfd = %d\n", connfd);
//         // 提取客户端地址和端口
//         char clientIP[INET_ADDRSTRLEN];
//         inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
//         int clientPort = ntohs(clientAddr.sin_port);
//         // 打印客户端地址和端口
//         printf("Connected with client: %s:%d\n", clientIP, clientPort);
//         time_t current_time;
//         struct tm *time_info;
//         char time_str[50];
//         time(&current_time);
//         time_info = localtime(&current_time);
//         // 格式化时间字符串
//         //strftime(time_str, sizeof(time_str), "%Y年%m月%d日%H时%M分%S秒", time_info);
//         strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
//         // 打印当前时间
//         printf("当前时间：%s\n", time_str);
//         FILE *file = fopen(FILE_NAME, "a");
//         if (file == NULL) {
//             printf("无法打开文件！\n");
//             exit(1);
//         }
//         fprintf(file, "ip地址：%s 端口：%d 时间：%s\n", clientIP, clientPort,time_str);
//         fclose(file);
// 		pthread_create(&tid, NULL, clint, (void *)&connfd);
//     }
// 	ret=tcp_close();
//     return 0;
// }
// /**************************************************************
//  * 备注          :  tcp通信
// ***************************************************************/
// void my_delay(int ms)
// {
//     for(int i=0;i<ms;i++)
// 	{
// 		for(int j=0;j<8900;j++);
// 	}
// }
// /**************************************************************
//  * 备注          :  tcp通信
// ***************************************************************/
// int tcp_server(void)
// {
// 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 	if(sockfd < 0)
// 	{
// 		perror("socket err");                  //打印系统调用函数的错误信息
// 		return 0;
// 	}
// 	//2.绑定
// 	struct sockaddr_in ser;
// 	ser.sin_family = AF_INET;
// 	ser.sin_port = htons(5901);           //将主机序转成网络序
// 	ser.sin_addr.s_addr = inet_addr("10.0.0.16"); //将IP地址转成网络字节序的IP地址

// 	int ret = bind(sockfd, (struct sockaddr *)&ser, sizeof(ser));
// 	if(ret == -1)
// 	{
// 		perror("bind err");
// 		return 0;
// 	}
// 	//3.监听
// 		ret = listen(sockfd, 5);
// 		if(ret == -1)
// 		{
// 			perror("listen err");
// 			return 0;
// 		}
// 		printf("listing-------------\n");
// }
// void *clint(void *arg)
// {
// 	int connfd = *(int *)arg;
// 	char recv_buf[1024] = "";
//     ret =mqtt1((void *)&connfd);
// 	while(1)
// 	{
// 		memset(recv_buf, 0, sizeof(recv_buf));
// 		ret = read(connfd, recv_buf, sizeof(recv_buf));      //读取客户端数据，等待中
// 		if(ret <= 0)
// 		{
// 			printf("客户端退出！！！\n");
// 			break;
// 		}
//         if(strcmp(recv_buf, "fan_on")==0)
//         {
//             mqtt_pub(mqtt_client,fan_on,PUB_TOPIC1);
//         }else if(strcmp(recv_buf, "fan_off")==0)
//         {
//             mqtt_pub(mqtt_client,fan_off,PUB_TOPIC1);
//         }else if(strcmp(recv_buf, "lamp_on")==0)
//         {
//             mqtt_pub(mqtt_client,lamp_on,PUB_TOPIC1);
//         }else if(strcmp(recv_buf, "lamp_off")==0)
//         {
//             mqtt_pub(mqtt_client,lamp_off,PUB_TOPIC1);
//         }else if(strcmp(recv_buf, "doorLock_on")==0)
//         {
//             mqtt_pub(mqtt_client,doorLock_on,PUB_TOPIC1);
//         }else if(strcmp(recv_buf, "doorLock_off")==0)
//         {
//             mqtt_pub(mqtt_client,doorLock_off,PUB_TOPIC1);
//         }else if(strcmp(recv_buf, "alarm_on")==0)
//         {
//             mqtt_pub(mqtt_client,alarm_on,PUB_TOPIC1);
//         }else if(strcmp(recv_buf, "alarm_off")==0)
//         {
//             mqtt_pub(mqtt_client,alarm_off,PUB_TOPIC1);
//         }
// 	}
// 	return NULL;
// }
// int tcp_close(void)
// {
// 	close(sockfd);
// 	pthread_detach(tid);
// }
// /**************************************************************
//  * 备注          :  mqtt通信
// ***************************************************************/
// int mqtt_pub(MQTTClient mqtt_client,const void *paylaod,const char* topicName)
// {
//     int ret;
//     MQTTClient_deliveryToken token;
//     ret=MQTTClient_publish(mqtt_client,topicName,strlen(paylaod),paylaod,QoS,0,&token);
//     if (ret!= MQTTCLIENT_SUCCESS)
//     {
//         printf("MQTT发布消息失败, return code %d\n",ret);
//         return ret;
//     }
//     //printf("MQTT发布消息成功\n");
//     return 0;
// }
// // 用于接受消息
// int recv_message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
// {
//     int connfd = *(int *)context;
//     cJSON* root = cJSON_Parse((char *)message->payload);
//     if (root == NULL) {
//             cJSON_Delete(root);
//         }
//     if (cJSON_HasObjectItem(root, "tem"))
//     {
//         cJSON * tem_item =  cJSON_GetObjectItem(root,"tem");
//         tem = tem_item->valuedouble;
//         cJSON * hum_item =  cJSON_GetObjectItem(root,"hum");
//         hum = hum_item->valuedouble;
//         cJSON * id_item =  cJSON_GetObjectItem(root,"id");
//         zhi = id_item->valueint;
//     }
//     if (cJSON_HasObjectItem(root, "light"))
//     {
//         cJSON * light_item =  cJSON_GetObjectItem(root,"light");
//         light = light_item->valuedouble;
//     }
//     if (cJSON_HasObjectItem(root, "co2"))
//     {
//         cJSON * co2_item =  cJSON_GetObjectItem(root,"co2");
//         co2 = co2_item->valuedouble;
//     }
//     if (cJSON_HasObjectItem(root, "infrared"))   //人体
//     {
//         cJSON * infrared_item =  cJSON_GetObjectItem(root,"infrared");
//         infrared = cJSON_IsTrue(infrared_item);
//     }
//     if (cJSON_HasObjectItem(root, "sleet"))
//     {
//         cJSON * sleet_item =  cJSON_GetObjectItem(root,"sleet");   //雨雪
//         sleet = cJSON_IsTrue(sleet_item);
//     }
//     if (cJSON_HasObjectItem(root, "fan"))
//     {
//         cJSON * fan_item =  cJSON_GetObjectItem(root,"fan");
//         fan = cJSON_IsTrue(fan_item);
//     }
//     if (cJSON_HasObjectItem(root, "alarm"))
//     {
//         cJSON * alarm_item =  cJSON_GetObjectItem(root,"alarm");
//         alarm1 = cJSON_IsTrue(alarm_item);
//     }
//     if (cJSON_HasObjectItem(root, "lamp"))
//     {
//         cJSON * lamp_item =  cJSON_GetObjectItem(root,"lamp");
//         lamp = cJSON_IsTrue(lamp_item);
//     }
//     if (cJSON_HasObjectItem(root, "doorLock"))
//     {
//         cJSON * doorLock_item =  cJSON_GetObjectItem(root,"doorLock");
//         doorLock = cJSON_IsTrue(doorLock_item);
//     }
// 	//sprintf(data_buf3, "{\"tem\":%f,\"hum\":%f,\"id\":%d,\"fan\":%d,\"flag2\":%d}", tem, hum, zhi,fan,flag2);
//     sprintf(data_buf3, "{\"tem\":%f,\"hum\":%f,\"light\":%f,\"co2\":%f,\"id\":%d,\"infrared\":%d,\"sleet\":%d,\"fan\":%d,\"lamp\":%d,\"doorLock\":%d,\"alarm\":%d}", tem, hum, light, co2, zhi, infrared, sleet, fan, lamp, doorLock, alarm1);
//     printf("获取的值1：%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d\n",tem, hum, light, co2, zhi, infrared, sleet, fan, lamp, doorLock, alarm1, flag2);
//     write(connfd, data_buf3, sizeof(data_buf3));
//     my_delay(3000);
//     memset(data_buf3, 0, sizeof(data_buf3));
//     cJSON_Delete(root);
//     MQTTClient_freeMessage(&message);
//     MQTTClient_free(topicName);
//     return 1;
// }

// int mqtt1(void *arg)
// {
//     int connfd = *(int *)arg;
//     // 创建MQTTClient
//     ret = MQTTClient_create(&mqtt_client, MQTT_ADDR, ClientID1, MQTTCLIENT_PERSISTENCE_NONE, NULL);
//     if (ret != MQTTCLIENT_SUCCESS)
//     {
//         printf("MQTTClient_create failed\n");
//         return -1;
//     }
//     // 设置回调函数（用于接受数据）
//     ret = MQTTClient_setCallbacks(mqtt_client,&connfd, NULL, recv_message,NULL);
//     if (ret != MQTTCLIENT_SUCCESS)
//     {
//         printf("MQTTClient_setCallbacks failed\n");
//         return -1;
//     }
//     // 连接到mqtt代理服务器
//     MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//     conn_opts.keepAliveInterval = 20;
//     conn_opts.cleansession = 1;
//     ret = MQTTClient_connect(mqtt_client, &conn_opts);
//     if (ret != MQTTCLIENT_SUCCESS)
//     {
//         printf("MQTTClient_connect failed\n");
//         return -1;
//     }
//     // 订阅
//     ret = MQTTClient_subscribe(mqtt_client, SUB_TOPIC1, QoS);
//     if (ret != MQTTCLIENT_SUCCESS)
//     {
//         printf("MQTTClient_subscribe failed\n");
//         return -1;
//     }
//     return 0;
// }


