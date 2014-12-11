#include "World.h"

static float moveUp[3] = {0.0f, 20.0f, 00.f};

World::World(HINSTANCE hInstance) : D3DApp(hInstance), mSky(0), mDirtTex(0), mBrickTex(0), mWaterTex(0), mRandomTexSRV(0), mFlareTexSRV(0), mRainTexSRV(0), 
mWalkCamMode(false), mWaterTexAnimate(0.0f, 0.0f), mBrickNormalMap(0), mWaterNormalMap(0), mVertexLayout(0), mSmap(0)
{
	mMainWndCaption = L"CS470 Midterm";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mCam.SetPosition(-7.0f, 2.0f + moveUp[1], 15.0f);
	XMFLOAT3 target = XMFLOAT3(0.0f, moveUp[1], 0.0f);
	mCam.LookAt(mCam.GetPosition(), target, mCam.GetUp());

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

	//compute and set the transformations of stationary objects while initializing
	initialTransformations();

	mDirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.7f, 0.7f, 0.6f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, 0.0f, 0.707f);

	mDirtMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirtMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirtMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWaterMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mWaterMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	mWaterMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);

	mBrickMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mBrickMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mBrickMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);

}

World::~World(){
	md3dImmediateContext->ClearState();
	ReleaseCOM(mDirtTex);
	ReleaseCOM(mBrickTex);
	ReleaseCOM(mWaterTex);
	ReleaseCOM(mRandomTexSRV);
	ReleaseCOM(mFlareTexSRV);
	ReleaseCOM(mRainTexSRV);
	ReleaseCOM(mBrickNormalMap);
	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
	SafeDelete(mSky);
	SafeDelete(mSmap);
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

bool World::Init(){
	if (!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);

	//set texture for the floor
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/dirt.png", 0, 0, &mDirtTex, 0));
	//set texture for the walls
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/darkbrick.bmp", 0, 0, &mBrickTex, 0));
	//set texture for water
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/water2.dds", 0, 0, &mWaterTex, 0));

	//set normal map for brick shelter
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice, L"Textures/brickNormalMap.dds", 0, 0, &mBrickNormalMap, 0));

	mTable.Init(md3dDevice, "Meshes/table.m3d");

	mSmap = new ShadowMap(md3dDevice, SMapSize, SMapSize);

	mSky = new Sky(md3dDevice, L"Textures/desertcube1024.dds", 100.0f);

	Terrain::InitInfo volcanoInitInfo;
	volcanoInitInfo.HeightMapFilename = L"Textures/volcano_terrain_mask.dds";
	volcanoInitInfo.LayerMapFilename0 = L"Textures/darkdirt.dds";
	volcanoInitInfo.LayerMapFilename1 = L"Textures/darkdirt.dds";
	volcanoInitInfo.LayerMapFilename2 = L"Textures/stone.dds";
	volcanoInitInfo.LayerMapFilename3 = L"Textures/darkdirt.dds";
	volcanoInitInfo.LayerMapFilename4 = L"Textures/darkdirt.dds";
	volcanoInitInfo.BlendMapFilename = L"Textures/blend.dds";
	volcanoInitInfo.HeightScale = 50.0f;
	volcanoInitInfo.HeightmapWidth = 513;
	volcanoInitInfo.HeightmapHeight = 513;
	volcanoInitInfo.CellSpacing = 0.5f;

	//initialize terrain
	volcanoTerrain.Init(md3dDevice, md3dImmediateContext, volcanoInitInfo);
	
	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);
	
	//set textures for particles
	std::vector<std::wstring> raindrops;
	raindrops.push_back(L"Textures/raindrop.dds");

	std::vector<std::wstring> flares;
	flares.push_back(L"Textures/flare0.dds");

	mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, flares);
	mFlareTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, flares);

	//initialize fire
	for (int i = 0; i < numFires; i++)
		volcanoFire[i].Init(md3dDevice, Effects::FireFX, mFlareTexSRV, mRandomTexSRV, 500);
	//set position for all the fires
	float firePos[3] = { 47.0f, 9.7f + moveUp[1], -18.0f };
	volcanoFire[0].SetEmitPos(XMFLOAT3(firePos[0], firePos[1], firePos[2]));
	volcanoFire[1].SetEmitPos(XMFLOAT3(firePos[0] + 10.0f, firePos[1], firePos[2] - 10.0f ));
	volcanoFire[2].SetEmitPos(XMFLOAT3(5.0f + firePos[0], firePos[1], firePos[2] - 5.0f));

	//initialize raining fire
	mRain.Init(md3dDevice, Effects::RainFX, mRainTexSRV, mRandomTexSRV, 10000);

	buildMeshFX();
	buildVertexAndIndexBuffers();

	return true;
}

