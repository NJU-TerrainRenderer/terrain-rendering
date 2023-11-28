#include"Accelerator.h"

//获取相机和z=0平面的交点
void Accelerator::getFieid(
    std::shared_ptr<Camera> camera, 
    int& xmin, int& xmax, int& ymin, int& ymax)
{
    auto position = camera->getPosition();
    auto z = position[2];

    auto forward = camera->getDirection();
    auto right = camera->rightDirection();

    //获取相机视野信息
    auto fov = camera->getFov(); //弧度制
    auto width = camera->getWidth();
    auto height = camera->getHeight();

    //希望这个信息能提前处理好
    auto aspect = width / height;    

    //设置最远范围，防止相机几乎平行于平面时，视线和平面范围太广
    xmin = position[0]-100, xmax = position[0]+100;
    ymin = position[1]-100, ymax = position[1]+100;
    //最小值不能小于0
    xmin = std::max(xmin, 0);
    ymin = std::max(ymin, 0);
    xmax = std::min(range_x, xmax);
    ymax = std::min(range_y, ymax);

    //预估相机视野范围，计算相机视线和z=0平面的交点
    //Step1，计算相机坐标系的旋转矩阵
    //假设实现了相机的toLoacl和toWorld，希望相机能实现这个函数
    //这里假设相机的Local坐标系中 foward = (0,1,0,0), right = (1,0,0,0), up = (0,0,1,0)
    auto worldToLocal = camera->toCameraMatrix();
    auto localToWorld = camera->toWorldMatrix();
    //Step2，根据相机fov信息，获取相机视野的四条射线的方向
    auto d = tan(fov / 2);
    auto h = d / sqrt(1 + aspect * aspect);
    auto w = h * aspect;
    //四条射线的方向
    vector<Eigen::Vector4f> directions;
    for(int i = 0; i < 4; i++) {
        int signw = (i & 1) ? 1 : -1;
        int signh = (i & 2) ? 1 : -1;
        directions.push_back(Eigen::Vector4f(signw * w, 1,signh * h, 0));
    }
    //Step3，将四条射线的方向转换到世界坐标系
    for(int i = 0; i < 4; i++) {
        directions[i] = localToWorld * directions[i];
    }
    //Step4，计算四条射线和z=0平面的交点，这里注意如果平行或者方向相反，就不用计算了
    vector<Eigen::Vector4f> points;
    for(int i = 0; i < 4; i++) {
        auto dir = directions[i];
        //平行
        if((dir[2]) < FLT_EPSILON) continue;
        //方向相反
        if(dir[2] * z > 0) continue;
        auto t = -z / dir[2];
        points.push_back(position + t * dir);
    }
    //Step5，计算四条射线和z=0平面的交点的x和y的最大最小值,特判一下每个点是否有效
    for(int i = 0; i < points.size(); i++) {
        auto point = points[i];
        if(point[0] < xmin) xmin = point[0];
        if(point[0] > xmax) xmax = point[0];
        if(point[1] < ymin) ymin = point[1];
        if(point[1] > ymax) ymax = point[1];
    }
}