//#include <stdlib.h>
//#include <stdio.h>
//#define GLUT_DISABLE_ATEXIT_HACK
//#include "glut.h"//GL/
//#include "constant.h"
//#include<memory>
//using namespace std;
//
//
//
//
//void Rotate(Homo3D axis_point, Homo3D axis_direction, float rotate_angle,vector<Homo3D>model) {
//	// 计算将旋转轴对齐到z轴所需的旋转角度
//	Homo3D angles = calculateRotationAngles(axis_direction);
//
//	// 构建矩阵
//	Matrix4 T_to_origin(translate, -axis_point.x, -axis_point.y, -axis_point.z);
//	Matrix4 T_back(translate, axis_point.x, axis_point.y, axis_point.z);
//
//	Matrix4 Rz_align(angles.z, z_axis);
//	Matrix4 Rz_align_i(-angles.z, z_axis);
//
//	Matrix4 Ry_align(angles.y, y_axis);
//	Matrix4 Ry_align_i(-angles.y, y_axis);
//
//	Matrix4 R_true(rotate_angle, z_axis);
//
//	// 组合变换矩阵
//	Matrix4 final_transform = T_back * Rz_align_i * Ry_align_i * R_true * Ry_align * Rz_align * T_to_origin;
//
//	// 应用到三维模型
//		for (int i = 0; i < model.size(); i++) {
//			model[i] = model[i] * final_transform;
//		}
//	}
//
//
////键盘交互事件
//void keyboard(unsigned char key, int x, int y)
//{
//	switch (key)
//	{//物体变换
//
//	case 27:
//		exit(0);
//		break;
//	}
//}
//
////鼠标交互事件
//void mouse(int button, int state, int x, int y)
//{
//	int glX = x - screen_width / 2;
//	int glY = screen_height / 2 - y;
//	switch (button)
//	{
//	case GLUT_LEFT_BUTTON:
//
//		break;
//	case GLUT_RIGHT_BUTTON:
//
//		break;
//	case GLUT_MIDDLE_BUTTON:  // 
//
//		break;
//	default:
//		break;
//	}
//}
//
//
//
//void display(void)
//{
//	glClearColor(1.f, 1.f, 1.f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT);
//	// 绘制图形
//
//	glFlush();
//}
//void reshape(GLsizei w, GLsizei h)
//{
//	glViewport(0, 0, w, h); //它负责把视景体截取的场景按照怎样的高和宽显示到屏幕上。
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluOrtho2D(-0.5 * w, 0.5 * w, -0.5 * h, 0.5 * h);
//	glMatrixMode(GL_MODELVIEW);
//}
//
//
//int main(int argc, char** argv)
//{
//
//	glutInit(&argc, argv);    //初始化GLUT库
//	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); //设置即将创建的窗口的显示模式，SINGLE：单缓冲，RGB：使用RGB颜色模式
//	glutInitWindowPosition(screen_origin.x, screen_origin.y);//设置窗口左上角在屏幕上的??初始位置??。
//	glutInitWindowSize(screen_width, screen_height); //设置窗口的??初始大小
//	glutCreateWindow(argv[0]);
//	glutReshapeFunc(reshape);
//	glutKeyboardFunc(keyboard);
//	glutMouseFunc(mouse);
//	glutDisplayFunc(display);
//	glutMainLoop();
//	return 0;
//}
//
