#include "World.h"

World::World(HINSTANCE hInstance) : D3DApp(hInstance), mSky(0), mDirtTex(0), mBrickTex(0), mWaterTex(0), mRandomTexSRV(0), mFlareTexSRV(0), mRainTexSRV(0), 
mWalkCamMode(false), mWaterTexAnimate(0.0f, 0.0f)
{
	mMainWndCaption = L"CS470 Midterm";
	mEnable4xMsaa = false;

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mCam.SetPosition(0.0f, 2.0f, 100.0f);

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

	initialTransformations();

	mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);

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
	SafeDelete(mSky);
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

	mSky = new Sky(md3dDevice, L"Textures/grasscube1024.dds", 10.0f);
	/*
	Terrain::InitInfo tii;
	tii.HeightMapFilename = L"Textures/terrain.raw";
	tii.LayerMapFilename0 = L"Textures/grass.dds";
	tii.LayerMapFilename1 = L"Textures/darkdirt.dds";
	tii.LayerMapFilename2 = L"Textures/stone.dds";
	tii.LayerMapFilename3 = L"Textures/lightdirt.dds";
	tii.LayerMapFilename4 = L"Textures/snow.dds";
	tii.BlendMapFilename = L"Textures/blend.dds";
	tii.HeightScale = 50.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 0.5f;

	mTerrain.Init(md3dDevice, md3dImmediateContext, tii);

	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);
	*/
	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\flare0.dds");
	mFlareTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, flares);

	mFire.Init(md3dDevice, Effects::FireFX, mFlareTexSRV, mRandomTexSRV, 500);
	mFire.SetEmitPos(XMFLOAT3(0.0f, 1.0f, 120.0f));

	std::vector<std::wstring> raindrops;
	raindrops.push_back(L"Textures\\raindrop.dds");
	mRainTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, raindrops);

	mRain.Init(md3dDevice, Effects::RainFX, mRainTexSRV, mRandomTexSRV, 10000);

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

	// Walk/fly mode
	if (GetAsyncKeyState('2') & 0x8000)
		mWalkCamMode = true;
	if (GetAsyncKeyState('3') & 0x8000)
		mWalkCamMode = false;

	// Clamp camera to terrain surface in walk mode.
	if (mWalkCamMode){
		XMFLOAT3 camPos = mCam.GetPosition();
		float y = mTerrain.GetHeight(camPos.x, camPos.z);
		mCam.SetPosition(camPos.x, y + 2.0f, camPos.z);
	}

	// Reset particle systems.
	if (GetAsyncKeyState('R') & 0x8000){
		mFire.Reset();
		mRain.Reset();
	}

	//animate water texture
	mWaterTexAnimate.x += 0.05f*dt;
	mWaterTexAnimate.y += 0.05f*dt;
	XMMATRIX waterTexTranslate = XMMatrixTranslation(0.0f, mWaterTexAnimate.y, mWaterTexAnimate.x);
	XMStoreFloat4x4(&mWaterTexTransform, waterTexTranslate);

	mFire.Update(dt, mTimer.TotalTime());
	mRain.Update(dt, mTimer.TotalTime());

	mCam.UpdateViewMatrix();
}

void World::DrawScene(){
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mCam.GetPosition());
	Effects::BasicFX->SetFogColor(Colors::Silver);
	Effects::BasicFX->SetFogStart(15.0f);
	Effects::BasicFX->SetFogRange(175.0f);

	//draw each object
	drawObject(mFloor, mDirtTex, mFloorTexTransform, mDirtMat);
	drawObject(mWall, mBrickTex, mWallTexTransform, mBrickMat);
	drawObject(mWall_2, mBrickTex, mWallTexTransform, mBrickMat);
	drawObject(mWall_3, mBrickTex, mWallTexTransform, mBrickMat);
	drawObject(mWall_4, mBrickTex, mWallTexTransform, mBrickMat);
	drawObject(mWater, mWaterTex, mWaterTexTransform, mWaterMat);

	/*
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (GetAsyncKeyState('1') & 0x8000)
		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);

	mTerrain.Draw(md3dImmediateContext, mCam, mDirLights);

	md3dImmediateContext->RSSetState(0);
	*/
	mSky->Draw(md3dImmediateContext, mCam);

	/*
	//draw fire particle system
	mFire.SetEyePos(mCam.GetPosition());
	mFire.Draw(md3dImmediateContext, mCam);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default

	//draw rain particle system
	mRain.SetEyePos(mCam.GetPosition());
	mRain.SetEmitPos(mCam.GetPosition());
	mRain.Draw(md3dImmediateContext, mCam);

	// restore default states.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	*/
	HR(mSwapChain->Present(0, 0));
}

void World::drawObject(XMFLOAT4X4& objWorld, ID3D11ShaderResourceView* objTexture, XMFLOAT4X4& textureTransform, Material& mat){
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3TexFogTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	
	// Set per object constants.
	XMMATRIX world = XMLoadFloat4x4(&objWorld);
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

	Effects::BasicFX->SetWorld(world);
	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::BasicFX->SetWorldViewProj(world * mCam.ViewProj());
	Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&textureTransform));
	Effects::BasicFX->SetMaterial(mat);
	Effects::BasicFX->SetDiffuseMap(objTexture);

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
	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < floor.Vertices.size(); ++i, ++k){
		vertices[k].Pos = floor.Vertices[i].Position;
		vertices[k].Normal = floor.Vertices[i].Normal;
		vertices[k].Tex = floor.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
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
	translate = XMMatrixTranslation(-2.3f, 0.2f, 0.0f);
	scale = XMMatrixScaling(2.0f, 0.001f, 7.0f);
	XMStoreFloat4x4(&mWater, scale * translate);
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
