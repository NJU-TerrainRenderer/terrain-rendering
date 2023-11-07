#include "LOD.h"

<<<<<<< HEAD
#ifndef M_PI
#define M_PI 3.141592654
#endif // M_PI

static float	s_vertical_scale = 1.0;

// 效率检测
static int	s_chunks_in_use = 0;
static int	s_chunks_with_data = 0;
static int	s_chunks_with_texture = 0;
static int	s_bytes_in_use = 0;
static int	s_textures_bound = 0;


using vec3=Eigen::Vector3f;
using lod_chunk_tree=LOD;
/*
 ----------------------------------------------------------------------------------------------------
 ----------------------------------------------------------------------------------------------------
*/

//顶点信息
struct vertex_info{
    int	vertex_count;
    struct vertex{
        int16_t x[3];
        int16_t y_delta;
    };
    vertex* vertices;

    int index_count;
    uint16_t* indices;

    int triangle_count;

    vertex_info()
		: vertex_count(0),
		  vertices(0),
		  index_count(0),
		  indices(0),
		  triangle_count(0)
	{
	}

    ~vertex_info(){
        if(vertices){
            free(vertices);
            vertices = NULL;
        }
        if(indices){
            free(indices);
            indices = NULL;
        }
    }

    void read();

    int get_data_size() const{
        return sizeof(*this) + vertex_count * sizeof(vertices[0]) + index_count * sizeof(indices[0]);
    }
};

void vertex_info::read(){
    //TODO 根据输入的组织形式进行读取.
}

/*
 ----------------------------------------------------------------------------------------------------
 ----------------------------------------------------------------------------------------------------
*/

// 单个chunk的 Mesh数据 
struct lod_chunk_data {
	vertex_info	m_verts;


	lod_chunk_data()
	{

	}


	~lod_chunk_data()
	{
	}

	void render();

	int	get_data_size() const
	// Return data bytes used by this object.
	{
		return sizeof(*this) + m_verts.get_data_size();
	}
};

void lod_chunk_data::render(){
    //TODO 
}

/*
 ----------------------------------------------------------------------------------------------------
 ----------------------------------------------------------------------------------------------------
*/

struct lod_chunk{
    //data:
	lod_chunk*	m_parent;
	lod_chunk*	m_children[4];

	union {
		int	m_label;
		lod_chunk*	m_chunk;
	} m_neighbor[4];

	// Chunk "address" (its position in the quadtree).
	uint16_t	m_x, m_z;
	uint8_t 	m_level;

	bool	m_split;	// true if this node should be rendered by descendents.  @@ pack this somewhere as a bitflag.  LSB of lod?
	uint16_t	m_lod;		// LOD of this chunk.  high byte never changes; low byte is the morph parameter.

	// Vertical bounds, for constructing bounding box.
	int16_t	m_min_y, m_max_y;

	long	m_data_file_position;
	lod_chunk_data*	m_data;

    //methods:
    ~lod_chunk()
	{
		if (m_data) {
			delete m_data;
			m_data = 0;
		}
	}

    
	void    lod_chunk::read(lod_chunk_tree* tree,int recurse_count);
    void	clear();
	void	update(lod_chunk_tree* tree, const vec3& viewpoint);
	void	update_texture(lod_chunk_tree* tree, const vec3& viewpoint);

	void	do_split(lod_chunk_tree* tree, const vec3& viewpoint);
	bool	can_split(lod_chunk_tree* tree);	// return true if this chunk can split.  Also, request the necessary data for future, if not.
	void	unload_data();

	void	warm_up_data(lod_chunk_tree* tree, float priority);
	void	request_unload_subtree(lod_chunk_tree* tree);

	void	request_unload_textures(lod_chunk_tree* tree);

	//int	render(const lod_chunk_tree& c, const view_state& v, cull::result_info cull_info, render_options opt, bool texture_bound);

	//void	read(SDL_RWops* in, int recursion_count, lod_chunk_tree* tree, bool vert_data_at_end);
	void	lookup_neighbors(lod_chunk_tree* tree);

	// Utilities.

	bool	has_resident_data() const
	{
		return m_data != NULL;
	}

	bool	has_children() const
	{
		return m_children[0] != NULL;
	}

	int	get_level() const
	{
		return m_level;
	}

