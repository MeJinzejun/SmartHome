#include "mqttser.h"
#include "ui_mqttser.h"
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QMqttClient>
#include <QDebug>
#include <QString>
#include <QEventLoop>
#include <QTimer>
//mqttser::mqttser(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::mqttser)
//{
//    ui->setupUi(this);
//}

mqttser::~mqttser()
{
    delete ui;
}
