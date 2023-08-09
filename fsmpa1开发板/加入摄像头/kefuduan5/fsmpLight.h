#ifndef LIGHT_H
#define LIGHT_H

/***************************************************************************
 *
 * 类名：fsmpLight
 * 功能说明：扩展板 光照强度检测
 * 公有函数：
 *      fsmpLight（）
 *      double getValue(void)                   获取光照强度
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
#include <QDebug>

class fsmpLight:public QObject{
    Q_OBJECT

public:
    explicit fsmpLight(QObject *parent = nullptr)
        :QObject(parent)
    {
        light_fd = ::open("/sys/bus/iio/devices/iio:device1/in_illuminance_input",O_RDONLY|O_NONBLOCK);
        if(light_fd < 0)
        {
            perror("open");
            return;
        }
    }

    ~fsmpLight()
    {
        ::close(light_fd);
    }

    /*
     * Description: 获取光照强度
     * input: null
     * output: null
     * return: current light
    */
    double getValue(void)
    {
        char buf[10] = {0};

        int ret = read(light_fd,buf,10);
        if(ret < 0)
        {
            perror("read");
            return false;
        }
        double num;
        sscanf(buf, "%lf", &num);
        lseek(light_fd, 0, SEEK_SET);

        return  num;
    }

private:
    int light_fd; //设备文件描述符

};

#endif // LIGHT_H
