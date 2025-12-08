#pragma once
#include"constant.h"

long long combination(int n, int k) {
    // 边界条件
    if (k == 0 || k == n) return 1;
    if (k > n) return 0;
    if (k > n - k) k = n - k; // 利用对称性 C(n,k) = C(n,n-k)
    // 递归公式：C(n,k) = C(n-1,k-1) + C(n-1,k)
    return combination(n - 1, k - 1) + combination(n - 1, k);
}
Homo3D bezierCurve(const std::vector<Homo3D>& controlPoints, float t) {
    int n = controlPoints.size() - 1; // 控制点数量减一
    Homo3D point = { 0.0f, 0.0f,0.0f };
    for (int i = 0; i <= n; ++i) {
        long long coeff = combination(n, i); 
		float term = coeff * pow(t, i) * pow(1 - t, n - i); // 伯恩函数，次数与n有关
		point += controlPoints[i] * term; // 加权累加
    }
    return point;
}
Homo3D bezier2Curve(const vector<Face>& controlPoints, float u, float v) {
    int n1 = controlPoints.size() - 1;    //后循环对应v
    int n2 = (controlPoints[0].pointsetModel.size()) - 1; //先循环，对应u
    Homo3D point = { 0.0f, 0.0f,0.0f };
    for (int q = 0; q <= n1; ++q) {        //v-q
        for (int p = 0; p <= n2; p++) {    //u-p
            long long coeff1 = combination(n1, q); // 计算组合数 C(n, i)
            long long coeff2 = combination(n2, p); // 计算组合数 C(n, i)
            float term = coeff2 * coeff1 * pow(u, p) * pow(1 - u, n1 - p) * pow(v, q) * pow(1 - v, n2 - q); // 伯恩函数
            point += controlPoints[q].pointsetModel[p] * term; // 加权累加
        }
    }
    return point;
}

Homo3D BezierCurve(const std::vector<Homo3D>& controlPoints, float t,int k,int i) {
    if (k == 0) {
		return controlPoints[i];
    }
    else {
		return  BezierCurve(controlPoints, t, k - 1, i + 1)*t  +  BezierCurve(controlPoints, t, k - 1, i)*(1 - t) ;
    }
}

Homo3D Bezier2Curve(const vector<Face>& controlPoints, float u, float v,int k,int l,int i,int j) {
    if (k == 0 && l == 0) {
		return controlPoints[i].pointsetModel[j];
    }
    else if (k == 0) {
        return Bezier2Curve(controlPoints, u, v, k, l - 1, i, j + 1) * v + Bezier2Curve(controlPoints, u, v, k, l - 1, i, j) * (1 - v);
    }
    else {
		return Bezier2Curve(controlPoints, u, v, k - 1, l, i + 1, j) * u + Bezier2Curve(controlPoints, u, v, k - 1, l, i, j) * (1 - u);
    }
}
void displayBezierCurve(const vector<Homo3D>& controlPoints, float dertaT, Color colorCurve = BLACK) {
    glColor3f(colorCurve.R, colorCurve.G, colorCurve.B);
    glBegin(GL_POINTS);
    for (float i = 0; i < 1; i += dertaT) {
        Homo3D point = BezierCurve(controlPoints, i, controlPoints.size()-1,0);
        glVertex2i(point.x, point.y);
    }
    glEnd();
}

vector<Homo3D> getBezierCurveSet(const vector<Face>& controlPoints, float dertaT) {
    vector<Homo3D> curveSet;
    for (float v = 0; v < 1; v += dertaT) {
        for (float u = 0; u < 1; u += dertaT) {
            curveSet.push_back(Bezier2Curve(controlPoints, u, v, controlPoints[0].pointsetModel.size()-1,controlPoints.size()-1,0,0));
        }
    }
	return curveSet;
}

float ui(int u,int segment) {
    return (static_cast<float>(u) / segment);
}