void World::OnResize(){
	D3DApp::OnResize();
	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 3000.0f);
}

void World::UpdateScene(float dt){
	// Control the camera.
	if (GetAsyncKeyState('W') & 0x8000)
		mCam.Walk(10.0f*dt);
	if (GetAsyncKeyState('S') & 0x8000)
		mCam.Walk(-10.0f*dt);
	if (GetAsyncKeyState('A') & 0x8000)
		mCam.Strafe(-10.0f*dt);
	if (GetAsyncKeyState('D') & 0x8000)
		mCam.Strafe(10.0f*dt);

	// Reset particle systems.
	if (GetAsyncKeyState('R') & 0x8000){
		for (int i = 0; i < numFires; i++)
			volcanoFire[i].Reset();
		mRain.Reset();
	}

	//animate water texture
	mWaterTexAnimate.x += 0.05f*dt;
	mWaterTexAnimate.y += 0.05f*dt;
	XMMATRIX waterTexTranslate = XMMatrixTranslation(0.0f, mWaterTexAnimate.y, mWaterTexAnimate.x);
	XMStoreFloat4x4(&mWaterTexTransform, waterTexTranslate);

	for (int i = 0; i < numFires; i++)
		volcanoFire[i].Update(dt, mTimer.TotalTime());
	mRain.Update(dt, mTimer.TotalTime());

	//buildShadow();

	mCam.UpdateViewMatrix();
}

void World::DrawScene(){

	mSmap->BindDsvAndSetNullRenderTarget(md3dImmediateContext);

	//drawShadowMap();

	ID3D11RenderTargetView* renderTargets[1] = { mRenderTargetView };
	md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);

	//draw mesh table
	mFXMatVar->SetMatrix(reinterpret_cast<float*>(&mCam.ViewProj()));
	for (UINT subset = 0; subset < mTable.SubsetCount; ++subset){
		mTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
		mTable.MeshData.Draw(md3dImmediateContext, subset);
	} 

	UINT stride = sizeof(Vertex::PosNormalTexTan);
	UINT offset = 0;

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//set basic effects
	Effects::DisplacementMapFX->SetDirLights(mDirLights);
	Effects::DisplacementMapFX->SetEyePosW(mCam.GetPosition());
	Effects::DisplacementMapFX->SetFogColor(Colors::Silver);
	Effects::DisplacementMapFX->SetFogStart(3.0f);
	Effects::DisplacementMapFX->SetFogRange(75.0f);

	//set displacement map effects
	Effects::DisplacementMapFX->SetDirLights(mDirLights);
	Effects::DisplacementMapFX->SetEyePosW(mCam.GetPosition());
	Effects::DisplacementMapFX->SetCubeMap(mSky->CubeMapSRV());

	Effects::DisplacementMapFX->SetHeightScale(0.05f);
	Effects::DisplacementMapFX->SetMaxTessDistance(1.0f);
	Effects::DisplacementMapFX->SetMinTessDistance(5.0f);
	Effects::DisplacementMapFX->SetMinTessFactor(1.0f);
	Effects::DisplacementMapFX->SetMaxTessFactor(5.0f);

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);

	drawObject(mFloor, mDirtTex, mFloorTexTransform, mDirtMat, mBrickNormalMap);
	drawObject(mWall, mBrickTex, mWallTexTransform, mBrickMat, mBrickNormalMap);
	drawObject(mWall_2, mBrickTex, mWallTexTransform, mBrickMat, mBrickNormalMap);
	drawObject(mWall_3, mBrickTex, mWallTexTransform, mBrickMat, mBrickNormalMap);
	drawObject(mWall_4, mBrickTex, mWallTexTransform, mBrickMat, mBrickNormalMap);
	drawObject(mWater, mWaterTex, mWaterTexTransform, mWaterMat, mBrickNormalMap);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	volcanoTerrain.Draw(md3dImmediateContext, mCam, mDirLights);
	
	mSky->Draw(md3dImmediateContext, mCam);

	//draw fire particle system
	for (int i = 0; i < numFires; i++){
		volcanoFire[i].SetEyePos(mCam.GetPosition());
		volcanoFire[i].Draw(md3dImmediateContext, mCam);
	}

	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	
	//draw rain particle system
	mRain.SetEyePos(mCam.GetPosition());
	mRain.SetEmitPos(mCam.GetPosition());
	mRain.Draw(md3dImmediateContext, mCam);
	
	// restore default states.
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	md3dImmediateContext->PSSetShaderResources(0, 16, nullSRV);
	
	HR(mSwapChain->Present(0, 0));
}

