#include <stdlib.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include<GL/freeglut.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include "constant.h"
#include<memory>


//操作模式
shadingMode current_shadingMode = gouraud; //初始为phong着色
mode current_mode =scale;    //初始为正常模式
Axis current_axis = all_axis;  //初始为全轴
//摄像机位置与视线方向
Camera viewCamera = { {0,0,100},{0,0,-1} ,false};

vector<Gameobject> AllScene;
vector<Homo3D> world_coordinate= {worldOrigin,x_Axis,y_Axis,z_Axis};

pointLight L_1 =  { {100,0,0},{1,1,1} } ;
std::vector<Light*> L1;


//用户输入相关
int CinTransformIndex = 0, CinOriginIndex=1;
Homo3D transformation[3] = { {0,0,0},{1000,1000,1000},{0,0,0} };//平移旋转缩放
Homo3D transformation_view[3] = { {0,0,0},{1,1,1},{0,0,0} };

bool sceneChanged;
bool underlight = false;
float rotateM1 =0;
float rotateV=0;

LightSource MaterialBall("./model/klane.mtl");
ObjLoader objManager;
void init() {
	objManager = ObjLoader("./model/klane.obj");
	AllScene = fromOBJloadFace(objManager,PURPLE);
	L1.push_back(&L_1);
}

////xyz轴分别要旋转的夹角
void switchXYZ(Axis a, Homo3D& p, float add) {
	switch (a) {
	case all_axis:
		p+= add;
		break;
	case x_axis:
		p.x += add;
		break;
	case y_axis:
		p.y += add;
		break;
	case z_axis:
		p.z += add;
		break;
	}
}


//键盘交互事件
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{//物体变换
	case'w':
	case 'W':
	{
		current_mode = mode::translate;
		break;
	}
	case'r':
	case 'R':
	{
		current_mode = mode::rotate;
		break;
	}
	case's':
	case 'S':
	{
		current_mode = mode::scale;
		break;
	}
	//视点变换
	case'b':
	case 'B':
	{
		current_mode = mode::view_translate;
		break;
	}
	case'v':
	case 'V':
	{
		current_mode = mode::view_rotate;
		break;
	}
	//轴选择
	case'x':
	case 'X':
	{
		if (current_mode != normal) {
			current_axis = x_axis;

		}
		break;
	}
	case'y':
	case 'Y':
	{
		if (current_mode != normal) {
			current_axis = y_axis;

		}
		break;
	}
	case'z':
	case 'Z':
	{
		if (current_mode != normal) {
			current_axis = z_axis;

		}
		break;
	}
	//旋转
	case 'e':
	case 'E':
	{
		if (current_mode == mode::rotate)
		{
			rotateM1 += 0.1 * pai;
		}
		else if (current_mode == mode::view_rotate && current_axis != all_axis)
		{
			rotateV+= 0.1 * pai;
		}
		glutPostRedisplay();
		break;
	}
	case 'q':
	case 'Q':
	{
		if (current_mode == mode::rotate)
			rotateM1 -= 0.1 * pai;
		else if (current_mode == mode::view_rotate && current_axis != all_axis)
		{
			rotateV-= 0.1 * pai;
		}
		glutPostRedisplay();
		break;
	}
	//投影
	case 'T':
	case't':
	{
		viewCamera.projectedMode = !viewCamera.projectedMode;
		glutPostRedisplay();
		break;
	}
	case'`':
	{
		current_shadingMode = static_cast<shadingMode>((current_shadingMode + 1) % 3);
		glutPostRedisplay();
		break;
	}
	case'o':
	{
		current_mode = mode::ObjectChoose;
		glutPostRedisplay();
		break;
	}
	case'p':
	{
		current_mode = mode::OriginChoose;
		glutPostRedisplay();
		break;
	}
	case'1':
	{
		underlight = !underlight;
		glutPostRedisplay();
		break;
	}
	//取景框变换
	case'j':
	case'J':
	{
		for (int i = 0; i < view.pointsetModel.size(); i++) {
			view.pointsetModel[i].x -= 10;
		}
		glutPostRedisplay();
		break;
	}
	case'k':
	case'K':
	{
		for (int i = 0; i < view.pointsetModel.size(); i++) {
			view.pointsetModel[i].y -= 10;
		}
		glutPostRedisplay();
		break;
	}
	case'l':
	case'L':
	{
		for (int i = 0; i < view.pointsetModel.size(); i++) {
			view.pointsetModel[i].x += 10;
		}
		glutPostRedisplay();
		break;
	}
	case'i':
	case'I':
	{
		for (int i = 0; i < view.pointsetModel.size(); i++) {
			view.pointsetModel[i].y += 10;
		}
		glutPostRedisplay();
		break;
	}
	case 27:
		exit(0);
		break;
	}
}

