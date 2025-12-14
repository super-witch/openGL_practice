#pragma once
#include"constant.h"
#include<cmath>
#define mixPara 0.5f


typedef struct pointLight {
	Homo3D position;
	Color intensity;
	pointLight(Homo3D pos = { 0,0,0 }, Color inten = WHITE) :position(pos), intensity(inten) {};
}pointLight;



Color getColorFromImage(UV vt,LightSource Ls,string ImageName) {
	ImageDara kdTexture = Ls.ImageSets[ImageName];
	int u = vt.u * kdTexture.width;
	int v = vt.v * kdTexture.height;
	Color BaseColor = kdTexture.data[v * kdTexture.width + u];
	return BaseColor;
}



Color clampColor(const Color& color) {
	Color result;
	result.R = std::min(1.0f, std::max(0.0f, color.R));
	result.G = std::min(1.0f, std::max(0.0f, color.G));
	result.B = std::min(1.0f, std::max(0.0f, color.B));
	return result;
}

void setFaceColorAfterLight (Face& selected, Color en_light, vector<pointLight> LT, Camera camera, LightSource lightLource, bool key) {
	Color realColor = BLACK;

	for (const pointLight&light: LT) {
		material nowMaterial = lightLource.MatSets[selected.materialName];
		//光线方向
		Homo3D Ldirection = (light.position - selected.origin).normalize();

		//法线方向
		Homo3D Ndirection = selected.caculateNormal();
		//N·L
		float NdotL = std::max(0.0f, pointProduct(Ldirection, Ndirection));
		//反射光线方向
		Homo3D Outdirection = (Ndirection * (2.0f * NdotL) - Ldirection).normalize();
		//视线方向
		Homo3D cameraDir = key ? (camera.position - selected.origin).normalize() : -camera.direction.normalize();// 
		Color Lnen = en_light * nowMaterial.Ka;
		Color LnDn = light.intensity * nowMaterial.Kd * (NdotL);
		Color LnSpec = light.intensity * nowMaterial.Ks * powf(std::max(0.0f, pointProduct(Outdirection, cameraDir)), nowMaterial.Ns);
		realColor += clampColor(Lnen + LnDn + LnSpec);
	}
	float scalar = max({ realColor.R, realColor.G, realColor.B });
	if (scalar > 1.0) {
		realColor = realColor / scalar;
	}
	selected.color = realColor;
}

void setPointColorAfterLight(Homo2D& selected, string materialName,Color en_light,vector<pointLight> LT, Camera camera, LightSource lightLource, bool key) {
	Color realColor = BLACK;
	material nowMaterial = lightLource.MatSets[materialName];
	for (const pointLight& light: LT) {
		Homo3D point3DSe = { selected.x,selected.y,selected.depth };
		//光线方向
		Homo3D Ldirection = (light.position - point3DSe).normalize();

		//法线方向
		Homo3D Ndirection = { selected.vn.vx,selected.vn.vy,selected.vn.vz }; Ndirection = Ndirection.normalize();
		//N·L
		float NdotL = std::max(0.0f, pointProduct(Ldirection, Ndirection));
		//反射光线方向
		Homo3D Outdirection = (Ndirection * (2.0f * NdotL) - Ldirection).normalize();
		//视线方向
		Homo3D cameraDir = key ? (camera.position - point3DSe).normalize() : -camera.direction.normalize();// 
		Color Lnen = en_light * nowMaterial.Ka;
		Color LnDn = light.intensity * nowMaterial.Kd * (NdotL);       //漫反射
		Color LnSpec = light.intensity * nowMaterial.Ks * powf(std::max(0.0f, pointProduct(Outdirection, cameraDir)), nowMaterial.Ns);
		realColor += clampColor(Lnen + LnDn + LnSpec);
	}
	if (!nowMaterial.kdMapName.empty()) {
		Color BaseColor = getColorFromImage(selected.vt, lightLource, nowMaterial.kdMapName);
		realColor = realColor * mixPara + BaseColor * (1.0f - mixPara);
	}
	float scalar = max({ realColor.R, realColor.G, realColor.B });
	if (scalar > 1.0) {
		realColor = realColor / scalar;
	}
	selected.color = realColor;
}







void fillColor(pixel_Dictionary& bufferData,Buffer_Dictionary& Shadow_bufferData)
{
	for (const auto& pair : bufferData) {
		const auto& key = pair.first;
		const auto& value = pair.second;
		Color realColor=get<0>(value).color;
		auto it = Shadow_bufferData.find(key);
		if (it!= Shadow_bufferData.end()) {
			realColor *= get<1>(it->second);
		}
		glColor3f(realColor.R, realColor.G, realColor.B);
		glVertex2f(key.first, key.second);
	}
}

void fillColor(pixel_Dictionary& bufferData)
{
	for (const auto& pair : bufferData) {
		const auto& key = pair.first;
		const auto& value = pair.second;
		Color realColor;
		realColor =get<0>(value).color;
		glColor3f(realColor.R, realColor.G, realColor.B);
		glVertex2f(key.first, key.second);
	}
}