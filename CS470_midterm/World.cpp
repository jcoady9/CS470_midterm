#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"

#include "World.h"

World::World(HINSTANCE hInstance)
: D3DApp(hInstance), mFloorVB(0), mFloorIB(0), mFloorMapSRV(0), mWallVB(0), mWallIB(0), mWallMapSRV(0), mWallVB_2(0), mWallIB_2(0), mWallVB_3(0), mWallIB_3(0), mWallVB_4(0), mWallIB_4(0),
mWaterMapSRV(0), mWaterVB(0), mWaterIB(0), mWaterTexAnimate(0.0f, 0.0f),
mCameraPos(0.0f, 50.0f, 8.0f), mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(2.5f)
{
	mMainWndCaption = L"CS470 Midterm";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mTableMeshFilename = "Meshes/table.m3d";

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mFloor, I);
	XMStoreFloat4x4(&mWall, I);
	XMStoreFloat4x4(&mWall_2, I);
	XMStoreFloat4x4(&mWall_3, I);
	XMStoreFloat4x4(&mWall_4, I);
	XMStoreFloat4x4(&mWater, I);
	XMStoreFloat4x4(&mFloorTexTransform, I);
	XMStoreFloat4x4(&mWallTexTransform, I);
	XMStoreFloat4x4(&mWaterTexTransform, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
	XMStoreFloat4x4(&mWVP, I);

	XMMATRIX scale, rotate, translate;
	XMVECTOR axis;

	//scale floor
	scale = XMMatrixScaling(7.0f, 0.2f, 7.0f);
	XMStoreFloat4x4(&mFloor, scale);

	//transform back-wall
	axis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	rotate = XMMatrixRotationAxis(axis, 80.0f);
	translate = XMMatrixTranslation(3.0f, 1.0f, 0.0f);
	scale = XMMatrixScaling(2.0f, 0.1f, 3.0f);
	XMStoreFloat4x4(&mWall, scale * rotate * translate);

	//transform wall2
	axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	rotate = XMMatrixRotationAxis(axis, 80.0f);
	translate = XMMatrixTranslation(2.0f, 1.0f, 1.5f);
	scale = XMMatrixScaling(2.0f, 0.1f, 2.0f);
	XMStoreFloat4x4(&mWall_2, scale * rotate * translate);

	//transform wall3
	axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	rotate = XMMatrixRotationAxis(axis, 1.57f);
	translate = XMMatrixTranslation(2.0f, 1.0f, -1.5f);
	scale = XMMatrixScaling(2.0f, 0.1f, 2.0f);
	XMStoreFloat4x4(&mWall_3, scale * rotate * translate);

	//transform roof
	translate = XMMatrixTranslation(2.0f, 2.0f, 0.0f);
	scale = XMMatrixScaling(2.0f, 0.1f, 3.0f);
	XMStoreFloat4x4(&mWall_4, scale * translate);

	//transform water
	//translate = XMMatrixTranslation(-2.3f, 0.2f, 0.0f);
	//scale = XMMatrixScaling(2.0f, 0.001f, 7.0f);
	//XMStoreFloat4x4(&mWater, scale * translate);

	mDirLights[0].Ambient  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);
 
	mDirLights[1].Ambient  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	mFloorMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mFloorMat.Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mFloorMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

	mWallMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mWallMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mWallMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

	mWaterMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mWaterMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mWaterMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

}

