#include "LOD.h"
#include "../element/Mesh.h"
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
using std::cout;
using std::endl;
using std::set;
using std::map;

//临时组织的Mesh数据，用于LODNode的化简
struct NodeMesh {
    struct Vertex {
        Eigen::Vector4f point;
    };//顶点

    void update(vector<int>vMap) {
        map<int, int>idxMap;//顶点的映射，从老的idx映射到新的idx。
        set<std::pair<int, int>>eMap;//边的映射
        int idx = 0;
        //建立顶点映射
        for (int i = 0; i < vertices.size(); i++) {
            int toIdx = vMap[i];
            if(idxMap.count(toIdx)) {
                continue;
            }
            idxMap[toIdx] = idx++;
        }
        
        //重新构建边
        for (int i = 0; i < edges.size(); i++) {
            for (const int& j :edges[i]) {
                if (i < j)continue;
                int nxti = idxMap[vMap[i]];
                int nxtj = idxMap[vMap[j]];
                if (nxti == nxtj)continue;
                if (nxti > nxtj) {
                    std::swap(nxti, nxtj);
                }
                if (eMap.count({ nxti,nxtj })) {
                    continue;
                }
                eMap.insert({ nxti,nxtj });
            }
        }

        vector<Vertex>newVertices(idx);
        vector<vector<int>>newEdges(idx);
        for (const auto& [key, value] : idxMap) {
            newVertices[value] = vertices[key];
        }
        for (const auto& [u, v] : eMap) {
            newEdges[u].push_back(v);
            newEdges[v].push_back(u);
        }
        vertices = newVertices;
        edges = newEdges;
       

    }

    void insertTriangle(Vertex p1, Vertex p2, Vertex p3) {
        int v1, v2, v3;
        if (vMap.count(p1)) {
            v1 = vMap[p1];
        }
        else {
            vMap[p1] = v1 = vertices.size();
            vertices.push_back(p1);
            edges.push_back(vector<int>(0));
        }
        if (vMap.count(p2)) {
            v2 = vMap[p2];
        }
        else {
            vMap[p2] = v2 = vertices.size();
            vertices.push_back(p2);
            edges.push_back(vector<int>(0));
        }
        if (vMap.count(p3)) {
            v3 = vMap[p3];
        }
        else {
            vMap[p3] = v3 = vertices.size();
            edges.push_back(vector<int>(0));
            vertices.push_back(p3);
        }
        if (v2 > v3) std::swap(v2, v3);
        if (v1 > v2) std::swap(v1, v2);
        if (v2 > v3) std::swap(v2, v3);
        if (!eMap.count({ v1,v2 })) {
            edges[v1].push_back(v2);
            edges[v2].push_back(v1);
            eMap.insert({ v1,v2 });
        }
        if (!eMap.count({ v1,v3 })) {
            edges[v1].push_back(v3);
            edges[v3].push_back(v1);
            eMap.insert({ v1,v3 });
        }
        if (!eMap.count({ v2,v3 })) {
            edges[v2].push_back(v3);
            edges[v3].push_back(v2);
            eMap.insert({ v2,v3 });
        }

    }
    //邻接表表示边
    map<Vertex, int>vMap;
    set<std::pair<int, int>>eMap;
    vector<Vertex> vertices;
    vector<vector<int>> edges;
};

static bool operator < (const NodeMesh::Vertex& lhs, const NodeMesh::Vertex& rhs) {
    //按照高度为第一优先级排序
    if (lhs.point[2] < rhs.point[2]) return true;
    else if (lhs.point[2] > rhs.point[2]) return false;
    // 保证顶点存在偏序关系
    return std::pair<float, float>{lhs.point[0], lhs.point[1]} < std::pair<float, float>{rhs.point[0], rhs.point[1]};
}

