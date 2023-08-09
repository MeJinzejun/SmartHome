#include "signup.h"
#include "ui_signup.h"
#include "widget.h"
#include <QString>
#include <QDebug>
#include <QTextEdit>
#include <QTcpSocket>
#include<QPixmap>
#include<QWidget>
#include<QMessageBox>
#include<QFile>
#include<QFileDialog>
#include<QFileInfo>
#include <stdbool.h>
#include "cJSON.h"
signup::signup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::signup)
{
    ui->setupUi(this);
    tcpSocket2 = new QTcpSocket(this);
    tcpSocket2->connectToHost("62.234.20.135",5902);
    connect(tcpSocket2,SIGNAL(connected()),this,SLOT(connected_slot()));
    //设置图片
    QPixmap c;
    c.load(":/new/prefix1/ui5.png");
    ui->label_image->setScaledContents(true);
    ui->label_image->setPixmap(c);

    QPixmap cc;
    cc.load(":/new/prefix1/1.png");
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(cc);
}

signup::~signup()
{
    delete ui;
}

void signup::connected_slot()        //接受框
{
    connect(tcpSocket2,SIGNAL(readyRead()),this,SLOT(readyRead_slot()));     //和服务器连接成功后，就需要就收服务器发送过来的数据了，                                                                        //如果客户端收到发送过来的数据，系统会产生readyRead信号
}

void signup::readyRead_slot()
{
    char buf[1024];
    memset(buf,0,sizeof(buf));
    tcpSocket2->read(buf,sizeof(buf));
    qDebug()<<buf;
    cJSON* root = cJSON_Parse(buf);
    if (cJSON_HasObjectItem(root, "zhuce"))
    {
        cJSON * tem_item =  cJSON_GetObjectItem(root,"zhuce");
        QString tem = QString::fromUtf8(tem_item->valuestring);
        if(strcmp(tem.toStdString().c_str(), "kkkk")==0)
        {
            ui->label_4->setText("注册成功！");
        }
        if(strcmp(tem.toStdString().c_str(), "mmmm")==0)
        {
            ui->label_4->setText("注册失败 重新输入！");
        }
    }
}

void signup::on_click_button_clicked()
{ 
    this->close();
    Widget *www=new Widget();
    www->show();
    tcpSocket2->disconnectFromHost();

}

void signup::on_sure_buttton_clicked()
{
    if((ui->username->text()!=NULL)&&(ui->password->text()==ui->surepass->text()))
    {
        QString data_buf1 =ui->username->text();
        QString data_buf2 = ui->password->text();
        QString data_buf3 = "zhuce";
        char test[1024];
        //QString test;
        sprintf(test,"{\"zhuce\":\"%s\",\"yonghu\":\"%s\",\"mima\":\"%s\"}", data_buf3.toStdString().c_str(),data_buf1.toStdString().c_str(),data_buf2.toStdString().c_str());
        qDebug()<<test;

        //connect(tcpSocket2,SIGNAL(connected()),this,SLOT(connected_slot()));   //客户端连接服务器成功后会产生connected信号
        //ui->pushButton_startOrStop->setText("UNLink");    //修改按键的提示字，为下次关闭做准备
        //QByteArray byteArray = test.toUtf8();
        tcpSocket2->write(test);
    }
}
