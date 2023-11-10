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

TIN MeshData::GetMesh(int x1, int y1, int x2, int y2) {
	GDALAllRegister();
	GDALDataset* poDataset;

	CGAL::Point_set_3<Point_3> points;
	TIN dsm0(points.points().begin(), points.points().end());

	poDataset = (GDALDataset*)GDALOpen(MeshPath, GA_ReadOnly);
	if (poDataset == NULL) {
		cout << "The file cannot be opened" << endl;
		return dsm0;
	}

	int nImgSizeX = ImgSizeX;
	int nImgSizeY = ImgSizeY;
	//cout << nImgSizeX << "  *  " << nImgSizeY << endl;

	//��ȡ����任ϵ�� 
	double trans[6];
	poDataset->GetGeoTransform(trans);
	double dx = trans[1];
	double startx = trans[0] + 0.5 * dx;
	double dy = -trans[5];
	double starty = trans[3] - nImgSizeY * dy + 0.5 * dy;

	//��ȡͼ�񲨶� 
	GDALRasterBand* poBand;
	int bandcount;
	bandcount = poDataset->GetRasterCount();

	//������
	int* pafScanline;
	pafScanline = new int[nImgSizeX * nImgSizeY];
	poBand = poDataset->GetRasterBand(bandcount);
	poBand->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanline, nImgSizeX, nImgSizeY, GDALDataType(poBand->GetRasterDataType()), 0, 0);


	int num_image_size = 0;

	if (x1 < 0 || x1 >= nImgSizeX || y1 < 0 || y1 >= nImgSizeY) {
		cout << "coordinate out of range" << endl;
		return dsm0;
	}
	if (x2 < 0 || x2 >= nImgSizeX || y2 < 0 || y2 >= nImgSizeY) {
		cout << "coordinate out of range" << endl;
		return dsm0;
	}

	for (int i = x1; i <= x2; i++) {
		for (int j = y1; j <= y2; j++) {
			num_image_size++;
			cout << pafScanline[i * nImgSizeY + j] << " ";
			cout << num_image_size << endl;

			double gx = startx + dx * i;
			double gy = starty + dy * j;
			size_t m = (size_t)i * nImgSizeY + j;
			//tinyCG::Vec3d P(gx, gy, pafScanline[m]);
			points.insert(Point_3(gx, gy, pafScanline[m]));
		}
	}

	//������������
	TIN dsm(points.points().begin(), points.points().end());
	//�����������Ķ�����Ŀ
	cout << dsm.number_of_vertices() << endl;

	return dsm;
}


/*
MeshData::MeshData(std::vector<Triangle>triangles) {
	//TODO
	return;
}

bool MeshData::insertTriangle(Triangle t) {
	return false;
}

bool MeshData::insertTriangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3) {
	return false;
}


std::shared_ptr<std::vector<Triangle>> MeshData::getMesh(){
	std::shared_ptr<std::vector<Triangle>>ptr(&triangles);
	return ptr;
}
*/

