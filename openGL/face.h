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
	vector<Line> createLine();
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
};



Homo3D Face::caculateOrigin() {
	Homo3D origin1;
	for (const Homo3D& it : pointsetModel) {
		origin1 += it;
	}
	return origin1 / pointsetModel.size();
}

vector<Line> Face::createLine() {
	vector<Line> lineExcel;
	if (projectedPointset.size() != 0) {
		for (int i = 0; i < projectedPointset.size(); i++) {
			Line lin({ projectedPointset[i] }, projectedPointset[(i + 1) % projectedPointset.size()]);
			lineExcel.push_back(lin);
		}
	}
	else if (pointsetModel.size() != 0 && isvisible) {
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

Face get_buffer(Buffer_Dictionary& bufferData, Camera viewCamera) {
	vector<Homo3D> realPointSets;
	Matrix4 viewMatrix = viewCamera.calculateViewMatrix().Inverse();
	for (const auto& pair : bufferData) {
		float x = static_cast<float>(pair.first.first);
		float y = static_cast<float>(pair.first.second);
		float z = std::get<0>(pair.second);

		Homo3D p(x, y, z, 1.0);
		p = p * viewMatrix;
		realPointSets.push_back(p);
	}
	return { realPointSets };
}


//投影函数
void projectface(Face& f,Camera realCamera,bool projectedMode) {
	vector<Homo2D> newface;
	Matrix4 viewMatrix = realCamera.calculateViewMatrix();
	for (auto& it : f.pointsetModel) {
		Homo3D viewSpacePoint = it * viewMatrix;
		viewSpacePoint.vn = it.vn;//viewP体现z值
		newface.push_back(Homo3Projection(viewSpacePoint, projectedMode, -realCamera.position));
	}
	f.projectedPointset = newface;
}