	void	compute_bounding_box(const lod_chunk_tree& tree, vec3* box_center, vec3* box_extent)
	{
		float	level_factor = (1 << (tree.m_tree_depth - 1 - m_level));

		box_center->x() = (m_max_y + m_min_y) * 0.5f * tree.m_vertical_scale;
		box_extent->y() = (m_max_y - m_min_y) * 0.5f * tree.m_vertical_scale;
			     
		box_center->x() = (m_x + 0.5f) * level_factor * tree.m_base_chunk_dimension;
		box_center->z() = (m_z + 0.5f) * level_factor * tree.m_base_chunk_dimension;
			     
		const float  	EXTRA_BOX_SIZE = 1e-3f;	// this is to make chunks overlap by about a millimeter, to avoid cracks.
		box_extent->x() = level_factor * tree.m_base_chunk_dimension * 0.5f + EXTRA_BOX_SIZE;
		box_extent->z() = box_extent->x();
	}
};


/*
 ----------------------------------------------------------------------------------------------------
 ----------------------------------------------------------------------------------------------------
*/

//用于控制Chunk 的加载，缓存，卸载
//需要线程安全
struct chunk_tree_loader{
	//TODO
};

/*
 ----------------------------------------------------------------------------------------------------
 ----------------------------------------------------------------------------------------------------
*/

static void	morph_vertices(float* verts, const vertex_info& morph_verts, const vec3& box_center, const vec3& box_extent, float f)
// Adjust the positions of our morph vertices according to f, the
// given morph parameter.  verts is the output buffer for processed
// verts.
//
// The input is in chunk-local 16-bit signed coordinates.  The given
// box_center/box_extent parameters are used to produce the correct
// world-space coordinates.  The quantizing to 16 bits is a way to
// compress the input data.
//
// @@ This morphing/decompression functionality should be shifted into
// a vertex program for the GPU where possible.
{
	// Do quantization decompression, output floats.

	const float	sx = box_extent.x() / (1 << 14);
	const float	sz = box_extent.z() / (1 << 14);

	const float	offsetx = box_center.x();
	const float	offsetz = box_center.z();

	const float	one_minus_f = (1.0f - f);

	for (int i = 0; i < morph_verts.vertex_count; i++) {
		const vertex_info::vertex&	v = morph_verts.vertices[i];
		verts[i*3 + 0] = offsetx + v.x[0] * sx;
		verts[i*3 + 1] = (v.x[1] + v.y_delta * one_minus_f) * s_vertical_scale;	// lerp the y value of the vert.
		verts[i*3 + 2] = offsetz + v.x[2] * sz;
	}
#if 0
	// With a vshader, this routine would be replaced by an initial agp_alloc() & memcpy()/memmap().
#endif // 0
}

static void	count_chunk_stats(lod_chunk* c)
// Add up stats from c and its descendents.
{
	if (c->m_data)
	{
		s_chunks_with_data++;
		s_bytes_in_use += c->m_data->get_data_size();

		if (c->has_children()) {
			for (int i = 0; i < 4; i++) {
				count_chunk_stats(c->m_children[i]);
			}
		}
	}
	// else don't recurse; our descendents should not have any
	// data.  TODO: assert on this.
}

void	lod_chunk::clear()
// Clears the m_split values throughout the subtree.  If this node is
// not split, then the recursion does not continue to the child nodes,
// since their m_split values should be false.
//
// Do this before calling update().
{
	assert(m_data != NULL);

	s_chunks_in_use++;
	s_chunks_with_data++;
	s_bytes_in_use += m_data->get_data_size();

	if (m_split) {
		m_split = false;

		// Recurse to children.
		if (has_children()) {
			for (int i = 0; i < 4; i++) {
				m_children[i]->clear();
			}
		}
	}
	else
	{
		if (has_children()) {
			for (int i = 0; i < 4; i++) {
				count_chunk_stats(m_children[i]);
			}
		}
	}
}