//将多组 mesh 聚集到一起
static NodeMesh* mergeMesh(const NodeMesh* mesh[4]) {
    map<NodeMesh::Vertex,int>vMap;//顶点映射
    set<std::pair<int, int>>eMap;//边映射
    int idx = 0;//当前总边数，从0开始计数
    
    //顶点的离散化处理
    for (int i = 0; i < 4; i++) {
        for (const auto& vertex : mesh[i]->vertices) {
            if (vMap.count(vertex))continue;
            vMap[vertex] = idx++;
        }
    }

    
    //边的离散化处理
    for (int i = 0; i < 4; i++) {
        const auto& vertices = mesh[i]->vertices;
        const auto& edges = mesh[i]->edges;
        for (int u = 0; u < edges.size(); u++) {
            for (const int& v:edges[u]) {
                int uIndex = vMap[vertices[u]];
                int vIndex = vMap[vertices[v]];
                //保证顺序才能从set里查询
                if (uIndex > vIndex) {
                    std::swap(uIndex, vIndex);
                }
                eMap.insert({ uIndex,vIndex });
            }
        }
    }
    
    NodeMesh* newMesh = new NodeMesh;
    newMesh->vertices.resize(idx);
    newMesh->edges.resize(idx);
    //合并顶点
    for (const auto& [key, value] : vMap) {
        newMesh->vertices[value] = key;
    }
    //合并边
    for (const auto& [u,v] : eMap) {
        newMesh->edges[u].push_back(v);
        newMesh->edges[v].push_back(u);
    }
    return newMesh;
}

static vector<Triangle> getMeshFromNodeMesh(const NodeMesh* mesh) {
    vector<Triangle>retMesh;
    //Brute Force的四重循环!毁灭吧(x)
    for (int i = 0; i < mesh->vertices.size(); i++) {
        auto p1 = mesh->vertices[i].point;
        for (int j = 0; j < mesh->edges[i].size(); j++) {
            int u = mesh->edges[i][j];
            auto p2 = mesh->vertices[u].point;
            for (int k = j + 1; k < mesh->edges[i].size(); k++) {
                int v = mesh->edges[i][k];
                auto p3 = mesh->vertices[v].point;
                if(i>=u || u>=v){
                    continue;
                }
                for (const int& to : mesh->edges[u]) {
                    if (to == v) {
                        retMesh.push_back(Triangle(p1,p2,p3));
                        break;
                    }
                }
            }
        }
    }
    return retMesh;
}

struct LODNode {
    int depth=0;//当前深度
    NodeMesh* mesh =NULL;//自己的NodeMesh数据

    float x1=1e9, x2=-1e9, y1=1e9, y2=-1e9;//自己覆盖的范围
    
    LODNode* parent =NULL;        //自己的父节点
    LODNode* neighbour[4] = { 0 };  //自己的邻居
    LODNode* child[4] = { 0 };      //自己的子节点

    LOD* lod =NULL;//从属的LOD结构

    void insertTriangle(Eigen::Vector4f p1, Eigen::Vector4f p2,Eigen::Vector4f p3) {
#define UPDATE(p) {x1 = std::min(x1,p[0]); x2=std::max(x2,p[0]); y1=std::min(y1,p[1]); y2=std::max(y2,p[1]);}
        UPDATE(p1);
        UPDATE(p2);
        UPDATE(p3);
        lod->minHeight = std::min(lod->minHeight, p1[2]);
        lod->minHeight = std::min(lod->minHeight, p2[2]);
        lod->minHeight = std::min(lod->minHeight, p3[2]);
#undef UPDATE(x)
        mesh->insertTriangle({ p1 }, { p2 }, { p3 });
    }

    float calc_distance(std::shared_ptr<Camera> camera, float x1, float x2, float y1, float y2) {
        auto position = (*camera).getPosition();
        float xdif, ydif;
        if (position[0] < x1) {
            xdif = x1 - position[0];
        }
        else if (position[0] > x2) {
            xdif = position[0] - x2;
        }
        else {
            xdif = 0;
        }

        if (position[1] < y1) {
            ydif = y1 - position[1];
        }
        else if (position[1] > y2) {
            ydif = position[1] - y2;
        }
        else {
            ydif = 0;
        }
        return std::max(xdif, ydif);
    }

    vector<Triangle> getMesh(std::shared_ptr<Camera>camera) {
        vector<Triangle> ret;
        float dist = calc_distance(camera, x1, x2, y1, y2);
        
        // 越近越要用高层次的，当距离大于当前层次对应的距离时，选择当前情况，否则选择最差的
        if (dist-0.f>FLT_EPSILON&&
            dist > 
            lod->minDistance + (lod->maxDistance-lod->minDistance)*(lod->getMaxDepth()-depth)/lod->getMaxDepth()
            ) {
                cout << "current depth: " << depth << endl;
                cout << "current range: " << x1 << " " << x2 << " " << y1 << " " << y2 << endl;
                return getMeshFromNodeMesh(mesh);
        }
        if (dist > lod->maxDistance) {
            return vector<Triangle>(0);
        }
        for (int i = 0; i < 4; i++) {
            if (child[i]) {
                auto c = child[i]->getMesh(camera);
                for (auto x : c) {
                    ret.push_back(x);
                }
            }
        }
        if (ret.empty()) {
            cout << "current depth: " << depth << endl;
            cout << "current range: " << x1 << " " << x2 << " " << y1 << " " << y2 << endl<<endl;
            ret = getMeshFromNodeMesh(mesh);
        }
        return ret;
    }
    void updateDepth(int depth) {
        this->depth = depth;
        for (int i = 0; i < 4; i++) {
            if (child[i]) {
                child[i]->updateDepth(depth + 1);
            }
        }
    }
    ~LODNode() {
        //递归删除
        delete mesh;
        for (int i = 0; i < 4; i++) {
            if (child[i]) {
                child[i]->~LODNode();
            }
        }
    }
};

