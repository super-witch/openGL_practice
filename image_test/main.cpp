#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main(int argc,char**argv) {
    // 测试1：基本信息
// 读取图像"E:\novel\Ccode_test\openGL\image_test\painon.jpg"
    Mat image = cv::imread("E:/novel/Ccode_test/openGL/p.jpg");
	cout << "Image Width: " << image.cols << endl;
	cout << "Image Height: " << image.rows << endl;
    imshow("Window Title", image);
    waitKey(0);  // 等待按键
    return 0;
}