#include "Plain.h"
#include "../element/Mesh.h"




static int getIndex(int x1,int x2,int y1,int y2, int curx, int cury) {
    int y = cury - y1;
    int x = curx - x1;
    int length = y2 - y1 + 1;
    int index = y + length * x;
    return index;
}

void Plain::onCameraUpdate(const Mesh* mesh,std::shared_ptr<Camera>camera) {
    std::cout << "CameraUpdate in plain" << std::endl;
    if(init == false || !covered(camera)) {
        //获取相机位置信息
        init = true;
        range_x = mesh->GetImgSizeX();
        range_y = mesh->GetImgSizeY();

        getField(camera, x1, x2, y1, y2);
        std::cout <<"Get range in: "<< x1 << " " << x2 << " " << y1 << " " << y2 << std::endl;
        //获取mesh信息
        auto data = mesh->getData(x1, y1, x2, y2);
        triangles_raw.clear();
        int idx = 0;
        float minz = 1e9;
        for (int i = 0; i < data.size(); i++) {
            minz = std::min(minz, 1.f*data[i] / SCALE_RATIO);
        }
        
        for (int i = x1;i < x2; i++) { 

            for (int j = y1; j < y2; j++) {
                //采用正方形取样
                Eigen::Vector4f points[4];
                int dir[][2] = {{0,0},{0,1},{1,0},{1,1}};
                for(int k = 0; k < 4;k++) {
                    int idx = getIndex(x1,x2,y1,y2,i+dir[k][0],j+dir[k][1]);
                    points[k].x() = (i + dir[k][0]);
                    points[k].y() = (j + dir[k][1]); 
                    points[k].z() = (1.f*data[idx]/ SCALE_RATIO)-minz;
                }
                Triangle t1(points[0],points[1],points[2]);
                Triangle t2(points[1],points[2],points[3]);
                triangles_raw.push_back(t1);
                triangles_raw.push_back(t2);
            }
        }

        simplifiedMesh = triangles_raw;
    }
}

