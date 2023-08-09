#ifndef ELC_H
#define ELC_H

/***************************************************************************
 *
 * 类名：fsmpElectric
 * 功能说明：扩展板 电气信息检测
 * 公有函数：
 *      fsmpElectric（）
 *      float voltagemV(void)           获取电压mv（旋钮控制）
 *      float currentmA(void)           获取电流ma（板载设备真实电流）
 *
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
#include <stdint.h>
#include <QDebug>

class fsmpElectric:public QObject{
    Q_OBJECT
public:
    explicit fsmpElectric(QObject *parent = nullptr)
        :QObject(parent)
    {
    }
    ~fsmpElectric(){

    }

    float voltagemV(void)
    {
        electric_fd1 = open("/sys/bus/iio/devices/iio:device3/in_voltage1_raw",O_RDONLY|O_NONBLOCK);
        if(electric_fd1 < 0)
        {
            perror("open");
            return -1;
        }
        char buf[10] = {0};
        int ret = read(electric_fd1,buf,sizeof(buf));
        if(ret < 0)
        {
            perror("read");
            return -1;
        }
        int val = 0;
        sscanf(buf,"%d",&val);
        ::close(electric_fd1);

        return (float)3300 * val / 65536;
    }

    float currentmA(void)
    {
        electric_fd0 = open("/sys/bus/iio/devices/iio:device3/in_voltage0_raw",O_RDONLY|O_NONBLOCK);
        if(electric_fd0 < 0)
        {
            perror("open");
            return -1;
        }
        char buf[10] = {0};
        int ret = read(electric_fd0,buf,sizeof(buf));
        if(ret < 0)
        {
            perror("read");
            return -1;
        }
        int val = 0;
        sscanf(buf,"%d",&val);
        ::close(electric_fd0);
        return ((((float)3300 * val) /65536)/(10 + 100*1000 + 1000)) * 1000 / 0.1;
    }

private:
    int electric_fd0; //设备文件描述符
    int electric_fd1; //设备文件描述符
};
#endif // ELC_H
