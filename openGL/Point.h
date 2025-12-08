#pragma once
#include"constant.h"


struct vNormal {
	float vx;
	float vy;
	float vz;
	vNormal(float x = 0, float y = 0, float z = 0) : vx(x), vy(y), vz(z) {};
	vNormal operator*(float t);
	vNormal operator/(float t);
	vNormal operator+(vNormal vnn);
	vNormal operator-(vNormal vnn);
	void normalize() {
		float length = sqrt(vx * vx + vy *vy + vz * vz);
		if (length > 1e-8f) {
			vx /= length;
			vy /= length;
			vz /= length;
		}
	};

};
vNormal vNormal::operator*(float t) {
	return { vx * t,vy * t,vz * t };
}
vNormal vNormal::operator+(vNormal vnn) {
	return{ vx + vnn.vx,vy + vnn.vy,vz + vnn.vz };
}
vNormal vNormal::operator-(vNormal vnn) {
	return{ vx - vnn.vx,vy - vnn.vy,vz - vnn.vz };
}
vNormal vNormal::operator/(float t) {
	return { vx / t,vy / t,vz / t };
}


struct Homo2D {
	float x;
	float y;
	float depth;
	Color color;
	vNormal vn;
	Homo2D(float x = 0, float y = 0, float de = 0, Color co = BLACK, vNormal v = {0,0,0}) : x(x), y(y), depth(de), color(co),vn(v) {
	};
	void setColor(Color c) {
		color = c;
	};
	float magnitude() {
		return sqrt(x * x + y * y);
	};
	Homo2D normalize(){
		float length = magnitude();
		if (length == 0) return { 0, 0 };
		return { x / length,y / length };
	};
};
typedef struct Homo2D Homo2D;




