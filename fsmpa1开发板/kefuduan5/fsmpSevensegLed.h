#ifndef FSMPSEVENSEGLED_H
#define FSMPSEVENSEGLED_H

/***************************************************************************
 *
 * 类名：fsmpSevensegLed
 * 功能说明：扩展板 七段数码管
 * 公有函数：
 *      fsmpSevensegLed（）
 *      void display(int value)                   显示4位以内整数
 *      void display(const char *str)             显示4位字符，注：仅支持 a、b、c、d、e
 *
 * 信号：
 *      无
 *
 * *************************************************************************/

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <QThread>
#include <stdlib.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

class fsmpSevensegLed:public QThread{
    Q_OBJECT
public:
    explicit fsmpSevensegLed(QObject *parent = nullptr)
        :QThread(parent)
    {
        memset(data, 0, sizeof(data));
        device = "/dev/spidev0.0";
        bits = 8;
        speed = 400000;
        delay = 0;
    }

    void display(int value)
    {
        data[3] = value%10;
        data[2] = value/10%10;
        data[1] = value/100%10;
        data[0] = value/1000%10;
    }

    //"abcde"
    void display(const char *str)
    {
        data[3] = (strlen(str)-1 >= 0)?(str[strlen(str)-1] - 'a')+10:0;
        data[2] = (strlen(str)-2 >= 0)?(str[strlen(str)-2] - 'a')+10:0;
        data[1] = (strlen(str)-3 >= 0)?(str[strlen(str)-3] - 'a')+10:0;
        data[0] = (strlen(str)-4 >= 0)?(str[strlen(str)-4] - 'a')+10:0;
    }

private:
    void run()
    {
        int ret = 0;
        int fd;

        fd = open(device, O_RDWR);  //打开设备文件
        if (fd < 0)
            pabort("can't open device");
        /*
         * spi mode //设置spi设备模式
         */
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);    //写模式
        if (ret == -1)
            pabort("can't set spi mode");

        /*
         * bits per word    //设置每个字含多少位
         */
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);   //写每个字含多少位
        if (ret == -1)
            pabort("can't set bits per word");

        /*
         * max speed hz     //设置速率
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);   //写速率
        if (ret == -1)
            pabort("can't set max speed hz");

        //打印模式,每字多少位和速率信息
        printf("spi mode: %d\n", mode);
        printf("bits per word: %d\n", bits);
        printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

        //transfer(fd);   //传输测试
        int i = 0;
        int pos = 0;
        unsigned char buf[2];

        while(1) {
            pos = 1 << (i % 4);
            buf[0] = pos;
            buf[1] = num[data[i]];
            if (write(fd, buf, 2) < 0)
                perror("write");
            i++;
            if (i == 4) {
                i = 0;
            }
            usleep(3500);
        }

    }

    static void pabort(const char *s)
    {
        perror(s);
        abort();
    }
    const char *device;
    uint8_t mode;
    uint8_t bits;
    uint32_t speed;
    uint16_t delay;

    int data[4];

    unsigned char num[20] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
};

#endif // FSMPSEVENSEGLED_H
