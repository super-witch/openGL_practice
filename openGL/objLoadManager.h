#pragma once
#include"constant.h"

#include <iostream>
#include <fstream>



using namespace std;
typedef std::map<std::string, material> MatSet;
typedef std::map<std::string,ImageData> ImageSet;
//定义向量

//三维空间中构成一个三角面的三个点序列，逆时针
struct my_triangle_point_indices
{
	int first_point_index;
	int second_point_index;
	int third_point_index;
	string Mat_Name;
};


vector<string> split( string spstr, char delimiter) {
	vector<string>parameters;
	string ans = "";
	if( spstr[0] == '#' || spstr.empty())return parameters;
	spstr+=delimiter;
	for (int i = 0; i < spstr.length(); i++)
	{
		char ch = spstr[i];
		if (ch !=delimiter) {
			ans += ch;
		}
		else
		{
			parameters.push_back(ans); //取出字符串中的元素，以空格切分
			ans = "";
		}
	}
	return parameters;
}



class ObjLoader
{
public:
	vector<int> startFaceIndex;//存放组信息
	vector<Homo3D> pointSets;//存放顶点(x,y,z)坐标
	vector <UV> VtSets;//存放顶点(x,y)纹理坐标
	vector<vNormal> pointNormalSets;//存放顶点(x,y,z)法向量
	vector<my_triangle_point_indices> faceSets;//存放面的三个顶点索引
	vector<my_triangle_point_indices> NormalSetsIndex;//存放面的三个法向量索引
	vector<my_triangle_point_indices> VTSetsIndex;//存放面的三个法向量索引
public:
	ObjLoader() {}
	ObjLoader(string filename) //构造函数
	{
		string line; string currentMatName;
		fstream f;
		f.open(filename, ios::in);
		if (!f.is_open()) {
			cout << "Something Went Wrong When Opening Objfiles" << endl;
		}
		int faceCount = 0;
		while (!f.eof()) {
			getline(f, line);//拿到obj文件中一行，作为一个字符串
			vector<string>parameters;
			string tailMark = " ";
			string ans = "";

			line = line.append(tailMark);
			for (int i = 0; i < line.length(); i++)
			{
				char ch = line[i];
				if (ch != ' ') {
					ans += ch;
				}
				else
				{
					parameters.push_back(ans); //取出字符串中的元素，以空格切分
					ans = "";
				}
			}
			//cout << parameters.size() << endl;
			if (parameters[0] == "v")  //顶点,从1开始，将顶点的xyz三个坐标放入顶点vector 
			{
				Homo3D curPoint;
				curPoint.x = atof(parameters[1].c_str());
				curPoint.y = atof(parameters[2].c_str());
				curPoint.z = atof(parameters[3].c_str());
				pointSets.push_back(curPoint);
			}
			else if (parameters[0] == "o") {
				startFaceIndex.push_back(faceCount);
			}
			else if (parameters[0] == "vn") //顶点的法向量
			{
				vNormal curPointNormal;
				curPointNormal.vx = atof(parameters[1].c_str());
				curPointNormal.vy = atof(parameters[2].c_str());
				curPointNormal.vz = atof(parameters[3].c_str());
				pointNormalSets.push_back(curPointNormal);
			}
			else if (parameters[0] == "vt") {
				UV curVt;
				curVt.u = atof(parameters[1].c_str());
				curVt.v = atof(parameters[2].c_str());
				VtSets.push_back(curVt);
			}
			else if (parameters[0] == "f") //面，存放三个顶点的索引
			{
				vector<string> faceElements1 = split(parameters[1], '/');
				vector<string> faceElements2 = split(parameters[2], '/');
				vector<string> faceElements3 = split(parameters[3], '/');
				//因为顶点索引在obj文件中是从1开始的，而我们存放的顶点vector是从0开始的，因此要减1
				my_triangle_point_indices curTri; my_triangle_point_indices curTriVnormal;
				my_triangle_point_indices curTriVt;
				curTri.first_point_index = stoi(faceElements1[0]) - 1;
				curTriVt.first_point_index = stoi(faceElements1[1]) - 1;
				curTriVnormal.first_point_index = stoi(faceElements1[2]) - 1;
	
				curTri.second_point_index = stoi(faceElements2[0]) - 1;
				curTriVt.second_point_index = stoi(faceElements2[1]) - 1;
				curTriVnormal.second_point_index = stoi(faceElements2[2]) - 1;

				curTri.third_point_index = stoi(faceElements3[0]) - 1;
				curTriVt.third_point_index = stoi(faceElements3[1]) - 1;
				curTriVnormal.third_point_index = stoi(faceElements3[2]) - 1;
				curTri.Mat_Name = currentMatName;
				faceSets.push_back(curTri);
				NormalSetsIndex.push_back(curTriVnormal);
				VTSetsIndex.push_back(curTriVt);
				faceCount++;
			}
			else if (parameters[0] == "usemtl") {
				currentMatName = parameters[1];
			}
		}
		f.close();
	}
};



