#include "mqttser.h"
#include <QThread>
#include <QApplication>
#include <QMqttClient>
#include <QString>
#include <QDebug>
#include <QTextEdit>
#include <QMessageBox>
#include <stdbool.h>
#include "cJSON.h"
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QEventLoop>
#include <QTimer>
#include "widget.h"

mqttser::mqttser(QWidget *parent)
    : QWidget(parent)
{
    m_client = new QMqttClient(this);
    m_client->setHostname("mqtt.yyzlab.com.cn");
    m_client->setPort(1883);
    connect(m_client,&QMqttClient::messageReceived,this,&mqttser::mqtt_recv);
    connect(m_client,&QMqttClient::connected,this,&mqttser::mqtt_connected);
    connect(m_client,&QMqttClient::stateChanged,this,&mqttser::mqtt_state);
    qDebug()<<"mqtt sub";

    mytimer = new QTimer(this);
    connect(mytimer,&QTimer::timeout,[=](){
        m_client->connectToHost();
    });
    mytimer->start(3000);
}

void mqttser::mqtt_state(int state)
{
    qDebug()<<"mqtt_state:"<<state;
}
void mqttser::mqtt_connected()
{
    qDebug()<<"mqtt_connected";
    mytimer->stop();
    QString sub_topic="dev/sub_test";
    m_client->subscribe(sub_topic);
}
void mqttser::mqtt_recv(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug()<<message;

    if(message == "LED_ON"){
        qDebug()<<"led_on";
    }
    else if(message == "LED_OFF"){
        qDebug()<<"led_off";
    }
}
//// 自定义线程类
//class MyThread : public QThread
//{
//    Q_OBJECT
//public:
//    void run() override
//    {
//        //QMqttClient *m_client;
//        //QTimer *mytimer;
//        gggg myshu;
//        myshu.m_client = new QMqttClient(this);
//        myshu.m_client->setHostname("mqtt.yyzlab.com.cn");
//        myshu.m_client->setPort(1883);

//        connect(myshu.m_client,&QMqttClient::messageReceived,this,&gggg::mqtt_recv);
//        connect(myshu.m_client,&QMqttClient::connected,this,&gggg::mqtt_connected);


//        qDebug()<<"mqtt sub";
//        myshu.mytimer = new QTimer(this);
//        connect(myshu.mytimer,&QTimer::timeout,this,myshu.m_client->connectToHost());
//        myshu.mytimer->start(3000);
//    }
//};
//void gggg::mqtt_connected()
//{
//    qDebug()<<"mqtt_connected";
//    mytimer->stop();
//    QString sub_topic="dev/sub_test";
//    m_client->subscribe(sub_topic);
//}
//void gggg::mqtt_recv(const QByteArray &message, const QMqttTopicName &topic)
//{
//    qDebug()<<message;
//}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    // 创建线程
    //MyThread myThread;
    // 启动线程
    //myThread.start();
    return a.exec();
}
