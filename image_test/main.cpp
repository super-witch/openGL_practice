#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main(int argc,char**argv) {
    // 测试1：基本信息
// 读取图像"E:\novel\Ccode_test\openGL\image_test\painon.jpg"
    Mat image = cv::imread("E:/novel/Ccode_test/openGL/p.jpg");
	cout << "Image Width: " << image.cols << endl;
	cout << "Image Height: " << image.rows << endl;
    imshow("Window Title", image);
    waitKey(0);  // 等待按键
    return 0;
}


//
////处理好点的世界坐标与光源坐标系下的投影坐标
//void get_buffer(Face& realPointSets, vector<Homo2D>& bufferData, Camera viewCamera, pointLight L, Homo3D boxOrign) {
//	Camera L_c = { L.position,(L.position - boxOrign).normalize() };
//	Matrix4 lightView = L_c.calculateViewMatrix();
//	//如果没有构建世界空间坐标则构建
//	if (realPointSets.pointsetModel.empty()) {
//		Matrix4 mainView = viewCamera.calculateViewMatrix();
//		// 灯光视图矩阵
//
//		// 复合矩阵
//		Matrix4 viewTransform = mainView.Inverse();
//		for (const auto& pair : bufferData) {
//			Homo3D p(pair.x, pair.y, pair.depth, 1.0);
//			p = p * viewTransform;
//			realPointSets.pointsetModel.push_back(p);
//		}
//	}
//	vector<Homo2D> newface;
//	for (auto& it : realPointSets.pointsetModel) {
//		Homo3D viewSpacePoint = it * lightView;
//		viewSpacePoint.vn = it.vn;//viewP体现z值
//		viewSpacePoint.vt = it.vt;
//		newface.push_back(Homo3Projection(viewSpacePoint, viewCamera.projectedMode, -L_c.position));
//	}
//	realPointSets.projectedPointset = newface;
//}
//
//void  shadow_Mapping(Face& realPoints, Index_Dictionary& shadow_bufferDataNear, vector<Homo2D>bufferData) {
//
//	vector<Homo2D>& model = realPoints.projectedPointset;
//	for (int i = 0; i < model.size(); i++)
//	{
//		//光源视角下的当前二维坐标
//		int x = round(model[i].x);
//		int y = round(model[i].y);
//		auto key = std::make_pair(x, y);
//		//当前屏幕坐标
//		int x2 = round(bufferData[i].x);
//		int y2 = round(bufferData[i].y);
//		auto pos1 = std::make_pair(x2, y2);
//
//		//光源视角下的当前深度值
//		float z = model[i].depth;
//
//
//		//查找当前二维坐标是否有前人
//		auto it = shadow_bufferDataNear.find(key);
//
//		//如果存在则比较二者深度值，不存在则加入
//		if (it != shadow_bufferDataNear.end()) {
//			float minDepth = get<0>(it->second);//储存的最小深度值
//
//			if (SHADOWBIAS + z < minDepth) {
//				//如果更小则更新shadow_bufferDataNear储存的深度
//				shadow_bufferDataNear[key] = { z,pos1 };
//
//			}
//		}
//		else {
//			shadow_bufferDataNear[key] = { z,pos1 };  //没有则添加
//		}
//	}
//	//strengthenShadow(shadow_bufferData, bufferData,model);
//}
//
////先用bufferData生成原坐标与多个光源的投影坐标，再进行阴影计算，shadow_bufferNear持续储存最前面的光源深度值与屏幕坐标
////MultiShadow_buffer则储存多张光源下的阴影图，方便后期混合，当新点的光源坐标对应的旧点的深度值小于新点则将对应光源生成的阴影图的对应屏幕坐标的color置成阴影，同时存入该点的深度值
////
//void shadow_Mapping_MultipyLight(vector<Index_Dictionary>& shadow_bufferNear,   //存放光源最前面
//	Homo3D objOrigin, vector<pointLight> L, Camera viewCamera,
//	vector<Homo2D>& bufferData) {
//	//最靠近光源的
//	Face realPoints;
//	for (int i = 0; i < L.size(); i++) {
//		if (i >= shadow_bufferNear.size()) {
//			Index_Dictionary newbuffer;
//			shadow_bufferNear.push_back(newbuffer);
//		}
//		get_buffer(realPoints, bufferData, viewCamera, L[i], objOrigin);     //
//		shadow_Mapping(realPoints, shadow_bufferNear[i], bufferData);
//	}
//
//}
//
//Buffer_Dictionary mix_shadowBuffer(vector<Buffer_Dictionary>& MultiShadow_buffer) {
//	if (MultiShadow_buffer.empty()) {
//		return Buffer_Dictionary();
//	}
//
//	Buffer_Dictionary finalShadow_buffer;
//	std::map<std::pair<int, int>, int> keyCount;
//
//	for (auto& buffer : MultiShadow_buffer) {
//		for (auto& pair : buffer) {
//			keyCount[pair.first]++;
//		}
//	}
//
//	// 3. 根据出现次数处理
//	for (auto& pair : keyCount) {
//		const auto& key = pair.first;
//		auto& value = pair.second;  //出现次数
//		Color realColor = { 0.3f,0.3f,0.3f };
//		//realColor *= value * SHADOWADD;
//		//realColor /= (MultiShadow_buffer.size()+1 - value) * SHADOWLOW;
//		get<1>(finalShadow_buffer[key]) = clampColor(realColor);
//	}
//	return finalShadow_buffer;
//}