class Homo3D {
public:
	float x;
	float y;
	float z;
	float w;
	vNormal vn;
	Homo3D(float x = 0, float y = 0, float z = 0, float w = 1, vNormal v = {0,0,0}) : x(x), y(y), z(z), w(w), vn(v) {};
	float magnitude() {
		return sqrt(x * x + y * y + z * z);
	}
	Homo3D normalize() {
		float length = magnitude();
		if (length == 0) return { 0, 0, 0, w };
		return { x / length,y / length, z / length, w };
	}
	Homo3D operator+(const Homo3D& other) const {
		return Homo3D(x + other.x, y + other.y, z + other.z, w, vn);
	}
	Homo3D operator+(const float& other) const {
		return Homo3D(x + other, y + other, z + other, w, vn);
	}
	// 复合加法赋值
	Homo3D& operator+=(const Homo3D& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	Homo3D& operator+=(const float& other) {
		x += other;
		y += other;
		z += other;
		return *this;
	}
	// 向量减法
	Homo3D operator-(const Homo3D& other) const {
		return Homo3D(x - other.x, y - other.y, z - other.z, w, vn);
	}

	// 复合减法赋值
	Homo3D& operator-=(const Homo3D& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	// 一元负号
	Homo3D operator-() const {
		return Homo3D(-x, -y, -z);
	}
	Homo3D operator*(float scalar) const {
		return Homo3D(x * scalar, y * scalar, z * scalar,w,vn);
	}
	//矩阵在左，向量在右
	Homo3D operator*(Matrix4 matrix) const {
		Homo3D result;
		result.x = matrix.m[0][0] * x + matrix.m[0][1] * y + matrix.m[0][2] * z + matrix.m[0][3] * w;
		result.y = matrix.m[1][0] * x + matrix.m[1][1] * y + matrix.m[1][2] * z + matrix.m[1][3] * w;
		result.z = matrix.m[2][0] * x + matrix.m[2][1] * y + matrix.m[2][2] * z + matrix.m[2][3] * w;
		result.w = matrix.m[3][0] * x + matrix.m[3][1] * y + matrix.m[3][2] * z + matrix.m[3][3] * w;
		result.vn = vn;
		return  result;
	}
	Homo3D& operator*=(float scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	// 标量除法
	Homo3D operator/(float scalar) const {
		if (scalar == 0) {
			// 处理除零错误，可以抛出异常或返回零向量
			return Homo3D(0, 0, 0, 1);
		}
		return Homo3D(x / scalar, y / scalar, z / scalar);
	}
	Homo3D& operator/=(float scalar) {
		if (scalar == 0) {
			// 处理除零错误
			x = y = z = 0;
		}
		else {
			x /= scalar;
			y /= scalar;
			z /= scalar;
		}
		return *this;
	}
};



//3d向量叉乘
Homo3D crossProduct(Homo3D u, Homo3D v)
{
	Homo3D normal;
	normal.x = u.y * v.z - u.z * v.y;
	normal.y = u.z * v.x - u.x * v.z;
	normal.z = u.x * v.y - u.y * v.x;
	return normal;
}
//向量点乘
float pointProduct(Homo3D a, Homo3D b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

float pointProduct(Homo3D a, vNormal b)
{
	return (a.x * b.vx + a.y * b.vy + a.z * b.vz);
}




//点的投影
Homo2D Homo3Projection(Homo3D H3, bool flag, Homo3D viewP) {
	vNormal v3 = H3.vn;
	Matrix4 per(project, viewP.x, viewP.y, viewP.z);
	Matrix4 zhe(mode::rotate, 1, 1, 0);
	if (flag) {
		H3 = H3 * per;
		if (H3.w != 0.0f) {
			H3 /= H3.w;
			H3.w = 1;
		}
	}
	else {
		H3 = H3 * zhe;
	}

	return { H3.x,H3.y ,H3.z,BLACK,v3};
}

Homo3D calculateRotationAngles(Homo3D homo) {
	Homo3D angle;
	float magnitude = homo.magnitude();
	if (magnitude == 0.0f) {
		angle = 0.0f;
		return angle;
	}

	// 计算与z轴的夹角
	float theta_z = acos(homo.z / magnitude);

	// 计算在xy平面上的投影
	float proj_xy = sqrt(homo.x * homo.x + homo.y * homo.y);

	if (proj_xy < 1e-6f) {
		// 向量已经在z轴上
		angle.z = 0.0f;
		angle.y = (homo.z >= 0) ? 0.0f : pai;
	}
	else {
		// 计算绕z轴旋转的角度（使向量落在xz平面）
		angle.z = -atan2(homo.y, homo.x);
		// 计算绕y轴旋转的角度
		angle.y = (homo.z >= 0) ? theta_z : pai - theta_z;
	}

	return angle;
}





float findZFromY(Homo2D p1, Homo2D p2, float y0) {

	// 检查直线是否垂直（y方向无变化）
	if (fabs(p2.y - p1.y) < 1e-6f) {
		if (fabs(y0 - p1.y) < 1e-6f) {
			// 如果y0等于端点y值，返回z1（或平均值）
			return (p1.depth + p2.depth) / 2.0f;
		}
		else {
			// 该y值不在直线上
			return NAN; // 或抛出异常
		}
	}

	// 计算参数t
	float t = (y0 - p1.y) / (p2.y - p1.y);

	// 线性插值计算z值
	float z0 = p1.depth + t * (p2.depth - p1.depth);

	return z0;
}


Color findColorFromY(Homo2D p1, Homo2D p2, float y0) {

	// 检查直线是否垂直（y方向无变化）
	if (fabs(p2.y - p1.y) < 1e-6f) {
		if (fabs(y0 - p1.y) < 1e-6f) {
			// 如果y0等于端点y值，返回z1（或平均值）
			return (p1.color + p2.color) / 2.0f;
		}
		else {
			// 该y值不在直线上
			return NAN; // 或抛出异常
		}
	}

	// 计算参数t
	float t = (y0 - p1.y) / (p2.y - p1.y);

	// 线性插值计算z值
	Color z0 = p1.color + t * (p2.color +p1.color*-1);

	return z0;
}

vNormal findVNFromY(Homo2D p1, Homo2D p2, float y0) {

	// 检查直线是否垂直（y方向无变化）
	if (fabs(p2.y - p1.y) < 1e-6f) {
		if (fabs(y0 - p1.y) < 1e-6f) {
			// 如果y0等于端点y值，返回z1（或平均值）
			return (p1.vn + p2.vn) / 2.0f;
		}
		else {
			// 该y值不在直线上
			return NAN; // 或抛出异常
		}
	}

	// 计算参数t
	float t = (y0 - p1.y) / (p2.y - p1.y);

	// 线性插值计算z值
	vNormal z0 = p1.vn + (p2.vn - p1.vn) * t;

	return z0;
}


// 球形线性插值实现
vNormal slerp(vNormal a, vNormal b, float t) {
	// 计算点积和夹角
	float dot = a.vx * b.vx + a.vy * b.vy + a.vz * b.vz;
	dot = std::max(-1.0f, std::min(1.0f, dot)); // 夹紧

	float theta = acos(dot) * t;
	vNormal relative = {
		b.vx - a.vx * dot,
		b.vy - a.vy * dot,
		b.vz - a.vz * dot
	};

	relative.normalize();
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);
	// 计算插值结果
	vNormal result = {
		a.vx * cosTheta + relative.vx * sinTheta,
		a.vy * cosTheta + relative.vy * sinTheta,
		a.vz * cosTheta + relative.vz * sinTheta
	};

	return result;
}
vNormal findVNFromYSlerp(Homo2D p1, Homo2D p2, float y0) {
	// 检查直线是否垂直
	if (fabs(p2.y - p1.y) < 1e-6f) {
		if (fabs(y0 - p1.y) < 1e-6f) {
			// 使用Slerp在垂直情况下
			return slerp(p1.vn, p2.vn, 0.5f);
		}
		return { 0, 0, 1 }; // 默认法线
	}

	float t = (y0 - p1.y) / (p2.y - p1.y);
	t = std::max(0.0f, std::min(1.0f, t));

	// 球形线性插值
	return slerp(p1.vn, p2.vn, t);
}
