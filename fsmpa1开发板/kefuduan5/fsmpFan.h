#ifndef FAN_H
#define FAN_H


/***************************************************************************
 *
 * 类名：fsmpFan
 * 功能说明：扩展板 风扇控制
 * 公有函数：
 *      fsmpFan（）
 *      void start(void)            开启风扇）
 *      void stop(void)             关闭风扇
 *      void setSpeed(int speed)    设置风扇速度（0-255）
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

class fsmpFan:public QObject{
    Q_OBJECT
public:
    explicit fsmpFan(QObject *parent = nullptr):QObject(parent) {
        const char *device = "/sys/class/hwmon/hwmon1/pwm1";
        fan_fd = ::open(device,O_WRONLY|O_TRUNC);
        if(fan_fd < 0)
        {
            perror("open fan");
            return;
        }

        fan_speed = 0;
    };
    ~fsmpFan(){
        ::close(fan_fd);
    }

    /*
     * Description: 关闭设备
     * input: null
     * output: null
     * return: 成功true/失败false
    */
    void stop(void)
    {
        write(fan_fd,"0",strlen("0"));
    }

    /*
     * Description: start设备
     * input: null
     * output: null
     * return: 成功true/失败false
    */
    void start(void)
    {
        char speedstr[100] = {0};
        sprintf(speedstr, "%d", fan_speed);
        write(fan_fd, speedstr, strlen(speedstr));
    }

    /*
     * Description: 控制风扇转速
     * intput: speed 转速
     * output: null
     * return: null
    */
    void setSpeed(int speed)
    {
        fan_speed = speed%255;
    }

private:
    int fan_fd; //设备文件描述符
    int fan_speed;
};

#endif // FAN_H
