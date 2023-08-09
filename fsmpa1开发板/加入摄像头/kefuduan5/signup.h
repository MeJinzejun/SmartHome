#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QPixmap>
#include <QDebug>
#include <QTcpSocket>
namespace Ui {
class signup;
}

class signup : public QWidget
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent = 0);
    ~signup();
    QTcpSocket *tcpSocket2;

private slots:
    void on_click_button_clicked();
    //void on_sure_buttton_clicked();
    //void on_label_2_linkActivated(const QString &link);

    //void on_label_3_linkActivated(const QString &link);

    void on_sure_buttton_clicked();
    void connected_slot();
    void readyRead_slot();

private:
    Ui::signup *ui;
};

#endif // SIGNUP_H
