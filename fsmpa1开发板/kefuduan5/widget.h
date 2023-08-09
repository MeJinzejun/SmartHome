#ifndef WIDGET_H
#define WIDGET_H
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QPixmap>
#include"gonnengjiemian.h"
//#include"signup.h"
#include<QMessageBox>
#include <QTcpSocket>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QMqttClient>
#include <QDebug>
#include <QString>
#include <QEventLoop>
#include <QTimer>
#include <QWidget>
#include <QMqttClient>
#include <QString>
#include <QDebug>
#include <QTextEdit>
#include <cstdio>
#include <cstring>
#include<QLabel>
#include<QPixmap>
#include <QTimer>
#include <QThread>
#include<QObject>
#include<QJsonArray>
#include <QJsonObject>
#include<QJsonValue>
#include<QLCDNumber>
#include<QJsonDocument>
#include<QByteArray>
#include <iomanip>
#include <sstream>
//#include"fsmpBeeper.h"
//#include"fsmpCamera.h"
//#include"fsmpElectric.h"
//#include"fsmpEvents.h"
#include"fsmpFan.h"
//#include"fsmpLed.h"
//#include"fsmpLight.h"
//#include"fsmpProximity.h"
//#include"fsmpSevensegLed.h"
#include"fsmpTempHum.h"
//#include"fsmpVibrator.h"
#include "fsmpCamera_v1.h"
#include"signup.h"
#include <QTcpSocket>
#include "cJSON.h"



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QTcpSocket *tcpSocket3;
    int fan_zhuang;
private slots:
    void on_login_button_clicked();
    void on_pushButton_send_2_clicked();
    void connected_slot();
    void readyRead_slot();
    void mqtt_recv(const QByteArray &message, const QMqttTopicName &topic);
    void mqtt_connected();
    void mqtt_state(int state);
    void onPubMqttConnected();
private:
     QLineEdit *lineEdit;
     Ui::Widget *ui;
    gonnengjiemian *new_ui;
    QMessageBox Box;
    QMqttClient *m_client;

    char k[50];
    QMqttClient *mqtt_client;
    fsmpFan *fan_ptr;
    fsmpTempHum *tem_hum_ptr;
    fsmpCamera *cam_ptr;
    QJsonObject data;
    QJsonObject data_pub;
    QTimer *mytimer;
    QTimer *mytimer2;
    QString pub_topic = "1688716903574/AIOTSIM2APP";
    QString sub_topic = "1688716903574/APP2AIOTSIM";

};



#endif // WIDGET_H
