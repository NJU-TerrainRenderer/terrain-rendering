#pragma once

#include <iostream>
#include<vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <gdal.h>
#include <gdal_priv.h>

<<<<<<< HEAD
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/boost/graph/graph_traits_Delaunay_triangulation_2.h>
#include <CGAL/boost/graph/copy_face_graph.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Projection_traits = CGAL::Projection_traits_xy_3<Kernel>;
using TIN = CGAL::Delaunay_triangulation_2<Projection_traits>;

using Point_3 = Kernel::Point_3;
typedef int BYTE;

class MeshData : public Element {
	//std::vector<Triangle>triangles;
public:
	MeshData(char* path);
	int GetImgSizeX();
	int GetImgSizeY();
	TIN GetMesh(int x1, int y1, int x2, int y2);

	//TODO()

	//MeshData(std::vector<Triangle>triangles);
	//bool insertTriangle(Triangle t);
	//bool insertTriangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3);
private:
	char* MeshPath;
	int ImgSizeX;
	int ImgSizeY;

	//MeshData(std::vector<Triangle>triangles);
	//bool insertTriangle(Triangle t);
	//bool insertTriangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3);
	/*virtual std::shared_ptr<std::vector<Triangle>> getMesh() override {
		Triangle t;
		return t.getMesh();
	 };
	 */
=======
    //MeshData(std::vector<Triangle>triangles);
    //bool insertTriangle(Triangle t);
    //bool insertTriangle(const Eigen::Vector3f& point1, const Eigen::Vector3f& point2, const Eigen::Vector3f& point3);
    virtual std::shared_ptr<std::vector<Triangle>> getMesh() override {
        Triangle t;
        return t.getMesh();
    };

    void deserializeFrom(Json json) override;
>>>>>>> 1e2aee89b9b302a7f9b7e003d969be0890f57b29
};