//鼠标交互事件
void mouse(int button, int state, int x, int y)
{
	int glX = x - screen_width / 2;
	int glY = screen_height / 2 - y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN && current_mode >= 1 && current_mode <= 4)//平移缩放投影
		{
			switchXYZ(current_axis, transformation[current_mode - 1], 10);
		}
		else if (state == GLUT_DOWN && current_mode == view_translate)
		{
			switchXYZ(current_axis, transformation_view[current_mode - 5], 10);
		}
		else if (current_mode == ObjectChoose) {
			CinTransformIndex = CinTransformIndex == AllScene.size()-1 ? AllScene.size() - 1 : CinTransformIndex +1;
		}
		else if (current_mode == OriginChoose) {
			CinOriginIndex = CinOriginIndex== AllScene.size() - 1 ? AllScene.size() - 1 : CinOriginIndex + 1;
		}
		glutPostRedisplay();
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN && current_mode >= 1 && current_mode <= 4)
		{
			switchXYZ(current_axis, transformation[current_mode - 1], -10);

		}
		else if (state == GLUT_DOWN && current_mode == view_translate)
		{
			switchXYZ(current_axis, transformation_view[current_mode - 5], -10);
		}
		else if (current_mode == ObjectChoose) {
			CinTransformIndex = CinTransformIndex == 0 ? 0 : CinTransformIndex-1;
		}
		else if (current_mode == OriginChoose) {
			CinOriginIndex = CinOriginIndex == -1 ? -1 : CinOriginIndex - 1;
		}
		glutPostRedisplay();
		break;
	case GLUT_MIDDLE_BUTTON:  // 
		if (state == GLUT_DOWN) {
			current_mode = normal;
			current_axis = all_axis;
		}
		break;
	default:
		break;
	}
}





void clearMa() {
	for (auto& pair : L1) {
		pair->shadow_buffer.clear();
	}
	transformation[0] = { 0,0,0 }; transformation[1] = { 1,1,1 };
	transformation[2] = { 0,0,0 };

	transformation_view[0] = { 0,0,0 }; transformation_view[1] = { 1,1,1 };
	transformation_view[2] = { 0,0,0 };
	rotateV=0;
	rotateM1 = 0;

	
}


void Transform(Gameobject &ownface) {
	Matrix4 T(mode::translate, transformation[translate - 1].x, transformation[translate - 1].y, transformation[translate - 1].z);
	for (auto& it : ownface) {
		for (int i = 0; i < it.pointsetModel.size(); i++) {
			it.pointsetModel[i] = it.pointsetModel[i] * T;
		}
	}
}


void Scale(Gameobject& ownface) {
	Matrix4 S(scale, 1 + (transformation[scale - 1].x - 1) * 0.01f, 1 + (transformation[scale - 1].y - 1) * 0.01f, 1 + (transformation[scale - 1].z - 1) * 0.01f);
	for (auto& it : ownface) {
		for (int i = 0; i < it.pointsetModel.size(); i++) {
			it.pointsetModel[i] = it.pointsetModel[i] * S;
		}
	}
}

void Rotate(Homo3D axis_point, Homo3D axis_direction, float rotate_angle,Gameobject& ownface) {
	// 计算将旋转轴对齐到z轴所需的旋转角度
	Homo3D angles = calculateRotationAngles(axis_direction);

	// 构建矩阵
	Matrix4 T_to_origin(translate, -axis_point.x, -axis_point.y, -axis_point.z);
	Matrix4 T_back(translate, axis_point.x, axis_point.y, axis_point.z);

	Matrix4 Rz_align(angles.z, z_axis);
	Matrix4 Rz_align_i(-angles.z, z_axis);

	Matrix4 Ry_align(angles.y, y_axis);
	Matrix4 Ry_align_i(-angles.y, y_axis);

	Matrix4 R_true(rotate_angle, z_axis);

	// 组合变换矩阵
	Matrix4 final_transform = T_back * Rz_align_i * Ry_align_i * R_true * Ry_align * Rz_align * T_to_origin;

	// 应用到三维模型
	for (auto& it : ownface) {
		for (int i = 0; i < it.pointsetModel.size(); i++) {
			it.pointsetModel[i] = it.pointsetModel[i] * final_transform;
			Homo3D normal_homo(it.pointsetModel[i].vn.vx, it.pointsetModel[i].vn.vy, it.pointsetModel[i].vn.vz, 0.0f);
			normal_homo = (normal_homo * final_transform).normalize();
			it.pointsetModel[i].vn = { normal_homo.x, normal_homo.y, normal_homo.z };
		}
	}
}



