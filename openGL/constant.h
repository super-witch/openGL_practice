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
       
enum dimension {
	Object_2d,
	Object_3d
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
typedef std::map<std::pair<int, int>, std::tuple<float, std::pair<int, int>>> Index_Dictionary;

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
const int screen_width = 1000;         //宽度
const int screen_height = 1000;       //高度

//裁剪窗
Face view = { { -400, -300 }, {400,-300 } ,{400,300} , {-400,300} };

//三维坐标轴
float draw_T = 500;
Homo3D worldOrigin = { 0,0,0 };
Homo3D x_Axis = { 1,0,0 ,0};
Homo3D y_Axis = { 0,1,0,0 };
Homo3D z_Axis = { 0,0,-1,0 };

Color environmentLight = { 0.2f,0.2f,0.2f };