float Boor_box(float u, int k,int segment,int i) {
    if (k == 0) {
        if (u< ui(i + 1, segment) && u > ui(i, segment)) {
            return 1.0f;
        }
        else
            return 0.0f;
    }
    else {
        float denom1 = ui(i + k, segment) - ui(i, segment) == 0 ? 0 : (u - ui(i, segment)) / (ui(i + k, segment) - ui(i, segment));
		float denom2 = ui(i + k + 1, segment) - ui(i + 1, segment) == 0 ? 0 : (ui(i + 1 + k, segment) - u) /(ui(i + k + 1, segment) - ui(i + 1, segment));
        float num=Boor_box(u, k - 1, segment, i) * denom1 + Boor_box(u, k - 1, segment, i+1) * denom2;
        return num;
    }
}   

Homo3D B_splineCurve(const std::vector<Homo3D>& controlPoints,float u,int k) {
    int n = controlPoints.size() - 1;
	int segment = n + k;
    Homo3D point = { 0.0f, 0.0f,0.0f };
    for (int i = 0; i <= n; ++i) {
        float term = Boor_box(u, k, segment, i);
        point += controlPoints[i] * term; // 加权累加
    }
    return point;
}

Homo3D B_splineCurves(const vector<Face>& controlPoints, float u, float v, int p,int q) {
    int n = controlPoints.size() - 1;
	int m = (controlPoints[0].pointsetModel.size()) - 1;
    int segment_u = m + p;
	int segment_v = n + q; 
    Homo3D point = { 0.0f, 0.0f,0.0f };
    for (int j = 0; j <= n; j++) {
        for (int i = 0; i <= m; ++i) {
            float term1 = Boor_box(u, p, segment_u, i);
			float term2 = Boor_box(v, q, segment_v, j);
            point += controlPoints[i].pointsetModel[j] * term1*term2; // 加权累加
        }
    }
    return point;
}

void displayBLineCurve(const vector<Homo3D>& controlPoints, int k,  float dertaT,Color colorCurve=BLACK) {
    int n = controlPoints.size() - 1;
    int segment = n + k;
    glBegin(GL_POINTS);
	glColor3f(colorCurve.R, colorCurve.G, colorCurve.B);
    for (float u = ui(k, segment); u < ui(n, segment); u += dertaT) {
        Homo3D point=B_splineCurve(controlPoints, u, k);
		glVertex2f(point.x, point.y);
    }
	glEnd();
}
vector<Homo3D> getBlineCurveSet(const vector<Face>& controlPoints, int p, int q, float dertaT) {
    int n = controlPoints.size() - 1;
    int m = (controlPoints[0].pointsetModel.size()) - 1;
    int segment_u = m + p;
    int segment_v = n + q;
    vector<Homo3D> curveSet;
    for (float v = ui(q,segment_v);v< ui(n + 1, segment_v); v += dertaT) {
        for (float u =ui( p,segment_u); u < ui(m+1, segment_u); u+=dertaT) {
                curveSet.push_back( B_splineCurves(controlPoints, u,v,p,q ));
            }
    }
	return curveSet;
}


void displayControlPolygon(vector<Face> controlPoints, Color color = BLACK) {
    
    glColor3f(color.R, color.G, color.B);
    glBegin(GL_POINTS);
    for (int i = 0; i < controlPoints.size() ; i++) {
        for(int j=0;j< controlPoints[i].pointsetModel.size()-1;j++)
        {
            Line lin = { {controlPoints[i].pointsetModel[j].x,controlPoints[i].pointsetModel[j].y },{controlPoints[i].pointsetModel[(j + 1)].x,controlPoints[i].pointsetModel[(j + 1)].y } };
            lin.savePoint();
            display_gragh(lin.getPointSet());
		}
    }
    for (int i = 0; i < controlPoints[0].pointsetModel.size(); i++) {
        for (int j = 0; j < controlPoints.size() - 1; j++)
        {
            Line lin = { {controlPoints[j].pointsetModel[i].x,controlPoints[j].pointsetModel[i].y },{controlPoints[j+1].pointsetModel[i].x,controlPoints[j+1].pointsetModel[i].y } };
            lin.savePoint();
            display_gragh(lin.getPointSet());
        }
    }
    glEnd();
}
 