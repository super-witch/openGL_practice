#pragma once
#include <math.h>


struct Matrix3x3 {
	float m[3][3];
	Matrix3x3() {
		// 初始化为单位矩阵
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				m[i][j] = (i == j) ? 1.0f : 0.0f;
			}
		}
	}
};  
typedef struct Matrix3x3 Matrix3x3;

//矩阵乘法
Matrix3x3 multiplyMatrices(const Matrix3x3& a, const Matrix3x3& b) {
	Matrix3x3 result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.m[i][j] = 0;
			for (int k = 0; k < 3; k++) {
				result.m[i][j] += a.m[i][k] * b.m[k][j];
			}
		}
	}
	return result;
}
//点与矩阵乘法
Homo2D transformPoint(const Matrix3x3& matrix, const Homo2D& point) {
	Homo2D result;
	result.x = matrix.m[0][0] * point.x + matrix.m[0][1] * point.y + matrix.m[0][2] * point.w;
	result.y = matrix.m[1][0] * point.x + matrix.m[1][1] * point.y + matrix.m[1][2] * point.w;
	result.w = matrix.m[2][0] * point.x + matrix.m[2][1] * point.y + matrix.m[2][2] * point.w;
	return result;
}
//平移矩阵
Matrix3x3 createTranslationMatrix(Homo2D homo) {
	Matrix3x3 mat;
	mat.m[0][2] = homo.x;
	mat.m[1][2] = homo.y;
	return mat;
}

//旋转矩阵
Matrix3x3 createRotationMatrix(float angle) {
	Matrix3x3 mat;
	float cosA = cos(angle); float sinA = sin(angle);
	mat.m[0][0] = cosA;
	mat.m[0][1] = -sinA; mat.m[1][0] = sinA;
	mat.m[1][1] = cosA;
	return mat;
}

//缩放矩阵
Matrix3x3 createScaleMatrix(Homo2D homo) {
	Matrix3x3 mat;
	mat.m[0][0] = homo.x;
	mat.m[1][1] = homo.y;
	return mat;
}


//反射矩阵，k对称=tan(angle)
Matrix3x3 createmirrorMatrix(float angle) {
	Matrix3x3 mat;
	float cosA = cos(angle); float sinA = sin(angle);
	mat.m[0][0] = 1-2*cosA*cosA;
	mat.m[0][1] = -2*sinA*cosA; mat.m[1][0] =-2* sinA*cosA;
	mat.m[1][1] =1-2*sinA*sinA;
	return mat;
}
// 在Matrix.h中添加矩阵求逆函数
Matrix3x3 invertMatrix(const Matrix3x3& mat) {
	Matrix3x3 inv;
	float det = mat.m[0][0] * (mat.m[1][1] * mat.m[2][2] - mat.m[2][1] * mat.m[1][2]) -
		mat.m[0][1] * (mat.m[1][0] * mat.m[2][2] - mat.m[1][2] * mat.m[2][0]) +
		mat.m[0][2] * (mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0]);

	if (fabs(det) < 1e-6) {
		// 矩阵不可逆，返回单位矩阵
		return Matrix3x3();
	}

	float invDet = 1.0f / det;

	inv.m[0][0] = (mat.m[1][1] * mat.m[2][2] - mat.m[2][1] * mat.m[1][2]) * invDet;
	inv.m[0][1] = (mat.m[0][2] * mat.m[2][1] - mat.m[0][1] * mat.m[2][2]) * invDet;
	inv.m[0][2] = (mat.m[0][1] * mat.m[1][2] - mat.m[0][2] * mat.m[1][1]) * invDet;
	inv.m[1][0] = (mat.m[1][2] * mat.m[2][0] - mat.m[1][0] * mat.m[2][2]) * invDet;
	inv.m[1][1] = (mat.m[0][0] * mat.m[2][2] - mat.m[0][2] * mat.m[2][0]) * invDet;
	inv.m[1][2] = (mat.m[1][0] * mat.m[0][2] - mat.m[0][0] * mat.m[1][2]) * invDet;
	inv.m[2][0] = (mat.m[1][0] * mat.m[2][1] - mat.m[2][0] * mat.m[1][1]) * invDet;
	inv.m[2][1] = (mat.m[2][0] * mat.m[0][1] - mat.m[0][0] * mat.m[2][1]) * invDet;
	inv.m[2][2] = (mat.m[0][0] * mat.m[1][1] - mat.m[1][0] * mat.m[0][1]) * invDet;

	return inv;
}




