#include <stdlib.h>
#include <stdio.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include "glut.h"//GL/
#include "constant.h"
#include<memory>

//操作模式
shadingMode current_shadingMode = phong; //初始为phong着色
mode current_mode =scale;    //初始为正常模式
Axis current_axis = all_axis;  //初始为全轴
//摄像机位置与视线方向
Camera viewCamera = { {0,0,100},{0,0,-1} };
//模型旋转轴
Homo3D c1 = { 0,0,0 }, homoc = { 0,0,1,0};
//= {f1, f2,f3,f4,f5,f6,f7,f8,f10,f9 }

vector<Gameobject> AllScene;
vector<Homo3D> world_coordinate= {worldOrigin,x_Axis,y_Axis,z_Axis};

pointLight L1 = { {0,0,100},{1,1,1} };

//用户输入相关
int CinTransformIndex = 0, CinOriginIndex=1;
Homo3D transformation[3] = { {0,0,0},{300,300,300},{0,0,0} };//平移旋转缩放
Homo3D transformation_view[3] = { {0,0,0},{1,1,1},{0,0,0} };

bool keyShift = false;
float rotateM1 =0;
float rotateV=0;

LightSource MaterialBall("scene1.mtl");
ObjLoader objManager;
void init() {
	objManager = ObjLoader("scene1.obj");
	AllScene = fromOBJloadFace(objManager,PURPLE);
	//ownface = { f1, f2, f3, f4, f5, f6, f7, f8, f10, f9 };
	//for (int i = 0; i < controlPoints.size(); i++) {
	//	ownface.push_back(Face(controlPoints[i],RED));
	//}
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
		keyShift = !keyShift;
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


void resetColorAftershading(vector<Homo2D>&pointset,Face se) {
	switch (current_shadingMode) {
	case flat:
		for (auto& it : pointset) {
			it.color = se.color;
		}
		break;
	case gouraud:
		//顶点颜色已在LightFace中设置
		break;
	case phong:
		for(auto& it : pointset) {
			setPointColorAfterLight(it, se.materialName, environmentLight, L1, viewCamera, MaterialBall, keyShift);
		}
		break;
	}
}

void FaceMain(vector<Face>& model) {
	Buffer_Dictionary bufferData;
	
	for (int i = 0; i < model.size(); i++) {

		Face selected = model[i];

		//背面剔除
		if (back_face_culling(viewCamera, selected,keyShift))continue;
		//LightFace,设置面颜色
		setFaceColorAfterLight(selected,environmentLight ,L1 , viewCamera,MaterialBall,keyShift);
		//投影
		projectface(selected,viewCamera,keyShift);

		//裁剪，会对被裁剪出来的点进行颜色与法线进行修改
		vector<Line> kuang = view.createLine();
		for (int i = 0; i < kuang.size(); i++) {
			clip(selected, kuang[i]);
		}
		//设置顶点颜色
		for (auto& it : selected.projectedPointset) {
			setPointColorAfterLight(it, selected.materialName, environmentLight, L1, viewCamera, MaterialBall, keyShift);
		}
		//填充
		vector<Line> ET = selected.createLine();
		vector<Homo2D> facePointset = polygon_filtter(ET);   //该2D点集中法线插值与颜色插值均已设置好，且是针对面的

		//根据模式重新设置颜色
		resetColorAftershading(facePointset, selected);
		//消隐
		color_buffer1(facePointset, bufferData);
	}
	Face realPointSet = get_buffer(bufferData,viewCamera);
	//更新阴影缓冲图
	shadow_Mapping(model, L1, realPointSet, keyShift,bufferData,Shadow_bufferData);
	//光栅绘制
	glBegin(GL_POINTS);
	fillColor(bufferData,Shadow_bufferData);
	glEnd();
}


void curveMain(vector<Face> controlP) {
	std::map<std::pair<int, int>, std::tuple<float, Color>> bufferData;
	Face newface;
	//newface.pointsetModel=getBezierCurveSet(controlP, DERTAT);
	newface.pointsetModel = getBlineCurveSet(controlP,2,2, DERTAT);
	projectface(newface,viewCamera,keyShift);
	for (auto& it : newface.projectedPointset) {
		it.setColor(PINK);
	}
	color_buffer1(newface.projectedPointset, bufferData);
	glBegin(GL_POINTS);
	fillColor(bufferData);
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
		Homo2D draw_viewSpacePoint = i == 0 ? Homo3Projection(viewSpacePoint, keyShift, -viewCamera.position) : Homo3Projection(viewSpacePoint, keyShift, -viewCamera.position).normalize();
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
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	displayview();
	draw_coordinate();

	// 绘制图形
	FaceMain(ownface);
	//curveMain(ownface);	
	//displayBLineCurve(ownface[0].pointsetModel, 2, DERTAT, RED);
	//displayControlPolygon( ownface);
	glFlush();
	clearMa();
}
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h); //它负责把视景体截取的场景按照怎样的高和宽显示到屏幕上。
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-0.5 * w, 0.5 * w, -0.5 * h, 0.5 * h);
	glMatrixMode(GL_MODELVIEW);
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
	return 0;
}







