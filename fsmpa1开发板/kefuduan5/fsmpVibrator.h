#ifndef VIBRATOR_H
#define VIBRATOR_H

/***************************************************************************
 *
 * 类名：fsmpVibrator
 * 功能说明：扩展板 振动马达
 * 公有函数：
 *      fsmpVibrator（）
 *      bool setParameter(int strong_magnitude, int timems)       设置振动强度（0-0xffff）及时长（ms）
 *      bool start(void)                       启动马达
 *      bool stop(void)                        停止马达
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
#include <sys/ioctl.h>
#include <QDebug>

class fsmpVibrator:public QObject{
    Q_OBJECT
public:
    explicit fsmpVibrator(QObject *parent = nullptr)
        :QObject(parent)
    {
        vibrator_fd = ::open("/dev/input/event1", O_RDWR|O_NONBLOCK);
        if(vibrator_fd < 0)
        {
            perror("open vibrator");
        }
    }
    ~fsmpVibrator()
    {
        ::close(vibrator_fd);
    }

      /*
      * Description:设置
      * input:频率
      * output：null
      * return：成功true/失败false
      */
    bool setParameter(int strong_magnitude = 0xFFFF, int timems = 5000)
    {
        //qDebug()<<strong_magnitude<<timems;
        effect.type = FF_RUMBLE,
        effect.id = -1,
        effect.u.rumble.strong_magnitude = strong_magnitude;
        effect.u.rumble.weak_magnitude = 0;
        effect.replay.length = timems;
        effect.replay.delay = 0;

        if (ioctl(vibrator_fd, EVIOCSFF, &effect) < 0)
        {
            printf("Error creating new effect: %s\n", strerror(errno));
            return false;
        }

        return true;
    }

    bool start(void)
    {
        struct input_event play = {
            .type = EV_FF,
            .code = (__U16_TYPE)effect.id,
            .value = 1
        };
        int ret = write(vibrator_fd, (const void*) &play, sizeof(play));
        if(ret < 0)
        {
            perror("vibrator on");
            return false;
        }
        //qDebug()<<"start";

        return true;
    }

    bool stop(void)
    {
        struct input_event play = {
            .type = EV_FF,
            .code = (__U16_TYPE)effect.id,
            .value = 0
        };

        int ret = write(vibrator_fd, (const void*) &play, sizeof(play));
        if(ret < 0)
        {
            perror("vibrator off");
            return false;
        }
        //qDebug()<<"stop";
        return true;
    }

private:
    int vibrator_fd;//设备文件描述符
    struct ff_effect effect;


};



#endif // VIBRATOR_H
