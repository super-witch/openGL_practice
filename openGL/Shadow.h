#pragma once
#include"constant.h"
#include<cmath>

#define BUFFERADD 5
#define BUFFERNUMBER 4
#define SHADOWBIAS 5.0f


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

void strengthenShadow(Buffer_Dictionary& bufferData, const Buffer_Dictionary& OriginalbufferD) {
	// 收集应该变成黑色而没有的点
	std::vector<std::pair<int, int>> pointsToBlacken;
	for (const auto& pair : OriginalbufferD) {
		if (!isKeyExists(bufferData, pair.first)) {
			int x = pair.first.first;
			int y = pair.first.second;
			bool verticalBlack = false;     //垂直方向
			bool horizontalBlack = false;    //水平

			// 检查垂直方向（上j到下j范围内是否有阴影点）
			bool hasTopBlack = false, hasBottomBlack = false;
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x, y + j })) {
					hasTopBlack = true;
					break;
				}
			}
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x, y - j })) {
					hasBottomBlack = true;
					break;
				}
			}
			verticalBlack = hasTopBlack && hasBottomBlack;

			bool hasLeftBlack = false, hasRightBlack = false;
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x - j, y })) {
					hasLeftBlack = true;
					break;
				}
			}
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x + j, y })) {
					hasRightBlack = true;
					break;
				}
			}
			horizontalBlack = hasLeftBlack && hasRightBlack;

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






void  shadow_Mapping(const vector<Face>& obj, pointLight L, Face& realPoints, bool projectedMode, Buffer_Dictionary& bufferData, Buffer_Dictionary& shadow_bufferData) {
	Homo3D boxOrign = surround_box_Origin(obj);
	Camera L_c = { L.position,(L.position - boxOrign).normalize() };
	Index_Dictionary shadow_buffer;      //最靠近光源的
	//光源视角、投影
	projectface(realPoints, L_c, projectedMode);
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

		if (currentY > minY) {
			minY = currentY;
		}
	}

	return minY;
}

Buffer_Dictionary JuanJi_buffer(
	Buffer_Dictionary& bufferData) {
	Buffer_Dictionary newBF;
	newBF = bufferData;
	int maxX = getMaxXFromData(bufferData);
	int minX = getMinXFromData(bufferData);
	int maxY = getMaxYFromData(bufferData);
	int minY = getMinYFromData(bufferData);
	Color newColor;
	for (int i = minX; i < maxX; i = i + 2) {
		for (int j = minY; j < maxY; j = j + 2) {
			Color color1 = std::get<1>(bufferData[{i, j}]);
			Color color2 = std::get<1>(bufferData[{i + 1, j}]);
			Color color3 = std::get<1>(bufferData[{i, j + 1}]);
			Color color4 = std::get<1>(bufferData[{i + 1, j + 1}]);
			newColor.R = (color1.R + color2.R + color3.R + color4.R) / 3;
			newColor.G = (color1.G + color2.G + color3.G + color4.G) / 3;
			newColor.B = (color1.B + color2.B + color3.B + color4.B) / 3;
			std::get<1>(newBF[{i, j}]) = newColor;
			std::get<1>(newBF[{i + 1, j}]) = newColor;
			std::get<1>(newBF[{i, j + 1}]) = newColor;
			std::get<1>(newBF[{i + 1, j + 1}]) = newColor;
		}
	}
	return newBF;
}