static float fitPlane(const vector<Eigen::Vector4f>& points) {
    // 计算均值
    float meanX = 0.0, meanY = 0.0, meanZ = 0.0;
    for (const auto& p : points) {
        meanX += p[0];
        meanY += p[1];
        meanZ += p[2];
    }
    meanX /= points.size();
    meanY /= points.size();
    meanZ /= points.size();

    // 计算最小二乘法的参数
    float a = 0.0, b = 0.0, c = 0.0;
    float numerator = 0.0, denominator = 0.0;
    for (const auto& p : points) {
        float deltaX = p[0] - meanX;
        float deltaY = p[1] - meanY;
        numerator += deltaX * deltaY * (p[2] - meanZ);
        denominator += std::powf(deltaX, 2) + std::powf(deltaY, 2);
    }

    a = numerator / denominator;
    b = -numerator / denominator;
    c = meanZ - a * meanX - b * meanY;

    //计算距离平方和
    float total = 0;
    denominator = a * a + b * b + 1;
    for (const auto& p : points) {
        numerator = a * p[0] + b * p[1] - p[2] + c;
        total += (numerator * numerator)/denominator ;
    }
    return total;
}

//评估单个点的"可缩性"
static float evaluateVertex(NodeMesh* mesh,int index) {
    vector<Eigen::Vector4f>vertices;
    auto v = mesh->vertices[index].point;
    vertices.push_back(v);

    //对于该点，有两个评估项
    //1.该点和周围节点的边的长度，越长越应该慎重
    float para1 = 0;
    for (int i = 0; i < mesh->edges[index].size(); i++) {
        auto u = mesh->vertices[mesh->edges[index][i]].point;
        para1 += sqrt((v[0]-u[0])*(v[0]-u[0]) + (v[1]-u[1])*(v[1]-u[1]) + (v[2]-u[2])*(v[2]-u[2]));
        vertices.push_back(u);
    }
    

    //2.该点和周围节点的平坦程度
    // 对于这一点，我们可以考虑用最小二乘法进行平面拟合，然后计算各个点和平面的距离平方和。
    // 不过这样有一个问题，就是如果只有该点不平坦，得到的结果不太好
    // 越不平坦我们越不应该进行化简
    float para2 = fitPlane(vertices);
    para2 ;

    //最终的评估结果我们使用para1 * para2 完成。
    return para1 * para2;
}

//选取较为合适的点
static int selectVertex(NodeMesh* data,vector<int>vMap) {
    //采用雇佣问题的算法，随机选取sqrt n个元素，取可缩性最小的位置，接下来如果碰到任何一个可缩性小于该元素的位置，即缩该点
    while (1) {
        assert(data->vertices.size() == vMap.size());
        float mValue = 1e9f;
        int upBound = data->vertices.size();
        int lpl = -1;
        for (int i = 0, j = 0; j < upBound; j += 2 * i + 1, i++) {
            
            int index = rand() % upBound;
            if (vMap[index] != index) {
                i--;
                continue;
            }
            if (evaluateVertex(data, index) < mValue) {
                mValue = std::min(mValue, evaluateVertex(data, index));
                lpl = index;
            }
            
        }

        for (int i = 0, j = 0; j < upBound; j += 2 * i + 1, i++) {
            
            int index = rand() % upBound;
            if (vMap[index] != index) {
                i--;
                continue;
            }
            if (evaluateVertex(data, index) < mValue) {
                return index;
            }
        }
        return lpl;
    }
    return 0;
}

