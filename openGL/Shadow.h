#pragma once
#include"constant.h"
#include<cmath>

//阴影判定范围
#define BUFFERNUMBER 5

//z值判定范围
#define SHADOWBIAS 5.0f
#define SHADOWBIAS2 0.5f
//软阴影扩展范围
#define SHADOWEXTEND 3


#define SHADOWLOW 0.6f
#define SHADOWADD 1.0f

int countBlackNeighbors(const Buffer_Dictionary& buffer, int x, int y, int radius) {
	int count = 0;
	for (int dx = -radius; dx <= radius; dx++) {
		for (int dy = -radius; dy <= radius; dy++) {
			if (dx == 0 && dy == 0) continue; // 跳过自身

			auto key = std::make_pair(x + dx, y + dy);
			if (isKeyExists(buffer, key)) {
				count++;
			}
		}
	}
	return count;
}

void strengthenShadow(Buffer_Dictionary& bufferData, const pixel_Dictionary& OriginalbufferD) {
	// 收集应该变成黑色而没有的点
	std::vector<std::pair<int, int>> pointsToBlacken;
	for (const auto& pair : OriginalbufferD) {
		if (!isKeyExists(bufferData, pair.first)) {
			int x = pair.first.first;
			int y = pair.first.second;
			bool verticalBlack = false;     //垂直方向
			bool horizontalBlack = false;    //水平

			// 检查垂直方向（上j到下j范围内是否有阴影点）
			float judgeZ[4] = { infinity,infinity,infinity,infinity };
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x, y + j }) ) {
					judgeZ[0] = std::get<0>(bufferData.at({ x, y + j }));
					break;
				}
			}
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x, y - j }) ) {
					judgeZ[1] = std::get<0>(bufferData.at({ x, y - j }));
					break;
				}
			}
			verticalBlack = judgeZ[0]!= infinity&& judgeZ[1] != infinity&&fabs(judgeZ[1]-judgeZ[0])<SHADOWBIAS2;

			bool hasLeftBlack = false, hasRightBlack = false;
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x - j, y }) ) {
					judgeZ[2] = std::get<0>(bufferData.at({ x - j, y }));
					break;
				}
			}
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x + j, y })) {
					judgeZ[3] = std::get<0>(bufferData.at({ x + j, y }));
					break;
				}
			}
			horizontalBlack = judgeZ[2] != infinity && judgeZ[3] != infinity && fabs(judgeZ[2] - judgeZ[3]) < SHADOWBIAS2;

			// 或者检查3×3邻域内的阴影点数量
			int neighborBlackCount = countBlackNeighbors(bufferData, x, y, BUFFERNUMBER);
			// 条件：垂直或水平都有黑点，且邻域内有足够多的阴影点
			if ((verticalBlack || horizontalBlack) && neighborBlackCount >= 2) {
				pointsToBlacken.push_back({ x, y });
			}
		}
	}

	// 批量更新为阴影
	for (const auto& point : pointsToBlacken) {
		auto it = OriginalbufferD.find(point);
		if (it != OriginalbufferD.end()) {
			bufferData[point] = { 0,Color{0.3f,0.3f,0.3f} };
		}
	}
}
Face get_buffer(pixel_Dictionary& bufferData, Camera viewCamera, pointLight L, const vector<Face>& obj, bool projectedMode) {
	vector<Homo3D> realPointSets;
	Homo3D boxOrign = surround_box_Origin(obj);
	Camera L_c = { L.position,(L.position - boxOrign).normalize() };
	Matrix4 mainView = viewCamera.calculateViewMatrix();

	// 灯光视图矩阵
	Matrix4 lightView = L_c.calculateViewMatrix();

	// 复合矩阵：屏幕空间 → 主相机视图空间 → 世界空间 → 灯光视图空间
	Matrix4 viewTransform = mainView.Inverse() * lightView;

	Matrix4 fullTransform = viewTransform;


	for (const auto& pair : bufferData) {
		float x = static_cast<float>(pair.first.first);
		float y = static_cast<float>(pair.first.second);
		float z = get<0>(pair.second).depth;

		Homo3D p(x, y, z, 1.0);
		p = p * fullTransform;
		realPointSets.push_back(p);
	}	
	Face realPointSetsFace(realPointSets);	vector<Homo2D> newface;
	for (auto& it : realPointSetsFace.pointsetModel) {
		newface.push_back(Homo3Projection(it, projectedMode, -L_c.position));
	}
	realPointSetsFace.projectedPointset = newface;

	return realPointSetsFace;
}


void  shadow_Mapping(Face& realPoints,  const pixel_Dictionary& bufferData, Buffer_Dictionary& shadow_bufferData) {
	Index_Dictionary shadow_buffer;      //最靠近光源的
	vector<Homo2D>& model = realPoints.projectedPointset;
	for (int i = 0; i < model.size(); i++)
	{
		//光源视角下的二维坐标
		int x = round(model[i].x);
		int y = round(model[i].y);
		//该点对应的z_buffer数据
		auto bufferIt = bufferData.begin(); advance(bufferIt, i);
		//光源视角下的当前深度值
		float z = model[i].depth;
		//光源视角下该二维坐标的值最小键值对
		auto key = std::make_pair(x, y);
		auto it = shadow_buffer.find(key);

		if (it != shadow_buffer.end()) {
			//存在该坐标，此为记录的最小深度值
			float minDepth = model[it->second].depth;
			auto minBufferIt = bufferData.begin(); advance(minBufferIt, it->second);
			if (SHADOWBIAS + z < minDepth) {
				//如果更小则更新储存的索引
				shadow_buffer[key] = i;
				//将该点标记为阴影
				if (isKeyExists(shadow_bufferData, minBufferIt->first)) {
					get<1>(shadow_bufferData[minBufferIt->first]) *= 0.3f;
				}
				else {
					shadow_bufferData[minBufferIt->first] = std::make_tuple(z, Color{ 0.3f,0.3f,0.3f });
				}
			}
		}
		else {
			shadow_buffer[key] = i;
		}
	}
	strengthenShadow(shadow_bufferData, bufferData);
}


