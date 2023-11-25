#include "LOD.h"
#include <thread>
#include <mutex>
#include <vector>
using std::vector;
using std::thread;
using std::mutex;

#ifdef __GNUC__
	// use the builtin (gcc) operator. ugly, but not my call.
	#define imax _max
	#define fmax _max
	#define _max(a,b) ((a)>?(b))
	#define imin _min
	#define fmin _min
	#define _min(a,b) ((a)<?(b))
#else // not GCC
	inline int	imax(int a, int b) { if (a < b) return b; else return a; }
	inline float	fmax(float a, float b) { if (a < b) return b; else return a; }
	inline int	imin(int a, int b) { if (a < b) return a; else return b; }
	inline float	fmin(float a, float b) { if (a < b) return a; else return b; }
#endif // not GCC

static inline int	iclamp(int i, int min, int max) {
	assert( min <= max );
	return imax(min, imin(i, max));
}

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
class chunk_tree_loader
{
public:
	chunk_tree_loader(lod_chunk_tree* tree);	// @@ should make tqt* a parameter.
	~chunk_tree_loader();

	void	sync_loader_thread();	// called by lod_chunk_tree::update(), to implement any changes that are ready.
	void	request_chunk_load(lod_chunk* chunk, float urgency);
	void	request_chunk_unload(lod_chunk* chunk);

	// Call this to enable/disable the background loader thread.
	void	set_use_loader_thread(bool use);

	int	loader_thread();		// thread function!
private:
	void	start_loader_thread();
	bool	loader_service_data();		// in loader thread
	
	struct pending_load_request
	{
		lod_chunk*	m_chunk;
		float	m_priority;

		pending_load_request() : m_chunk(NULL), m_priority(0.0f) {}
		
		pending_load_request(lod_chunk* chunk, float priority)
			: m_chunk(chunk), m_priority(priority)
		{
		}

		static int	compare(const void* r1, const void* r2)
		// Comparison function for qsort.  Sort based on priority.
		{
			float	p1 = ((pending_load_request*) r1)->m_priority;
			float	p2 = ((pending_load_request*) r2)->m_priority;
			
			if (p1 < p2) { return -1; }
			else if (p1 > p2) { return 1; }
			else { return 0; }
		}
	};

	struct retire_info
	// A struct that associates a chunk with its newly loaded
	// data.  For communicating between m_loader_thread and the
	// main thread.
	{
		lod_chunk*	m_chunk;
		lod_chunk_data*	m_chunk_data;

		retire_info() : m_chunk(0), m_chunk_data(0) {}
	};

	lod_chunk_tree*	m_tree;

	// These two are for the main thread's use only.  For
	// update()/update_texture() to communicate with
	// sync_loader_thread().
	vector<lod_chunk*>	m_unload_queue;
	vector<pending_load_request>	m_load_queue;

	vector<lod_chunk*>	m_unload_texture_queue;
	vector<lod_chunk*>	m_load_texture_queue;

	// These two are for the main thread to communicate with the
	// loader thread & vice versa.
#define REQUEST_BUFFER_SIZE 4
	lod_chunk* volatile	m_request_buffer[REQUEST_BUFFER_SIZE];	// chunks waiting to be loaded; filled will NULLs otherwise.
	retire_info volatile	m_retire_buffer[REQUEST_BUFFER_SIZE];	// chunks waiting to be united with their loaded data.

	// Loader thread stuff.
	thread m_loader_thread;
	volatile bool	m_run_loader_thread;	// loader thread watches for this to go false, then exits.
	mutex m_mutex;
};

chunk_tree_loader::chunk_tree_loader(lod_chunk_tree* tree)
	: m_tree(tree),
	  m_run_loader_thread(false)
{
	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
	{
		m_request_buffer[i] = NULL;
		m_retire_buffer[i].m_chunk = NULL;
	}
	start_loader_thread();
}

chunk_tree_loader::~chunk_tree_loader()
{
	// 停止所有线程
	set_use_loader_thread(false);
}

void	chunk_tree_loader::start_loader_thread()
// Initiate the loader thread, then return.
{
	m_run_loader_thread = true;

	// Thunk to wrap loader_thread(), which is a member fn.
	struct wrapper {
		static int	thread_wrapper(void* loader)
		{
			return ((chunk_tree_loader*) loader)->loader_thread();
		}
	};
	m_loader_thread = thread(wrapper::thread_wrapper, this);
}

void	chunk_tree_loader::set_use_loader_thread(bool use)
// Call this to enable/disable the use of a background loader thread.
// May take a moment of latency to return, since if the background
// thread is active, then this function has to signal it and wait for
// it to terminate.
{
	if (m_run_loader_thread) {
		if (use) {
			// We're already using the thread; nothing to do.
			return;
		}
		else {
			// Thread is running -- kill it.
			m_run_loader_thread = false;
			m_loader_thread.join();
			return;
		}
	}
	else {
		if (use == false) {
			// We're already not using the loader thread; nothing to do.
			return;
		} else {
			// Thread is not running -- start it up.
			start_loader_thread();
		}
	}
}

