

#define GLUT_DISABLE_ATEXIT_HACK
#define _USE_MATH_DEFINES
#include<vector>
#include "GLUT.H"
#include <math.h>
#include <string.h> 

using namespace std;

#define ZVALUE 20.0f

#define ellipse_a 100
#define ellipse_b 90
#define circle_r0 20
#define circle_r1 5
#define parabola_p 10
#define cir_x0 45	//眼睛中心
#define cir_y0 30	
#define para_x0 0	//嘴中心
#define para_y0 -60

/////////旋转中心
#define ROT_CENTER_X 0
#define ROT_CENTER_Y 0
/////////缩放中心
#define SCA_CENTER_X 0
#define SCA_CENTER_Y 0


int w_width = 600; //视口、窗口的宽度
int w_height = 600; //视口、窗口的高度


//齐次坐标下的点和向量
struct position
{
	float x, y;
	int z = 1;
};

vector<position> All;	//all point
int center_x = 0, center_y = 0;
double Angle = 0;//角度
float scale_x = 1.0f, scale_y = 1.0f;//缩放倍数


//图形的光栅化
void cal_elli();
void cal_cir();
void cal_para();
void init_pos();

//绘制进行若干操作后的图形
void draw_all();

void draw_coordinate();	//绘制坐标系
void display(void);	//绘制内容


struct position multiply(double matrix[][3], struct position input_v);
position scale(position point);
position Myrotate(position point, double Angle);

//键盘交互事件
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'W':
	{
		center_y++;
		glutPostRedisplay();
		break;
	}
	case 's':
	case 'S':
	{
		center_y--;
		glutPostRedisplay();
		break;
	}
	case 'a':
	case 'A':
	{
		center_x--;
		glutPostRedisplay();
		break;
	}
	case 'd':
	case 'D':
	{
		center_x++;
		glutPostRedisplay();
		break;
	}
	case 'q':
	case 'Q':
	{
		scale_x = scale_x + 0.1f;
		glutPostRedisplay();
		break;
	}
	case 'e':
	case 'E':
	{
		scale_x = scale_x - 0.1f;
		glutPostRedisplay();
		break;
	}
	case 'r':
	case 'R':
	{
		scale_y = scale_y + 0.1f;
		glutPostRedisplay();
		break;
	}
	case 'f':
	case 'F':
	{
		scale_y = scale_y - 0.1f;
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
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			Angle += 0.1;
			glutPostRedisplay();
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			Angle -= 0.1;
			glutPostRedisplay();
		}
		break;
	default:
		break;
	}
}

