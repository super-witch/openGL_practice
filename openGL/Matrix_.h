#pragma once
#include"constant.h"

class Matrix4 {
private:
public:
	float m[4][4];
	Matrix4();
	Matrix4(mode m, float x, float y, float z);
	Matrix4(float angle, Axis ax);
	Matrix4 operator*(const Matrix4& other) const {
		Matrix4 result;
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				result.m[i][j] = 0.0f;
				for (int k = 0; k < 4; ++k) {
					result.m[i][j] += m[i][k] * other.m[k][j];
				}
			}
		}
		return result;
	}
	Matrix4& operator*=(const Matrix4& other) {
		*this = *this * other;  // 利用上面定义的 operator*
		return *this;
	}
	Matrix4 Inverse() const {
		// 假设这是一个仿射变换矩阵（最后一行为[0,0,0,1]）
		// 这对于视图矩阵和模型矩阵通常是成立的

		Matrix4 inv;
		// 计算旋转部分的逆（转置）
		inv.m[0][0] = m[0][0];
		inv.m[0][1] = m[1][0];
		inv.m[0][2] = m[2][0];
		inv.m[0][3] = 0.0f;

		inv.m[1][0] = m[0][1];
		inv.m[1][1] = m[1][1];
		inv.m[1][2] = m[2][1];
		inv.m[1][3] = 0.0f;

		inv.m[2][0] = m[0][2];
		inv.m[2][1] = m[1][2];
		inv.m[2][2] = m[2][2];
		inv.m[2][3] = 0.0f;

		// 计算平移部分的逆
		inv.m[3][0] = -(m[3][0] * m[0][0] + m[3][1] * m[1][0] + m[3][2] * m[2][0]);
		inv.m[3][1] = -(m[3][0] * m[0][1] + m[3][1] * m[1][1] + m[3][2] * m[2][1]);
		inv.m[3][2] = -(m[3][0] * m[0][2] + m[3][1] * m[1][2] + m[3][2] * m[2][2]);
		inv.m[3][3] = 1.0f;

		return inv;
	}
};

Matrix4::Matrix4() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] = (i == j) ? 1.0f : 0.0f;
		}
	}
}


Matrix4::Matrix4(mode type, float x, float y, float z) :Matrix4() {
	switch (type) {
	case translate:
		m[0][3] = x;
		m[1][3] = y;
		m[2][3] = z;
		break;
	case view_translate:
		m[0][3] = x;
		m[1][3] = y;
		m[2][3] = z;
		break;
	case scale:
		m[0][0] = x;
		m[1][1] = y;
		m[2][2] = z;
		break;
	case project:
		m[3][2] = 1.0f / z;
		m[2][2] = 0.0f;
	}
}

Matrix4::Matrix4(float angle, Axis ax) :Matrix4() {
	float cosA = cos(angle); float sinA = sin(angle);
	switch (ax) {
	case x_axis:

		m[1][1] = cosA;
		m[1][2] = -sinA;
		m[2][1] = sinA;
		m[2][2] = cosA;
		break;
	case y_axis:
		m[0][0] = cosA;
		m[0][2] = sinA;
		m[2][0] = -sinA;
		m[2][2] = cosA;

		break;
	case z_axis:
		m[0][0] = cosA;
		m[0][1] = -sinA;
		m[1][0] = sinA;
		m[1][1] = cosA;

		break;
	}
}