void World::drawObject(XMFLOAT4X4& objWorld, ID3D11ShaderResourceView* objTexture, XMFLOAT4X4& textureTransform, Material& mat, ID3D11ShaderResourceView* normalMap){
	ID3DX11EffectTechnique* activeTech = Effects::DisplacementMapFX->Light3TexFogTech;
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	
	// Set per object constants.
	XMMATRIX world = XMLoadFloat4x4(&objWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

	Effects::DisplacementMapFX->SetWorld(world);
	Effects::DisplacementMapFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::DisplacementMapFX->SetViewProj(mCam.ViewProj());
	Effects::DisplacementMapFX->SetWorldViewProj(world * mCam.ViewProj());
	Effects::DisplacementMapFX->SetTexTransform(XMLoadFloat4x4(&textureTransform));
	Effects::DisplacementMapFX->SetMaterial(mat);
	Effects::DisplacementMapFX->SetDiffuseMap(objTexture);
	Effects::DisplacementMapFX->SetNormalMap(normalMap);

	activeTech->GetPassByIndex(0)->Apply(0, md3dImmediateContext);
	md3dImmediateContext->DrawIndexed(mBoxIndexCount, 0, 0);
}

void World::buildVertexAndIndexBuffers(){

	GeometryGenerator::MeshData floor;
	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, floor);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	// Cache the index count of each object.
	mBoxIndexCount = floor.Indices.size();
	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;

	UINT totalVertexCount = floor.Vertices.size();
	UINT totalIndexCount = mBoxIndexCount;

	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	std::vector<Vertex::PosNormalTexTan> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < floor.Vertices.size(); ++i, ++k){
		vertices[k].Pos = floor.Vertices[i].Position;
		vertices[k].Normal = floor.Vertices[i].Normal;
		vertices[k].Tex = floor.Vertices[i].TexC;
		//vertices[k].TangentU = floor.Vertices[i].TangentU;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVertexBuffer));

	// Pack the indices of all the meshes into one index buffer.
	std::vector<UINT> indices;
	indices.insert(indices.end(), floor.Indices.begin(), floor.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIndexBuffer));

}

