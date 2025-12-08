#pragma once
#include"constant.h"
#include<cmath>

//阴影判定范围
#define BUFFERNUMBER 4

//z值判定范围
#define SHADOWBIAS 5.0f

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




void  shadow_Mapping(const vector<Face>& obj, pointLight L, Face& realPoints, bool projectedMode, const Buffer_Dictionary& bufferData, Buffer_Dictionary& shadow_bufferData) {
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


void shadow_Mapping_MultipyLight(const vector<Face>& obj, vector<pointLight> L, Face& realPoints, bool projectedMode,const Buffer_Dictionary& bufferData, Buffer_Dictionary& shadow_bufferData) {
	vector<Buffer_Dictionary> MultiShadow_buffer;
	for (int i = 0; i < L.size(); i++) {
		Buffer_Dictionary Shadow_buffer;
		shadow_Mapping(obj, L[i], realPoints, projectedMode, bufferData, Shadow_buffer);
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