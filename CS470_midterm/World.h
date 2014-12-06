#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Camera.h"
#include "Sky.h"
#include "Terrain.h"
#include "ParticleSystem.h"

#define NUMFIRES 3

class World : public D3DApp{
public:
	World(HINSTANCE hInstance);
	~World();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void buildVertexAndIndexBuffers();
	void initialTransformations();
	void drawObject(XMFLOAT4X4& objWorld, ID3D11ShaderResourceView* objTexture, XMFLOAT4X4& textureTransform, Material& mat, ID3D11ShaderResourceView* normalMap);

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	Sky* mSky;
	Terrain volcanoTerrain;

	//matrices to store object worlds
	XMFLOAT4X4 mFloor;
	XMFLOAT4X4 mWall;
	XMFLOAT4X4 mWall_2;
	XMFLOAT4X4 mWall_3;
	XMFLOAT4X4 mWall_4;
	XMFLOAT4X4 mWater;

	//matrices for texture transformations
	XMFLOAT4X4 mFloorTexTransform;
	XMFLOAT4X4 mWallTexTransform;
	XMFLOAT4X4 mWaterTexTransform;

	//matrix for animating water texture
	XMFLOAT2 mWaterTexAnimate;

	//buffers for vertices & indices of generic cube
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	//textures
	ID3D11ShaderResourceView* mDirtTex;
	ID3D11ShaderResourceView* mBrickTex;
	ID3D11ShaderResourceView* mWaterTex;

	ID3D11ShaderResourceView* mFlareTexSRV;
	ID3D11ShaderResourceView* mRainTexSRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	//normal maps
	ID3D11ShaderResourceView* mBrickNormalMap;
	ID3D11ShaderResourceView* mWaterNormalMap;

	//materials
	Material mDirtMat;
	Material mBrickMat;
	Material mWaterMat;

	int numFires = NUMFIRES;

	ParticleSystem volcanoFire[NUMFIRES];
	ParticleSystem mRain;

	DirectionalLight mDirLights[3];

	Camera mCam;

	bool mWalkCamMode;

	POINT mLastMousePos;

	int mBoxVertexOffset;
	UINT mBoxIndexOffset;
	UINT mBoxIndexCount;

};