//视点平移
void Transform_view() {
	Matrix4 T(mode::view_translate, transformation_view[view_translate - 5].x, transformation_view[view_translate - 5].y, transformation_view[view_translate - 5].z);
	viewCamera.position = viewCamera.position * T;
	
}

void Rotate_view(Homo3D axis_point, Homo3D axis_direction, float rotate_angle) {
	// 计算将旋转轴对齐到z轴所需的旋转角度
	Homo3D angles = calculateRotationAngles(axis_direction);

	// 构建矩阵
	Matrix4 T_to_origin(translate, -axis_point.x, -axis_point.y, -axis_point.z);
	Matrix4 T_back(translate, axis_point.x, axis_point.y, axis_point.z);

	Matrix4 Rz_align(angles.z, z_axis);
	Matrix4 Rz_align_i(-angles.z, z_axis);

	Matrix4 Ry_align(angles.y, y_axis);
	Matrix4 Ry_align_i(-angles.y, y_axis);

	Matrix4 R_true(rotate_angle, z_axis);

	// 组合变换矩阵
	 Matrix4 rotation_matrix = T_back * Rz_align_i * Ry_align_i * R_true * Ry_align * Rz_align * T_to_origin;
	 //计算视点位置
	Homo3D viewRelativeToAxis =viewCamera.position - axis_point;
	Homo3D newViewRelative = viewRelativeToAxis * rotation_matrix;

	// 更新
	viewCamera.position = axis_point + newViewRelative;

	// 旋转部分
	Matrix4 pure_rotation = Rz_align_i * Ry_align_i * R_true * Ry_align * Rz_align;
	viewCamera.direction = viewCamera.direction * pure_rotation;
	viewCamera.up= viewCamera.up * pure_rotation;
}

//应用几何/模型变换
void vertexTransform(Gameobject& ownface,Homo3D Origin) {
	if (rotateM1 != 0) {
		switch (current_axis) {
		case x_axis:
			Rotate(Origin, world_coordinate[1], rotateM1, ownface);
			break;
		case y_axis:
			Rotate(Origin, world_coordinate[2], rotateM1, ownface);
			break;
		case z_axis:
			Rotate(Origin, world_coordinate[3], rotateM1, ownface);
			break;
		}
	}
	if (current_mode != normal) {
		Transform(ownface);
	}

}


void switchVvertexTransform(int transformIndex, int OriginIndex) {
	Homo3D OriginReal =OriginIndex==-1?world_coordinate[0]:surround_box_Origin(AllScene[OriginIndex]) ;
	vertexTransform(AllScene[transformIndex], OriginReal);
	if (current_mode != normal) {
		for (Gameobject& object : AllScene) {
			Scale(object);
		}

	}
}

//视点变换
void viewTransform()
{
	if (rotateV != 0) {
		switch (current_axis) {
		case x_axis:
			Rotate_view(world_coordinate[0], world_coordinate[1], rotateV);
			break;
		case y_axis:
			Rotate_view(world_coordinate[0], world_coordinate[2], rotateV);
			break;
		case z_axis:
			Rotate_view(world_coordinate[0], world_coordinate[3], rotateV);
			break;
		}
		
	}

	if (current_mode != normal) {
		Transform_view();
	}


}


Gameobject transformObjectToFaceSet(vector<Gameobject>&  Scene ) {
	Gameobject result;
	for (const auto& gameobject : Scene) {
		result.insert(result.end(), gameobject.begin(), gameobject.end());
	}
	return result;
}


void resetColorAftershading(pixel_Dictionary&pointset,LightSource Ls) {
	switch (current_shadingMode) {
	case flat:
		break;
	case gouraud:
		for (auto& it : pointset) {
			string ImageName =Ls.MatSets[get<1>(it.second)].kdMapName;
			if (ImageName.empty())continue;
			Color BaseColor = getColorFromImage(get<0>(it.second).vt, Ls, ImageName);
			Color realColor = get<0>(it.second).color * mixPara + BaseColor * (1.0f - mixPara);
			get<0>(it.second).color = realColor;
		}
		//顶点颜色已设置
		break;
	case phong:
		for(auto& it : pointset) {
			setPointColorAfterLight(get<0>(it.second), get<1>(it.second), environmentLight, L1, viewCamera, MaterialBall);
		}
		break;
	}
}


