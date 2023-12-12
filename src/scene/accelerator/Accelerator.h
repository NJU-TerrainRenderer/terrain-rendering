#pragma once
#include <vector>
using std::vector;

#include "../camera/Camera.h"
#include "../element/Triangle.h"
#include "../camera/CameraListener.h"

#define SCALE_RATIO 1000000
class Mesh;

class Camera;

class Accelerator {
protected:
    void getField(std::shared_ptr<Camera> camera, int& xmin, int& xmax, int& ymin, int& ymax);
    int x1, x2, y1, y2;//表示当前获取的数据范围
    vector<Triangle> simplifiedMesh;

    int range_x = 0, range_y = 0;//表示当前图片的x,y的范围
    int init = false;
    bool covered(std::shared_ptr<Camera> camera) ;//判断是否需要重新获取Mesh信息
public:

    Accelerator(){
        x1 = x2 = y1 = y2 = 0;
    };

    //其他模块的信号，声明这里要有更新操作，需要根据Camera的最新情况，更新rawMesh的范围，暂时不确定是否需要具体Event

    virtual void onCameraUpdate(const Mesh* mesh,std::shared_ptr<Camera>) = 0;

    virtual void deserializeFrom(Json) = 0;

    vector<Triangle> getMesh() {
        return simplifiedMesh;
    }
};