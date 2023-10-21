#pragma once

#include "EP_Numerics.h"

// forward declare the 'customer-facing' class 
class EP_ScratchAllocator;

// Setup and shutdown of the allocator
void scratch_init(void);
void scratch_term(void);

// Make a new frame
void* scratch_push(void);
// Remove the current frame and continue from the end of prev frame
void scratch_pop();
// Reset to start of current frame without pop
void scratch_framereset();

// e.g
//   a           b       c           d
// | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - 
//										 ^
// d is curr frame, ^ is next mem to alloc

// After scratch_push()
//   a           b       c           d   e
// | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - 
//										 ^

// After scratch_pop()
//   a           b       c           
// | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - 
//								     ^

// After scratch_framereset()
//   a           b       c           d
// | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - | - 
//									 ^	 

void scratch_new_allocator(EP_ScratchAllocator* epAll);
void scratch_deconst_allocator(EP_ScratchAllocator* epAll);

void* scratch_alloc(uint64 size);
void* scrach_alloc_aligned(uint64 size, uint64 alignment);
void* scratch_calloc(uint64 size);
void* scratch_calloc_aligned(uint64 size, uint64 alignment);

template <typename T>
inline T* scratch_alloc(uint32 numEls)
{
    void* outMem = scratch_alloc(sizeof(T) * numEls);
    return (T*)outMem;
}
