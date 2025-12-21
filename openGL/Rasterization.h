
#pragma once
#include"constant.h"
#include <algorithm>
using namespace std;

void display_gragh(vector<Homo2D> PointSet) {
	for (auto it = PointSet.begin(); it != PointSet.end(); ++it) {
		glColor3f(it->color.R, it->color.G, it->color.B);
		glVertex2i(it->x, it->y);
	}
}

class Camera {
public:
	Homo3D position;
	Homo3D direction;
	Homo3D up;
	bool projectedMode;
	Camera(Homo3D pos = { 0,0,0 }, Homo3D dir = { 0,0,-1,0 },bool keyShift=false) :position(pos), direction(dir),projectedMode(keyShift) {
		up = { 0,1,0,0 };
	};
	Matrix4 calculateViewMatrix() {
		// 标准化视线方向（注意：viewDirection 应该是从视点看向目标的方向）
		Homo3D forward = direction.normalize();
		Homo3D right =crossProduct(forward,up ).normalize();
		Homo3D calculatedUp = crossProduct(right, forward).normalize();  // 使用局部变量 = crossProduct(right,forward ).normalize();
		Matrix4 rotationMatrix;
		rotationMatrix.m[0][0] = right.x;   rotationMatrix.m[0][1] = right.y;   rotationMatrix.m[0][2] = right.z;   rotationMatrix.m[0][3] = 0;
		rotationMatrix.m[1][0] = calculatedUp.x;      rotationMatrix.m[1][1] = calculatedUp.y;      rotationMatrix.m[1][2] = calculatedUp.z;      rotationMatrix.m[1][3] = 0;
		rotationMatrix.m[2][0] = -forward.x; rotationMatrix.m[2][1] = -forward.y; rotationMatrix.m[2][2] = -forward.z; rotationMatrix.m[2][3] = 0;
		rotationMatrix.m[3][0] = 0;         rotationMatrix.m[3][1] = 0;         rotationMatrix.m[3][2] = 0;         rotationMatrix.m[3][3] = 1;

		// 平移矩阵的逆
		Matrix4 translationMatrix(translate, -position.x, -position.y, -position.z);
		// 旋转矩阵 * 平移矩阵
		return rotationMatrix * translationMatrix;
	}

};

class Gragh {
private:
	Homo2D origin;
	vector<Homo2D>pointSet;
public:
	Gragh(Homo2D o = { 0,0 }) :origin(o) {};
	const vector<Homo2D>& getPointSet() {
		return pointSet;
	}
	const Homo2D& getOrigin() {
		return origin;
	}
	void addPoint(Homo2D p) {
		pointSet.push_back(p);
	}
	void setOrigin(Homo2D o) {
		origin = o;
	}
	void setPointSet(vector<Homo2D>pS) {
		pointSet = pS;
	}
	virtual void savePoint(Color color) = 0;
	virtual void savePoint() = 0;
	virtual ~Gragh() = default;
};


class Line :public Gragh {

public:
	Homo2D point1;
	Homo2D point2;
	int ymin;
	int ymax;
	float m; float interx;
	Line(Homo2D p1 = { 0,0 }, Homo2D p2 = { 0,0 }) :point1(p1), point2(p2) {
		ymin = min(p1.y, p2.y); ymax = max(p2.y, p1.y);
		interx = p1.y > p2.y ? p2.x : p1.x; m = (p1.y - p2.y) != 0 ? (p1.x - p2.x) / (p1.y - p2.y) : 0;
	};
	virtual void savePoint()override;
	virtual void savePoint(Color color) override;
};

bool compareByX(const Line& a, const Line& b) {
	return a.interx < b.interx;  // 升序排序
}



void Line::savePoint(Color color) {
	float x1 = point1.x, y1 = point1.y, z1 = point1.depth;
	float x2 = point2.x, y2 = point2.y, z2 = point2.depth;

	bool steep = abs(y2 - y1) > abs(x2 - x1);
	if (steep) {
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
		std::swap(z1, z2);
	}

	int dx = x2 - x1;
	int dy = abs(y2 - y1);
	int d = 2 * dy - dx;
	float y = y1;
	int ystep = (y1 < y2) ? 1 : -1;

	for (float x = x1; x <= x2; x++) {
		// 基于x坐标计算t值，避免除零
		float t = (dx == 0) ? 0.5f : (x - x1) / dx;
		float depth = z1 + t * (z2 - z1);

		if (steep) {
			addPoint({ y, x, depth,color });
		}
		else {
			addPoint({ x, y, depth,color });
		}

		if (d < 0) {
			d += 2 * dy;
		}
		else {
			y += ystep;
			d += 2 * (dy - dx);
		}
	}
}

void Line::savePoint() {
	float x1 = point1.x, y1 = point1.y, z1 = point1.depth;
	float x2 = point2.x, y2 = point2.y, z2 = point2.depth;
	Color C1 = point1.color, C2 = point2.color;
	vNormal v1 = point1.vn, v2 = point2.vn;
	UV vt1 = point1.vt, vt2 = point2.vt;//UV

	bool steep = abs(y2 - y1) > abs(x2 - x1);
	if (steep) {
		std::swap(x1, y1);
		std::swap(x2, y2);
	}
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
		std::swap(z1, z2);
		std::swap(C1, C2);
		std::swap(vt1, vt2);
		swap(v1, v2);
	}
	int dx = x2 - x1;
	int dy = abs(y2 - y1);
	int d = 2 * dy - dx;
	float y = y1;
	int ystep = (y1 < y2) ? 1 : -1;

	for (float x = x1; x <= x2; x++) {
		// 基于x坐标计算t值，避免除零
		float t = (dx == 0) ? 0.5f : (x - x1) / dx;
		float depth = z1 + t * (z2 - z1);
		Color newColor = C1 * (1 - t) + C2 * t;
		vNormal newvn = v1 * (1 - t) + v2 * t;
		UV newvt = vt1 * (1 - t) + vt2 * t;

		newvn.normalize();
		if (steep) {
			addPoint({ y, x, depth,newColor,newvn,newvt });
		}
		else {
			addPoint({ x, y, depth,newColor,newvn,newvt });
		}

		if (d < 0) {
			d += 2 * dy;
		}
		else {
			y += ystep;
			d += 2 * (dy - dx);
		}
	}


}