void FaceMain(vector<Face>& model) {
	pixel_Dictionary bufferData;              //最终颜色缓冲图
	Buffer_Dictionary Shadow_bufferData;      //总阴影缓冲图
	Homo3D box_origin = surround_box_Origin(model);

	for (int i = 0; i < model.size(); i++) {
		Face selected = model[i];
		//LightFace,设置面颜色
		setFaceColorAfterLight(selected,environmentLight ,L1 , viewCamera,MaterialBall);
		//投影
		projectface(selected,viewCamera);

		//裁剪，会对被裁剪出来的点进行颜色与法线进行修改
		vector<Line> kuang = view.createLine(Object_3d);
		for (int i = 0; i < kuang.size(); i++) {
			clip(selected, kuang[i]);
		}
		//设置顶点颜色
		for (auto& it : selected.projectedPointset) {
			setPointColorAfterLight(it, selected.materialName, environmentLight, L1, viewCamera, MaterialBall);
		}

		selected.polygon_filtter(Object_2d);

		if (current_shadingMode == flat) {
			for (auto& it : selected.projectedPointset) {
				it.color = selected.color;
			}
		}

		for (auto& pair : L1) {
			pair->updateShadow_buffer(selected.projectedPointset, viewCamera, box_origin);
		}

		//背面剔除
		if (back_face_culling(viewCamera, selected))continue;
		//消隐
		color_buffer1(selected, selected.projectedPointset, bufferData);
	}

	//根据模式重新设置颜色
	resetColorAftershading(bufferData, MaterialBall);  

	for (auto& pair : L1) {
		judgeInshadow(bufferData,pair,Shadow_bufferData );
	}
	Shadow_bufferData = JuanJi_buffer(Shadow_bufferData);
	//光栅绘制
	glBegin(GL_POINTS);
	if (underlight) {
		fillColor(bufferData);
	}
	else {
		fillColor(bufferData,Shadow_bufferData);
	}
	glEnd();

}

//打印裁剪窗
void displayview() {
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < view.pointsetModel.size(); i++) {
		Line lin = { {view.pointsetModel[i].x,view.pointsetModel[i].y },{view.pointsetModel[(i + 1) % view.pointsetModel.size()].x,view.pointsetModel[(i + 1) % view.pointsetModel.size()].y } };
		lin.savePoint(BLACK);
		display_gragh(lin.getPointSet());
	}
	glEnd();
}


//坐标轴
void draw_coordinate()
{
	vector<Homo2D> axis_view ;
	Matrix4 viewMatrix = viewCamera.calculateViewMatrix();
	//投影
	for (int i = 0; i < world_coordinate.size(); i++) {
		Homo3D viewSpacePoint = world_coordinate[i] * viewMatrix;
		Homo2D draw_viewSpacePoint = i == 0 ? Homo3Projection(viewSpacePoint,viewCamera.projectedMode, -viewCamera.position) : Homo3Projection(viewSpacePoint, viewCamera.projectedMode, -viewCamera.position).normalize();
		axis_view.push_back(draw_viewSpacePoint);

	}

	glBegin(GL_POINTS);
	Line x = { {axis_view[0].x - axis_view[1].x * draw_T,axis_view[0].y - axis_view[1].y * draw_T}, {axis_view[0].x + axis_view[1].x * draw_T,axis_view[0].y +axis_view[1].y * draw_T} }; x.savePoint(RED);
	display_gragh(x.getPointSet());
	Line y = { {axis_view[0].x - axis_view[2].x * draw_T,axis_view[0].y - axis_view[2].y * draw_T}, {axis_view[0].x + axis_view[2].x * draw_T,axis_view[0].y + axis_view[2].y * draw_T} }; y.savePoint(GREEN);
	display_gragh(y.getPointSet());
	Line z = { {axis_view[0].x - axis_view[3].x * draw_T,axis_view[0].y - axis_view[3].y * draw_T}, {axis_view[0].x + axis_view[3].x * draw_T,axis_view[0].y + axis_view[3].y * draw_T} }; z.savePoint(BLUE);
	display_gragh(z.getPointSet());

	glEnd();
}


void display(void)
{
	switchVvertexTransform(CinTransformIndex, CinOriginIndex);
	viewTransform();
	Gameobject ownface=transformObjectToFaceSet(AllScene);
	clearMa();
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	displayview();
	draw_coordinate();

	// 绘制图形
	FaceMain(ownface);
	glFlush();

}
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h); //它负责把视景体截取的场景按照怎样的高和宽显示到屏幕上。
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-0.5 * w, 0.5 * w, -0.5 * h, 0.5 * h);
	glMatrixMode(GL_MODELVIEW);
}

void deletku() {
	for (auto& pair : L1) {
		delete pair;
	}
}
int main(int argc, char** argv)
{
	init();

	glutInit(&argc, argv);    //初始化GLUT库
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); //设置即将创建的窗口的显示模式，SINGLE：单缓冲，RGB：使用RGB颜色模式
	glutInitWindowPosition(screen_origin.x, screen_origin.y);//设置窗口左上角在屏幕上的??初始位置??。
	glutInitWindowSize(screen_width, screen_height); //设置窗口的??初始大小
	glutCreateWindow(argv[0]);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	glutMainLoop();
	deletku();
	return 0;
}







