#pragma once

#include "EP_Numerics.h"
#include "EP_Platform.h"
#include EP_PLATFORM_FILE(EP_AMMemory, .h)

// Credit to Aaron MacDougall for the template and info

constexpr int64 mem_page_size = 64_KiB;												// this is the selected 'optimal' page size that balances cpu efficiency with memory efficiency
																					// cpu will page in 4KiB or 2MiB, so this is (under the hood) 16 pages for PC (not that this is something we need to consider, OS will handle it)
constexpr int64 mem_address_space = 944_GiB;										// we reserve near enough all vmem off the bat so we don't need to do it again
constexpr int64 mem_small_inc_size = 8_B;											
constexpr int64 mem_small_max_size = 4_KiB;											// the max size of an allocation to the small pool
constexpr int64 mem_small_pool_size = 32_MiB;										// 32MB in the small mem pool; this should be enough for all our bitty allocations, this is about 8000 allocations at max size worth
constexpr int64 mem_small_pool_count = mem_small_max_size / mem_small_inc_size;		// we have a lotttt of small pools
constexpr int64 mem_small_address_space = mem_small_pool_size * mem_small_pool_count;
constexpr int64 mem_large_pool_count = 20;

// Some systems require mapping memory with different modes	; Platform files should make an effort to clarify any odd cases
enum map_flags : uint32
{
	none				= 0,											// no map flags ; potential sign that nothing has been done

	cpu_bus				= 1 << 0,										// PS4 will need to specify CPU or GPU bus
	gpu_bus				= 1 << 1,										// ^ ^ ^
	
	cpu_read			= 1 << 2,
	cpu_write			= 1 << 3,
	cpu_rw				= cpu_read | cpu_write,
	
	gpu_read			= 1 << 4,
	gpu_write			= 1 << 5,
	gpu_rw				= gpu_read | gpu_write,

	internal_			= 1 << 6,
	debug				= 1 << 7
};


///////////////////////////////////////////////////////////////////////////////
// Setup and shutdown
///////////////////////////////////////////////////////////////////////////////
class mem_initializer
{
public:
	mem_initializer();
private:
	static int32 counter;
};
static mem_initializer mem_initer;

void mem_init();
void mem_term();

///////////////////////////////////////////////////////////////////////////////
// Request and return unmapped memory
///////////////////////////////////////////////////////////////////////////////
void* mem_reserve(uint64* size);
void mem_release(const void* addr);

///////////////////////////////////////////////////////////////////////////////
// Manually handle memory mapping
///////////////////////////////////////////////////////////////////////////////
void* mem_map(const void* addr, int64 size, map_flags flags);
void mem_unmap(const void* addr, int64 size);


void mem_annotate(const void* addr, int64 size, const char* label);				// ??? is this a debug thing? mark #ifdef debug


///////////////////////////////////////////////////////////////////////////////
// Easy memory interfaces
///////////////////////////////////////////////////////////////////////////////
void* mem_alloc_internal(uint64 size, uint64 alignment, bool clear, const char* function, const char* file, int64 line);			// 
void mem_free_internal(const void* ptr, const char* function, const char* file, int line);



#define mem_alloc(size)							mem_alloc_internal(size, 0, false, __FUNCTION__, __FILE__, __LINE__)
#define mem_alloc_type(type)					(type*)mem_alloc_internal(sizeof(type), alignof(type), false, __FUNCTION__, __FILE__, __LINE__)
#define mem_alloc_aligned(size, alignment)		mem_alloc_internal(size, alignment, false, __FUNCTION__, __FILE__, __LINE__)
#define mem_calloc(size)						mem_alloc_internal(size, alignment, true, __FUNCTION__, __FILE__, __LINE__)
#define mem_calloc_type(type)					(type*)mem_alloc_internal(size, alignment, true, __FUNCTION__, __FILE__, __LINE__)
#define mem_calloc_aligned(size, alignment)		mem_alloc_internal(size, alignment, true, __FUNCTION__, __FILE__, __LINE__)
#define mem_free(ptr)							mem_free_internal(ptr, __FUNCTION__, __FILE__, __LINE__)




