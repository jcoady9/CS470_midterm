#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
//#include "RenderStates.h"
#include "Waves.h"
#include "MeshGeometry.h"

#include "World.h"

// Class methods
World::World(HINSTANCE hInstance) : D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0), mFXMatVar(0), mTableMapSRV(0),
mCameraPos(0.0f, 0.0f, 0.0f), mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(80.0f)
{
	// Change the window caption
	mMainWndCaption = L"Midterm Project";

	// Initialize matricies to identirty matrix
	XMMATRIX iMatrix = XMMatrixIdentity();

	XMStoreFloat4x4(&mView, iMatrix);
	XMStoreFloat4x4(&mProj, iMatrix);
	XMStoreFloat4x4(&mWVP, iMatrix);

	//scale textures

	// Initialize camera orientation
	mEye = { 0.0f, 0.0f, 1.0f };
	mAt = { 0.0f, 0.0f, 0.0f };
	mUp = { 0.0f, 1.0f, 0.0f };

	//set mesh filenames
	mTableMeshFilename = "Meshes/table.m3d";
	mChairMeshFilename = "Meshes/chair.m3d";

};

World::~World()
{
	ReleaseCOM(mTableMapSRV);
	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
};

bool World::Init()
{
	// Simply use base class initializer
	if (!D3DApp::Init())
		return false;

	// initialize mesh objects
	mTableMesh.Init(md3dDevice, mTableMeshFilename);


	// Create effect and vertex layout
	mShaderFilename = L"basicfx.fx";
	buildFX();
	buildVertexLayouts();

	// Initialize view matrix from camera
	mEye = { 5.0f, 5.0f, 5.0f };
	mAt = { 0.0f, 0.0f, 0.0f };
	mUp = { 0.0f, 1.0f, 0.0f };

	XMVECTOR e = XMLoadFloat3(&mEye);
	XMVECTOR a = XMLoadFloat3(&mAt);
	XMVECTOR u = XMLoadFloat3(&mUp);

	XMMATRIX camera = XMMatrixLookAtLH(e, a, u);
	XMStoreFloat4x4(&mView, camera);

	return true;
}

void World::OnResize()
{
	// Simply use base class resize
	D3DApp::OnResize();

	// Reset (90 degree fov) projection frustum
	XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, proj);
}

void World::UpdateScene(float dt){

	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	mCameraPos = XMFLOAT3(x, y, z);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);


	XMMATRIX tableScale = XMMatrixScaling(100.0f, 100.0f, 100.0f);
	XMStoreFloat4x4(&mTableMesh.World, tableScale);

}

void World::DrawScene(){
	// Check for valid rendering context and swap chain
	assert(md3dImmediateContext);
	assert(mSwapChain);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	// Simply clear render view to blue
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));

	// Clear depth buffer to 1.0 and stenci buffer to 0
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// TODO: Set input layout to object vertices
	md3dImmediateContext->IASetInputLayout(mVertexLayout);


	// TODO: Compute total modelview-projection matrix
	XMMATRIX v = XMLoadFloat4x4(&mView);
	XMMATRIX p = XMLoadFloat4x4(&mProj);
	XMMATRIX VP = v*p;


	//Get technique and loop over passes
	D3DX11_TECHNIQUE_DESC td;
	mTech->GetDesc(&td);
	for (UINT p = 0; p < td.Passes; p++){
		// Set modelview projection matrix for object
		XMMATRIX table = XMLoadFloat4x4(&mTableMesh.World);
		XMMATRIX wvp = table*VP;
		mFXMatVar->SetMatrix(reinterpret_cast<float*>(&VP));
		// Draw subsets
		for (UINT subset = 0; subset < mTableMesh.SubsetCount; ++subset){
			mTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			mTableMesh.MeshData.Draw(md3dImmediateContext, subset);
		}
	}

	// Swap back buffer
	HR(mSwapChain->Present(0, 0));
};

// Build effect
void World::buildFX()
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
	HRESULT hr = D3DX11CompileFromFile(mShaderFilename.c_str(), 0, 0, 0, "fx_5_0", shaderFlags, 0, 0, &shader, &compilationErrors, 0);


	// Output debug info if compilation failed
	if (FAILED(hr)){
		// Check for compilation errors or warnings
		if (compilationErrors != 0){
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
	}

	// TODO: Create effect from shader
	HR(D3DX11CreateEffectFromMemory(shader->GetBufferPointer(), shader->GetBufferSize(), 0, md3dDevice, &mFX));
	ReleaseCOM(shader);

	// TODO: Set technique and associate variables
	// Get technique from effect
	mTech = mFX->GetTechniqueByName("BasicTech");

	// Associate modelview-projection shader variable
	mFXMatVar = mFX->GetVariableByName("gWVP")->AsMatrix();
}

// Build vertex layout
void World::buildVertexLayouts()
{
	// Create vertex layout for pass 0
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
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.1f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.1f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 20.0f, 500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}