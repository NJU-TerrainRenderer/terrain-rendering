#pragma once
#include <vector>
using std::vector;

#include "../camera/Camera.h"
#include "../element/Triangle.h"
#include "../camera/CameraListener.h"


class Mesh;

class Camera;

class Accelerator {
protected:
    void getFieid(std::shared_ptr<Camera> camera, int& xmin, int& xmax, int& ymin, int& ymax);
    vector<Triangle> triangles_raw;
    int x1, x2, y1, y2;//表示当前三角形的范围
    vector<Triangle> simplifiedMesh;

    int range_x = 0, range_y = 0;

    std::shared_ptr<Mesh> getRawMesh(std::shared_ptr<Camera> camera);
    virtual bool covered() const = 0;//判断是否需要重新获取Mesh信息
public:

    Accelerator(){
        x1 = x2 = y1 = y2 = 0;
    };

    //其他模块的信号，声明这里要有更新操作，需要根据Camera的最新情况，更新rawMesh的范围，暂时不确定是否需要具体Event

    virtual void onCameraUpdate(const Mesh* mesh,std::shared_ptr<Camera>) = 0;

    virtual void build(std::shared_ptr<Camera>) = 0; //建立or更新数据结构。
    virtual void deserializeFrom(Json) = 0;
    virtual vector<Triangle> simplify() = 0;  //利用数据结构对rawMesh化简。
    vector<Triangle> getMesh() {
        return simplifiedMesh;
    }
};