void shadow_Mapping_MultipyLight(const vector<Face>& obj, vector<pointLight> L, Camera viewCamera, bool projectedMode,pixel_Dictionary& bufferData, Buffer_Dictionary& shadow_bufferData) {
	vector<Buffer_Dictionary> MultiShadow_buffer;
	for (int i = 0; i < L.size(); i++) {
		Buffer_Dictionary Shadow_buffer;
		Face realPoints = get_buffer(bufferData,viewCamera, L[i],obj,projectedMode);
		shadow_Mapping( realPoints, bufferData, Shadow_buffer);
		MultiShadow_buffer.push_back(Shadow_buffer);
	}
	Buffer_Dictionary finalShadow_buffer= MultiShadow_buffer[0];
	for (int i = 1; i < MultiShadow_buffer.size(); i++) {
		for (auto& pair : MultiShadow_buffer[i]) {
			auto it = finalShadow_buffer.find(pair.first);
			if (it != finalShadow_buffer.end()) {
				get<1>(it->second) *= SHADOWADD;
			}
			else {
				finalShadow_buffer[pair.first] = pair.second;
				get<1>(finalShadow_buffer[pair.first]) /= SHADOWLOW;
			}
		}
	}
	for (auto& pair : finalShadow_buffer) {
		get<1>(pair.second) = clampColor(get<1>(pair.second));
	}
	shadow_bufferData = finalShadow_buffer;
}


int getMaxXFromData(
	const Buffer_Dictionary& bufferData) {

	if (bufferData.empty()) {
		return 0;
	}

	int maxX = bufferData.begin()->first.first;
	for (const auto& entry : bufferData) {
		const auto& coordinate = entry.first;
		int currentX = coordinate.first;

		if (currentX > maxX) {
			maxX = currentX;
		}
	}

	return maxX;
}

// 求所有坐标中x的最小值
int getMinXFromData(
	const Buffer_Dictionary& bufferData) {

	if (bufferData.empty()) {
		return 0;
	}

	int minX = bufferData.begin()->first.first;
	for (const auto& entry : bufferData) {
		const auto& coordinate = entry.first;
		int currentX = coordinate.first;

		if (currentX < minX) {
			minX = currentX;
		}
	}

	return minX;
}

// 求所有坐标中y的最大值
int getMaxYFromData(
	const Buffer_Dictionary& bufferData) {

	if (bufferData.empty()) {
		return 0;
	}

	int maxY = bufferData.begin()->first.second;
	for (const auto& entry : bufferData) {
		const auto& coordinate = entry.first;
		int currentY = coordinate.second;

		if (currentY > maxY) {
			maxY = currentY;
		}
	}

	return maxY;
}

// 求所有坐标中y的最小值
int getMinYFromData(
	const Buffer_Dictionary& bufferData) {

	if (bufferData.empty()) {
		return 0;
	}
	int minY = bufferData.begin()->first.second;
	for (const auto& entry : bufferData) {
		const auto& coordinate = entry.first;
		int currentY = coordinate.second;

		if (currentY < minY) {
			minY = currentY;
		}
	}

	return minY;
}

// 获取x,y键的颜色，如果未定义则返回白色
Color getColorAt(const Buffer_Dictionary& bufferData, int x, int y) {
	auto it = bufferData.find({ x, y });
	if (it != bufferData.end()) {
		return std::get<1>(it->second);
	}
	return WHITE;  // 默认白色
}


Buffer_Dictionary JuanJi_buffer(const Buffer_Dictionary& bufferData) {
	Buffer_Dictionary newBF= bufferData;

	for (int i = 0; i < SHADOWEXTEND;i++) {
	// 获取有定义的边界
		int maxX = getMaxXFromData(newBF);
		int minX = getMinXFromData(newBF);
		int maxY = getMaxYFromData(newBF);
		int minY = getMinYFromData(newBF);

		// 扩展，确保边界的软
		int extendedMinX = minX - 1;
		int extendedMaxX = maxX + 1;
		int extendedMinY = minY - 1;
		int extendedMaxY = maxY + 1;

		for (int i = extendedMinX; i <= extendedMaxX - 1; i++) {
			for (int j = extendedMinY; j <= extendedMaxY - 1; j++) {
				Color color1 = getColorAt(newBF, i, j);
				Color color2 = getColorAt(newBF, i + 1, j);
				Color color3 = getColorAt(newBF, i, j + 1);
				Color color4 = getColorAt(newBF, i + 1, j + 1);

				Color newColor;
				newColor =(color1+ color2+ color3+ color4) / 4;
				if (newColor != WHITE) {
				std::get<1>(newBF[{i, j}]) = newColor;
				}

			}
		}
	}

	return newBF;
}