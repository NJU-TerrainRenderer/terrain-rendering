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
	int maxDepth = 4;//���ݽṹ���
	int scaleRatio = 2;//��ʾ�����Ұ�ķ�Χ�����ݽṹ�ı���
	int maxVertex = 1000;//��ʾÿһ��С��֧�������ֵĶ�����Ŀ
	float maxDistance = 40;// ���������������پ��볬�����ֵ����ֹͣ�ֲ�
	float minDistance = 5;// ���������������پ���С�����ֵ����Ϊ��ϸ���
};