#pragma once

#include "Accelerator.h"

class LOD : Accelerator {
public:
	LOD();

    virtual void onCameraUpdate(const MeshData& mesh,std::shared_ptr<Camera>) override;

    virtual vector<Triangle> simplify() override;
    virtual void build(std::shared_ptr<Camera>)override;
    virtual bool covered()const override;
	
	
	void	set_parameters(float max_pixel_error, float screen_width, float horizontal_FOV_degrees);
	void	get_bounding_box(vec3* box_center, vec3* box_extent);
	uint16_t compute_lod(const vec3& center, const vec3& extent, const vec3& viewpoint) const;
	void	set_use_loader_thread(bool use);
//data:
    lod_chunk*	m_chunks;
	int	m_chunks_allocated;
	int	m_tree_depth;	// from chunk data.
	float	m_error_LODmax;	// from chunk data.
	float	m_distance_LODmax;	// computed from chunk data params and set_parameters() inputs --> controls displayed LOD level.
	float	m_texture_distance_LODmax;	// computed from texture quadtree params and set_parameters() --> controls when textures kick in.
	float	m_vertical_scale;	// from chunk data; displayed_height = y_data * m_vertical_scale.
	float	m_base_chunk_dimension;	// x/z size of highest LOD chunks.
	int	m_chunk_count;
	lod_chunk**	m_chunk_table;
	chunk_tree_loader*	m_loader;
};