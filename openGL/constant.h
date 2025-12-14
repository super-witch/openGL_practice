#pragma once
#define pai 3.14
#include<vector>
#include<map>
#include <tuple>
#include <string> 

using namespace std;
#define infinity  10000000.0f


// //通用枚举
enum Axis {
	all_axis,
	x_axis,
	y_axis,
	z_axis
};

enum shadingMode {
	flat,
	gouraud,
	phong
};

enum mode {
	normal,
	translate,
	scale,
	rotate,
	project,
	view_translate,
	view_rotate,
	ObjectChoose,
	OriginChoose
};



#include"Color.h"

typedef std::map<std::pair<int, int>, std::tuple<float, Color>> Buffer_Dictionary;
typedef std::map<std::pair<float, float>, int> Index_Dictionary;
#include"Matrix_.h"
#include"Point.h"


typedef std::map<std::pair<int, int>, std::tuple<Homo2D,string>>pixel_Dictionary;

#include"Rasterization.h"
#include"face.h"
typedef vector<Face> Gameobject;
#include"objLoadManager.h"
#include"clip.h"
#include"Light.h"
#include"Shadow.h"
#include"Curve.h"

//窗口设置
const Homo2D screen_origin = { 0,0 };  //原点
const int screen_width = 800;         //宽度
const int screen_height = 600;       //高度

//裁剪窗
Face view = { { -100, -100 }, {200,-100 } ,{200,200} , {-100,200} };

//三维坐标轴
float draw_T = 500;
Homo3D worldOrigin = { 0,0,0 };
Homo3D x_Axis = { 1,0,0 ,0};
Homo3D y_Axis = { 0,1,0,0 };
Homo3D z_Axis = { 0,0,-1,0 };

Color environmentLight = { 0.2f,0.2f,0.2f };

//定义三维的阶梯模型
Homo3D p1 = { -50,-50,-100 };
Homo3D p2 = { -50,50,-100 };
Homo3D p3 = { 100,50,-100 };
Homo3D p4 = { 100,-50,-100 };
Homo3D p5 = { 100,-50,-80 };
Homo3D p6 = { 100,50,-80 };
Homo3D p7 = { 50,-50,-80 };
Homo3D p8 = { 50,50,-80 };
Homo3D p9 = { 50,-50,-60 };
Homo3D p10 = { 50,50,-60 };
Homo3D p11 = { 0,50,-60 };
Homo3D p12 = { 0,-50,-60 };
Homo3D p13 = { 0,-50,-40 };
Homo3D p14 = { 0,50,-40 };
Homo3D p15 = { -50,50,-40 };
Homo3D p16 = { -50,-50,-40 };
//逆时针储存
Face f1 = { {p1, p2, p3, p4},0 , 0.2, 0.2 };//f1.points[0].x
Face f2 = { {p6, p5, p4, p3}, 0.3, 0.3, 0.3 };
Face f3 = { {p5, p6, p8, p7}, 0.4, 0.4, 0.4 };
Face f4 = { {p7, p8, p10, p9}, 0.3, 0.3, 0.3 };
Face f5 = { {p12, p9, p10, p11}, 0.4, 0.4, 0.4 };
Face f6 = { {p14, p13, p12, p11}, 0.3, 0.3, 0.3 };
Face f7 = { {p14, p15, p16, p13}, 0.4, 0.4, 0.4 };
Face f8 = { {p15, p2, p1, p16}, 0.5, 0.5, 0.5 };
Face f9 = { {p15, p14, p11, p10, p8, p6, p3, p2}, 0.6, 0.6, 0.6 };
Face f10 = { {p1,p4,p5, p7,p9 ,p12, p13,p16}, 0, 0.7, 0.7 };


const float DERTAT = 0.001f;
vector<vector<Homo3D>> controlPoints = {
	{ {15, 15, 0}, {15, 165, 0}, {165, 165, 0}, {165, 15, 0} },
	{ {30, 30, 60}, {30, 150, 90}, {150, 150, 90}, {150, 30, 60} },
	{ {45, 45, 120}, {45, 135, 150}, {135, 135, 150}, {135, 45, 120} },
	{ {60, 60, 90}, {60, 120, 120}, {120, 120, 120}, {120, 60, 90} }
};