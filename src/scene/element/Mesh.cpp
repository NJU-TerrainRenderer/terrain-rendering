#include "Mesh.h"
#include "../accelerator/Accelerator.h"
#include "../accelerator/Plain.h"
#include "../accelerator/LOD.h"
#include <iostream>
using  std::cout;
using std::endl;
#include <gdal.h>
#include <gdal_priv.h>

void Mesh::meshInit(std::string path) {
	MeshPath = new char[32];
	strcpy(MeshPath, path.c_str());
	GDALAllRegister();
	GDALDataset* poDataset;

	poDataset = (GDALDataset*)GDALOpen(MeshPath, GA_ReadOnly);
	if (poDataset == NULL) {
		cout << "The file cannot be opened" << endl;
		return;
	}

	ImgSizeX = poDataset->GetRasterXSize();
	ImgSizeY = poDataset->GetRasterYSize();
}

Mesh::Mesh(char* path): CameraListener(true) {
	MeshPath = path;
	GDALAllRegister();
	GDALDataset* poDataset;

	poDataset = (GDALDataset*)GDALOpen(path, GA_ReadOnly);
	if (poDataset == NULL) {
		cout << "The file cannot be opened" << endl;
		return;
	}

	ImgSizeX = poDataset->GetRasterXSize();
	ImgSizeY = poDataset->GetRasterYSize();
}

int Mesh::GetImgSizeX() const {
	return ImgSizeX;
}
int Mesh::GetImgSizeY() const {
	return ImgSizeY;
}

GDALDataset* createRaster(char* filename, int nRow, int nCol, GDALDataType type)
{
	GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");
	GDALDataset* ds = driver->Create(
		filename,
		nRow,
		nCol,
		1,
		type,
		NULL);
	return ds;
}

vector<int> Mesh::getData(int x1, int y1, int x2, int y2) const{
	GDALAllRegister();
	GDALDataset* poDataset;

	poDataset = (GDALDataset*)GDALOpen(MeshPath, GA_ReadOnly);


	int nImgSizeX = ImgSizeX;
	int nImgSizeY = ImgSizeY;
	//cout << nImgSizeX << "  *  " << nImgSizeY << endl;

	//获取坐标变换系数 
	double trans[6];
	poDataset->GetGeoTransform(trans);
	double dx = trans[1];
	double startx = trans[0] + 0.5 * dx;
	double dy = -trans[5];
	double starty = trans[3] - nImgSizeY * dy + 0.5 * dy;

	//获取图像波段 
	GDALRasterBand* poBand;
	int bandcount;
	bandcount = poDataset->GetRasterCount();

	//读数据
	int* pafScanline;
	pafScanline = new int[nImgSizeX * nImgSizeY];
	poBand = poDataset->GetRasterBand(bandcount);
	poBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanline, nImgSizeX, nImgSizeY, GDALDataType(poBand->GetRasterDataType()), 0, 0);


	int num_image_size = 0;

	vector<int> height;

	for (int i = x1; i <= x2; i++) {
		for (int j = y1; j <= y2; j++) {
			num_image_size++;
			//cout << pafScanline[i * nImgSizeY + j] << " ";
			//cout << num_image_size << endl;

			double gx = startx + dx * i;
			double gy = starty + dy * j;
			size_t m = (size_t)i * nImgSizeY + j;
			//tinyCG::Vec3d P(gx, gy, pafScanline[m]);
			//points.insert(Point_3(gx, gy, pafScanline[m]));
			height.push_back(pafScanline[m]);
		}
	}

	//生成三角网格mmmmmmmm
	//TIN dsm(points.points().begin(), points.points().end());
	//输出三角网格的顶点数目
	//cout << dsm.number_of_vertices() << endl;

	return height;
}

void Mesh::deserializeFrom(Json json) {
	//获取加速结构信息
	Json accJson = json["acceleretor"];
	std::string acceleratorType = accJson["type"];
	if (acceleratorType == "plain") {
		std::shared_ptr<Plain> plainPtr = std::make_shared<Plain>();
		accelerator = std::dynamic_pointer_cast<Accelerator>(plainPtr);
		assert(accelerator != nullptr);
	}
	else if (acceleratorType == "LOD") {
		
	} else{
		std::shared_ptr<Plain> plainPtr = std::make_shared<Plain>();
		accelerator = std::dynamic_pointer_cast<Accelerator>(plainPtr);
	}
	accelerator->deserializeFrom(accJson);
	
	//获取文件路径
	std::string path = json["path"];
	meshInit(path);
	//其他参数暂时不需要?
}