//void displayOther() {
//	// 绘制falorigin1,用大一点的蓝色点
//	glColor3f(0.0f, 0.0f, 1.0f);
//	glPointSize(8.0f);
//	glBegin(GL_POINTS);
//	glVertex2i(falOrigin1.x * 2, falOrigin1.y * 2);
//	glEnd();
//	// 绘制falorigin2,用大一点的红色色点
//	glColor3f(1.0f, 0.0f, 0.0f);
//	glBegin(GL_POINTS);
//	glVertex2i(falOrigin2.x * 2, falOrigin2.y * 2);
//	glEnd();
//	glPointSize(1.0f);
//
//	// 绘制坐标轴
//	glColor3f(0.5f, 0.5f, 0.5f);
//	glBegin(GL_LINES);
//	// x轴
//	glVertex2i(-screen_width / 2, 0);
//	glVertex2i(screen_width / 2, 0);
//	// y轴
//	glVertex2i(0, -screen_height / 2);
//	glVertex2i(0, screen_height / 2);
//	glEnd();
//
//}


//void clearMa() {
//	transformM1 = { 0,0 };
//	scaleM1 = { 1,1 };
//	rotateM1 = 0;
//}
//void display_LaughFace0() {
//	Matrix3x3 Ro1 = createScaleMatrix(scaleM1);
//	Matrix3x3 Ro2 = createRotationMatrix(rotateM1);
//	Matrix3x3 Ro3 = createTranslationMatrix(transformM1);
//	Matrix3x3 RoAll = multiplyMatrices(Ro2, Ro1);
//	for (int i = 0; i < laughFace.size(); i++) {
//		Gragh* child = laughFace[i].get();
//		vector<Homo2D> allPointSet;
//		Homo2D temp1 = transformPoint(RoAll, child->getOrigin());
//		Homo2D temp2 = transformPoint(Ro3, temp1);
//		for (auto it = child->getPointSet().begin(); it != child->getPointSet().end(); ++it) {
//			allPointSet.push_back(transformPoint(RoAll, *it));
//		}
//		display_gragh(temp2, allPointSet);
//	}
//}
//void display_LaughFace() {
//	Matrix3x3 RoRotateToOrigin = createTranslationMatrix({ -falOrigin1.x, -falOrigin1.y });
//	Matrix3x3 RoRotateBack = createTranslationMatrix(falOrigin1);
//	Matrix3x3 RoScaleToOrigin = createTranslationMatrix({ -falOrigin2.x, -falOrigin2.y });
//	Matrix3x3 RoScaleBack = createTranslationMatrix(falOrigin2);
//	Matrix3x3 RoScale = createScaleMatrix(scaleM1);
//	Matrix3x3 RoRotate = createRotationMatrix(rotateM1);
//	Matrix3x3 RoTranslate = createTranslationMatrix(transformM1);
//
//	Matrix3x3 RoRotateAll = multiplyMatrices(RoRotateBack, multiplyMatrices(RoRotate, RoRotateToOrigin));
//	Matrix3x3 RoScaleAll = multiplyMatrices(RoScaleBack, multiplyMatrices(RoScale, RoScaleToOrigin));
//	Matrix3x3 RoAll = multiplyMatrices(RoTranslate, multiplyMatrices(RoRotateAll, RoScaleAll));
//
//	for (int i = 0; i < laughFace.size(); i++) {
//		Gragh* child = laughFace[i].get();
//		vector<Homo2D> allPointSet;
//		for (auto it = child->getPointSet().begin(); it != child->getPointSet().end(); ++it) {
//			allPointSet.push_back(transformPoint(RoAll, *it));
//		}
//		child->setPointSet(allPointSet);
//		Homo2D transformedOrigin = transformPoint(RoAll, child->getOrigin());
//		child->setOrigin(transformedOrigin);
//		display_gragh(transformedOrigin, allPointSet);
//	}
//}


//Ellipse e1 = { {0,0},100,100 };
//Ellipse e2 = { {-50,15},30,15 };
//Ellipse e3 = { {50,15},30,15 };
//Ellipse e4 = { {-50,15},15,15 };
//Ellipse e5 = { {50,15},15,15 };
//Parabola paMouth = { pMouth,30, 20, 20 };
//
//
//
//vector<unique_ptr<Gragh>>laughFace;
//
//void initialLaughFace() {
//	laughFace.push_back(make_unique<Ellipse>(e1));
//	laughFace.push_back(make_unique<Ellipse>(e2));
//	laughFace.push_back(make_unique<Ellipse>(e3));
//	laughFace.push_back(make_unique<Ellipse>(e4));
//	laughFace.push_back(make_unique<Ellipse>(e5));
//	laughFace.push_back(make_unique<Parabola>(paMouth));
//	for (auto& it : laughFace) {
//		it->savePoint();
//	}
//}


//Homo2D p1 = { -25,-25 };
//Homo2D p2 = { 25,-25 };
//Homo2D pMouth = { 0,-50 };
//bool keyShift = false;
//Homo2D falOrigin1 = { 0,0 };
//Homo2D falOrigin2 = { 0,0 };
//void initial_box() {
//	for (int i = 0; i < 4; i++) {
//		for (int j = i + 1; j < 4; j++) {
//			Line li = { {box[i].x,box[i].y},{box[j].x,box[j].y} };
//			boxLine.push_back(li);
//		}
//	}
//	for (auto& it : boxLine) {
//		it.savePoint();
//	}
//}
