#include "Plain.h"
#include "../element/Mesh.h"

static int getIndex(int minx,int maxx,int miny,int maxy, int curx, int cury) {
    int y = cury - miny;
    int x = curx - minx;
    int length = maxx - minx + 1;
    int index = y + length * x;
    return index;
}

void Plain::onCameraUpdate(const Mesh* mesh,std::shared_ptr<Camera>camera) {
    if(init == false || !covered()) {
        //获取相机位置信息
        init = true;
        float minx, maxx, miny, maxy;
        getFieid(camera, minx, maxx, miny, maxy);
        //获取mesh信息
        auto data = mesh->getData(minx, maxx, miny, maxy);
        triangles_raw.clear();
        int idx = 0;

        for (int i = minx;i < maxx; i++) { 
            for (int j = miny; j < maxy; j++) {
                
                Eigen::Vector4f points[4];
                int dir[][2] = {{0,0},{0,1},{1,0},{1,1}};
                for(int k = 0; k < 4;k++) {
                    int idx = getIndex(minx,maxx,miny,maxy,i+dir[k][0],j+dir[k][1]);
                    points[k].x() = i + dir[k][0];
                    points[k].y() = j + dir[k][1]; 
                    points[k].z() = data[idx];
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

vector<Triangle> Plain::simplify() {
    return simplifiedMesh;
}

void Plain::build(std::shared_ptr<Camera>) {
    return;
}

bool Plain::covered() const {
    return false;
}