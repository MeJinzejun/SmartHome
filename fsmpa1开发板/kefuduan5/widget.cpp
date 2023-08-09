#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
   ui->setupUi(this);
   tcpSocket3 = new QTcpSocket(this);
   tcpSocket3->connectToHost("62.234.20.135",5902);
   ui->lineEdit_1->setEchoMode ( QLineEdit::Password );
   new_ui=new gonnengjiemian;
   connect(tcpSocket3,SIGNAL(connected()),this,SLOT(connected_slot()));
   QPixmap a;
   a.load(":/new/prefix1/ui3.png");
   ui->label_2->setScaledContents(true);
   ui->label_2->setPixmap(a);

   m_client = new QMqttClient(this);
   m_client->setHostname("mqtt.yyzlab.com.cn");
   m_client->setPort(1883);
   connect(m_client,&QMqttClient::messageReceived,this,&Widget::mqtt_recv);
   connect(m_client,&QMqttClient::connected,this,&Widget::mqtt_connected);
   connect(m_client,&QMqttClient::stateChanged,this,&Widget::mqtt_state);

   qDebug()<<"mqtt sub";
   mytimer = new QTimer(this);
   connect(mytimer,&QTimer::timeout,[=](){
       m_client->connectToHost();
   });
   mytimer->start(3000);

   //连接之后，2s间隔发布数据
  mytimer2 =new QTimer(this);
  connect(mytimer2,&QTimer::timeout,this,&Widget::onPubMqttConnected);
  mytimer2->start(2000); //2s

  //温湿度
  tem_hum_ptr=new fsmpTempHum(this);
  //风扇
  fan_ptr=new fsmpFan(this);
  fan_ptr->setSpeed(1000);
}

void Widget::mqtt_state(int state)
{
    qDebug()<<"mqtt_state:"<<state;
}
void Widget::mqtt_connected()
{
    qDebug()<<"开始订阅主题\n";
    mytimer->stop();  //定时结束
    m_client->subscribe(sub_topic);
}

void Widget::mqtt_recv(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug()<<"message:"<<QString(message);
    qDebug()<<"top:"<<topic;

    cJSON* root = cJSON_Parse(message);
    if (root == NULL) {
            qDebug()<<"Failed to parse JSON data.\n";
            cJSON_Delete(root);
        }
    if (cJSON_HasObjectItem(root, "tem"))
    {
        cJSON * tem_item =  cJSON_GetObjectItem(root,"tem");
        float tem = tem_item->valuedouble;
        //ui->lcdNumber->display(tem);
    }
    if (cJSON_HasObjectItem(root, "hum"))
    {
        cJSON * hum_item =  cJSON_GetObjectItem(root,"hum");
        float hum = hum_item->valuedouble;
        //ui->lcdNumber_2->display(hum);
    }
    if (cJSON_HasObjectItem(root, "fan"))
    {
        cJSON * fan_item =  cJSON_GetObjectItem(root,"fan");
        bool fan = cJSON_IsTrue(fan_item);
        qDebug()<<"fan="<<fan;
//        if(fan==1)
//        {
 //       fan_ptr->start();
        //fan_zhuang=1;
//            ui->label_2->setPixmap(QPixmap(":/dian1.png"));
//        }else{
//            ui->label_2->setPixmap(QPixmap(":/dian0.png"));
   //     fan_ptr->stop();
        //fan_zhuang=0;
//        }

    }
    cJSON_Delete(root);
}
void Widget::onPubMqttConnected()
{
    char send_data[1024];
    int id=0;
    sprintf(send_data,"{\"tem\":%f,\"hum\":%f,\"id\":%d}", tem_hum_ptr->temperature(),tem_hum_ptr->humidity(),id);

    qDebug()<<send_data;
    mqtt_client->publish(pub_topic,send_data);
}

//tcp通信
void Widget::connected_slot()
{
    connect(tcpSocket3,SIGNAL(readyRead()),this,SLOT(readyRead_slot()));     //和服务器连接成功后，就需要就收服务器发送过来的数据了，                                                                        //如果客户端收到发送过来的数据，系统会产生readyRead信号
}

void Widget::readyRead_slot()
{
    char buf[1024];
    memset(buf,0,sizeof(buf));
    tcpSocket3->read(buf,sizeof(buf));
    qDebug()<<buf;
    cJSON* root = cJSON_Parse(buf);
    if (cJSON_HasObjectItem(root, "denglu"))
    {
        cJSON * tem_item =  cJSON_GetObjectItem(root,"denglu");
        QString tem = QString::fromUtf8(tem_item->valuestring);
        if(strcmp(tem.toStdString().c_str(), "aaaa")==0)
        {
            this->close();
            new_ui->show();
            tcpSocket3->disconnectFromHost();
            ui->label_3->setText("登录成功！");
        }
        if(strcmp(tem.toStdString().c_str(), "bbbb")==0)
        {
            ui->label_3->setText("登录失败 重新输入！");
        }
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_login_button_clicked()
{
    QString data_buf3 = "denglu";
    char test[1024];
    QString data_buf1 =ui->lineEdit->text();
    QString data_buf2 = ui->lineEdit_1->text();
    sprintf(test,"{\"denglu\":\"%s\",\"yonghu\":\"%s\",\"mima\":\"%s\"}", data_buf3.toStdString().c_str(),data_buf1.toStdString().c_str(),data_buf2.toStdString().c_str());
    qDebug()<<test;
    tcpSocket3->write(test);
}

void Widget::on_pushButton_send_2_clicked()
{
    this->close();
    signup *ww=new signup();
    ww->show();
}

