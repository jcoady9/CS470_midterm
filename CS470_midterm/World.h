#include "d3dApp.h"
#include "M3DMesh.h"
#include <string>

// Subclass declaration
class World : public D3DApp {

public:
	World(HINSTANCE);
	~World();

	// Overriden methods
	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:

	//Shader methods
	void buildFX();
	void buildVertexLayouts();

	void buildBuffers();

	// Cube mesh object
	M3DMesh mTableMesh;
	M3DMesh mChairMesh;


	XMFLOAT4X4 mTable;
	XMFLOAT4X4 mTableTransform;

	std::string mTableMeshFilename;
	std::string mChairMeshFilename;



	// Shader variables
	std::wstring mFXFileName;
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mFXMatVar;

	// Vertex buffer
	ID3D11InputLayout* mVertexLayout;

	// Global transformations
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mWVP;

	// Camera vectors
	XMFLOAT3 mEye;
	XMFLOAT3 mAt;
	XMFLOAT3 mUp;

	XMFLOAT3 mCameraPos;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

};