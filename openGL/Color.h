#pragma once
#include"constant.h"
#include <vector>
#include <tuple>
#include <limits>

struct Color {
	float R;
	float G;
	float B;
	Color(float r = 1.0f, float g = 1.0f, float b = 1.0f) :R(r), G(g), B(b) {}
    Color operator*(float scalar) const {
        return Color(R * scalar, G * scalar, B * scalar);
    }
    Color operator+(Color scalar) const {
        return Color(R + scalar.R, G +scalar.G, B + scalar.B);
    }
    Color operator+(float scalar) const {
        return Color(R +scalar, G + scalar, B + scalar);
    }
    // 标量 * Color
    friend Color operator*(float scalar, const Color& color) {
        return color * scalar;
    }

    Color operator*(const Color& other) const {
        return Color(R * other.R, G * other.G, B * other.B);
    }

    bool operator==(const Color& other) const {
        return R == other.R && G == other.G && B == other.B;
    }

    bool operator!=(const Color& other) const {
        return !(*this == other);
    }
    bool equals(const Color& other, float epsilon = 0.0001f) const {
        return std::abs(R - other.R) < epsilon &&
            std::abs(G - other.G) < epsilon &&
            std::abs(B - other.B) < epsilon;
    }
    Color& operator*=(float scalar) {
        R *= scalar;
        G *= scalar;
        B *= scalar;
        return *this;
    }

    Color& operator*=(const Color& other) {
        R *= other.R;
        G *= other.G;
        B *= other.B;
        return *this;
    }   
    Color operator/(float scalar) const {
        return Color(R / scalar, G / scalar, B / scalar);
    }
    Color& operator/=(float scalar) {
        R /= scalar;
        G /= scalar;
        B /= scalar;
        return *this;
    }

    Color operator/(Color scalar) const {
        return Color(R / scalar.R, G / scalar.G, B / scalar.B);
    }
    Color& operator+=(const Color& other) {
        R += other.R;
        G += other.G;
        B += other.B;
        return *this;
    }
};

typedef struct material {
    float Ns;
    Color Ka;
    Color Kd;
    Color Ks;
    string kdMapName;
}material;



typedef struct ImageData {
    int width;
    int height;
    std::vector<Color> data;
}ImageDara;                           // 使用一维向量存储颜色数据

#define WHITE Color(1.0f, 1.0f, 1.0f)
#define RED Color(1.0f, 0.0f, 0.0f)
#define GREEN Color(0.0f, 1.0f, 0.0f)
#define BLUE Color(0.0f, 0.0f, 1.0f)
#define BLACK Color(0.0f, 0.0f, 0.0f)
#define YELLOW Color(1.0f, 1.0f, 0.0f)
#define CYAN Color(0.0f, 1.0f, 1.0f)
#define MAGENTA Color(1.0f, 0.0f, 1.0f)
#define GRAY Color(0.5f, 0.5f, 0.5f)
#define ORANGE Color(1.0f, 0.65f, 0.0f)
#define PURPLE Color(0.5f, 0.0f, 0.5f)
#define BROWN Color(0.6f, 0.4f, 0.2f)
#define PINK Color(1.0f, 0.75f, 0.8f)
#define LIGHT_GRAY Color(0.83f, 0.83f, 0.83f)
#define DARK_GRAY Color(0.33f, 0.33f, 0.33f)
#define LIGHT_BLUE Color(0.68f, 0.85f, 0.9f)
#define LIGHT_GREEN Color(0.56f, 0.93f, 0.56f)
#define LIGHT_YELLOW Color(1.0f, 1.0f, 0.88f)
#define LIGHT_PINK Color(1.0f, 0.71f, 0.76f)
#define LIGHT_ORANGE Color(1.0f, 0.87f, 0.68f)
#define LIGHT_PURPLE Color(0.8f, 0.6f, 0.8f)
#define LIGHT_BROWN Color(0.82f, 0.71f, 0.55f)
#define DARK_BLUE Color(0.0f, 0.0f, 0.55f)
#define DARK_GREEN Color(0.0f, 0.39f, 0.0f)
#define DARK_RED Color(0.55f, 0.0f, 0.0f)
#define DARK_YELLOW Color(0.55f, 0.55f, 0.0f)
#define DARK_PINK Color(0.75f, 0.0f, 0.5f)
#define DARK_ORANGE Color(1.0f, 0.55f, 0.0f)
#define DARK_PURPLE Color(0.58f, 0.0f, 0.83f)
#define DARK_BROWN Color(0.4f, 0.26f, 0.13f)

Color randomColor() {
	float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	return Color(r, g, b);
}

float getDepLerp(float val, float minVal, float maxVal, float minD=0, float MaxD=1) {
	if (maxVal - minVal == 0) return 0.0f; // 避免除以零
	float z1 = (val - minVal) / (maxVal - minVal);
	float z2 = minD + z1 * (MaxD - minD);
	return z2;
}


std::vector<Color> convertMatToColors(const cv::Mat& image, bool normalize = true) {
    std::vector<Color> colors;

    if (image.empty()) {
        std::cerr << "错误: 图像为空!" << std::endl;
        return colors;
    }

    // 检查通道数
    int channels = image.channels();
    if (channels < 3) {
        std::cerr << "警告: 图像只有 " << channels << " 个通道，可能需要转换" << std::endl;
    }

    int totalPixels = image.rows * image.cols;
    colors.reserve(totalPixels);

    switch (image.type()) {
    case CV_8UC3: {  
        //先行再列
        for (int r = 0; r < image.rows; r++) {
            const cv::Vec3b* row = image.ptr<cv::Vec3b>(r);
            for (int c = 0; c < image.cols; c++) {
                const cv::Vec3b& pixel = row[c];
                // OpenCV是BGR顺序，Color是RGB顺序
                if (normalize) {
                    colors.emplace_back(
                        pixel[2] / 255.0f,  // R
                        pixel[1] / 255.0f,  // G
                        pixel[0] / 255.0f   // B
                    );
                }
                else {
                    colors.emplace_back(
                        static_cast<float>(pixel[2]),  // R
                        static_cast<float>(pixel[1]),  // G
                        static_cast<float>(pixel[0])   // B
                    );
                }
            }
        }
        break;
    }
    case CV_32FC3: {  // 32位浮点3通道图像
        for (int r = 0; r < image.rows; r++) {
            const cv::Vec3f* row = image.ptr<cv::Vec3f>(r);
            for (int c = 0; c < image.cols; c++) {
                const cv::Vec3f& pixel = row[c];
                // OpenCV是BGR顺序，Color是RGB顺序
                colors.emplace_back(pixel[2], pixel[1], pixel[0]);
            }
        }
        break;
    }
    default:
        std::cerr << "不支持的图像类型: " << image.type() << std::endl;
        // 可以尝试转换为CV_8UC3再处理
        cv::Mat converted;
        image.convertTo(converted, CV_8UC3);
        if (!converted.empty()) {
            return convertMatToColors(converted, normalize);
        }
        break;
    }

    return colors;
}



