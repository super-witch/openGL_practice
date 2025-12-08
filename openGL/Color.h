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
    // ±Í¡ø * Color
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
}material;

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
	if (maxVal - minVal == 0) return 0.0f; // ±‹√‚≥˝“‘¡„
	float z1 = (val - minVal) / (maxVal - minVal);
	float z2 = minD + z1 * (MaxD - minD);
	return z2;
}