void	chunk_tree_loader::sync_loader_thread()
// Call this periodically, to implement previously requested changes
// to the lod_chunk_tree.  Most of the work in preparing changes is
// done in a background thread, so this call is intended to be
// low-latency.
//
// The chunk_tree_loader is not allowed to make any changes to the
// lod_chunk_tree, except in this call.
{
	// mutex section
	m_mutex.lock();
	{
		// Unload data.
		for (int i = 0; i < m_unload_queue.size(); i++) {
			lod_chunk*	c = m_unload_queue[i];
			// Only unload the chunk if it's not currently in use.
			// Sometimes a chunk will be marked for unloading, but
			// then is still being used due to a dependency in a
			// neighboring part of the hierarchy.  We want to
			// ignore the unload request in that case.
			if (c->m_parent != NULL
			    && c->m_parent->m_split == false)
			{
				c->unload_data();
			}
		}
		m_unload_queue.resize(0);

		// Retire any serviced requests.
		{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
		{
			retire_info&	r = const_cast<retire_info&>(m_retire_buffer[i]);	// cast away 'volatile' (we're inside the mutex section)
			if (r.m_chunk)
			{
				assert(r.m_chunk->m_data == NULL);

				if (r.m_chunk->m_parent != NULL
				    && r.m_chunk->m_parent->m_data == NULL)
				{
					// Drat!  Our parent data was unloaded, while we were
					// being loaded.  Only thing to do is discard the newly loaded
					// data, to avoid breaking an invariant.
					// (No big deal; this situation is rare.)
					delete r.m_chunk_data;
				}
				else
				{
					// Connect the chunk with its data!
					r.m_chunk->m_data = r.m_chunk_data;
				}
			}
			// Clear out this entry.
			r.m_chunk = NULL;
			r.m_chunk_data = NULL;
		}}

		//
		// Pass new data requests to the loader thread.  Go in
		// order of priority, and only take a few.
		//

		// Wipe out stale requests.
		{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
			m_request_buffer[i] = NULL;
		}}

		// Fill in new requests.
		int	qsize = m_load_queue.size();
		if (qsize > 0)
		{
			int	req_count = 0;

			// Sort by priority.
			qsort(&m_load_queue[0], qsize, sizeof(m_load_queue[0]), pending_load_request::compare);
			{for (int i = 0; i < qsize; i++)
			{
				lod_chunk*	c = m_load_queue[qsize - 1 - i].m_chunk;	// Do the higher priority requests first.
				// Must make sure the chunk wasn't just retired.
				if (c->m_data == NULL
				    && (c->m_parent == NULL || c->m_parent->m_data != NULL))
				{
					// Request this chunk.
					m_request_buffer[req_count++] = c;
					if (req_count >= REQUEST_BUFFER_SIZE) {
						// We've queued up enough requests.
						break;
					}
				}
			}}
		
			m_load_queue.resize(0);	// forget this frame's requests; we'll generate a fresh list during the next update()
		}
	}
	m_mutex.unlock();


	if (m_run_loader_thread == false)
	{
		// Loader thread is not actually running (at client
		// request, via set_use_loader_thread()), so instead,
		// service any requests synchronously, right now.
		int	count;
		for (count = 0; count < 4; count++) {
			bool	loaded = loader_service_data();
			if (loaded == false) break;
		}
	}
}

void	chunk_tree_loader::request_chunk_load(lod_chunk* chunk, float urgency)
// Request that the specified chunk have its data loaded.  May
// take a while; data doesn't actually show up & get linked in
// until some future call to sync_loader_thread().
{
	assert(chunk);
	assert(chunk->m_data == NULL);

	// Don't schedule for load unless our parent already has data.
	if (chunk->m_parent == NULL
	    || chunk->m_parent->m_data != NULL)
	{
		m_load_queue.push_back(pending_load_request(chunk, urgency));
		
	}
}


void	chunk_tree_loader::request_chunk_unload(lod_chunk* chunk)
// Request that the specified chunk have its data unloaded;
// happens within short latency.
{
	m_unload_queue.push_back(chunk);
}


