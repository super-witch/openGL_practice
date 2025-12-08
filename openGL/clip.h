#pragma once
#include"constant.h"
//判断点在裁剪边里面还是外面

bool inside(Homo2D poi, Line lin)
{
	Homo2D p1 = lin.point1, p2 = lin.point2;
	Homo2D v1 = { poi.x - p1.x,poi.y - p1.y };
	Homo2D v2 = { poi.x - p2.x,poi.y - p2.y };
	float res = v1.x * v2.y - v1.y * v2.x;
	if (res > 0) return true;
	else return false;
}
//交点的颜色也要计算
//计算一条线段和裁剪边的交点
Homo2D intersection(Line lin, Homo2D en, Homo2D ou)
{
	Homo2D p1 = lin.point1, p2 = lin.point2;
	float ABx = p2.x - p1.x, ABy = p2.y - p1.y;
	float CDx = ou.x - en.x, CDy = ou.y - en.y;
	float ACx = en.x - p1.x, ACy = en.y - p1.y;
	float det = ABx * CDy - ABy * CDx;
	float t = (ACx * CDy - ACy * CDx) / det;
	Color color = en.color * (1 - t) + ou.color * t;
	float depth = en.depth * (1 - t) + ou.depth * t;

	// 插值法线
	vNormal normal;
	normal.vx = en.vn.vx * (1 - t) + ou.vn.vx * t;
	normal.vy = en.vn.vy * (1 - t) + ou.vn.vy * t;
	normal.vz = en.vn.vz * (1 - t) + ou.vn.vz * t;

	return { p1.x + t * ABx, p1.y + t * ABy ,depth,color,normal};
}
//裁剪
void  clip(Face& obj, Line bian)
{
	Face ele = obj;
	obj.projectedPointset.clear();
	for (int i = 0; i < ele.projectedPointset.size(); i++) {
		bool enter_poi = inside(ele.projectedPointset[i], bian);
		bool out_poi = inside(ele.projectedPointset[(i + 1) % ele.projectedPointset.size()], bian);
		if (enter_poi && out_poi) {
			obj.projectedPointset.push_back(ele.projectedPointset[(i + 1) % ele.projectedPointset.size()]);
		}
		else if (!enter_poi && out_poi) {
			Homo2D Interse = intersection(bian, ele.projectedPointset[i], ele.projectedPointset[(i + 1) % ele.projectedPointset.size()]);
			obj.projectedPointset.push_back(Interse);
			obj.projectedPointset.push_back(ele.projectedPointset[(i + 1) % ele.projectedPointset.size()]);
		}
		else if (enter_poi && !out_poi) {
			Homo2D Interse = intersection(bian, ele.projectedPointset[i], ele.projectedPointset[(i + 1) % ele.projectedPointset.size()]);
			obj.projectedPointset.push_back(Interse);
		}
	}
	if (obj.projectedPointset.size() == 0) {
		obj.isvisible = false;
	}
}
////////////////消隐////////////////
//dictionary tool
bool isKeyExists(const Buffer_Dictionary& buffer, const std::pair<int, int>& key) {
	auto it = buffer.find(key);
	if (it != buffer.end()) {
		return true;
	}
	return false;
}

float getMinZFromData(
	const std::map<std::pair<int, int>, std::tuple<float, Color>>& bufferData) {

	if (bufferData.empty()) {
		return 0.0f;
	}

	float minVal = std::numeric_limits<float>::max();
	for (const auto&pair : bufferData) {
		float currentVal = std::get<0>(pair.second);
		if (currentVal < minVal) {
			minVal = currentVal;
		}
	}

	return minVal;
}

float getMaxZFromData(
	const std::map<std::pair<int, int>, std::tuple<float, Color>>& bufferData) {

	if (bufferData.empty()) {
		return 0.0f;
	}

	float maxVal = std::numeric_limits<float>::lowest();

	for (const auto& pair : bufferData) {
		float currentVal = std::get<0>(pair.second);
		if (currentVal > maxVal) {
			maxVal = currentVal;
		}
	}

	return maxVal;
}

//背面剔除
bool back_face_culling(Camera camera, Face& face,bool key)
{
	face.updateOrigin();
	Homo3D norm = face.caculateNormal();
	Homo3D eyedirection;
	if (key) {
		// 透视投影
		eyedirection = face.origin - camera.position;
	}
	else {
		// 正交投影
		eyedirection = camera.direction;
	}

	// 标准化视线方向
	eyedirection = eyedirection.normalize();

	// 计算点积
	float prod = pointProduct(norm, eyedirection);
	return prod > 0.1f;

}

void color_buffer1(const vector<Homo2D>model, std::map<std::pair<int, int>, std::tuple<float,Color>>& bufferData)
{	 //左下0,0点
	for (int i = 0; i < model.size(); i++)
	{
		int x = model[i].x;
		int y = model[i].y;
		float z = model[i].depth;
		auto key = std::make_pair(x, y);
		auto it = bufferData.find(key);
		if (it != bufferData.end()) {
			if (z > get<0>(bufferData[key]))
			{
				std::get<0>(it->second) = z;        // 修改深度
				std::get<1>(it->second) = model[i].color;       // 修改颜色
			}
		}
		else {
			bufferData[key] = std::make_tuple(z, model[i].color);
		}
	}

}


