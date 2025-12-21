#pragma once
#include"constant.h"

class Face {
private:
	Homo3D caculateOrigin();
public:
	vector<Homo3D> pointsetModel;
	vector<Homo2D>projectedPointset;
	Homo3D origin;
	Color color;
	bool isvisible;
	string materialName;
	Face() {
		color = WHITE;
		isvisible = true;
	}
	// 使用初始化列表的构造函数
	Face(const vector<Homo3D>& list, float R = 0.0f, float G = 0.0f, float B = 0.0f) : pointsetModel(list) {
		color={ R, G, B };
		origin = caculateOrigin();
		isvisible = true;
		materialName = "";
	}
	Face(const vector<Homo3D>& list,Color c) : pointsetModel(list),color(c) {
		origin = caculateOrigin();
		isvisible = true;
		materialName = "";
	}
	Face(const vector<Homo3D>& list, string c) : pointsetModel(list), materialName(c) {
		origin = caculateOrigin();
		isvisible = true;
	}
	Face(std::initializer_list<Homo3D> initList) : pointsetModel(initList) {
		origin = caculateOrigin();
		isvisible = true;
		materialName = "";
	}
	vector<Line> createLine(dimension di);
	void updateOrigin() { origin = caculateOrigin(); }
	Homo3D caculateNormal(){
		// 取面上三点计算法向量
		Homo3D p1 = pointsetModel[0];
		Homo3D p2 = pointsetModel[1];
		Homo3D p3 = pointsetModel[2];
		Homo3D v1 = p2 - p1;
		Homo3D v2 = p3 - p1;
		Homo3D norm = crossProduct(v1, v2);
		return norm.normalize();
	}

	void polygon_filtter(dimension di);
};



Homo3D Face::caculateOrigin() {
	Homo3D origin1;
	for (const Homo3D& it : pointsetModel) {
		origin1 += it;
	}
	return origin1 / pointsetModel.size();
}

vector<Line> Face::createLine(dimension di) {
	vector<Line> lineExcel;
	if (di==Object_2d) {
		for (int i = 0; i < projectedPointset.size(); i++) {
			Line lin({ projectedPointset[i] }, projectedPointset[(i + 1) % projectedPointset.size()]);
			lineExcel.push_back(lin);
		}
	}
	else if (di==Object_3d) {
		for (int i = 0; i < pointsetModel.size(); i++) {
			Line lin({ pointsetModel[i].x,pointsetModel[i].y }, { pointsetModel[(i + 1) % pointsetModel.size()].x,pointsetModel[(i + 1) % pointsetModel.size()].y });
			lineExcel.push_back(lin);
		}

	}
	return lineExcel;
}


Homo3D surround_box_Origin(vector<Face> obj)
{
	float x_MAX = -infinity, x_MIN = infinity;
	float y_MAX = -infinity, y_MIN = infinity;
	float z_MAX = -infinity, z_MIN = infinity;
	for (Face& ele : obj) {
		for (Homo3D &poi : ele.pointsetModel) {
			x_MAX = max(x_MAX, poi.x);
			x_MIN = min(x_MIN, poi.x);
			y_MAX = max(y_MAX, poi.y);
			y_MIN = min(y_MIN, poi.y);
			z_MAX = max(z_MAX, poi.z);
			z_MIN = min(z_MIN, poi.z);
		}
	}
	return Homo3D{ (x_MAX + x_MIN) / 2, (y_MAX + y_MIN) / 2 ,(z_MAX + z_MIN) / 2 };
}



//点的投影
Homo2D Homo3Projection(Homo3D H3, bool flag, Homo3D viewP) {
	vNormal v3 = H3.vn;
	UV vt3 = H3.vt;
	float depth = H3.z;
	Matrix4 per(project, viewP.x, viewP.y, viewP.z);
	Matrix4 zhe(mode::scale, 1, 1, 0);
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

	return { H3.x,H3.y ,depth,BLACK,v3,vt3 };
}


//投影函数
void projectface(Face& f,Camera realCamera) {
	vector<Homo2D> newface;
	Matrix4 viewMatrix = realCamera.calculateViewMatrix();
	for (auto& it : f.pointsetModel) {
		Homo3D viewSpacePoint = it * viewMatrix;
		viewSpacePoint.vn = it.vn;//viewP体现z值
		viewSpacePoint.vt = it.vt;
		newface.push_back(Homo3Projection(viewSpacePoint, realCamera.projectedMode, -realCamera.position));
	}
	f.projectedPointset = newface;
}


void Face::polygon_filtter(dimension di) {
	vector<Line>ET = createLine(di);
	vector<Homo2D>filledPoints;
	if (ET.size() < 3) {
		return;
	}
	vector<Line>AET; int ymax = ET[0].ymax, ymin = ET[0].ymin;
	for (Line it : ET) {
		it.savePoint();
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
			Homo2D p1 = findPfromY(AET[i].point1, AET[i].point2, scany, AET[i].interx);
			Homo2D p2 = findPfromY(AET[i + 1].point1, AET[i + 1].point2, scany, AET[i + 1].interx);
			Line line = { p1,p2 };
			line.savePoint(); //display_gragh(line.getPointSet());
			filledPoints.insert(filledPoints.end(), line.getPointSet().begin(), line.getPointSet().end());
		}
		for (Line& it : AET) {
			it.interx += it.m;
		}
	}
	if (di == Object_2d) {
		projectedPointset = filledPoints;
	}
	else {
		vector<Homo3D> temp = simple_2D_T0_3D(filledPoints);
		pointsetModel.insert(pointsetModel.end(), temp.begin(), temp.end()); 
	}
}

