#ifndef MQTTSER_H
#define MQTTSER_H
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QMqttClient>
#include <QDebug>
#include <QString>
#include <QEventLoop>
#include <QTimer>
#include <QWidget>

namespace Ui {
class mqttser;
}

class mqttser : public QWidget
{
    Q_OBJECT

public:
    explicit mqttser(QWidget *parent = nullptr);
    ~mqttser();
private slots:

    void mqtt_recv(const QByteArray &message, const QMqttTopicName &topic);

    void mqtt_connected();

    void mqtt_state(int state);

private:
    Ui::mqttser *ui;
    QMqttClient *m_client;

    QTimer *mytimer;
};

#endif // MQTTSER_H
