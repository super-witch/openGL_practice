
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
	Camera(Homo3D pos = { 0,0,0 }, Homo3D dir = { 0,0,-1,0 }) :position(pos), direction(dir) {
		up = { 0,1,0,0 };
	};
	Matrix4 calculateViewMatrix() {
		// 标准化视线方向（注意：viewDirection 应该是从视点看向目标的方向）
		Homo3D forward = direction.normalize();
		Homo3D right =crossProduct(forward,up ).normalize();
		up = crossProduct(right,forward ).normalize();
		Matrix4 rotationMatrix;
		rotationMatrix.m[0][0] = right.x;   rotationMatrix.m[0][1] = right.y;   rotationMatrix.m[0][2] = right.z;   rotationMatrix.m[0][3] = 0;
		rotationMatrix.m[1][0] = up.x;      rotationMatrix.m[1][1] = up.y;      rotationMatrix.m[1][2] = up.z;      rotationMatrix.m[1][3] = 0;
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

vector<Homo2D> polygon_filtter(vector<Line>ET) {
	vector<Homo2D>filledPoints;
	if (ET.size() < 3) return filledPoints;
	vector<Line>AET; int ymax = ET[0].ymax, ymin = ET[0].ymin;
	for (Line it : ET) {
		it.savePoint(); //display_gragh(it.getPointSet());
		//filledPoints.insert(filledPoints.end(), it.getPointSet().begin(), it.getPointSet().end());
		if (it.ymax > ymax)ymax = it.ymax;
		if (it.ymin < ymin)ymin = it.ymin;
	}
	for (float scany = ymin; scany < ymax; scany++) {
		for (const Line& it : ET) {
			if (it.point1.y == it.point2.y) continue;
			if (it.ymin == scany)AET.push_back(it);
		}
		AET.erase(
			std::remove_if(AET.begin(), AET.end(),
				[&scany](const Line& x) { return x.ymax <= scany; }),
			AET.end()
		);
		std::sort(AET.begin(), AET.end(), compareByX);
		for (int i = 0; i + 1 < AET.size(); i += 2) {
			float z1 = findZFromY(AET[i].point1, AET[i].point2, scany);
			float z2 = findZFromY(AET[i + 1].point1, AET[i + 1].point2, scany);
			Color c1 = findColorFromY(AET[i].point1, AET[i].point2, scany);
			Color c2 = findColorFromY(AET[i+1].point1, AET[i+1].point2, scany);
			vNormal v1 = findVNFromYSlerp(AET[i].point1, AET[i].point2, scany);
			vNormal v2 = findVNFromYSlerp(AET[i+1].point1, AET[i+1].point2, scany);
			Line line = { {AET[i].interx,scany,z1,c1,v1},{AET[i + 1].interx,scany,z2,c2,v2} };
			line.savePoint(); //display_gragh(line.getPointSet());
			filledPoints.insert(filledPoints.end(), line.getPointSet().begin(), line.getPointSet().end());
		}
		for (Line& it : AET) {
			it.interx += it.m;
		} 
	}
	return filledPoints;
}







class Ellipse :public Gragh {
private:
	int a;
	int b;
public:
	Ellipse(Homo2D origin, int ai = 0, int bi = 0) :a(ai), b(bi), Gragh(origin) {};
	virtual void savePoint();
};

class Parabola :public Gragh {
private:
	int p;
	int border1, border2;
public:
	Parabola(Homo2D origin, int pi = 0, int b1 = 0, int b2 = 0) :p(pi), border1(b1), border2(b2), Gragh(origin) {};
	virtual void savePoint();
};

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
		newvn.normalize();
		if (steep) {
			addPoint({ y, x, depth,newColor,newvn });
		}
		else {
			addPoint({ x, y, depth,newColor,newvn });
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

void Ellipse::savePoint() {
	int x1, y1;
	x1 = a * a / sqrt(a * a + b * b);
	y1 = b * b / sqrt(a * a + b * b);
	int di;
	float xi, yir;
	xi = 0, yir = b;
	addPoint({ xi, yir });
	addPoint({ -xi, yir });
	addPoint({ xi, -yir });
	addPoint({ -xi, -yir });
	di = -a * a - 4 * b * b + a * a * b * 4;
	for (xi = 1; xi <= x1; xi++) {
		if (di >= 0) {
			yir = yir;
			di = di - 4 * b * b * (2 * xi + 2);
		}
		else {
			di = di - 4 * b * b * (2 * xi + 2) - 8 * a * a * (1 - yir);
			yir = yir - 1;
		}
		addPoint({ xi, yir });
		addPoint({ -xi, yir });
		addPoint({ xi, -yir });
		addPoint({ -xi, -yir });
	}
	float xir, yi;
	xir = a, yi = 0;
	addPoint({ xir, yi });
	addPoint({ -xir, yi });
	addPoint({ xir, -yi });
	addPoint({ -xir, -yi });
	di = a * b * b * 4 - 4 * a * a - b * b;
	for (yi = 1; yi <= y1; yi++) {
		if (di >= 0) {
			xir = xir;
			di = di - 8 * a * a - 8 * a * a * (yi);
		}
		else {
			di = di - 8 * a * a - 8 * a * a * (yi)+8 * b * b * (xir)-8 * b * b;
			xir = xir - 1;
		}
		addPoint({ xir, yi });
		addPoint({ -xir, yi });
		addPoint({ xir, -yi });
		addPoint({ -xir, -yi });
	}
}

void Parabola::savePoint() {
	int di;
	float xi, yir;
	int x1 = border1; int x2 = border2;
	xi = 0, yir = 0;
	addPoint({ xi, yir });
	addPoint({ -xi, yir });
	di = 1 - 2 * p;
	for (xi = 1; xi <= fmax(x1, x2); xi++) {
		if (di < 0) {
			yir = yir;
			di = di + 4 * xi + 2;
		}
		else {
			di = di + 4 * xi + 2 - 4 * p;
			yir = yir + 1;
		}
		if (xi >= p)break;
		addPoint({ xi, yir });
		addPoint({ -xi, yir });
	}
	float xir, yi;
	xir = p, yi = p / 2;
	if (xir <= fmax(x1, x2))
		addPoint({ xir, yi });
	di = 4 - 8 * p;
	for (yi; xir <= fmax(x1, x2); yi++) {
		if (di >= 0) {
			xir = xir;
			di = di - 8 * p;
		}
		else {
			di = di + 8 * xir + 8 - 8 * p;
			xir = xir + 1;
		}
		addPoint({ xir, yi });
		addPoint({ -xir, yi });
	}
}
