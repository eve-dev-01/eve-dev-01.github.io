#include "EP_PipelineState_Platform.h"

#include "../Common/EP_PipelineState.h"
#include "../Common/EP_RenderInput.h"
#include "../Common/EP_Shaders.h"
#include "../Common/EP_ShaderSignature.h"
#include "../Common/EP_Device.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ID3D12PipelineState* MakePlatformState(ID3D12Device* pDevice, EP_PipelineStateDesc* pipelineStateDesc);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EP_PipelineState_Platform::EP_PipelineState_Platform(EP_Device* pDevice, EP_PipelineStateDesc* pPSODesc)
{
    ID3D12Device* pDevicePlat = pDevice->Platform().Device();
    pPipeState_ = MakePlatformState(pDevicePlat, pPSODesc);
    EP_ASSERT(pPipeState_);
}
EP_PipelineState_Platform::~EP_PipelineState_Platform()
{
    if (pPipeState_)
        pPipeState_->Release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef D3D12_BLEND EP_BlendType_Platform;
typedef D3D12_BLEND_OP EP_BlendOp_Platform;
typedef D3D12_LOGIC_OP EP_BlendLogicOp_Platform;

D3D12_BLEND blendTypeMap[] =
{
    D3D12_BLEND_ZERO,
    D3D12_BLEND_ONE,

    D3D12_BLEND_SRC_COLOR,
    D3D12_BLEND_INV_SRC_COLOR,
    D3D12_BLEND_DEST_COLOR,
    D3D12_BLEND_INV_DEST_COLOR,

    D3D12_BLEND_SRC_ALPHA,
    D3D12_BLEND_INV_SRC_ALPHA,
    D3D12_BLEND_DEST_ALPHA,
    D3D12_BLEND_INV_DEST_ALPHA,

    D3D12_BLEND_BLEND_FACTOR,
    D3D12_BLEND_INV_BLEND_FACTOR
};
D3D12_BLEND GetPlatformBlend(EP_BlendType B) { return blendTypeMap[(uint32)B]; };

D3D12_BLEND_OP blendOpTypeMap[] =
{
    D3D12_BLEND_OP_ADD,
    D3D12_BLEND_OP_SUBTRACT,
    D3D12_BLEND_OP_MIN,
    D3D12_BLEND_OP_MAX
};
D3D12_BLEND_OP GetPlatformBlendOp(EP_BlendOp B) { return blendOpTypeMap[(uint32)B]; };

D3D12_LOGIC_OP blendLogicOpTypeMap[] =
{
    D3D12_LOGIC_OP_CLEAR,
    D3D12_LOGIC_OP_SET,
    D3D12_LOGIC_OP_COPY
};
D3D12_LOGIC_OP GetPlatformBlendLogicOp(EP_BlendLogicOp B) { return blendLogicOpTypeMap[(uint32)B]; };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef D3D12_COMPARISON_FUNC EP_DepthFunc_Platform;
typedef D3D12_STENCIL_OP EP_DepthStencilOp_Platform;

D3D12_COMPARISON_FUNC depthFuncTypeMap[] =
{
    D3D12_COMPARISON_FUNC_NEVER,
    D3D12_COMPARISON_FUNC_LESS,
    D3D12_COMPARISON_FUNC_EQUAL,
    D3D12_COMPARISON_FUNC_LESS_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER,
    D3D12_COMPARISON_FUNC_NOT_EQUAL,
    D3D12_COMPARISON_FUNC_GREATER_EQUAL,
    D3D12_COMPARISON_FUNC_ALWAYS
};
D3D12_COMPARISON_FUNC GetPlatformDepthFunc(EP_DepthFunc D) { return depthFuncTypeMap[(uint32)D]; };

D3D12_STENCIL_OP depthStencilOpTypeMap[] =
{
    D3D12_STENCIL_OP_KEEP,
    D3D12_STENCIL_OP_ZERO,
    D3D12_STENCIL_OP_REPLACE,
    D3D12_STENCIL_OP_INCR_SAT,
    D3D12_STENCIL_OP_DECR_SAT,
    D3D12_STENCIL_OP_INVERT,
    D3D12_STENCIL_OP_INCR,
    D3D12_STENCIL_OP_DECR
};
D3D12_STENCIL_OP GetPlatformDepthStencilOp(EP_DepthStencilOp D) { return depthStencilOpTypeMap[(uint32)D]; };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef D3D12_PRIMITIVE_TOPOLOGY_TYPE EP_TopologyFamily_Platform;
typedef D3D12_PRIMITIVE_TOPOLOGY EP_Topology_Platform;

D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyFamilyTypeMap[] =
{
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
};
D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyToFamilyTypeMap[] =
{
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,

    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,

    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
};
D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPlatformTopologyFamily(EP_TopologyFamily T) { return topologyFamilyTypeMap[(uint32)T]; };
D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPlatformTopologyFamily(EP_Topology T) { return topologyToFamilyTypeMap[(uint32)T]; };

D3D12_PRIMITIVE_TOPOLOGY topologyTypeMap[] =
{
    D3D_PRIMITIVE_TOPOLOGY_POINTLIST,

    D3D_PRIMITIVE_TOPOLOGY_LINELIST,
    D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,

    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
};
D3D12_PRIMITIVE_TOPOLOGY GetPlatformTopology(EP_Topology T) { return topologyTypeMap[(uint32)T]; };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ID3D12PipelineState* MakePlatformState(ID3D12Device* pDevice, EP_PipelineStateDesc* pipelineStateDesc)
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC platformDesc;
    ZeroMemory(&platformDesc, sizeof(platformDesc));

    platformDesc.pRootSignature = pipelineStateDesc->ShaderSignature->Platform().D3D12RootSig();

	// Translate shader data
    if (pipelineStateDesc->VertexShader)
    {
        platformDesc.VS = {
            pipelineStateDesc->VertexShader->Platform().GetShaderCode().pShaderBytecode,
            pipelineStateDesc->VertexShader->Platform().GetShaderCode().BytecodeLength
        };
    }
    if (pipelineStateDesc->PixelShader)
    {
        platformDesc.PS = {
            pipelineStateDesc->PixelShader->Platform().GetShaderCode().pShaderBytecode,
            pipelineStateDesc->PixelShader->Platform().GetShaderCode().BytecodeLength
        };
    }

	// Translate the blend state
    {
        D3D12_BLEND_DESC& platformBlend = platformDesc.BlendState;
        EP_BlendState& blendState = pipelineStateDesc->blendState;

        platformBlend.AlphaToCoverageEnable = blendState.alphaToCoverageEnabled;
        platformBlend.IndependentBlendEnable = blendState.independentBlendEnabled;

        for (uint32 i = 0; i < EP_BlendState::NUM_TARGETS; ++i)
        {
            EP_TargetBlendState& targetBlendState = blendState.blendTarget[i];
            D3D12_RENDER_TARGET_BLEND_DESC& platformTargetBlend = platformBlend.RenderTarget[i];

            platformTargetBlend.BlendEnable = targetBlendState.enableBlend;
            platformTargetBlend.LogicOpEnable = targetBlendState.enableLogicOp;
            platformTargetBlend.SrcBlend = GetPlatformBlend(targetBlendState.sourceBlend);
            platformTargetBlend.DestBlend = GetPlatformBlend(targetBlendState.destBlend);
            platformTargetBlend.BlendOp = GetPlatformBlendOp(targetBlendState.blendOp);
            platformTargetBlend.SrcBlendAlpha = GetPlatformBlend(targetBlendState.sourceBlendAlpha);
            platformTargetBlend.DestBlendAlpha = GetPlatformBlend(targetBlendState.destBlendAlpha);
            platformTargetBlend.BlendOpAlpha = GetPlatformBlendOp(targetBlendState.blendOpAlpha);
            platformTargetBlend.LogicOp = GetPlatformBlendLogicOp(targetBlendState.logicOp);
            platformTargetBlend.RenderTargetWriteMask = 0x0F;
        };
    }

	// Translate the raster state
    {
        D3D12_RASTERIZER_DESC& platformRaster = platformDesc.RasterizerState;
        EP_RasterState& rasterState = pipelineStateDesc->rasterState;

        platformRaster.FillMode = rasterState.fillMode == EP_FillMode::Solid ? D3D12_FILL_MODE_SOLID : D3D12_FILL_MODE_WIREFRAME;
        platformRaster.CullMode = rasterState.cullMode == EP_CullMode::Back ? D3D12_CULL_MODE_BACK : rasterState.cullMode == EP_CullMode::Front ? D3D12_CULL_MODE_FRONT : D3D12_CULL_MODE_NONE;
        platformRaster.FrontCounterClockwise = rasterState.frontCounterClockwise;
        platformRaster.DepthBias = rasterState.depthBias;
        platformRaster.DepthBiasClamp = rasterState.depthBiasClamp;
        platformRaster.SlopeScaledDepthBias = rasterState.slopeScaledDepthBias;
        platformRaster.DepthClipEnable = rasterState.enableDepthClip;
        platformRaster.MultisampleEnable = rasterState.enableMultisample;
        platformRaster.AntialiasedLineEnable = rasterState.enableAntialiasedLine;
        platformRaster.ForcedSampleCount = rasterState.forcedSampleCount;
        platformRaster.ConservativeRaster = rasterState.enableConservativeRaster ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    }

	// Translate the depth stencil state
    {
        D3D12_DEPTH_STENCIL_DESC& platformDepth = platformDesc.DepthStencilState;
        EP_DepthStencilState& depthState = pipelineStateDesc->depthState;

        platformDepth.DepthEnable = depthState.enableDepth;
        platformDepth.StencilEnable = depthState.enableStencil;
        platformDepth.DepthWriteMask = depthState.enableDepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        platformDepth.DepthFunc = GetPlatformDepthFunc(depthState.depthFunc);
        platformDepth.StencilReadMask = depthState.stencilReadMask;
        platformDepth.StencilWriteMask = depthState.stencilWriteMask;

        platformDepth.FrontFace.StencilFunc = GetPlatformDepthFunc(depthState.frontStencilFunc);
        platformDepth.FrontFace.StencilFailOp = GetPlatformDepthStencilOp(depthState.frontStencilFailOp);
        platformDepth.FrontFace.StencilDepthFailOp = GetPlatformDepthStencilOp(depthState.frontDepthFailOp);
        platformDepth.FrontFace.StencilPassOp = GetPlatformDepthStencilOp(depthState.frontPassOp);

        platformDepth.BackFace.StencilFunc = GetPlatformDepthFunc(depthState.backStencilFunc);
        platformDepth.BackFace.StencilFailOp = GetPlatformDepthStencilOp(depthState.backStencilFailOp);
        platformDepth.BackFace.StencilDepthFailOp = GetPlatformDepthStencilOp(depthState.backDepthFailOp);
        platformDepth.BackFace.StencilPassOp = GetPlatformDepthStencilOp(depthState.backPassOp);
    }

	// Handle small assorted bits
    platformDesc.PrimitiveTopologyType = GetPlatformTopologyFamily(pipelineStateDesc->topologyType);
    platformDesc.NumRenderTargets = pipelineStateDesc->numRenderTargets;
    platformDesc.DSVFormat = GetPlatformFormat(pipelineStateDesc->dephtStencilFormat);
    for (uint32 i = 0; i < platformDesc.NumRenderTargets; ++i)
    {
        platformDesc.RTVFormats[i] = GetPlatformFormat(pipelineStateDesc->renderTargetFormats[i]);
    }


	// Translate the render input
    const uint32 numInEls = pipelineStateDesc->numInputElements;
    D3D12_INPUT_ELEMENT_DESC* pInputEls = new D3D12_INPUT_ELEMENT_DESC[numInEls];
    platformDesc.InputLayout.NumElements = numInEls;
    platformDesc.InputLayout.pInputElementDescs = pInputEls;
    for (uint32 i = 0; i < numInEls; ++i)
    {
        D3D12_INPUT_ELEMENT_DESC* pEl = pInputEls + i;
        EP_RenderInputElement* pInEl = pipelineStateDesc->pInputElements + i;

        pEl->SemanticName = pInEl->semanticName;
        pEl->SemanticIndex = pInEl->semanticIdx;
        pEl->Format = GetPlatformFormat(pInEl->format);
        pEl->InputSlot = pInEl->inputSlot;
        pEl->InputSlotClass =
            pInEl->inputClass == PerVertex ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA :
            D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        pEl->InstanceDataStepRate = pInEl->instanceDataStep;
        pEl->AlignedByteOffset = pInEl->byteOffset;
    }

    platformDesc.SampleDesc.Count = 1;
    platformDesc.SampleDesc.Quality = 0;
    platformDesc.SampleMask = UINT_MAX;
    platformDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    EP_ASSERT(pDevice != nullptr, "Need valid device for creating PSOs");
    
	// Actually create the pipeline state
    ID3D12PipelineState* pPipeState = nullptr;
    HRESULT hr = pDevice->CreateGraphicsPipelineState(
        &platformDesc,
        IID_PPV_ARGS(&pPipeState)
    );
    EP_ASSERT(SUCCEEDED(hr));
    EP_ASSERT(pPipeState != nullptr);


    delete[] pInputEls;
    return pPipeState;
}