World::~World()
{
	ReleaseCOM(mWallVB);
	ReleaseCOM(mWallIB);
	ReleaseCOM(mWallVB_2);
	ReleaseCOM(mWallIB_2);
	ReleaseCOM(mWallVB_3);
	ReleaseCOM(mWallIB_3);
	ReleaseCOM(mWallIB_4);
	ReleaseCOM(mWallMapSRV);
	ReleaseCOM(mWaterVB);
	ReleaseCOM(mWaterIB);
	ReleaseCOM(mWaterMapSRV);
	ReleaseCOM(mFloorVB);
	ReleaseCOM(mFloorIB);
	ReleaseCOM(mFloorMapSRV);
	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool World::Init()
{
	if(!D3DApp::Init())
		return false;

	XMMATRIX tableScale = XMMatrixScaling(0.0f, 0.1f, 0.0f);
	XMStoreFloat4x4(&mTableMesh.World, tableScale);

	//mTableMesh.Init(md3dDevice, mTableMeshFilename);

	//mFXFileName = L"FX/meshfx.fx";
	//buildMeshFX();
	//buildVertexMeshLayouts();

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	//set texture for the floor
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/dirt.png", 0, 0, &mFloorMapSRV, 0));
	//set texture for the walls
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/darkbrick.bmp", 0, 0, &mWallMapSRV, 0));
	//set texture for water
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/water2.dds", 0, 0, &mWaterMapSRV, 0));
 
	buildBuffers();

	return true;
}

void World::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void World::UpdateScene(float dt)
{

	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	mCameraPos = XMFLOAT3(x, y, z);

	// Build the view matrix.
	XMVECTOR pos    = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up     = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	//animate water texture
	mWaterTexAnimate.x += 0.05*dt;
	mWaterTexAnimate.y += 0.05*dt;

	XMMATRIX waterTexTranslate = XMMatrixTranslation(0.0f, mWaterTexAnimate.y, mWaterTexAnimate.x);
	XMStoreFloat4x4(&mWaterTexTransform, waterTexTranslate);

}

void World::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;
 
	XMMATRIX view  = XMLoadFloat4x4(&mView);
	XMMATRIX proj  = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view*proj;

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mCameraPos);
 
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3TexTech;
    D3DX11_TECHNIQUE_DESC techDesc;

	activeTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mFloorVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mFloorIB, DXGI_FORMAT_R32_UINT, 0);

		// Draw the floor.
		XMMATRIX world = XMLoadFloat4x4(&mFloor);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mFloorTexTransform));
		Effects::BasicFX->SetMaterial(mFloorMat);
		Effects::BasicFX->SetDiffuseMap(mFloorMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
    }
	////////////////////////////////////////////////////////////////////////////////////////////////
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mWallVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mWallIB, DXGI_FORMAT_R32_UINT, 0);

		// draw wall
		XMMATRIX world = XMLoadFloat4x4(&mWall);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mWallTexTransform));
		Effects::BasicFX->SetMaterial(mWallMat);
		Effects::BasicFX->SetDiffuseMap(mWallMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mWallVB_2, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mWallIB_2, DXGI_FORMAT_R32_UINT, 0);

		// draw wall
		XMMATRIX world = XMLoadFloat4x4(&mWall_2);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mWallTexTransform));
		Effects::BasicFX->SetMaterial(mWallMat);
		Effects::BasicFX->SetDiffuseMap(mWallMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mWallVB_3, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mWallIB_3, DXGI_FORMAT_R32_UINT, 0);

		// draw wall
		XMMATRIX world = XMLoadFloat4x4(&mWall_3);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mWallTexTransform));
		Effects::BasicFX->SetMaterial(mWallMat);
		Effects::BasicFX->SetDiffuseMap(mWallMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mWallVB_4, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mWallIB_4, DXGI_FORMAT_R32_UINT, 0);

		// draw wall
		XMMATRIX world = XMLoadFloat4x4(&mWall_4);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mWallTexTransform));
		Effects::BasicFX->SetMaterial(mWallMat);
		Effects::BasicFX->SetDiffuseMap(mWallMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//draw water

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (UINT p = 0; p < techDesc.Passes; ++p){
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mWaterVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mWaterIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mWater);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mWaterTexTransform));
		Effects::BasicFX->SetMaterial(mWaterMat);
		Effects::BasicFX->SetDiffuseMap(mWaterMapSRV);

		//md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		md3dImmediateContext->RSSetState(RenderStates::CCWcullMode);
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, 0, 0);
		md3dImmediateContext->RSSetState(0);
		md3dImmediateContext->RSSetState(RenderStates::CWcullMode);
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, 0, 0);
		md3dImmediateContext->RSSetState(0);
		// Restore default blend state
		//md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	md3dImmediateContext->IASetInputLayout(mVertexLayout);

	XMMATRIX v = XMLoadFloat4x4(&mView);
	XMMATRIX p = XMLoadFloat4x4(&mProj);
	XMMATRIX VP = v*p;

	D3DX11_TECHNIQUE_DESC td;
	mTech->GetDesc(&td);
	for (UINT p = 0; p < td.Passes; p++){
		// Set modelview projection matrix for object
		XMMATRIX w = XMLoadFloat4x4(&mTableMesh.World);
		XMMATRIX wvp = w*VP;
		mFXMatVar->SetMatrix(reinterpret_cast<float*>(&VP));
		// Draw subsets
		for (UINT subset = 0; subset < mTableMesh.SubsetCount; ++subset){
			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			mTableMesh.MeshData.Draw(md3dImmediateContext, subset);
		}
	}
	*/
	HR(mSwapChain->Present(0, 0));
}

void World::buildMeshFX(){
	DWORD shaderFlags = 0;
#if defined(DEBUG) | defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//TODO: Create shader
	// Load effects file
	ID3D10Blob* shader = 0;
	ID3D10Blob* compilationErrors = 0;
	HR(D3DX11CompileFromFile(mFXFileName.c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &shader, &compilationErrors, 0));

	// TODO: Create effect from shader
	HR(D3DX11CreateEffectFromMemory(shader->GetBufferPointer(), shader->GetBufferSize(), 0, md3dDevice, &mFX));
	ReleaseCOM(shader);

	// Get technique from effect
	mTech = mFX->GetTechniqueByName("BasicTech");

	// Associate modelview-projection shader variable
	mFXMatVar = mFX->GetVariableByName("gWVP")->AsMatrix();
}

void World::buildVertexMeshLayouts(){
	D3DX11_PASS_DESC pd;
	mTech->GetPassByIndex(0)->GetDesc(&pd);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, pd.pIAInputSignature, pd.IAInputSignatureSize, &mVertexLayout));
}

void World::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void World::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void World::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around floor.
		mTheta += dx;
		mPhi   += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi-0.1f);
	}
	else if( (btnState & MK_RBUTTON) != 0 )
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.01f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.01f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 1.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void World::buildBuffers()
{
	
	GeometryGenerator::MeshData floor;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, floor);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;

	// Cache the index count of each object.
	mBoxIndexCount = floor.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset      = 0;
	
	UINT totalVertexCount = floor.Vertices.size();

	UINT totalIndexCount = mBoxIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for(size_t i = 0; i < floor.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos    = floor.Vertices[i].Position;
		vertices[k].Normal = floor.Vertices[i].Normal;
		vertices[k].Tex    = floor.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mFloorVB));
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mWallVB));
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mWallVB_2));
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mWallVB_3));
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mWallVB_4));
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mWaterVB));
	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), floor.Indices.begin(), floor.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mFloorIB));
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWallIB));
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWallIB_2));
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWallIB_3));
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWallIB_4));
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mWaterIB));

}
 
