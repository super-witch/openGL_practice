#pragma once
#include"constant.h"
#include<cmath>
typedef struct pointLight {
	Homo3D position;
	Color intensity;
	pointLight(Homo3D pos = { 0,0,0 }, Color inten = WHITE) :position(pos), intensity(inten) {};
}pointLight;

Color clampColor(const Color& color) {
	Color result;
	result.R = std::min(1.0f, std::max(0.0f, color.R));
	result.G = std::min(1.0f, std::max(0.0f, color.G));
	result.B = std::min(1.0f, std::max(0.0f, color.B));
	return result;
}

void setFaceColorAfterLight (Face& selected, Color en_light, pointLight light, Camera camera, LightSource lightLource,bool key) {
	Color realColor;
	material nowMaterial = lightLource.MatSets[selected.materialName];
	//π‚œﬂ∑ΩœÚ
	Homo3D Ldirection = (light.position-selected.origin ).normalize();

	//∑®œﬂ∑ΩœÚ
	Homo3D Ndirection = selected.caculateNormal();
	//N°§L
	float NdotL = std::max(0.0f,pointProduct(Ldirection, Ndirection));
	//∑¥…‰π‚œﬂ∑ΩœÚ
	Homo3D Outdirection = (Ndirection * (2.0f * NdotL) - Ldirection).normalize();
	// ”œﬂ∑ΩœÚ
	Homo3D cameraDir =key? (camera.position-selected.origin ).normalize():-camera.direction.normalize();// 
	Color Lnen = en_light * nowMaterial.Ka;
	Color LnDn = light.intensity * nowMaterial.Kd * (NdotL);
	Color LnSpec = light.intensity * nowMaterial.Ks * powf(std::max(0.0f,pointProduct(Outdirection,cameraDir)), nowMaterial.Ns);
	realColor = clampColor(Lnen + LnDn + LnSpec);
	selected.color = realColor;
}

void setPointColorAfterLight(Homo2D& selected, string materialName,Color en_light, pointLight light, Camera camera, LightSource lightLource, bool key) {
	Color realColor;
	material nowMaterial = lightLource.MatSets[materialName];
	Homo3D point3DSe = { selected.x,selected.y,selected.depth };
	//π‚œﬂ∑ΩœÚ
	Homo3D Ldirection = (light.position - point3DSe).normalize();

	//∑®œﬂ∑ΩœÚ
	Homo3D Ndirection = { selected.vn.vx,selected.vn.vy,selected.vn.vz }; Ndirection = Ndirection.normalize();
	//N°§L
	float NdotL = std::max(0.0f, pointProduct(Ldirection, Ndirection));
	//∑¥…‰π‚œﬂ∑ΩœÚ
	Homo3D Outdirection = (Ndirection * (2.0f * NdotL) - Ldirection).normalize();
	// ”œﬂ∑ΩœÚ
	Homo3D cameraDir = key ? (camera.position - point3DSe).normalize() : -camera.direction.normalize();// 
	Color Lnen = en_light * nowMaterial.Ka;
	Color LnDn = light.intensity * nowMaterial.Kd * (NdotL);
	Color LnSpec = light.intensity * nowMaterial.Ks * powf(std::max(0.0f, pointProduct(Outdirection, cameraDir)), nowMaterial.Ns);
	realColor = clampColor(Lnen + LnDn + LnSpec);
	selected.color = realColor;
}

#define BUFFERADD 5
#define BUFFERNUMBER 4
#define SHADOWBIAS 5.0f


bool isKeyExistsAndColorBlack(const Buffer_Dictionary& buffer, const std::pair<int, int>& key) {
	auto it = buffer.find(key);
	if (it != buffer.end()) {
		const Color& color = std::get<1>(it->second); // Ëé∑ÂèñColorÈÉ®ÂàÜ
		return color == BLACK;
	}
	return false;
}