void World::initialTransformations(){
	XMMATRIX scale, rotate, translate;
	XMVECTOR axis;

	//scale floor
	scale = XMMatrixScaling(7.0f, 0.2f, 7.0f);
	translate = XMMatrixTranslation(moveUp[0], moveUp[1], moveUp[2]);
	XMStoreFloat4x4(&mFloor, scale * translate);

	//transform back-wall
	axis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	rotate = XMMatrixRotationAxis(axis, 80.0f);
	translate = XMMatrixTranslation(3.0f, 1.0f + moveUp[1], 0.0f);
	scale = XMMatrixScaling(2.0f, 0.1f, 3.0f);
	XMStoreFloat4x4(&mWall, scale * rotate * translate);

	//transform wall2
	axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	rotate = XMMatrixRotationAxis(axis, 80.0f);
	translate = XMMatrixTranslation(2.0f, 1.0f + moveUp[1], 1.5f);
	scale = XMMatrixScaling(2.0f, 0.1f, 2.0f);
	XMStoreFloat4x4(&mWall_2, scale * rotate * translate);

	//transform wall3
	axis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	rotate = XMMatrixRotationAxis(axis, 1.57f);
	translate = XMMatrixTranslation(2.0f, 1.0f + moveUp[1], -1.5f);
	scale = XMMatrixScaling(2.0f, 0.1f, 2.0f);
	XMStoreFloat4x4(&mWall_3, scale * rotate * translate);

	//transform roof
	translate = XMMatrixTranslation(2.0f, 2.0f + moveUp[1], 0.0f);
	scale = XMMatrixScaling(2.0f, 0.1f, 3.0f);
	XMStoreFloat4x4(&mWall_4, scale * translate);

	//transform water
	translate = XMMatrixTranslation(-2.3f, 0.2f + moveUp[1], 0.0f);
	scale = XMMatrixScaling(2.0f, 0.001f, 7.0f);
	XMStoreFloat4x4(&mWater, scale * translate);

	//transform volcano terrain
	translate = XMMatrixTranslation(0.0f, -20.0f, 0.0f);
	volcanoTerrain.SetWorld(translate);

	translate = XMMatrixTranslation(0.0f, moveUp[1] + 5.0f, 0.0f);
	axis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	rotate = XMMatrixRotationAxis(axis, 1.57f);
	XMStoreFloat4x4(&mTable.World, translate * rotate);

}

void World::buildMeshFX()
{
	// Set shader flags
	DWORD shaderFlags = 0;
#if defined(DEBUG) | defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	//TODO: Create shader
	// Load effects file
	ID3D10Blob* shader = 0;
	ID3D10Blob* compilationErrors = 0;
	HR(D3DX11CompileFromFile(L"FX/meshfx.fx", 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &shader, &compilationErrors, 0));

	// TODO: Create effect from shader
	HR(D3DX11CreateEffectFromMemory(shader->GetBufferPointer(), shader->GetBufferSize(), 0, md3dDevice, &mFX));
	ReleaseCOM(shader);

	// TODO: Set technique and associate variables
	// Get technique from effect
	mTech = mFX->GetTechniqueByName("BasicTech");

	// Associate modelview-projection shader variable
	mFXMatVar = mFX->GetVariableByName("gWVP")->AsMatrix();
}

void World::buildVertexLayouts(){

	const D3D11_INPUT_ELEMENT_DESC vertexDesc[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3DX11_PASS_DESC pd;
	mTech->GetPassByIndex(0)->GetDesc(&pd);
	HR(md3dDevice->CreateInputLayout(vertexDesc, 2, pd.pIAInputSignature, pd.IAInputSignatureSize, &mVertexLayout));
}

void World::drawShadowMap()
{
	XMMATRIX view = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(mCam.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	// These properties could be set per object if needed.
	Effects::BuildShadowMapFX->SetHeightScale(0.07f);
	Effects::BuildShadowMapFX->SetMaxTessDistance(1.0f);
	Effects::BuildShadowMapFX->SetMinTessDistance(25.0f);
	Effects::BuildShadowMapFX->SetMinTessFactor(1.0f);
	Effects::BuildShadowMapFX->SetMaxTessFactor(5.0f);

	ID3DX11EffectTechnique* tessSmapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ID3DX11EffectTechnique* smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	tessSmapTech = Effects::BuildShadowMapFX->TessBuildShadowMapTech;


}

void World::buildShadow()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&mDirLights[0].Direction);
	XMVECTOR lightPos = -2.0f*mSceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void World::OnMouseDown(WPARAM btnState, int x, int y){
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void World::OnMouseUp(WPARAM btnState, int x, int y){
	ReleaseCapture();
}

void World::OnMouseMove(WPARAM btnState, int x, int y){
	if ((btnState & MK_LBUTTON) != 0){
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
