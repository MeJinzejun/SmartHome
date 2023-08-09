#ifndef fsmpCamera_H
#define fsmpCamera_H
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <string.h>
#include <exception>
#include <QThread>
#include <QImage>
#include <QObject>

using namespace  std;

const int WIDTH= 640;
const int HEIGHT=480;
//异常类
class VideoException : public exception
{
public:
    VideoException(string err):errStr(err) {}
    ~VideoException(){};
    const char* what()const  noexcept
    {
        return errStr.c_str();
    }
private:
    string errStr;
};


struct VideoFrame
{
    char *start; //保存内核空间映射到用户空间的空间首地址
    int length;//空间长度
};

class fsmpCamera :public QThread
{
    Q_OBJECT

signals:
    void currentImage(QImage);
public:
    fsmpCamera(const char *dname="/dev/video0", int count=4);
    ~fsmpCamera();
private:
    void grapImage(char *imageBuffer, int *length);
    bool yuyv_to_rgb888(unsigned char *yuyvdata, unsigned char *rgbdata, int picw=WIDTH, int pich=HEIGHT);
    void video_init();
    void video_mmap();
    void open();
    void close();
    //定义run函数
    void run();

private:
    string deviceName;
    int vfd;//保存文件描述符
    int count;//缓冲区个数
    vector<struct VideoFrame> framebuffers;
    char buffer[WIDTH*HEIGHT*3];
    char rgbbuffer[WIDTH*HEIGHT*3];
};

#endif // fsmpCamera_H