int countBlackNeighbors(const Buffer_Dictionary& buffer, int x, int y, int radius) {
	int count = 0;
	for (int dx = -radius; dx <= radius; dx++) {
		for (int dy = -radius; dy <= radius; dy++) {
			if (dx == 0 && dy == 0) continue; // Ë∑≥ËøáËá™Ë∫´

			auto key = std::make_pair(x + dx, y + dy);
			if (isKeyExistsAndColorBlack(buffer, key)) {
				count++;
			}
		}
	}
	return count;
}

	void strengthenShadow(Buffer_Dictionary& bufferData) {
		// Êî∂ÈõÜÂ∫îËØ•ÂèòÊàêÈªëËâ≤ËÄåÊ≤°ÊúâÁöÑÁÇπ
		std::vector<std::pair<int, int>> pointsToBlacken;
		for (const auto& pair : bufferData) {
			if (get<1>(pair.second) != BLACK) {
				int x = pair.first.first;
				int y = pair.first.second;
				bool verticalBlack = false;     //ÂûÇÁõ¥ÊñπÂêë
				bool horizontalBlack = false;    //Ê∞¥Âπ≥

				// Ê£ÄÊü•ÂûÇÁõ¥ÊñπÂêëÔºà‰∏äjÂà∞‰∏ãjËåÉÂõ¥ÂÜÖÊòØÂê¶ÊúâÈªëÁÇπÔºâ
				bool hasTopBlack = false, hasBottomBlack = false;
				for (int j = 1; j <= BUFFERNUMBER; j++) {
					if (isKeyExistsAndColorBlack(bufferData, { x, y + j })) {
						hasTopBlack = true;
						break;
					}
				}
				for (int j = 1; j <= BUFFERNUMBER; j++) {
					if (isKeyExistsAndColorBlack(bufferData, { x, y - j })) {
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
					if (isKeyExistsAndColorBlack(bufferData, { x + j, y })) {
						hasRightBlack = true;
						break;
					}
				}
				horizontalBlack = hasLeftBlack && hasRightBlack;

				// ÊàñËÄÖÊ£ÄÊü•3√ó3ÈÇªÂüüÂÜÖÁöÑÈªëÁÇπÊï∞Èáè
				int neighborBlackCount = countBlackNeighbors(bufferData, x, y, BUFFERNUMBER);
				// Êù°‰ª∂ÔºöÂûÇÁõ¥ÊàñÊ∞¥Âπ≥ÈÉΩÊúâÈªëÁÇπÔºå‰∏îÈÇªÂüüÂÜÖÊúâË∂≥Â§üÂ§öÁöÑÈªëÁÇπ
				if ((verticalBlack || horizontalBlack) && neighborBlackCount >= 2) {
					pointsToBlacken.push_back({ x, y });
				}
			}
		}

		// ÊâπÈáèÊõ¥Êñ∞‰∏∫ÈªëËâ≤
		for (const auto& point : pointsToBlacken) {
			auto it = bufferData.find(point);
			if (it != bufferData.end()) {
				std::get<1>(it->second) = BLACK;
			}
		}
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

void  shadow_Mapping(const vector<Face>& obj, pointLight L, Face& realPoints,bool projectedMode,Buffer_Dictionary& bufferData) {
	Homo3D boxOrign = surround_box_Origin(obj); 
	Camera L_c = { L.position,(L.position-boxOrign  ).normalize() };
	Index_Dictionary shadow_buffer; 
	//π‚‘¥ ”Ω«°¢Õ∂”∞
	projectface(realPoints,L_c, projectedMode);
	vector<Homo2D>& model = realPoints.projectedPointset;
	for (int i = 0; i < model.size(); i++)
	{
		//π‚‘¥ ”Ω«œ¬µƒ∂˛Œ¨◊¯±Í
		int x = round(model[i].x) ;
		int y = round(model[i].y) ;
		//∏√µ„∂‘”¶µƒz_buffer ˝æ›
		auto bufferIt =bufferData.begin(); advance(bufferIt, i);
		//π‚‘¥ ”Ω«œ¬µƒµ±«∞…Ó∂»÷µ
		float z = model[i].depth;
		//π‚‘¥ ”Ω«œ¬∏√∂˛Œ¨◊¯±Íµƒ÷µ◊Ó–°º¸÷µ∂‘
		auto key = std::make_pair(x, y);
		auto it = shadow_buffer.find(key);

		if (it != shadow_buffer.end()) {
			//¥Ê‘⁄∏√◊¯±Í£¨¥ÀŒ™º«¬ºµƒ◊Ó–°…Ó∂»÷µ
			float minDepth = model[it->second].depth;
			auto minBufferIt = bufferData.begin(); advance(minBufferIt, it->second);
			if (SHADOWBIAS+z < minDepth) {
				//»Áπ˚∏¸–°‘Ú∏¸–¬¥¢¥ÊµƒÀ˜“˝
				shadow_buffer[key] = i;
				//≤¢∏¸–¬∂˛’ﬂµƒ—’…´
				//std::get<1>(bufferIt->second) = std::get<1>(bufferIt->second) / 0.3f;
				std::get<1>(minBufferIt->second) = std::get<1>(minBufferIt->second) * 0.3f;
			}	
		}
		else {
			shadow_buffer[key] = i;
		}
	}
}



void fillColor(Buffer_Dictionary& bufferData,Buffer_Dictionary& Shadow_bufferData)
{
	for (const auto& pair : bufferData) {
		const auto& key = pair.first;
		const auto& value = pair.second;
		Color realColor=std::get<1>(value);
		auto it = Shadow_bufferData.find(key);
		if (it!= Shadow_bufferData.end()) {
			realColor *= get<1>(it->second);
		}
		glColor3f(realColor.R, realColor.G, realColor.B);
		glVertex2f(key.first, key.second);
	}
}

void fillColor(Buffer_Dictionary& bufferData)
{
	for (const auto& pair : bufferData) {
		const auto& key = pair.first;
		const auto& value = pair.second;
		Color realColor;
		realColor = std::get<1>(value);
		glColor3f(realColor.R, realColor.G, realColor.B);
		glVertex2f(key.first, key.second);
	}
}