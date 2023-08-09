#ifndef GONNENGJIEMIAN_H
#define GONNENGJIEMIAN_H
#include <QPixmap>

#include <QWidget>
#include <QTcpSocket>

//QT_BEGIN_NAMESPACE
namespace Ui {
class gonnengjiemian;
}
//QT_END_NAMESPACE

class gonnengjiemian : public QWidget
{
    Q_OBJECT

public:
    explicit gonnengjiemian(QWidget *parent = nullptr);
    ~gonnengjiemian();
    QTcpSocket *tcpSocket;

private slots:
    void on_pushButton_startOrStop_clicked();


    void connected_slot();
    void readyRead_slot();

//    void on_pushButton_send_clicked();

//    void on_pushButton_send_2_clicked();

//    void on_pushButton_send_3_clicked();

//    void on_pushButton_send_5_clicked();

//    void on_pushButton_send_4_clicked();

//   // void on_lineEdit_send_cursorPositionChanged(int arg1, int arg2);

    void on_pushButton_send_2_clicked();


    void on_pushButton_send_5_clicked();

    void on_pushButton_send_4_clicked();

    void on_pushButton_send_7_clicked();

    void on_pushButton_send_6_clicked();

private:
    Ui::gonnengjiemian *ui;
    //QTcpSocket *tcpsocket;
};

#endif // GONNENGJIEMIAN_H
