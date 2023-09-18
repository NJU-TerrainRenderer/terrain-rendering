#pragma once
#include"../Camera.h"
#include"../element/Mesh.h"

class Camera;
class Loader;

struct AABB{};

class Accelerator {
protected:
	std::shared_ptr<Loader> loader;
	std::shared_ptr<MeshData> rawMesh,simplifiedMesh;
	std::shared_ptr<MeshData> getRawMesh(std::shared_ptr<Loader>loader,std::shared_ptr<Camera>camera);
	AABB boundingbox;

	virtual bool covered(AABB, std::shared_ptr<Camera>) const =0;//判断是否需要重新获取Mesh信息
public:

	Accelerator(std::shared_ptr<Loader>loader) :loader(loader){};
	//其他模块的信号，声明这里要有更新操作，需要根据Camera的最新情况，更新rawMesh的范围，暂时不确定是否需要具体Event
	virtual void onCameraCreate(std::shared_ptr<Camera>) = 0;
    virtual void onCameraUpdate(std::shared_ptr<Camera>) = 0;

	virtual void build(std::shared_ptr<Camera>) = 0; //建立or更新数据结构。
	virtual std::shared_ptr<MeshData> simplify() = 0;  //利用数据结构对rawMesh化简。
	std::shared_ptr<MeshData> getMesh(){
		return simplifiedMesh;
	}
};