void	lod_chunk::update(lod_chunk_tree* tree, const vec3& viewpoint)
// Computes 'lod' and split values for this chunk and its subtree,
// based on the given camera parameters and the parameters stored in
// 'base'.  Traverses the tree and forces neighbor chunks to a valid
// LOD, and updates its contained edges to prevent cracks.
//
// Invariant: if a node has m_split == true, then all its ancestors
// have m_split == true.
//
// Invariant: if a node has m_data != NULL, then all its ancestors
// have m_data != NULL.
//
// !!!  For correct results, the tree must have been clear()ed before
// calling update() !!!
{
	vec3	box_center, box_extent;
	compute_bounding_box(*tree, &box_center, &box_extent);

	uint16_t desired_lod = tree->compute_lod(box_center, box_extent, viewpoint);

	if (has_children()
		&& desired_lod > (m_lod | 0x0FF)
		&& can_split(tree))
	{
		do_split(tree, viewpoint);

		// Recurse to children.
		for (int i = 0; i < 4; i++) {
			m_children[i]->update(tree, viewpoint);
		}
	} else {
		// We're good... this chunk can represent its region within the max error tolerance.
		if ((m_lod & 0xFF00) == 0) {
			// Root chunk -- make sure we have valid morph value.
			//m_lod = iclamp(desired_lod, m_lod & 0xFF00, m_lod | 0x0FF);

			assert((m_lod >> 8) == m_level);
		}

		// Request residency for our children, and request our
		// grandchildren and further descendents be unloaded.
		if (has_children()) {
			float	priority = 0;
			if (desired_lod > (m_lod & 0xFF00)) {
				priority = (m_lod & 0x0FF) / 255.0f;
			}

			if (priority < 0.5f) {
				for (int i = 0; i < 4; i++) {
					m_children[i]->request_unload_subtree(tree);
				}
			}
			else
			{
				for (int i = 0; i < 4; i++) {
					m_children[i]->warm_up_data(tree, priority);
				}
			}
		}
	}
}

void	lod_chunk::do_split(lod_chunk_tree* tree, const vec3& viewpoint)
// Enable this chunk.  Use the given viewpoint to decide what morph
// level to use.
//
// If able to split this chunk, then the function returns true.  If
// unable to split this chunk, then the function doesn't change the
// tree, and returns false.
{
	if (m_split == false) {
		assert(this->can_split(tree));
		assert(has_resident_data());

		m_split = true;

		if (has_children()) {
			// Make sure children have a valid lod value.
			{for (int i = 0; i < 4; i++) {
				lod_chunk*	c = m_children[i];
				vec3	box_center, box_extent;
				c->compute_bounding_box(*tree, &box_center, &box_extent);
				uint16_t desired_lod = tree->compute_lod(box_center, box_extent, viewpoint);
				//c->m_lod = iclamp(desired_lod, c->m_lod & 0xFF00, c->m_lod | 0x0FF);
			}}
		}

		// make sure ancestors are split...
		for (lod_chunk* p = m_parent; p && p->m_split == false; p = p->m_parent) {
			p->do_split(tree, viewpoint);
		}
	}
}


bool	lod_chunk::can_split(lod_chunk_tree* tree)
// Return true if this chunk can be split.  Also, requests the
// necessary data for the chunk children and its dependents.
//
// A chunk won't be able to be split if it doesn't have vertex data,
// or if any of the dependent chunks don't have vertex data.
{
	if (m_split) {
		// Already split.  Also our data & dependents' data is already
		// freshened, so no need to request it again.
		return true;
	}

	if (has_children() == false) {
		// Can't ever split.  No data to request.
		return false;
	}

	bool	can_split = true;

	// Check the data of the children.
	{for (int i = 0; i < 4; i++) {
		lod_chunk*	c = m_children[i];
		if (c->has_resident_data() == false) {
			//tree->m_loader->request_chunk_load(c, 1.0f);
			can_split = false;
		}
	}}

	// Make sure ancestors have data...
	for (lod_chunk* p = m_parent; p && p->m_split == false; p = p->m_parent) {
		if (p->can_split(tree) == false) {
			can_split = false;
		}
	}

	// Make sure neighbors have data at a close-enough level in the tree.
	{for (int i = 0; i < 4; i++) {
		lod_chunk*	n = m_neighbor[i].m_chunk;

		const int	MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE = 2;	// allow up to two levels of difference between chunk neighbors.
		{for (int count = 0;
		     n && count < MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE;
		     count++)
		{
			n = n->m_parent;
		}}

		if (n && n->can_split(tree) == false) {
			can_split = false;
		}
	}}

	return can_split;
}


void	lod_chunk::unload_data()
// Immediately unload our data.
{
	assert(m_parent != NULL && m_parent->m_split == false);
	assert(m_split == false);

	// debug check -- we should only unload data from the leaves
	// upward.
	if (has_children()) {
		for (int i = 0; i < 4; i++) {
			assert(m_children[i]->m_data == NULL);
		}
	}

	// Do the unloading.
	if (m_data) {
		delete m_data;
		m_data = NULL;
	}
}


