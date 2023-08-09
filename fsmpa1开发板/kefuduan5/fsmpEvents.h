#ifndef ENV_H
#define ENV_H

/***************************************************************************
 *
 * 类名：fsmpEvents
 * 功能说明：扩展板 按钮、人体红外、光闸管、火焰
 * 公有函数：
 *      fsmpEvents（）
 * 信号：
 *      void lightTriggered();          光闸管阻隔
 *      void flameDetected();           火焰检测
 *      void peopleDetected(bool);      人体红外发现true/离开false
 *      void keyPressed(int keynum);    按键按下 keynum： 1、2、3
 *      void keyRelessed(int keynum);   按键释放 keynum： 1、2、3
 *
 * *************************************************************************/

#include <QObject>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <QIODevice>
#include <QTimer>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <QDebug>
#include <QProcess>
#include <QThread>
#include <stdio.h>
#include <stdlib.h>

class fsmpEvents:public QObject{
    Q_OBJECT
signals:
    /* edge trigle !!!*/
    void lightTriggered();//光电开关
    void flameDetected();//火焰检测
    void peopleDetected(bool);//人体红外
    void keyPressed(int keynum);
    void keyRelessed(int keynum);

public:

    explicit fsmpEvents(QObject *parent = nullptr):QObject(parent)
    {
        this->gpio_F_open();
        this->gpio_E_open();

        fs_timer = new QTimer(this);
        connect(fs_timer,SIGNAL(timeout()),this,SLOT(time_out()));
        fs_timer->start(10);

    };

private:
    bool gpio_F_open(const char *device = "/dev/gpiochip5")
    {
        int ret;
        int fd = open(device,O_RDONLY|O_NONBLOCK);
        if(fd < 0)
        {
            fprintf(stderr,"open");
            return false;
        }

        event_req.lineoffset = 9; //TODO
        event_req.handleflags = GPIOHANDLE_REQUEST_INPUT;
        event_req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE|GPIOEVENT_REQUEST_RISING_EDGE;
        ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &event_req);	// event测试的时候用这个
        if (ret == -1)
        {
            ret = -errno;
            fprintf(stderr, "Failed to issue GET LINEHANDLE IOCTL (%d)\n", ret);
        }
        key_1_fd = event_req.fd;
        int flags = fcntl(key_1_fd,F_GETFL,0);
        flags |= O_NONBLOCK;
        fcntl(key_1_fd,F_SETFL,flags);

        event_req.lineoffset = 8; //TODO
        event_req.handleflags = GPIOHANDLE_REQUEST_INPUT;
        event_req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
        ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &event_req);	// event测试的时候用这个
        if (ret == -1)
        {
            ret = -errno;
            fprintf(stderr, "Failed to issue GET LINEHANDLE IOCTL (%d)\n", ret);
        }
        key_3_fd = event_req.fd;
        flags = fcntl(key_3_fd,F_GETFL,0);
        flags |= O_NONBLOCK;
        fcntl(key_3_fd,F_SETFL,flags);
#if 0
        event_req.lineoffset = 5; //TODO
        event_req.handleflags = GPIOHANDLE_REQUEST_INPUT;
        event_req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
        ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &event_req);	// event测试的时候用这个
        if (ret == -1)
        {
            ret = -errno;
            fprintf(stderr, "Failed to issue GET LINEHANDLE IOCTL (%d)\n", ret);
        }
        fire_fd = event_req.fd;
        flags = fcntl(fire_fd,F_GETFL,0);
        flags |= O_NONBLOCK;
        fcntl(fire_fd,F_SETFL,flags);
#endif
        // fire
        req.lineoffsets[0] = 5;
        req.flags = GPIOHANDLE_REQUEST_INPUT;
//        strcpy(req.consumer_label, "fire");
        req.lines = 1;
        memcpy(req.default_values, &data, sizeof(req.default_values));

        ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
        if (ret == -1) {
            fprintf(stderr, "Failed to issue %s (%d), %s\n",
                    "GPIO_GET_LINEHANDLE_IOCTL", ret, strerror(errno));
        }
        fire_fd = req.fd;

        // key2
        req.lineoffsets[0] = 7;
        req.flags = GPIOHANDLE_REQUEST_INPUT;
//        strcpy(req.consumer_label, "key2");
        req.lines = 1;
        memcpy(req.default_values, &data, sizeof(req.default_values));

        ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
        if (ret == -1) {
            fprintf(stderr, "Failed to issue %s (%d), %s\n",
                    "GPIO_GET_LINEHANDLE_IOCTL", ret, strerror(errno));
        }
        key_2_fd = req.fd;

        // people
        req.lineoffsets[0] = 12;
        req.flags = GPIOHANDLE_REQUEST_INPUT;
//        strcpy(req.consumer_label, "key2");
        req.lines = 1;
        memcpy(req.default_values, &data, sizeof(req.default_values));

        ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
        if (ret == -1) {
            fprintf(stderr, "Failed to issue %s (%d), %s\n",
                    "GPIO_GET_LINEHANDLE_IOCTL", ret, strerror(errno));
        }
        people_fd = req.fd;
        return true;
    }

    bool gpio_E_open(const char *device = "/dev/gpiochip4")
    {
        int ret;
        int fd = open(device,O_RDONLY|O_NONBLOCK);
        if(fd < 0)
        {
            fprintf(stderr,"open");
            return false;
        }

        event_req.lineoffset = 15; //TODO
        event_req.handleflags = GPIOHANDLE_REQUEST_INPUT;
        event_req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
        ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &event_req);	// event测试的时候用这个
        if (ret == -1)
        {
            ret = -errno;
            fprintf(stderr, "Failed to issue GET LINEHANDLE IOCTL (%d)\n", ret);
        }
        light_trigger_fd = event_req.fd;
        int flags = fcntl(light_trigger_fd,F_GETFL,0);
        flags |= O_NONBLOCK;
        fcntl(light_trigger_fd,F_SETFL,flags);

        return true;
    }

    /*
     * Description: 关闭设备
     * input: null
     * output: null
     * return: 成功true/失败false
    */
    bool close(void);
