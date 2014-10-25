#ifndef M3DMESH_H
#define M3DMESH_H

#include "d3dUtil.h"
#include "MeshGeometry.h"
#include "Vertex.h"
#include <string>

class M3DMesh
{
public:
	M3DMesh();
	M3DMesh(ID3D11Device* device, const std::string& modelFilename);
	~M3DMesh();
	void Init(ID3D11Device* device, const std::string& modelFilename);

	// Model loader
	bool loadM3DMesh(const std::string& modelFilename);

private:
	void ReadSubsetTable(std::ifstream& fin, UINT numSubsets);
	void ReadVertices(std::ifstream& fin, UINT numVertices);
	void ReadTriangles(std::ifstream& fin, UINT numTriangles);

public:
	// TODO: Mesh data
	// Local Mesh data copies
	std::vector<MeshVertex> Vertices;
	std::vector<USHORT> Indices;
	std::vector<MeshGeometry::Subset> Subsets;

	UINT SubsetCount;

	// Mesh geometry and local instance transformation
	MeshGeometry MeshData;
	XMFLOAT4X4 World;
};

#endif // M3DMESH_H