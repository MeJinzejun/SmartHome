#ifndef LED_H
#define LED_H

/***************************************************************************
 *
 * 类名：fsmpLeds
 * 功能说明：扩展板 3颗灯泡
 * 公有函数：
 *      fsmpLeds（）
 *      bool on(lednum x)                   开灯 lednum： fsmpLeds::LED1/fsmpLeds::LED2/fsmpLeds::LED3
 *      bool off(lednum x)                  关灯 lednum： fsmpLeds::LED1/fsmpLeds::LED2/fsmpLeds::LED3
 * 信号：
 *      无
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

class fsmpLeds:public QObject{
    Q_OBJECT

public:
    enum lednum{
        LED1, LED2, LED3
      };
    explicit fsmpLeds(QObject *parent = nullptr)
        :QObject(parent)
    {
        led_fd1 = ::open("/sys/class/leds/led1/brightness",O_WRONLY|O_TRUNC);
        if(led_fd1 < 0)
        {
            perror("open led1");
            return ;
        }
        led_fd2 = ::open("/sys/class/leds/led2/brightness",O_WRONLY|O_TRUNC);
        if(led_fd2 < 0)
        {
            perror("open led2");
            return ;
        }
        led_fd3 = ::open("/sys/class/leds/led3/brightness",O_WRONLY|O_TRUNC);
        if(led_fd3 < 0)
        {
            perror("open led3");
            return ;
        }

    }

    ~fsmpLeds()
    {
        ::close(led_fd1);
        ::close(led_fd2);
        ::close(led_fd3);
    }
    /*
     * Description: 开启设备，默认/sys/class/leds/led1/brightness
     * input: lednum
     * output: null
     * return: 成功true/失败false
    */
    bool on(lednum x)
    {
        int fd;
        if(x == LED1)
            fd = led_fd1;
        else if(x == LED2)
            fd = led_fd2;
        else if(x == LED3){
            fd = led_fd3;
        }
        int ret = write(fd,"1",1);
        if(ret < 0)
        {
            return false;
        }

        return true;
    }

    /*
     * Description: 关闭设备
     * input: null
     * output: null
     * return: 成功true/失败false
    */
    bool off(lednum x)
    {
        int fd;
        if(x == LED1)
            fd = led_fd1;
        else if(x == LED2)
            fd = led_fd2;
        else if(x == LED3){
            fd = led_fd3;
        }
        int ret = write(fd,"0",1);
        if(ret < 0)
        {
            return false;
        }
        return true;
    }
private:
    int led_fd1; //设备文件描述符
    int led_fd2; //设备文件描述符
    int led_fd3; //设备文件描述符
};



#endif // LED_H