class LightSource {
public:
	MatSet MatSets;
	ImageSet ImageSets;
public:
	LightSource() {}
	LightSource(string filename) //构造函数
	{
		string line; string MatName;
		fstream f;
		f.open(filename, ios::in);
		if (!f.is_open()) {
			cout << "Something Went Wrong When Opening Objfiles" << endl;
		}

		while (!f.eof()) {
			getline(f, line);//拿到obj文件中一行，作为一个字符串
			vector<string>parameters;
			string tailMark = " ";
			string ans = "";
			if(line[0]=='#'||line.empty())continue;
			line = line.append(tailMark);
			for (int i = 0; i < line.length(); i++)
			{
				char ch = line[i];
				if (ch != ' ') {
					ans += ch;
				}
				else
				{
					parameters.push_back(ans); //取出字符串中的元素，以空格切分
					ans = "";
				}
			}
			//cout << parameters.size() << endl;
			if(parameters[0] == "newmtl")  //新材质
			{
				MatName = parameters[1];
				material mat;
				MatSets[MatName] = mat;
			}

			else if (parameters[0] == "Ns")  //顶点,从1开始，将顶点的xyz三个坐标放入顶点vector 
			{
				MatSets[MatName].Ns = stof(parameters[1].c_str());
			}
			else if (parameters[0] == "Ka")
			{
				MatSets[MatName].Ka = { stof(parameters[1].c_str()),stof(parameters[2].c_str()),stof(parameters[3].c_str()) };
			}
			else if (parameters[0] == "Kd")
			{
				MatSets[MatName].Kd = { stof(parameters[1].c_str()),stof(parameters[2].c_str()),stof(parameters[3].c_str()) };
			}
			else if (parameters[0] == "Ks")
			{
				MatSets[MatName].Ks = { stof(parameters[1].c_str()),stof(parameters[2].c_str()),stof(parameters[3].c_str()) };
			}
			else if (parameters[0] == "map_Kd")
			{
				MatSets[MatName].Kd = WHITE;
				MatSets[MatName].kdMapName = parameters[1];
				string imageFileName = parameters[1];
				//加载图片，存入ImageSets
				cv::Mat image = cv::imread(imageFileName);
				//这里假设你有一个函数loadImage返回Color*类型的图片数据
				vector<Color> imageData = convertMatToColors(image);
				ImageSets[imageFileName] = {image.cols ,image.rows,imageData };
			}
		}
		f.close();
	}
};

vector<Gameobject> fromOBJloadFace(ObjLoader obj,Color c) {
	vector<Gameobject> result;
	for (int i = 0; i < obj.startFaceIndex.size(); i++) {
		int startIndex = obj.startFaceIndex[i];
		int endIndex = (i == obj.startFaceIndex.size() - 1) ? obj.faceSets.size() : obj.startFaceIndex[i + 1];
		Gameobject result0;
		for (int i = startIndex; i < endIndex; i++) {
			struct my_triangle_point_indices Faceindex = obj.faceSets[i];
			struct my_triangle_point_indices normalindex = obj.NormalSetsIndex[i];
			struct my_triangle_point_indices vtsetsIn = obj.VTSetsIndex[i];
			Face temp = { { {obj.pointSets[Faceindex.first_point_index].x,obj.pointSets[Faceindex.first_point_index].y,obj.pointSets[Faceindex.first_point_index].z,1,obj.pointNormalSets[normalindex.first_point_index],obj.VtSets[vtsetsIn.first_point_index]},
							{obj.pointSets[Faceindex.second_point_index].x,obj.pointSets[Faceindex.second_point_index].y,obj.pointSets[Faceindex.second_point_index].z,1,obj.pointNormalSets[normalindex.second_point_index],obj.VtSets[vtsetsIn.second_point_index]},
							{obj.pointSets[Faceindex.third_point_index].x,obj.pointSets[Faceindex.third_point_index].y,obj.pointSets[Faceindex.third_point_index].z,1,obj.pointNormalSets[normalindex.third_point_index],obj.VtSets[vtsetsIn.third_point_index]}},
							Faceindex.Mat_Name };
			temp.color = c;
			result0.push_back(temp);
		}
		result.push_back(result0);
	}
	return result;

}