//void Ellipse::savePoint() {
//	int x1, y1;
//	x1 = a * a / sqrt(a * a + b * b);
//	y1 = b * b / sqrt(a * a + b * b);
//	int di;
//	float xi, yir;
//	xi = 0, yir = b;
//	addPoint({ xi, yir });
//	addPoint({ -xi, yir });
//	addPoint({ xi, -yir });
//	addPoint({ -xi, -yir });
//	di = -a * a - 4 * b * b + a * a * b * 4;
//	for (xi = 1; xi <= x1; xi++) {
//		if (di >= 0) {
//			yir = yir;
//			di = di - 4 * b * b * (2 * xi + 2);
//		}
//		else {
//			di = di - 4 * b * b * (2 * xi + 2) - 8 * a * a * (1 - yir);
//			yir = yir - 1;
//		}
//		addPoint({ xi, yir });
//		addPoint({ -xi, yir });
//		addPoint({ xi, -yir });
//		addPoint({ -xi, -yir });
//	}
//	float xir, yi;
//	xir = a, yi = 0;
//	addPoint({ xir, yi });
//	addPoint({ -xir, yi });
//	addPoint({ xir, -yi });
//	addPoint({ -xir, -yi });
//	di = a * b * b * 4 - 4 * a * a - b * b;
//	for (yi = 1; yi <= y1; yi++) {
//		if (di >= 0) {
//			xir = xir;
//			di = di - 8 * a * a - 8 * a * a * (yi);
//		}
//		else {
//			di = di - 8 * a * a - 8 * a * a * (yi)+8 * b * b * (xir)-8 * b * b;
//			xir = xir - 1;
//		}
//		addPoint({ xir, yi });
//		addPoint({ -xir, yi });
//		addPoint({ xir, -yi });
//		addPoint({ -xir, -yi });
//	}
//}
//
//void Parabola::savePoint() {
//	int di;
//	float xi, yir;
//	int x1 = border1; int x2 = border2;
//	xi = 0, yir = 0;
//	addPoint({ xi, yir });
//	addPoint({ -xi, yir });
//	di = 1 - 2 * p;
//	for (xi = 1; xi <= fmax(x1, x2); xi++) {
//		if (di < 0) {
//			yir = yir;
//			di = di + 4 * xi + 2;
//		}
//		else {
//			di = di + 4 * xi + 2 - 4 * p;
//			yir = yir + 1;
//		}
//		if (xi >= p)break;
//		addPoint({ xi, yir });
//		addPoint({ -xi, yir });
//	}
//	float xir, yi;
//	xir = p, yi = p / 2;
//	if (xir <= fmax(x1, x2))
//		addPoint({ xir, yi });
//	di = 4 - 8 * p;
//	for (yi; xir <= fmax(x1, x2); yi++) {
//		if (di >= 0) {
//			xir = xir;
//			di = di - 8 * p;
//		}
//		else {
//			di = di + 8 * xir + 8 - 8 * p;
//			xir = xir + 1;
//		}
//		addPoint({ xir, yi });
//		addPoint({ -xir, yi });
//	}
//}




//class Ellipse :public Gragh {
//private:
//	int a;
//	int b;
//public:
//	Ellipse(Homo2D origin, int ai = 0, int bi = 0) :a(ai), b(bi), Gragh(origin) {};
//	virtual void savePoint();
//};
//
//class Parabola :public Gragh {
//private:
//	int p;
//	int border1, border2;
//public:
//	Parabola(Homo2D origin, int pi = 0, int b1 = 0, int b2 = 0) :p(pi), border1(b1), border2(b2), Gragh(origin) {};
//	virtual void savePoint();
//};