void	lod_chunk::warm_up_data(lod_chunk_tree* tree, float priority)
// Schedule this node's data for loading at the given priority.  Also,
// schedule our child/descendent nodes for unloading.
{
	assert(tree);

	if (m_data == NULL)
	{
		// Request our data.
		//tree->m_loader->request_chunk_load(this, priority);
	}

	// Request unload.  Skip a generation if our priority is
	// fairly high.
	if (has_children()) {
		if (priority < 0.5f) {
			// Dump our child nodes' data.
			for (int i = 0; i < 4; i++) {
				m_children[i]->request_unload_subtree(tree);
			}
		}
		else
		{
			// Fairly high priority; leave our children
			// loaded, but dump our grandchildren's data.
			for (int i = 0; i < 4; i++) {
				lod_chunk*	c = m_children[i];
				if (c->has_children()) {
					for (int j = 0; j < 4; j++) {
						c->m_children[j]->request_unload_subtree(tree);
					}
				}
			}
		}
	}
}


void	lod_chunk::request_unload_subtree(lod_chunk_tree* tree)
// If we have any data, request that it be unloaded.  Make the same
// request of our descendants.
{
	if (m_data) {
		// Put descendents in the queue first, so they get
		// unloaded first.
		if (has_children()) {
			for (int i = 0; i < 4; i++) {
				m_children[i]->request_unload_subtree(tree);
			}
		}

		//tree->m_loader->request_chunk_unload(this);
	}
}

void	lod_chunk::lookup_neighbors(lod_chunk_tree* tree)
// Convert our neighbor labels to neighbor pointers.  Recurse to child chunks.
{
	for (int i = 0; i < 4; i++) {
		if (m_neighbor[i].m_label < -1 || m_neighbor[i].m_label >= tree->m_chunk_count)
		{
			assert(0);
			m_neighbor[i].m_label = -1;
		}
		if (m_neighbor[i].m_label == -1) {
			m_neighbor[i].m_chunk = NULL;
		} else {
			m_neighbor[i].m_chunk = tree->m_chunk_table[m_neighbor[i].m_label];
		}
	}

	if (has_children()) {
		{for (int i = 0; i < 4; i++) {
			m_children[i]->lookup_neighbors(tree);
		}}
	}
}

void lod_chunk::read(lod_chunk_tree* tree,int recurse_count) {
	m_split = false;
	m_data = NULL;

	//获取chunk编号
	int chunk_label; 
	//= READ;
	if(chunk_label > tree->m_chunk_count){
		assert(0);
	}
	tree->m_chunk_table[chunk_label] = this;
	for(int i = 0; i < 4; i++){
		//获取周围几个标签的数据
		m_neighbor[i].m_label = -1;
	}

	//获取chunk的位置
	m_level = 0;
	m_x = 0;
	m_z = 0;

	m_min_y = 0;
	m_max_y = 0;

	if (recurse_count > 0) {
		for (int i = 0; i < 4; i++) {
			m_children[i] = &tree->m_chunks[tree->m_chunks_allocated++];
			//子树的LOD层级要更加精细
			m_children[i]->m_lod = m_lod + 0x100;
			m_children[i]->m_parent = this;
			//获取子树的数据
			m_children[i]->read(tree, recurse_count - 1);
		}
	} else {
		for (int i = 0; i < 4; i++) {
			m_children[i] = NULL;
		}
	}
}

LOD::LOD(std::shared_ptr<Loader>loader):Accelerator(loader){
	// 读入设定参数
	// 暂时未确定如何读取
	m_tree_depth;
	m_error_LODmax;
	m_distance_LODmax;
	m_texture_distance_LODmax;
	m_vertical_scale;
	m_base_chunk_dimension;
	m_chunk_count;

	// 创建查找表
	m_chunk_table = new lod_chunk*[m_chunk_count];
	memset(m_chunk_table, 0, sizeof(m_chunk_table[0]) * m_chunk_count);

	// 创建chunk
	m_chunks_allocated = 0;
	m_chunks = new lod_chunk[m_chunk_count];

	m_chunks_allocated++;
	m_chunks[0].m_lod = 0;
	m_chunks[0].m_parent = 0;
	//这个initialize需要根据我们的数据格式完成
	m_chunks[0].read(this, m_tree_depth - 1);
	m_chunks[0].lookup_neighbors(this);

	// 创建loader
	m_loader = new chunk_tree_loader;
}

void LOD::onCameraCreate(std::shared_ptr<Camera>){
=======
void LOD::onCameraCreate(std::shared_ptr<Camera>) {
>>>>>>> 0cd6363a0a19dc139fca1955b83182ad9e67b373
    //TODO
}

void LOD::onCameraUpdate(std::shared_ptr<Camera>) {
    //TODO
}

std::shared_ptr<MeshData> LOD::simplify() {
    //TODO
    return NULL;
}

void LOD::build(std::shared_ptr<Camera>) {
    //TODO
}

bool LOD::covered(AABB, std::shared_ptr<Camera>) const {
    //TODO
    return false;
}