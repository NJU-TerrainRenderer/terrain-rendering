#include"CameraListener.h"

 std::vector<std::shared_ptr<CameraListener>> CameraListener::pool;

CameraListener::CameraListener(){
    std::shared_ptr<CameraListener>ptr(this);
    pool.push_back(ptr);
}