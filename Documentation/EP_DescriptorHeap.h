#pragma once

#include "EP_Numerics.h"
#include "EP_Assert.h"
#include "EP_Vector.h"

#include "../../Render/Common/EP_RingBuffer.h"
#include "EP_Device.h"
#include "EP_CommandQueue.h"

enum class EP_DescriptorType
{
    ConstBuff = 0,
    ShaderRes = 1,
    RenderTarget = 2,
    DepthStencil = 3,
    UnorderedAccess = 4,
    Sampler = 5
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct EP_DescriptorHandle
{
    D3D12_CPU_DESCRIPTOR_HANDLE hCpu;
    D3D12_GPU_DESCRIPTOR_HANDLE hGpu;
    uint64 count;
    bool valid = false;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class EP_StaticDescriptorHeap
{
public:
    EP_StaticDescriptorHeap(uint32 size);
    ~EP_StaticDescriptorHeap();

    uint64 Alloc(uint32 size);
    void Free(uint32 start, uint32 count);

private:
    inline void SetBit(uint32, bool);
    inline bool GetBit(uint32);

    Byte* pFlags_;
    uint32 totalSize_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EP_DescriptorHeap
{
public:
    EP_DescriptorHeap(EP_Device*, EP_CommandQueue*, EP_DescriptorType, bool shaderVisible = false, uint32 dynamicSize = DYNAMIC_SIZE, uint32 staticSize = STATIC_SIZE);
    ~EP_DescriptorHeap();

    void Copy(EP_DescriptorHandle from, EP_DescriptorHandle to, uint32 count);
    EP_DescriptorHandle Offset(EP_DescriptorHandle h, uint32 count);

    EP_DescriptorHandle AllocStatic(uint32 numDescs);
    void FreeStatic(EP_DescriptorHandle);

    EP_DescriptorHandle AllocDynamic(uint32 numDescs);
    void FinishFrame(uint64 fenceVal, uint64 lastFenceVal);

    inline ID3D12DescriptorHeap* Heap() {
        return pHeap_;
    }

private:
    static const uint32 HEAP_SIZE = 1024;
    static const uint32 DYNAMIC_SIZE = 512;
    static const uint32 STATIC_SIZE = 512;

    EP_Device* pDevice_;
    EP_CommandQueue* pCommQueue_;
    ID3D12DescriptorHeap* pHeap_;
    EP_Fence* pFence_;

    ep::render::RingBuffer dynamicAllocator_;
    EP_StaticDescriptorHeap staticAllocator_;

    D3D12_DESCRIPTOR_HEAP_TYPE heapType_;

    uint64 currentFence_;
    uint32 cachedDescriptorSize_;
   
    uint32 heapSize_;
    uint32 staticCount_;
    uint32 dynamicCount_;
};