//将相邻的四个节点合并
static LODNode* mergeNode(LODNode* nodes[4]) {
    //0,1,2,3分别表示左上，右上，左下，右下节点
    //建立新节点
    for (int i = 0; i < 4; i++) {
        std::cout << "Before Merge Vertex size: " << nodes[i]->mesh->vertices.size() << std::endl;
    }
    LODNode* newNode = new LODNode;
    newNode->depth = 0;
    newNode->parent = NULL;
    newNode->lod = nodes[0]->lod;
    for (int i = 0; i < 4; i++) {
        newNode->child[i] = nodes[i];
        newNode->x1 = std::min(newNode->x1, nodes[i]->x1);
        newNode->y1 = std::min(newNode->y1, nodes[i]->y1);
        newNode->x2 = std::max(newNode->x2, nodes[i]->x2);
        newNode->y2 = std::max(newNode->y2, nodes[i]->y2);
        for (int j = 0; j < 4; j++) {
            nodes[j]->neighbour[i] = nodes[i];
        }
        nodes[i]->parent = newNode;
    }
    //使用边坍缩算法实现三角形的merge
    //首先将自己的NodeMesh合并
    const NodeMesh* meshes[] = { nodes[0]->mesh,nodes[1]->mesh,nodes[2]->mesh,nodes[3]->mesh };
    newNode->mesh = mergeMesh(meshes);

    //采用雇佣问题的算法，随机选取最适合进行边坍缩的点，执行次数为n/e次初始化加上多次选择
    //获取最大顶点数
    int maxVertex = newNode->lod->getMaxVertex();
    int curVertex = newNode->mesh->vertices.size();
    int cnt = 0;
    vector<int> vMap(curVertex);//点向它实际代表的点的映射
    std::iota(vMap.begin(), vMap.end(), 0);
    //选取进行边坍缩的点,在选取超过sqrt(n)次后进行更新
    //std::cout << "Cutting edges" << std::endl;
    bool modified = false;
    while (curVertex > maxVertex) {
        int vertex = selectVertex(newNode->mesh,vMap);
        //随机选取目标点
        auto& edges = newNode->mesh->edges[vMap[vertex]];
        int size = edges.size();
        int toVertex = edges[rand() % size];
        vMap[vertex] = toVertex;
        cnt++;
        curVertex--;
        modified = true;
        if (true) {
            cnt = 0;
            modified = false;
            //暴力更新mesh数据
            newNode->mesh->update(vMap);
            //std::cout << "Updating edges, Left Vertices size: " << newNode->mesh->vertices.size() << std::endl;
            vMap.resize(newNode->mesh->vertices.size());
            curVertex = vMap.size();
            std::iota(vMap.begin(), vMap.end(), 0);
        }
    }
    if(modified)newNode->mesh->update(vMap);
    //更新newNode的深度
    newNode->updateDepth(0);
    //std::cout << "After Merge Vertex size: " << newNode->mesh->vertices.size() << std::endl;
    return newNode;
}

//LODNode的合并操作

static int getDotIndex(int x1, int x2, int y1, int y2, int curx, int cury) {
    int y = cury - y1;
    int x = curx - x1;
    int length = y2 - y1 + 1;
    int index = y + length * x;
    return index;
}

static int getNodeIndex(int x1, int x2, int y1, int y2, int curx, int cury,int size) {
    float deltax = (x2 - x1) / size;
    float deltay = (y2 - y1) / size;
    int xIdx = (1.f*curx - x1) / deltax;
    int yIdx = (1.f*cury - y1) / deltay;
    xIdx = std::min(size - 1, xIdx);
    yIdx = std::min(size - 1, yIdx);
    return yIdx + xIdx*size;
}

