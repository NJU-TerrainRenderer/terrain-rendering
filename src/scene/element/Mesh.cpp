#include "Meshdata.h"

MeshData::MeshData(char* path) {
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


int MeshData::GetImgSizeX() {
	return ImgSizeX;
}
int MeshData::GetImgSizeY() {
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

vector<int> MeshData::GetMesh(int x1, int y1, int x2, int y2) {
	GDALAllRegister();
	GDALDataset* poDataset;

	//CGAL::Point_set_3<Point_3> points;
	//TIN dsm0(points.points().begin(), points.points().end());

	vector<int> height;

	poDataset = (GDALDataset*)GDALOpen(MeshPath, GA_ReadOnly);
	if (poDataset == NULL) {
		cout << "The file cannot be opened" << endl;
		return height;
	}

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

	if (x1 < 0 || x1 >= nImgSizeX || y1 < 0 || y1 >= nImgSizeY) {
		cout << "coordinate out of range" << endl;
		return height;
	}
	if (x2 < 0 || x2 >= nImgSizeX || y2 < 0 || y2 >= nImgSizeY) {
		cout << "coordinate out of range" << endl;
		return height;
	}


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

	//生成三角网格
	//TIN dsm(points.points().begin(), points.points().end());
	//输出三角网格的顶点数目
	//cout << dsm.number_of_vertices() << endl;

	return height;
}



