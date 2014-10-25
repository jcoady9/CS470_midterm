#ifndef VERTEX_H
#define VERTEX_H

// TODO: Mesh vertex structure
struct MeshVertex {
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

// Create vertex layout description from Vertex structure
static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


#endif // VERTEX_H