static LODNode* build(LOD* lod, 
    vector<int>data,int x1,int y1,int x2,int y2
) {
    int size = (1<<lod->getMaxDepth());
    LODNode** base = new LODNode* [size * size];
    for (int i = 0; i < size * size; i++) {
        base[i] = new LODNode;
        base[i]->lod = lod;
        base[i]->mesh = new NodeMesh;
    }
    //六角形采样,奇数行采样奇数列，偶数行采样偶数列
    //屎山请注意
    //std::cout << "Sampling" << std::endl;
    for (int i = x1; i <= x2; i += 1) {
        for (int j = y1 + (i & 1); j <= y2; j += 2) {
            int nodeIdx = getNodeIndex(x1, x2, y1, y2, i, j, size);
            int pointIdx = getDotIndex(x1, x2, y1, y2, i, j);
            int verticeIdx = base[nodeIdx]->mesh->vertices.size();
            Eigen::Vector4f point(i,j,1.f*data[pointIdx]/SCALE_RATIO,.0f);
            //连接其他边
            std::pair<int, int> dir[] = { {-1,1},{-1,-1},{0,-2 } };
            bool exist[3] = { false,false,false };
            int nodeIndex[3];
            Eigen::Vector4f points[3];
            for (int k = 0; k < 3; k++) {
                int nx = i + dir[k].first;
                int ny = j + dir[k].second;
                if (nx < x1 || nx >= x2 ||
                    ny < y1 || ny >= y2) {
                    continue;
                }
                exist[k] = true;
                nodeIndex[k] = getNodeIndex(x1, x2, y1, y2, nx, ny, size);
                points[k] = Eigen::Vector4f( 1.f*nx,1.f*ny,1.f*data[getDotIndex(x1,x2,y1,y2,nx,ny)]/SCALE_RATIO,0.f );
            }
            for (int u = 0; u < 2; u++) {
                if (!exist[u] || !exist[u + 1]) {
                    continue;
                }
                // 插入
                Eigen::Vector4f nPoints[3] = { point,points[u],points[u + 1] };
                int nIndex[3] = { nodeIdx, nodeIndex[u], nodeIndex[u + 1] };
                for (int i1 = 0; i1 < 3; i1++) {

                    //去重
                    bool repflag = false;
                    for (int j1 = 0; j1 < i1; j1++) {
                        if (nIndex[i1] == nIndex[j1]) {
                            repflag = true;
                            break;
                        }
                    }
                    if (repflag)continue;
                    if (nIndex[i1] == 0) {
                        //cout << "Insert Triangle:\n" <<nPoints[0][0]<<" "<< nPoints[0][1]<< endl;
                        //cout << nPoints[1][0] << " " << nPoints[1][1] << endl;
                        //cout << nPoints[2][0] << " " << nPoints[2][1] << endl;
                    }
                    base[nIndex[i1]]->insertTriangle(nPoints[0], nPoints[1], nPoints[2]);
                }
            }
        }
    }
    std::cout << "Bulding data structure" << std::endl;
    //建立上层数据结构
    int level = lod->getMaxDepth()-1;
    while (level>=0) {
        int sz = (1 << level);
        LODNode** levelNode = new LODNode * [sz * sz];
        for (int i = 0; i < sz * sz; i++) {
            int line = i / sz;
            int col = i % sz;
            LODNode* merged[4];
            for (int j = 0; j < 4; j++) {
                int nline = (line << 1) ^ (j & 1);
                int ncol = (col << 1) ^ ((j >> 1) & 1);
                merged[j] = base[nline * size + ncol];
            }
            levelNode[i] = mergeNode(merged);
        }
        delete[] base;
        base = levelNode;
        size = sz;
        level--;
    }
    LODNode* root = base[0];
    delete[] base;
    std::cout << "Build Complete" << std::endl;
    return root;
}

//默认构造函数，什么也不做
LOD::LOD() {
    root = NULL;
    init = false;
}

void LOD::onCameraUpdate(const Mesh* mesh, std::shared_ptr<Camera>camera) {
    std::cout << "CameraUpdate in LOD" << std::endl;
    if (init == false || !covered(camera)) {
        // 需要更新数据结构,更新数据结构后返回对应的结果
        if (root != NULL) {
            delete root;
        }

        init = true;
        range_x = mesh->GetImgSizeX();
        range_y = mesh->GetImgSizeY();

        //获取相机视野范围，并扩大得到的范围量

        getField(camera, x1, x2, y1, y2);

        int deltaX = (x2 - x1);
        int deltaY = (y2 - y1);
        x1 = x1 - (x2 - x1) * (scaleRatio - 1);
        x2 = x2 + (x2 - x1) * (scaleRatio - 1);
        y1 = y1 - (y2 - y1) * (scaleRatio - 1);
        y2 = y2 + (y2 - y1) * (scaleRatio - 1);
        
        x1 = std::max(x1, 0);
        y1 = std::max(y1, 0);
        x2 = std::min(x2, range_x);
        y2 = std::min(y2, range_x);
        std::cout << "Get range in: " << x1 << " " << x2 << " " << y1 << " " << y2 << std::endl;

        //获取数据，六角形采样
        auto data = mesh->getData(x1, y1, x2, y2);
        std::cout << "Build LOD" << std::endl;
        root = build(this, data, x1, y1, x2, y2);
    }
    //从数据结构中获取数据
    std::cout << "Get range in: " << x1 << " " << x2 << " " << y1 << " " << y2 << std::endl;
    std::cout << "Get data" << std::endl;
    simplifiedMesh = root->getMesh(camera);
}