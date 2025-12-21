#pragma once
#include"constant.h"
#include<cmath>


template<typename T>
int countBlackNeighbors(const T buffer, int x, int y, int radius) {
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

void strengthenShadow(pixel_Dictionary& bufferData) {
	// 收集应该变成黑色而没有的点
	std::vector<std::pair<int,int>> pointsToBlacken;
	for(auto&pair:bufferData){
		int x1 =pair.first.first; int y1 = pair.first.second;
		if (get<0>(pair.second).inShadow == false) {
			bool verticalBlack = false;     //垂直方向
			bool horizontalBlack = false;    //水平

			// 检查垂直方向（上j到下j范围内是否有阴影点）
			float judgeZ[4] = { infinity,infinity,infinity,infinity };
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x1, y1 + j }) &&get<0>( bufferData[{ x1, y1 + j}]).inShadow==true) {
					judgeZ[0] = 1;
					break;
				}
			}
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x1, y1 - j }) && get<0>(bufferData[{ x1, y1 - j}]).inShadow == true) {
					judgeZ[1] = 1;
					break;
				}
			}
			verticalBlack = judgeZ[0]!= infinity&& judgeZ[1] != infinity;

			bool hasLeftBlack = false, hasRightBlack = false;
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x1 - j, y1 }) && get<0>(bufferData[{ x1-j, y1 }]).inShadow == true) {
					judgeZ[2] = 1;
					break;
				}
			}
			for (int j = 1; j <= BUFFERNUMBER; j++) {
				if (isKeyExists(bufferData, { x1 + j, y1 }) && get<0>(bufferData[{ x1 + j, y1 }]).inShadow == true) {
					judgeZ[3] =1;
					break;
				}
			}
			horizontalBlack = judgeZ[2] != infinity && judgeZ[3] != infinity;

			// 或者检查3×3邻域内的阴影点数量
			int neighborBlackCount = countBlackNeighbors(bufferData, x1, y1, BUFFERNUMBER);
			// 条件：垂直或水平都有黑点，且邻域内有足够多的阴影点
			if ((verticalBlack || horizontalBlack) && neighborBlackCount >= 2) {
				pointsToBlacken.push_back(pair.first);
			}
		}
	}

	// 批量更新为阴影
	for (const auto& i : pointsToBlacken) {
		get<0>(bufferData[i]).inShadow=true;

	}
}


void judgeInshadow(pixel_Dictionary& Point,Light* L,Buffer_Dictionary& shadow_real) {
	for (auto& pair : L->shadow_buffer) {

		get<0>(Point[get<1>(pair.second)]).inShadow = false;

	}
	strengthenShadow(Point);
	for (auto& pair2 : Point) {
		auto it = shadow_real.find(pair2.first);
		if (get<0>(pair2.second).inShadow == true) {
			if (it == shadow_real.end()) {
				shadow_real[pair2.first] = { 0,{0.3f,0.3f,0.3f} };
			}
		}
		else {
			if (it != shadow_real.end()) {
				get<1>(shadow_real[pair2.first]) /= SHADOWLOW;
			}
		}
	}
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
template<typename T>
Color getColorAt(const T& bufferData, int x, int y) {
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