#pragma once
#include "Accelerator.h"

class LOD : public Accelerator {
public:
	LOD();

    virtual void onCameraUpdate(const Mesh* mesh,std::shared_ptr<Camera>) override;
	virtual void deserializeFrom(Json json) override {
#define loadFromJson(name) {if (json.contains(#name)) {name = json[#name];}}
		loadFromJson(maxDepth);
		loadFromJson(scaleRatio);
		loadFromJson(maxVertex);
		return;
#undef loadFromJson(x)
	}

	struct LODNode* root;
	friend struct LODNode;

	int getMaxDepth() const { return maxDepth; }
	int getScaleRatio() const{ return scaleRatio; }
	int getMaxVertex() const { return maxVertex; }
	float minHeight=1e9;
private:
	int maxDepth = 4;//数据结构深度
	int scaleRatio = 2;//表示相机视野的范围和数据结构的比例
	int maxVertex = 1000;//表示每一个小分支里最多出现的顶点数目
	float maxDistance = 40;// 如果和相机的曼哈顿距离超过这个值，则停止分层
	float minDistance = 5;// 如果和相机的曼哈顿距离小于这个值，则为最细层次
};