public slots:
    void time_out()
    {

        static int key_1_flag = 0;
        static int key_2_flag = 0;
        static int key_3_flag = 0;
        static int light_trigger_flag = 0;
        static int people_flag = 0;
        static int flame_flag = 0;

        //捕获key_1按下
        read(key_1_fd,&event_data,sizeof (event_data));
        if(event_data.id == 2)
        {
            key_1_flag = 1;
            emit(this->keyPressed(1));
            //qDebug()<<"key_1 pressed";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               event_data.id = 0;
        }
        else if(event_data.id == 1 && key_1_flag==1)
        {
            key_1_flag = 0;
            emit keyRelessed(1);
            //qDebug()<<"key_1 relessed";
        }
        event_data.id = 0;

        //捕获key_2按下
        int ret = ioctl(key_2_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
        if (ret == -1) {
            ret = -errno;
            fprintf(stderr, "Failed to issue %s (%d), %s\n",
                    "GPIOHANDLE_GET_LINE_VALUES_IOCTL", ret, strerror(errno));
            exit(ret);
        }
        //qDebug() << "==========" << data.values[0];
        if(data.values[0] ==0&&key_2_flag==0)
        {
            emit(this->keyPressed(2));
            key_2_flag = 1;
            //qDebug()<<"key_2 pressed";
        }
        else if(data.values[0] ==1&&key_2_flag==1)
        {
            key_2_flag = 0;
            emit keyRelessed(2);
            //qDebug()<<"key_2 relessed";
        }
        //event_data.id = 0;

        //捕获key_3按下
        read(key_3_fd,&event_data,sizeof (event_data));
        //qDebug() << "key3 "<< event_data.id;
        if(event_data.id == 2 && key_3_flag==0)
        {
            key_3_flag = 1;
            emit(this->keyPressed(3));
            //qDebug()<<"key_3 pressed";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               event_data.id = 0;
        }
        else if(event_data.id == 1 && key_3_flag==1)
        {
            key_3_flag = 0;
            emit keyRelessed(3);
            //qDebug()<<"key_3 relessed";
        }
        event_data.id = 0;

        //捕获光电开关
        read(light_trigger_fd,&event_data,sizeof (event_data));
        if(event_data.id == 1 && light_trigger_flag==0)
        {
            //qDebug()<<"light triggered";
            light_trigger_flag = 1;
            emit(this->lightTriggered());
        }
        else
            light_trigger_flag = 0;
        event_data.id = 0;

        //捕获people coming
        ret = ioctl(people_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
        if (ret == -1) {
            ret = -errno;
            fprintf(stderr, "Failed to issue %s (%d), %s\n",
                    "GPIOHANDLE_GET_LINE_VALUES_IOCTL", ret, strerror(errno));
            exit(ret);
        }

        if(data.values[0] ==1&&people_flag==0)
        {
            emit(this->peopleDetected(true));
            people_flag = 1;
            //qDebug()<<"people coming";
        }
        else if(data.values[0] ==0&&people_flag==1)
        {
            people_flag = 0;
            emit(this->peopleDetected(false));
            //qDebug()<<"people leave";
        }
#if 0
        //捕获火焰检测
        read(fire_fd,&event_data,sizeof (event_data));
        qDebug() << "------------------"<<event_data.id;
        if(event_data.id == 2 && flame_flag == 0)
        {
            qDebug()<<"flamedetection on";
            flame_flag = 1;
            emit(this->flameDetected(true));
            event_data.id = 0;
        }
        else if(event_data.id == 0 && flame_flag == 1)
        {
            qDebug()<<"flamedetection off";
            flame_flag = 0;
            emit(this->flameDetected(false));
            event_data.id = 0;
        }
#endif
        //捕获key_2按下
        ret = ioctl(fire_fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
        if (ret == -1) {
            ret = -errno;
            fprintf(stderr, "Failed to issue %s (%d), %s\n",
                    "GPIOHANDLE_GET_LINE_VALUES_IOCTL", ret, strerror(errno));
            exit(ret);
        }
        //qDebug() << data.values[0];
        if(data.values[0] ==1&&flame_flag==0)
        {
            emit(this->flameDetected());
            flame_flag = 1;
            //qDebug()<<"flame on";
        }
        else
            flame_flag = 0;
    }

private:
    struct gpiohandle_request req;
    struct gpiohandle_data data;
    struct gpiochip_info chip_info;
    struct gpioevent_request event_req;
    struct gpioevent_data event_data;

    struct gpiod_chip *gpiochip5;
    struct gpiod_line *gpioline5_7;

    int key_1_fd; //设备文件描述符
    int key_2_fd;
    int key_3_fd;
    int light_trigger_fd;
    int people_fd;
    int fire_fd;

    QTimer * fs_timer;
};

#endif //ENV_H
