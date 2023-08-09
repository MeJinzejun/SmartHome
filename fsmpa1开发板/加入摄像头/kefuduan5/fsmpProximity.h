#ifndef FSMPPROXIMITY_H
#define FSMPPROXIMITY_H

/***************************************************************************
 *
 * 类名：fsmpProximity
 * 功能说明：扩展板 红外接近传感器（和光照传感器一起）
 * 公有函数：
 *      fsmpProximity（）
 *      double getValue(void)                   获取接近情况（0-2048越接近数据越大）
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

class fsmpProximity:public QObject{
    Q_OBJECT
public:
    explicit fsmpProximity(QObject *parent = nullptr)
        :QObject(parent)
    {
        proximity_fd = ::open("/sys/bus/iio/devices/iio:device1/in_proximity_raw",O_RDONLY|O_NONBLOCK);
        if(proximity_fd < 0)
        {
            perror("open");
            return;
        }
    }

    ~fsmpProximity()
    {
        ::close(proximity_fd);
    }

    /*
     * Description: 获取distence
     * input: null
     * output: null
     * return: current light
    */
    int getValue(void)
    {
        char buf[10] = {0};

        int ret = read(proximity_fd,buf,10);
        if(ret < 0)
        {
            perror("read");
            return false;
        }
        int num;
        sscanf(buf, "%d", &num);
        lseek(proximity_fd, 0, SEEK_SET);

        return  num;
    }

private:
    int proximity_fd; //设备文件描述符

};

#endif // FSMPPROXIMITY_H
