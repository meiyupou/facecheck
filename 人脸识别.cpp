#include <iostream>
#include<opencv2/opencv.hpp>
#include<Windows.h>
#include<mmsystem.h>//多媒体库
#pragma comment (lib,"winmm.lib")//静态库资源
#include<vector>
#include<string>
using namespace std;
using namespace cv;

Mat img;
Mat newimg;
VideoCapture cap;
VideoCapture cam = VideoCapture(0);
#define scalex 10
#define scaley 10

void showimage();//显示图片
void showvideo();//显示视频
void showmusic();//播放音频
bool drawface(Mat& img, vector<Rect>& face, int resizex, int resizey);//人脸识别 画人脸矩形
void onTrackerbarchange(int pos, void*);//进度条回调函数
void showcamera();//打开摄像头进行人脸识别

int main()
{
    int testcase;
    while (1)
    {
        cout << "*******************************************************************************************" << endl;
        cout << "***********************************输入0进行视频人脸识别***********************************" << endl;
        cout << "***********************************输入1打开摄像头人脸识别*********************************" << endl;
        cout << "*******************************************************************************************" << endl;
        while (cin >> testcase)
            {
             switch (testcase)
                {
                 case 0:
                 showvideo();//视频人脸识别
                 break;
                 case 1:
                 showcamera();//打开摄像头人脸识别
                 break;
                }

            }
    }
    cout << "结束测试" << endl;
    return 0;
}
void showimage()
{
    Mat bk = imread("bk.jpg");
    //通过路径加载图片
    //imread("url")
    //显示图像 imshow("window name",Mat bk)
    imshow("bk", bk);
    //opencv不能用死循环，否则会阻塞
    //按键响应 waitkey(0)表示程序会持续等待用户按下任意键，直到用户按下键盘上的任意键后才会继续执行后面的代码
    //waitkey(6000),程序会在等待6秒钟后继续执行后面的代码，不需要用户交互
    waitKey(0);
}
void showvideo()
{
    //VideoCapture 操作视频的类，也可也用来打开摄像头
    //1.打开视频
    cap = VideoCapture("1.mp4");
    if (!cap.isOpened())//判断视频是否正常打开
    {
        cout << "打开失败"<<endl;
        return;
    }
    else
    {
        cout << "打开成功" << endl;
    }
    namedWindow("video",WINDOW_AUTOSIZE);//这将创建一个名为"video"的窗口，并使窗口大小自适应显示图像的大小。
    //设置希望的分辨率
    int frames = (int)cap.get(CAP_PROP_FRAME_COUNT);//获得视频的总帧数
    int newwidth = (int)cap.get(CAP_PROP_FRAME_WIDTH)/scalex;//计算新的宽度（原始宽度除以10）
    int newheight = (int)cap.get(CAP_PROP_FRAME_HEIGHT)/scaley;//计算新的高度（原始宽度除以10）
    int slider =0;
    createTrackbar("slider", "video", &slider, frames,onTrackerbarchange);//创建进度条
    //2.每一帧给导出来
    //c++流操作 IO流 
    //标准输入输出流
    //文件流
    //字符流
    int flag = 1;
    vector<Rect> face;
    string face_file = "haarcascade_frontalface_default.xml";//采样文件的路径
    //创建人脸检测器（Haar级联分类器）
    CascadeClassifier object;
    //检测器加载采样文件路径
    object.load(face_file);
    while (cap.read(img))// 读取视频的每一帧
    //cap.read(image)是OpenCV中VideoCapture对象的一个方法，用于读取视频帧并将其存储到image变量中。
    //具体来说，cap.read(image)会从VideoCapture对象中读取下一帧视频，并将其存储到指定的image变量（通常是Mat对象）中。此方法返回一个布尔值，表示读取操作是否成功。如果成功读取到新的帧，则返回true，否则返回false。
    {
        face.clear();
        resize(img, newimg, Size(newwidth, newheight));//缩小图像尺寸,将结果存储在newimg中
        object.detectMultiScale(newimg, face);//人脸检测,将newimg里检测的人脸放入face里
        int current_pos = (int)cap.get(CAP_PROP_POS_FRAMES);
        setTrackbarPos("slider", "video", current_pos);
        drawface(img,face,scalex,scaley);
        if (flag == 1)//从第一帧开始播放音乐
        {
            showmusic();
            flag;//第一帧结束后，使flag=0，防止重复播放音乐
        }
        imshow("video", img);//显示缩小后的图像
        waitKey(15);//等待一定时间，以控制视频的帧率（每秒30帧）
    }
}
void showmusic()
{
    //mciSendString("指令",0,0,0)
    //使用mciSendString 需要将字符集改为使用多字节字符集
    //open 打开音频文件
    //close 关闭音频文件
    //play 播放音频文件
    mciSendString("open 1.mp3", 0, 0, 0);
    mciSendString("play 1.mp3", 0, 0, 0);
}
bool drawface(Mat& img, vector<Rect> &face,int resizex,int resizey)
{
    if (face.empty())
    {
        cout << "vector<Rect> face为空"<<endl;
        return false;
    }
    int x = face[0].tl().x *resizex;
    int y = face[0].tl().y *resizey;
    int width = face[0].width *resizex;
    int height = face[0].height *resizey;
    rectangle(img, Rect(x, y, width, height), Scalar(128, 255, 255));
    return true;
}
void onTrackerbarchange(int pos, void*)
{
    cap.set(CAP_PROP_POS_FRAMES, pos);
}
void showcamera()
{
    if (!cam.isOpened())//判断摄像头是否正常打开
    {
        cout << "打开失败" << endl;
        return;
    }
    else
    {
        cout << "打开成功" << endl;
    }
    namedWindow("camera", WINDOW_AUTOSIZE);//这将创建一个名为"camera"的窗口，并使窗口大小自适应显示图像的大小。
    //设置希望的分辨率
    int frames = (int)cam.get(CAP_PROP_FRAME_COUNT);//获得视频的总帧数
    int newwidth = (int)cam.get(CAP_PROP_FRAME_WIDTH)/10;//计算新的宽度（原始宽度除以10）
    int newheight = (int)cam.get(CAP_PROP_FRAME_HEIGHT)/10;//计算新的高度（原始宽度除以10）
    //2.每一帧给导出来
    //c++流操作 IO流 
    //标准输入输出流
    //文件流
    //字符流
    vector<Rect> face;
    string face_file = "haarcascade_frontalface_default.xml";//采样文件的路径
    //创建人脸检测器（Haar级联分类器）
    CascadeClassifier object;
    //检测器加载采样文件路径
    object.load(face_file);
    while (cam.read(img))// 读取视频的每一帧
        //cap.read(image)是OpenCV中VideoCapture对象的一个方法，用于读取视频帧并将其存储到image变量中。
        //具体来说，cap.read(image)会从VideoCapture对象中读取下一帧视频，并将其存储到指定的image变量（通常是Mat对象）中。此方法返回一个布尔值，表示读取操作是否成功。如果成功读取到新的帧，则返回true，否则返回false。
    {
        face.clear();
        resize(img, newimg, Size(newwidth, newheight));//缩小图像尺寸,将结果存储在newimg中
        object.detectMultiScale(newimg, face);//人脸检测,将newimg里检测的人脸放入face里
        drawface(img, face, scalex, scaley);
        imshow("camera", img);//显示缩小后的图像
        waitKey(15);//等待一定时间，以控制视频的帧率（每秒30帧）
    }
}