int	chunk_tree_loader::loader_thread()
// Thread function for the loader thread.  Sit and load chunk data
// from the request queue, until we get killed.
{
	while (m_run_loader_thread == true)
	{
		bool	loaded = false;
		loaded = loader_service_data() || loaded;

		if (loaded == false)
		{
			// We seem to be dormant; sleep for a while
			// and then check again.
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	return 0;
}


bool	chunk_tree_loader::loader_service_data()
// Service a request for data.  Return true if we actually serviced
// anything; false if there was nothing to service.
{
	// Grab a request.
	lod_chunk*	chunk_to_load = NULL;
	m_mutex.lock();
	{
		// Get first request that's not already in the
		// retire buffer.
		for (int req = 0; req < REQUEST_BUFFER_SIZE; req++)
		{
			chunk_to_load = m_request_buffer[0];	// (could be NULL)

			// shift requests down.
			int	i;
			for (i = 0; i < REQUEST_BUFFER_SIZE - 1; i++)
			{
				m_request_buffer[i] = m_request_buffer[i + 1];
			}
			m_request_buffer[i] = NULL;	// fill empty slot with NULL

			if (chunk_to_load == NULL) break;
			
			// Make sure the request is not in the retire buffer.
			bool	in_retire_buffer = false;
			{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
				if (m_retire_buffer[i].m_chunk == chunk_to_load) {
					// This request has already been serviced.  Don't
					// service it again.
					chunk_to_load = NULL;
					in_retire_buffer = true;
					break;
				}
			}}
			if (in_retire_buffer == false) break;
		}
	}
	m_mutex.unlock();

	if (chunk_to_load == NULL)
	{
		// There's no request to service right now.
		return false;
	}

	assert(chunk_to_load->m_data == NULL);
	assert(chunk_to_load->m_parent == NULL || chunk_to_load->m_parent->m_data != NULL);
	
	// Service the request by loading the chunk's data.  This
	// could take a while, and involves waiting on IO, so we do it
	// with the mutex unlocked so the main update/render thread
	// can hopefully get some work done.
	lod_chunk_data*	loaded_data = NULL;


	//TODO 需要修改，修改读入数据方式。/
	// Geometry.
	
	// SDL_RWseek(m_source_stream, chunk_to_load->m_data_file_position, SEEK_SET);
	// loaded_data = new lod_chunk_data(m_source_stream);

	// "Retire" the request.  Must do this with the mutex locked.
	// The main thread will do "chunk_to_load->m_data = loaded_data".
	m_mutex.lock();
	{
		for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
		{
			if (m_retire_buffer[i].m_chunk == 0)
			{
				// empty slot; put the info here.
				m_retire_buffer[i].m_chunk = chunk_to_load;
				m_retire_buffer[i].m_chunk_data = loaded_data;
				break;
			}
		}
		// TODO: assert if we didn't find a retire slot!
		// (there should always be one, because it's as big as
		// the request queue)
	}
	m_mutex.unlock();

	return true;
}


/*
 ----------------------------------------------------------------------------------------------------
 ----------------------------------------------------------------------------------------------------
*/


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
			m_lod = iclamp(desired_lod, m_lod & 0xFF00, m_lod | 0x0FF);

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
				c->m_lod = iclamp(desired_lod, c->m_lod & 0xFF00, c->m_lod | 0x0FF);
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
			tree->m_loader->request_chunk_load(c, 1.0f);
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

		tree->m_loader->request_chunk_unload(this);
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

LOD::LOD():Accelerator(){
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
	m_loader = new chunk_tree_loader(this);
}


void LOD::onCameraUpdate(const Mesh& mesh,std::shared_ptr<Camera>) {
    //TODO
}

vector<Triangle> LOD::simplify() {
    //TODO

}

void LOD::build(std::shared_ptr<Camera>) {
    //TODO
}

bool LOD::covered() const {
    //TODO
    return false;
}

void	lod_chunk_tree::set_parameters(float max_pixel_error, float screen_width_pixels, float horizontal_FOV_degrees)
// Initializes some internal parameters which are used to compute
// which chunks are split during update().
//
// Given a screen width and horizontal field of view, the
// lod_chunk_tree when properly updated guarantees a screen-space
// vertex error of no more than max_pixel_error (at the center of the
// viewport) when rendered.
{
	assert(max_pixel_error > 0);
	assert(screen_width_pixels > 0);
	assert(horizontal_FOV_degrees > 0 && horizontal_FOV_degrees < 180.0f);

	const float	tan_half_FOV = tanf(0.5f * horizontal_FOV_degrees * (float) M_PI / 180.0f);
	const float	K = screen_width_pixels / tan_half_FOV;

	// distance_LODmax is the distance below which we need to be
	// at the maximum LOD.  It's used in compute_lod(), which is
	// called by the chunks during update().
	m_distance_LODmax = (m_error_LODmax / max_pixel_error) * K;
}

void	lod_chunk_tree::set_use_loader_thread(bool use)
// Enables or disables loading of chunk data in the background.
{
	m_loader->set_use_loader_thread(use);
}

uint16_t lod_chunk_tree::compute_lod(const vec3& center, const vec3& extent, const vec3& viewpoint) const
// Given an AABB and the viewpoint, this function computes a desired
// LOD level, based on the distance from the viewpoint to the nearest
// point on the box.  So, desired LOD is purely a function of
// distance and the chunk tree parameters.
{
	vec3	disp = viewpoint - center;
	disp[0] = std::max(0.0f, std::abs(disp[0]) - extent[0]);
	disp[1] = std::max(0.0f, std::abs(disp[1]) - extent[1]);
	disp[2] = std::max(0.0f, std::abs(disp[2]) - extent[2]);
//	disp.set(1, 0);	//xxxxxxx just do calc in 2D, for debugging

	float	d = 0;
	d = disp.norm();

	return iclamp(((m_tree_depth << 8) - 1) - int(log2(fmax(1, d / m_distance_LODmax)) * 256), 0, 0x0FFFF);
}
