#ifndef FSMPBEEPER_H
#define FSMPBEEPER_H

/***************************************************************************
 *
 * 类名：fsmpBeeper
 * 功能说明：扩展板 蜂鸣器
 * 公有函数：
 *      fsmpBeeper（）
 *      bool setRate(uint32_t rate)     设置频率
 *      bool start(void)                启动蜂鸣器
 *      bool stop(void)                 关闭蜂鸣器
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

/*
    FSMP开发板（linux）蜂鸣器操作类 v0
    2023/1/9 by 张城
*/
class fsmpBeeper:public QObject{
    Q_OBJECT
public:
    explicit fsmpBeeper(const char *device = "/dev/input/event0", QObject *parent = nullptr)
        :QObject(parent)
    {
        event.type = EV_SND;

        time.tv_sec = 1;
        time.tv_usec = 0;
        event.time = time;
        beeper_fd = ::open(device,O_RDWR);
        if(beeper_fd < 0)
        {
            perror("open_beeper");
            return;
        }
    };

    ~fsmpBeeper()
    {
        ::close(beeper_fd);
    }

    /*
     * Description: 设置频率值
     * input: rate 频率
     * output: null
     * return: 成功true/失败false
    */
    bool setRate(uint32_t rate = 1000)
    {
        if(rate > 0)
        {
            event.code = SND_TONE;
            event.value = rate;
            return true;
        }

        return false;
    }

    /*
     * Description: start设备
     * input: null
     * output: null
     * return: 成功true/失败false
    */
    bool start(void)
    {
        int ret = write(beeper_fd, &event, sizeof(struct input_event));
        if(ret < 0)
        {
            perror("write");
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
    bool stop(void)
    {
        struct input_event event;
        event.type = EV_SND;
        event.code = SND_BELL;
        event.value = 0000;
        time.tv_sec = 1;
        time.tv_usec = 0;
        event.time = time;
        int ret = write(beeper_fd, &event, sizeof(struct input_event));
        if(ret < 0)
        {
            perror("write");
            return false;
        }
        return true;
    }

private:
    int beeper_fd; //设备文件描述符

    struct input_event event;
    struct timeval time;
};

#endif // FSMPBEEPER_H




