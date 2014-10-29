//***************************************************************************************
// RenderStates.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "RenderStates.h"

//TODO: Implement blend state struct
ID3D11BlendState*      RenderStates::TransparentBS = 0;
ID3D11RasterizerState* RenderStates::CCWcullMode = 0;
ID3D11RasterizerState* RenderStates::CWcullMode = 0;

////////////////////////////////////////////////////////////

void RenderStates::InitAll(ID3D11Device* device)
{


	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	HR(device->CreateRasterizerState(&cmdesc, &CCWcullMode));

	cmdesc.FrontCounterClockwise = false;
	HR(device->CreateRasterizerState(&cmdesc, &CWcullMode));

	//TODO: init a D3D11_BLEND_DESC
	D3D11_BLEND_DESC transparentWater = { 0 };

	//set blend state
	transparentWater.AlphaToCoverageEnable = false;
	transparentWater.IndependentBlendEnable = false;
	
	//set blend factors and alphas for source and destination pixels and set blend operation
	transparentWater.RenderTarget[0].BlendEnable = true;
	transparentWater.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentWater.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentWater.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentWater.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentWater.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentWater.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentWater.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	//TODO: call CreateBlendState
	HR(device->CreateBlendState(&transparentWater, &TransparentBS));

}

void RenderStates::DestroyAll()
{
	ReleaseCOM(TransparentBS);
	ReleaseCOM(CCWcullMode);
	ReleaseCOM(CWcullMode);
}