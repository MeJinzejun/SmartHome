#ifndef TEMPHUM_H
#define TEMPHUM_H

/***************************************************************************
 *
 * 类名：fsmpTempHum
 * 功能说明：扩展板 温度湿度
 * 公有函数：
 *      fsmpTempHum（）
 *      double temperature(void)                    提取温度
 *      double humidity(void)                       提取湿度
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

class fsmpTempHum:public QObject{
    Q_OBJECT
public:
    explicit fsmpTempHum(QObject *parent = nullptr)
        :QObject(parent)
    {

    }

    double temperature(void)
    {
        int temp_raw = 0;
        int temp_offset = 0;
        float temp_scale = 0;

        open_device_int("iio:device0","in_temp_raw",&temp_raw);
        open_device_int("iio:device0","in_temp_offset",&temp_offset);
        open_device_float("iio:device0","in_temp_scale",&temp_scale);
        return (temp_raw + temp_offset) * temp_scale / 1000;
    }

    double humidity(void)
    {
        int hum_raw = 0;
        int hum_offset = 0;
        float hum_scale = 0;

        open_device_int("iio:device0","in_humidityrelative_raw",&hum_raw);
        open_device_int("iio:device0","in_humidityrelative_offset",&hum_offset);
        open_device_float("iio:device0","in_humidityrelative_scale",&hum_scale);
        return (hum_raw + hum_offset) * hum_scale / 1000;
    }

private:

    bool open_device_int(const char *device, const char *filename, int *val)
    {
        int ret = 0;
        int fd;
        char temp[128] = {0};
        sprintf(temp, "/sys/bus/iio/devices/%s/%s", device, filename);
        fd = open(temp,O_RDONLY|O_NONBLOCK);
        if(fd < 0)
        {
            perror("open");
            return false;
        }
        char buf[10] = {0};
        ret = read(fd,buf,sizeof(buf));
        if(ret < 0)
        {
            perror("read");
            return false;
        }
        sscanf(buf,"%d",val);
        ::close(fd);

        return true;
    }

    bool open_device_float(const char *device, const char *filename, float *val)
    {
        int ret = 0;
        int fd;
        char temp[128] = {0};
        sprintf(temp, "/sys/bus/iio/devices/%s/%s", device, filename);
        fd = open(temp,O_RDONLY|O_NONBLOCK);
        if(fd < 0)
        {
            perror("open");
            return false;
        }
        char buf[10] = {0};
        ret = read(fd,buf,sizeof(buf));
        if(ret < 0)
        {
            perror("read");
            return false;
        }
        sscanf(buf,"%f",val);
        ::close(fd);

        return true;
    }

private:
    int temp_hum_fd;; //设备文件描述符

};
#endif // TEMPHUM_H
