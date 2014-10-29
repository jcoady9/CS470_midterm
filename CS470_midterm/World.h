#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"

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

	ID3D11ShaderResourceView* mFloorMapSRV;
	ID3D11ShaderResourceView* mWallMapSRV;

	DirectionalLight mDirLights[3];

	Material mFloorMat;
	Material mWallMat;

	XMFLOAT4X4 mFloorTexTransform;
	XMFLOAT4X4 mFloor;

	XMFLOAT4X4 mWallTexTransform;
	XMFLOAT4X4 mWall;
	XMFLOAT4X4 mWall_2;
	XMFLOAT4X4 mWall_3;
	XMFLOAT4X4 mWall_4;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	int mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;

	XMFLOAT3 mCameraPos;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};