//投影方式、modelview方式设置
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/*if (w <= h)
		glOrtho(-16.0, 16.0, -16.0*(GLfloat)h/(GLfloat)w, 16.0*(GLfloat)h/(GLfloat)w,
		-ZVALUE, ZVALUE);
	else
		glOrtho(-16.0*(GLfloat)h/(GLfloat)w, 16.0*(GLfloat)h/(GLfloat)w, -16.0, 16.0,
		-ZVALUE, ZVALUE);*/

	if (w <= h)
		glOrtho(-0.5 * w_width, 0.5 * w_width, -0.5 * w_height * (GLfloat)w_height / (GLfloat)w_width, 0.5 * w_height * (GLfloat)w_height / (GLfloat)w_width,
			-ZVALUE, ZVALUE);
	else
		glOrtho(-0.5 * w_width, 0.5 * w_width, -0.5 * w_height * (GLfloat)w_width / (GLfloat)w_height, 0.5 * w_height * (GLfloat)w_width / (GLfloat)w_height,
			-ZVALUE, ZVALUE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//主调函数
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(w_width, w_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("平移变换");

	init_pos();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}

//save all points
void cal_elli()
{
	int d0 = 4 * ellipse_b * ellipse_b + ellipse_a * ellipse_a - 4 * ellipse_a * ellipse_a * ellipse_b;

	int xi = 0;
	int yi = ellipse_b;

	int xir = 0;
	int yir = ellipse_b;

	int di = d0;

	int mid_x = ellipse_a * ellipse_a / (pow(ellipse_a * ellipse_a + ellipse_b * ellipse_b, 0.5)) + 0.5;
	int mid_y = ellipse_b * ellipse_b / (pow(ellipse_a * ellipse_a + ellipse_b * ellipse_b, 0.5)) + 0.5;
	for (xi = 0; xi < mid_x; xi++)
	{
		if (di > 0)
		{
			di = di + 4 * (ellipse_b * ellipse_b * (1 + 2 * (xi + 1)) + ellipse_a * ellipse_a * (1 - 2 * (yir - 0.5)));
			yir = yir - 1;

		}
		else//di<=0
		{
			di = di + 4 * (ellipse_b * ellipse_b * (1 + 2 * (xi + 1)));
			yir = yir;

		}
		position temp1 = { xi,yir,1 };
		position temp2 = { -xi,yir,1 };

		All.push_back(temp1);
		All.push_back(temp2);
		All.push_back(position{ (float)xi, (float)-yir, 1 });
		All.push_back({ (float)-xi, (float)-yir, 1 });
	}


	xi = ellipse_a;
	yi = 0;

	xir = ellipse_a;
	yir = 0;

	di = (2 * ellipse_a - ellipse_b) * (2 * ellipse_a - ellipse_b);

	for (yi = 0; yi < mid_y; yi++)
	{
		if (di > 0)
		{
			di = di + 8 * (ellipse_b * ellipse_b * (1 - xir) + ellipse_a * ellipse_a * (1 + yi));
			xir = xir - 1;

		}
		else//di<0
		{
			di = di + 8 * (ellipse_a * ellipse_a * (1 + yi));
			xir = xir;

		}

		position temp1 = { xir,yi,1 };
		position temp2 = { -xir,yi,1 };

		All.push_back(temp1);
		All.push_back(temp2);
		All.push_back(position{ (float)xir, (float)-yi, 1 });
		All.push_back({ (float)-xir, (float)-yi, 1 });
	}



}
void cal_cir()
{
	int xi = 0, yir = circle_r0;
	int di = 5 - 4 * circle_r0;
	int lim = sqrt(2) * circle_r0 / 2;

	position p[4] = { position{ (float)xi, (float)yir} ,position{ (float)-xi, (float)yir },position{ (float)yir, (float)xi },position{ (float)-yir, (float)xi } };
	for (int i = 0; i < 4; i++) {
		All.push_back(position{ p[i].x + cir_x0, p[i].y + cir_y0 });
		All.push_back(position{ -p[i].x + cir_x0, -p[i].y + cir_y0 });
		All.push_back(position{ p[i].x - cir_x0, p[i].y + cir_y0 });
		All.push_back(position{ -p[i].x - cir_x0, -p[i].y + cir_y0 });
	}

	for (; xi <= lim; xi++) {
		if (di >= 0) {
			di += (8 * (xi - yir) + 20);
			yir--;
		}
		else {
			di += (8 * xi + 12);
		}

		position p[4] = { position{ (float)xi, (float)yir} ,position{ (float)-xi, (float)yir },position{ (float)yir, (float)xi },position{ (float)-yir, (float)xi } };
		for (int i = 0; i < 4; i++) {
			All.push_back(position{ p[i].x + cir_x0, p[i].y + cir_y0 });
			All.push_back(position{ -p[i].x + cir_x0, -p[i].y + cir_y0 });
			All.push_back(position{ p[i].x - cir_x0, p[i].y + cir_y0 });
			All.push_back(position{ -p[i].x - cir_x0, -p[i].y + cir_y0 });
		}
	}

	xi = 0, yir = circle_r1;
	di = 5 - 4 * circle_r1;
	lim = sqrt(2) * circle_r1 / 2;

	position p0[4] = { position{ (float)xi, (float)yir} ,position{ (float)-xi, (float)yir },position{ (float)yir, (float)xi },position{ (float)-yir, (float)xi } };
	for (int i = 0; i < 4; i++) {
		All.push_back(position{ p0[i].x + cir_x0, p0[i].y + cir_y0 });
		All.push_back(position{ -p0[i].x + cir_x0, -p0[i].y + cir_y0 });
		All.push_back(position{ p0[i].x - cir_x0, p0[i].y + cir_y0 });
		All.push_back(position{ -p0[i].x - cir_x0, -p0[i].y + cir_y0 });
	}

	for (; xi <= lim; xi++) {
		if (di >= 0) {
			di += (8 * (xi - yir) + 20);
			yir--;
		}
		else {
			di += (8 * xi + 12);
		}

		position p[4] = { position{ (float)xi, (float)yir} ,position{ (float)-xi, (float)yir },position{ (float)yir, (float)xi },position{ (float)-yir, (float)xi } };
		for (int i = 0; i < 4; i++) {
			All.push_back(position{ p[i].x + cir_x0, p[i].y + cir_y0 });
			All.push_back(position{ -p[i].x + cir_x0, -p[i].y + cir_y0 });
			All.push_back(position{ p[i].x - cir_x0, p[i].y + cir_y0 });
			All.push_back(position{ -p[i].x - cir_x0, -p[i].y + cir_y0 });
		}
	}
}
void cal_para()
{
	int d0 = 1 - parabola_p;
	int d2p = 1;

	double xi = 0.5 * parabola_p;
	double yir = parabola_p;

	int yi = 0;
	int xir = 0;

	int di = d0;

	for (yi = 0; yi <= parabola_p; yi++)
	{
		if (di <= 0)
		{
			di = di + 2 * yi + 3;
			xir = xir;
		}
		else
		{
			di = di + 2 * yi + 3 - 2 * parabola_p;
			xir = xir + 1;
		}

		position temp1 = { xir,yi,1 };
		position temp2 = { xir,-yi,1 };

		position new_poi = Myrotate(temp1, 3.141592 / 2);
		All.push_back({ new_poi.x,new_poi.y + para_y0 });
		new_poi = Myrotate(temp2, 3.141592 / 2);
		All.push_back({ new_poi.x,new_poi.y + para_y0 });


	}


	di = d2p;

	glVertex2i(2 * parabola_p, 2 * parabola_p);

	for (xi = xir; xi <= 20; xi++)//200=边界
	{
		if (di >= 0)
		{
			di = di - 8 * parabola_p;
			yir = yir;
		}
		else
		{

			di = di + 8 + 8 * yir - 8 * parabola_p; yir = yir + 1;
		}
		position temp1 = { xi,yir,1 };
		position temp2 = { xi,-yir,1 };

		position new_poi = Myrotate(temp1, 3.141592 / 2);
		All.push_back({ new_poi.x,new_poi.y + para_y0 });
		new_poi = Myrotate(temp2, 3.141592 / 2);
		All.push_back({ new_poi.x,new_poi.y + para_y0 });


	}

}

//初始化图形
void init_pos()
{
	cal_elli();
	cal_cir();
	cal_para();
}

//矩阵乘法//一个一个点
//矩阵乘法
struct position multiply(double matrix[][3], struct position input_v)
{
	struct position translated_v;
	translated_v.x = matrix[0][0] * input_v.x + matrix[0][1] * input_v.y + matrix[0][2] * 1;
	translated_v.y = matrix[1][0] * input_v.x + matrix[1][1] * input_v.y + matrix[1][2] * 1;
	translated_v.z = matrix[2][0] * input_v.x + matrix[2][1] * input_v.y + matrix[2][2] * 1;
	return translated_v;
}

//旋转
position Myrotate(position point, double angle)// 原来的一个图像的一个点 ，逆时针旋转的角度,角度单位不是度数，是数，pi，
{
	//生成角度变化矩阵
	double matrix[3][3];
	//输入
	matrix[0][0] = cos(angle); matrix[0][1] = -sin(angle); matrix[0][2] = 0;
	matrix[1][0] = sin(angle); matrix[1][1] = cos(angle); matrix[1][2] = 0;
	matrix[2][0] = 0;		  matrix[2][1] = 0;			 matrix[2][2] = 1;
	//逆时针angle度
	point = multiply(matrix, point);
	return point;
}

//缩放
position scale(position point)//缩放
{
	double matrix[3][3];

	matrix[0][0] = scale_x; matrix[0][1] = 0; matrix[0][2] = 0;
	matrix[1][0] = 0; matrix[1][1] = scale_y; matrix[1][2] = 0;
	matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1;
	point = multiply(matrix, point);
	return point;
}


void draw_all()
{
	for (position ele : All) {
		position new_poi = ele;

		new_poi = Myrotate(position{ new_poi.x - ROT_CENTER_X,new_poi.y - ROT_CENTER_Y }, Angle);
		new_poi.x += ROT_CENTER_X;
		new_poi.y += ROT_CENTER_Y;
		new_poi = scale(position{ new_poi.x - SCA_CENTER_X, new_poi.y - SCA_CENTER_Y });
		new_poi.x += SCA_CENTER_X;
		new_poi.y += SCA_CENTER_Y;
		new_poi.x += center_x;
		new_poi.y += center_y;
		glVertex2i(new_poi.x, new_poi.y);
	}
}


//绘制坐标系
void draw_coordinate()
{
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0); //设置接下来显示的颜色
	glVertex2f(w_width, 0.0);
	glVertex2f(-w_width, 0.0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex2f(0.0, w_height);
	glVertex2f(0.0, -w_height);
	glEnd();
}

// 绘制内容
void display(void)
{
	glClearColor(1.f, 1.f, 1.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_coordinate(); //绘制坐标系

	glColor3f(0, 0, 0);
	glBegin(GL_POINTS); //GL_LINE_LOOP

	draw_all();

	glEnd();

	glutSwapBuffers();
}