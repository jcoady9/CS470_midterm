#include "d3dApp.h"
#include "Waves.h"
#include "M3DMesh.h"

class World : public D3DApp{
public:
	World(HINSTANCE hInstance);
	~World();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:

	void buildBuffers();
	void buildMeshFX();
	void buildVertexMeshLayouts();

	std::string mTableMeshFilename;

	M3DMesh mTableMesh;


	std::wstring mFXFileName;
	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mFXMatVar;

	// Vertex buffer
	ID3D11InputLayout* mVertexLayout;

	ID3D11Buffer* mFloorVB;
	ID3D11Buffer* mFloorIB;

	ID3D11Buffer* mWallVB;
	ID3D11Buffer* mWallIB;

	ID3D11Buffer* mWallVB_2;
	ID3D11Buffer* mWallIB_2;

	ID3D11Buffer* mWallVB_3;
	ID3D11Buffer* mWallIB_3;

	ID3D11Buffer* mWallVB_4;
	ID3D11Buffer* mWallIB_4;

	ID3D11Buffer* mWaterVB;
	ID3D11Buffer* mWaterIB;

	ID3D11ShaderResourceView* mFloorMapSRV;
	ID3D11ShaderResourceView* mWallMapSRV;
	ID3D11ShaderResourceView* mWaterMapSRV;

	DirectionalLight mDirLights[3];

	Material mFloorMat;
	Material mWallMat;
	Material mWaterMat;

	XMFLOAT4X4 mFloorTexTransform;
	XMFLOAT4X4 mFloor;

	XMFLOAT4X4 mWallTexTransform;
	XMFLOAT4X4 mWall;
	XMFLOAT4X4 mWall_2;
	XMFLOAT4X4 mWall_3;
	XMFLOAT4X4 mWall_4;

	XMFLOAT4X4 mWaterTexTransform;
	XMFLOAT4X4 mWater;

	Waves waveWater;

	XMFLOAT2 mWaterTexAnimate;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mWVP;

	int mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;

	XMFLOAT3 mCameraPos;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};