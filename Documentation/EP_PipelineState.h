#pragma once

#include "EP_Numerics.h"
#include "EP_Platform.h"
#include EP_PLATFORM_FILE(EP_PipelineState, .h)



// Forward declarations
class EP_Device;
class EP_ShaderSignature;
class EP_VertexShader;
class EP_PixelShader;
struct EP_RenderInputElement;

enum class EP_Topology;
enum class EP_TopologyFamily;
enum EP_RenderFormat;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Depth
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class EP_DepthFunc
{
    NEVER = 0,
    LESS = 1,
    EQUAL = 2,
    LESS_EQUAL = 3,
    GREATER = 4,
    NOT_EQUAL = 5,
    GREATER_EQUAL = 6,
    ALWAYS = 7
};
enum class EP_DepthStencilOp
{
    Keep = 0,
    Zero = 1,
    Replace =2,
    IncrSat = 3,
    DecrSat = 4,
    Invert = 5,
    Incr = 6,
    Decr = 7
};

struct EP_DepthStencilState
{
    Bool enableDepth;
    Bool enableStencil;

    // Depth specific
    Bool enableDepthWrite;
    EP_DepthFunc depthFunc;

    // Stencil specific
    uint8 stencilReadMask;
    uint8 stencilWriteMask;

    EP_DepthFunc frontStencilFunc;
    EP_DepthStencilOp frontStencilFailOp;
    EP_DepthStencilOp frontDepthFailOp;
    EP_DepthStencilOp frontPassOp;
    
    EP_DepthFunc backStencilFunc;
    EP_DepthStencilOp backStencilFailOp;
    EP_DepthStencilOp backDepthFailOp;
    EP_DepthStencilOp backPassOp;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Blend 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


enum class EP_BlendType
{
    Zero = 0,
    One = 1,

    SourceColor = 2,
    InverseSourceColor = 3,
    DestColor = 4,
    InverseDestColor = 5,

    SourceAlpha = 6,
    InverseSourceAlpha = 7,
    DestAlpha = 8,
    InverseDestAlpha = 9,

    BlendFactor = 10,
    InverseBlendFactor = 11
};
enum class EP_BlendOp
{
    Add = 0,
    Subtract = 1,
    Min = 2,
    Max = 3
};
enum class EP_BlendLogicOp
{
    Clear = 0,
    Set = 1,
    Copy = 2
};


struct EP_TargetBlendState
{
    Bool enableBlend;
    Bool enableLogicOp;

    EP_BlendType sourceBlend;
    EP_BlendType destBlend;
    EP_BlendOp blendOp;

    EP_BlendType sourceBlendAlpha;
    EP_BlendType destBlendAlpha;
    EP_BlendOp blendOpAlpha;

    EP_BlendLogicOp logicOp;

    uint8 writeMask = 0xFF;
};

struct EP_BlendState
{
    Bool alphaToCoverageEnabled;
    Bool independentBlendEnabled;
    const static uint32 NUM_TARGETS = 8;
    EP_TargetBlendState blendTarget[NUM_TARGETS];
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rasterizer
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class EP_FillMode
{
    Solid = 0,
    Wireframe = 1
};
enum class EP_CullMode
{
    Front = 0,
    Back = 1,
    None = 2
};


struct EP_RasterState
{
    EP_FillMode fillMode;
    EP_CullMode cullMode;

    Bool frontCounterClockwise;
    
    int32 depthBias;
    float32 depthBiasClamp;
    float32 slopeScaledDepthBias;

    Bool enableDepthClip;
    Bool enableMultisample;
    Bool enableAntialiasedLine;

    uint32 forcedSampleCount;
    Bool enableConservativeRaster;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Common states
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace CommonPipelineState
{
    static EP_RasterState DefaultRasterState{
        EP_FillMode::Solid,
        EP_CullMode::Back,
        true,
        0,
        0.f,
        0.f,
        false,
        false,
        false,
        0,
        true
    };

    static EP_BlendState DefaultBlendState{
        false,
        false,
        {{false, false, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendLogicOp::Clear, 0xFF},
        {false, false, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendLogicOp::Clear, 0xFF},
        {false, false, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendLogicOp::Clear, 0xFF},
        {false, false, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendLogicOp::Clear, 0xFF},
        {false, false, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendLogicOp::Clear, 0xFF},
        {false, false, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendLogicOp::Clear, 0xFF},
        {false, false, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendLogicOp::Clear, 0xFF},
        {false, false, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendType::Zero, EP_BlendType::Zero, EP_BlendOp::Add, EP_BlendLogicOp::Clear, 0xFF}
    } };

    static EP_DepthStencilState DefaultDepthState{
        true,
        false,
        true,
        EP_DepthFunc::LESS,
        0u,
        0u,
        EP_DepthFunc::LESS, EP_DepthStencilOp::Keep, EP_DepthStencilOp::Keep, EP_DepthStencilOp::Keep,
        EP_DepthFunc::LESS, EP_DepthStencilOp::Keep, EP_DepthStencilOp::Keep, EP_DepthStencilOp::Keep
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class EP_TopologyFamily
{
    Point = 0,
    Line = 1,
    Triangle = 2
};
enum class EP_Topology
{
    PointList = 0,

    LineList = 1,
    LineStrip = 2,

    TriangleList = 3,
    TriangleStrip = 4
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pipeline state
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct EP_PipelineStateDesc
{
    EP_ShaderSignature* ShaderSignature = nullptr; 

    EP_VertexShader* VertexShader = nullptr;
    EP_PixelShader* PixelShader = nullptr;
    // ^ ^ Other shaders here ^ ^ 

    EP_BlendState blendState = CommonPipelineState::DefaultBlendState;
    EP_RasterState rasterState = CommonPipelineState::DefaultRasterState;
    EP_DepthStencilState depthState = CommonPipelineState::DefaultDepthState;

    EP_RenderInputElement* pInputElements;
    uint32 numInputElements;

    EP_TopologyFamily topologyType;

    const static uint32 MAX_RENDER_TARGETS = 8;
    uint8 numRenderTargets = 1;
    EP_RenderFormat renderTargetFormats[MAX_RENDER_TARGETS];
    EP_RenderFormat dephtStencilFormat;
};


class EP_PipelineState
{
public:
    EP_PipelineState(EP_Device*, EP_PipelineStateDesc*);
    ~EP_PipelineState() {};

    inline EP_PipelineState_Platform& Platform() { return platform_; };
private:
    EP_PipelineState_Platform platform_;
};
