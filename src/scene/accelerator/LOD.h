#pragma once

<<<<<<< HEAD
class LOD:Accelerator{
    public:
	LOD(std::shared_ptr<Loader>loader);
=======
#include "Accelerator.h"

class LOD : Accelerator {
public:
>>>>>>> 0cd6363a0a19dc139fca1955b83182ad9e67b373
    virtual void onCameraCreate(std::shared_ptr<Camera>) override;

    virtual void onCameraUpdate(std::shared_ptr<Camera>) override;

    virtual std::shared_ptr<MeshData> simplify() override;
<<<<<<< HEAD
    virtual void build(std::shared_ptr<Camera>)override;
    virtual bool covered(AABB, std::shared_ptr<Camera>)const override;

	uint16_t compute_lod(const vec3& center, const vec3& extent, const vec3& viewpoint) const;

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
=======

    virtual void build(std::shared_ptr<Camera>) override;

    virtual bool covered(AABB, std::shared_ptr<Camera>) const override;
>>>>>>> 0cd6363a0a19dc139fca1955b83182ad9e67b373
};