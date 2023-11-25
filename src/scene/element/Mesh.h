#pragma once
#include"../accelerator/Accelerator.h"
#include <iostream>
#include<vector>


typedef int BYTE;

class Mesh : public Element {
	//std::vector<Triangle>triangles;
	std::shared_ptr<Accelerator> accelerator;
public:
	Mesh(char* path);
	Mesh() {};
	int GetImgSizeX();
	int GetImgSizeY();
	vector<int> getData(int x1, int y1, int x2, int y2) const;//这个改成加速结构获取数据

	void onCameraUpdate(std::shared_ptr<Camera> camera) {
		accelerator->onCameraUpdate(this, camera);
	};

	std::shared_ptr<vector<Triangle>> getMesh() {
		return std::make_shared<vector<Triangle>>(accelerator->getMesh());
	}//加速结构从这里获取数据

	void deserializeFrom(Json json) override;
	//TODO()

	//Mesh(std::vector<Triangle>triangles);
	//bool insertTriangle(Triangle t);
	//bool insertTriangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3);
private:
	char* MeshPath;
	int ImgSizeX;
	int ImgSizeY;

	//Mesh(std::vector<Triangle>triangles);
	//bool insertTriangle(Triangle t);
	//bool insertTriangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3);
	/*virtual std::shared_ptr<std::vector<Triangle>> getMesh() override {
		Triangle t;
		return t.getMesh();
	 };
	 */
};