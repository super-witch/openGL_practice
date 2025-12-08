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
			if (dx == 0 && dy == 0) continue; // Ìø¹ı×ÔÉí

			auto key = std::make_pair(x + dx, y + dy);
			if (isKeyExists(buffer, key)) {
				count++;
			}
		}
	}
	return count;
}

void strengthenShadow(Buffer_Dictionary& bufferData, const Buffer_Dictionary& OriginalbufferD) {
	// ÊÕ¼¯Ó¦¸Ã±ä³ÉºÚÉ«¶øÃ»ÓĞµÄµã
	std::vector<std::pair<int, int>> pointsToBlacken;
	for (const auto& pair : OriginalbufferD) {
		if (!isKeyExists(bufferData, pair.first)) {
			int x = pair.first.first;
			int y = pair.first.second;
			bool verticalBlack = false;     //´¹Ö±·½Ïò
			bool horizontalBlack = false;    //Ë®Æ½

			// ¼ì²é´¹Ö±·½Ïò£¨ÉÏjµ½ÏÂj·¶Î§ÄÚÊÇ·ñÓĞÒõÓ°µã£©
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

			// »òÕß¼ì²é3¡Á3ÁÚÓòÄÚµÄÒõÓ°µãÊıÁ¿
			int neighborBlackCount = countBlackNeighbors(bufferData, x, y, BUFFERNUMBER);
			// Ìõ¼ş£º´¹Ö±»òË®Æ½¶¼ÓĞºÚµã£¬ÇÒÁÚÓòÄÚÓĞ×ã¹»¶àµÄÒõÓ°µã
			if ((verticalBlack || horizontalBlack) && neighborBlackCount >= 2) {
				pointsToBlacken.push_back({ x, y });
			}
		}
	}

	// ÅúÁ¿¸üĞÂÎªÒõÓ°
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
	Index_Dictionary shadow_buffer;      //×î¿¿½ü¹âÔ´µÄ
	//¹âÔ´ÊÓ½Ç¡¢Í¶Ó°
	projectface(realPoints, L_c, projectedMode);
	vector<Homo2D>& model = realPoints.projectedPointset;
	for (int i = 0; i < model.size(); i++)
	{
		//¹âÔ´ÊÓ½ÇÏÂµÄ¶şÎ¬×ø±ê
		int x = round(model[i].x);
		int y = round(model[i].y);
		//¸Ãµã¶ÔÓ¦µÄz_bufferÊı¾İ
		auto bufferIt = bufferData.begin(); advance(bufferIt, i);
		//¹âÔ´ÊÓ½ÇÏÂµÄµ±Ç°Éî¶ÈÖµ
		float z = model[i].depth;
		//¹âÔ´ÊÓ½ÇÏÂ¸Ã¶şÎ¬×ø±êµÄÖµ×îĞ¡¼üÖµ¶Ô
		auto key = std::make_pair(x, y);
		auto it = shadow_buffer.find(key);

		if (it != shadow_buffer.end()) {
			//´æÔÚ¸Ã×ø±ê£¬´ËÎª¼ÇÂ¼µÄ×îĞ¡Éî¶ÈÖµ
			float minDepth = model[it->second].depth;
			auto minBufferIt = bufferData.begin(); advance(minBufferIt, it->second);
			if (SHADOWBIAS + z < minDepth) {
				//Èç¹û¸üĞ¡Ôò¸üĞÂ´¢´æµÄË÷Òı
				shadow_buffer[key] = i;
				//½«¸Ãµã±ê¼ÇÎªÒõÓ°
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

// ÇóËùÓĞ×ø±êÖĞxµÄ×îĞ¡Öµ
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

// ÇóËùÓĞ×ø±êÖĞyµÄ×î´óÖµ
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

// ÇóËùÓĞ×ø±êÖĞyµÄ×îĞ¡Öµ
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

// è·å–x,yé”®çš„é¢œè‰²ï¼Œå¦‚æœæœªå®šä¹‰åˆ™è¿”å›ç™½è‰²
Color getColorAt(const Buffer_Dictionary& bufferData, int x, int y) {
	auto it = bufferData.find({ x, y });
	if (it != bufferData.end()) {
		return std::get<1>(it->second);
	}
	return WHITE;  // é»˜è®¤ç™½è‰²
}


Buffer_Dictionary JuanJi_buffer(const Buffer_Dictionary& bufferData) {
	Buffer_Dictionary newBF= bufferData;

	for (int i = 0; i < SHADOWEXTEND;i++) {
	// è·å–æœ‰å®šä¹‰çš„è¾¹ç•Œ
		int maxX = getMaxXFromData(newBF);
		int minX = getMinXFromData(newBF);
		int maxY = getMaxYFromData(newBF);
		int minY = getMinYFromData(newBF);

		// æ‰©å±•ï¼Œç¡®ä¿è¾¹ç•Œçš„è½¯
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
				}

			}
		}
	}
	return newBF;
}