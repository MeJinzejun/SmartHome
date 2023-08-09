#include "gonnengjiemian.h"
#include "ui_gonnengjiemian.h"
#include <QTcpSocket>
//#include <windows.h>   //睡眠函数
#include <unistd.h>
#include<QPixmap>
#include<QWidget>
#include<QDebug>
#include<QMessageBox>

#include<QFile>
#include<QFileDialog>
#include<QFileInfo>

#include <QString>
#include <QDebug>
#include <QTextEdit>

#include <stdbool.h>
#include "cJSON.h"

QString gg="";

gonnengjiemian::gonnengjiemian(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::gonnengjiemian)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);
    QPixmap a;
    a.load(":/new/prefix1/ui1.png");
    ui->label_3->setScaledContents(true);
    ui->label_3->setPixmap(a);
    ui->label_4->setScaledContents(true);
    ui->label_2->setScaledContents(true);

}

gonnengjiemian::~gonnengjiemian()
{
    delete ui;
}

void gonnengjiemian::on_pushButton_startOrStop_clicked()
{
    if(ui->pushButton_startOrStop->text() == "Link")   //表示可以连接服务器
        {
            tcpSocket->connectToHost(ui->lineEdit_ip->text(),ui->lineEdit_port->text().toUInt());   //根据IP地址和端口号连接服务器
            connect(tcpSocket,SIGNAL(connected()),this,SLOT(connected_slot()));   //客户端连接服务器成功后会产生connected信号
            ui->pushButton_startOrStop->setText("UNLink");    //修改按键的提示字，为下次关闭做准备
        }
    else        //表示可以断开和服务器的连接
        {
            tcpSocket->close();     //断开和服务器的连接

            ui->pushButton_startOrStop->setText("Link");   //修改按键的提示字，为下次连接做准备
        }
}

void gonnengjiemian::connected_slot()        //接受框
{
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readyRead_slot()));     //和服务器连接成功后，就需要就收服务器发送过来的数据了，                                                                        //如果客户端收到发送过来的数据，系统会产生readyRead信号
}

void gonnengjiemian::readyRead_slot()
{
    char buf[1024];
    memset(buf,0,sizeof(buf));
    tcpSocket->read(buf,sizeof(buf));
    qDebug()<<buf;
    ui->textEdit->append(buf);
    cJSON* root = cJSON_Parse(buf);
//    if (root == NULL) {
//            qDebug()<<"Failed to parse JSON data.\n";
//            cJSON_Delete(root);
//        }

    if (cJSON_HasObjectItem(root, "tem"))
    {
        cJSON * tem_item =  cJSON_GetObjectItem(root,"tem");
        float tem = tem_item->valuedouble;
        cJSON * hum_item =  cJSON_GetObjectItem(root,"hum");
        float hum = hum_item->valuedouble;
        ui->lcdNumber->display(tem);
        ui->lcdNumber_2->display(hum);
    }
    if (cJSON_HasObjectItem(root, "fan"))
    {
        cJSON * fan_item =  cJSON_GetObjectItem(root,"fan");
        bool fan = cJSON_IsTrue(fan_item);
        //qDebug()<<"fan="<<fan;
        if(fan==1)
        {
            ui->label_2->setPixmap(QPixmap(":/new/prefix1/dian1.png"));
        }else{
            ui->label_2->setPixmap(QPixmap(":/new/prefix1/dian0.png"));
        }
    }
    if (cJSON_HasObjectItem(root, "flag2"))
    {
        cJSON * flag2_item =  cJSON_GetObjectItem(root,"flag2");
        int flag2 = flag2_item->valueint;
        //qDebug()<<"fan="<<flag3;
        if(flag2==1)
        {
            ui->label_4->setPixmap(QPixmap(":/new/prefix1/fan1.png"));
        }else{
            ui->label_4->setPixmap(QPixmap(":/new/prefix1/fan0.png"));
        }
    }
    cJSON_Delete(root);
}

void gonnengjiemian::on_pushButton_send_2_clicked()
{

    if(ui->pushButton_send_2->text() == "zaixian")
        {
            tcpSocket->write("zaixian");
            ui->pushButton_send_2->setText("lixian");    //修改按键的提示字，为下次关闭做准
        }
    else
        {
            tcpSocket->write("lixian");
            ui->pushButton_send_2->setText("zaixian");    //修改按键的提示字，为下次关闭做准
        }
}
void gonnengjiemian::on_pushButton_send_5_clicked()
{
    tcpSocket->write("shebei1");
    qDebug()<<"shebei1";
}
void gonnengjiemian::on_pushButton_send_4_clicked()
{
    tcpSocket->write("shebei2");
    qDebug()<<"shebei2";
}

void gonnengjiemian::on_pushButton_send_7_clicked()
{
    tcpSocket->write("fan_on");
}
void gonnengjiemian::on_pushButton_send_6_clicked()
{
    tcpSocket->write("fan_off");
}

