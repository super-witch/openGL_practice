#pragma once
#include"constant.h"
#include<cmath>
#define mixPara 0.0f

//阴影判定范围
#define BUFFERNUMBER 2

//z值判定范围
#define SHADOWBIAS 5.0f

//软阴影扩展范围
#define SHADOWEXTEND 3


#define SHADOWLOW 0.6f
#define SHADOWADD 1.0f

class Light {
public:
	Homo3D position;
	Color intensity;
	Index_Dictionary shadow_buffer;
	Light(Homo3D pos = { 0,0,0 }, Color inten = WHITE) :position(pos), intensity(inten) {};
	virtual void updateShadow_buffer(vector<Homo2D>& model, Camera mainCamera,Homo3D dir) = 0;
	virtual ~Light() = default;
};

//无衰减指向场景中心的光 
class pointLight:public Light {
public:
	pointLight(Homo3D pos = { 0,0,0 }, Color inten = WHITE) :Light(pos, inten) {};
	void updateShadow_buffer(vector<Homo2D>& model, Camera mainCamera,Homo3D dir);
};



void pointLight::updateShadow_buffer(vector<Homo2D>&model,Camera mainCamera, Homo3D dir) {
	Camera L_c = { position,(position - dir).normalize() };
	Matrix4 viewMatrix = mainCamera.calculateViewMatrix().Inverse()*L_c.calculateViewMatrix();
	for (int i = 0; i < model.size(); i++) {
		Homo3D p = { model[i].x,model[i].y,model[i].depth };
		p = p * viewMatrix;
		Homo2D projectedP = Homo3Projection(p, L_c.projectedMode, -L_c.position);

		int x_shadow = round(projectedP.x); int y_shadow = round(projectedP.y);
		auto key= std::make_pair(x_shadow, y_shadow);
		int x = round(model[i].x);
		int y = round(model[i].y);
		auto pos = std::make_pair(x, y);

		//光源视角下的当前深度值
		float z = projectedP.depth;

		//查找当前二维坐标是否有前人
		auto it = shadow_buffer.find(key);

		//如果存在则比较二者深度值，不存在则加入
		if (it != shadow_buffer.end()) {
			float minDepth = get<0>(it->second);//储存的最小深度值
			if ( z < minDepth+SHADOWBIAS) {
				//如果更小则更新shadow_bufferDataNear储存的深度
				shadow_buffer[key] = { z,pos};

			}
		}
		else {
			shadow_buffer[key] = { z,pos };  //没有则添加    //存储世界坐标
		}
	}
}



Color getColorFromImage(UV vt,LightSource Ls,string ImageName) {
	ImageDara kdTexture = Ls.ImageSets[ImageName];
	int u =vt.u * kdTexture.width;
	int v =(1-vt.v) * kdTexture.height;
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

void setFaceColorAfterLight (Face& selected, Color en_light, vector<Light*> LT, Camera camera, LightSource lightLource) {
	Color realColor = BLACK;

	for (const Light*light: LT) {
		material nowMaterial = lightLource.MatSets[selected.materialName];
		//光线方向
		Homo3D Ldirection = (light->position - selected.origin).normalize();

		//法线方向
		Homo3D Ndirection = selected.caculateNormal();
		//N·L
		float NdotL = std::max(0.0f, pointProduct(Ldirection, Ndirection));
		//反射光线方向
		Homo3D Outdirection = (Ndirection * (2.0f * NdotL) - Ldirection).normalize();
		//视线方向
		Homo3D cameraDir = camera.projectedMode ? (camera.position - selected.origin).normalize() : -camera.direction.normalize();// 
		Color Lnen = en_light * nowMaterial.Ka;
		Color LnDn = light->intensity * nowMaterial.Kd * (NdotL);
		Color LnSpec = light->intensity * nowMaterial.Ks * powf(std::max(0.0f, pointProduct(Outdirection, cameraDir)), nowMaterial.Ns);
		realColor += clampColor(Lnen + LnDn + LnSpec);
	}
	float scalar = max({ realColor.R, realColor.G, realColor.B });
	if (scalar > 1.0) {
		realColor = realColor / scalar;
	}
	selected.color = realColor;
}

void setPointColorAfterLight(Homo2D& selected, string materialName,Color en_light,vector<Light*> LT, Camera camera, LightSource lightLource) {
	Color realColor = BLACK;
	material nowMaterial = lightLource.MatSets[materialName];
	for (const Light* light: LT) {
		Homo3D point3DSe = { selected.x,selected.y,selected.depth };
		//光线方向
		Homo3D Ldirection = (light->position - point3DSe).normalize();

		//法线方向
		Homo3D Ndirection = { selected.vn.vx,selected.vn.vy,selected.vn.vz }; Ndirection = Ndirection.normalize();
		//N·L
		float NdotL = std::max(0.0f, pointProduct(Ldirection, Ndirection));
		//反射光线方向
		Homo3D Outdirection = (Ndirection * (2.0f * NdotL) - Ldirection).normalize();
		//视线方向
		Homo3D cameraDir = camera.projectedMode ? (camera.position - point3DSe).normalize() : -camera.direction.normalize();// 
		Color Lnen = en_light * nowMaterial.Ka;
		Color LnDn = light->intensity * nowMaterial.Kd * (NdotL);       //漫反射
		Color LnSpec = light->intensity * nowMaterial.Ks * powf(std::max(0.0f, pointProduct(Outdirection, cameraDir)), nowMaterial.Ns);
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