#include "M3DMesh.h"

// Constructors
M3DMesh::M3DMesh()
{
	// Initialize local transformation to identity
	XMMATRIX Ident = XMMatrixIdentity();
	XMStoreFloat4x4(&World, Ident);
}

M3DMesh::M3DMesh(ID3D11Device* device, const std::string& modelFilename)
{
	Init(device, modelFilename);
}

// Destructor
M3DMesh::~M3DMesh()
{

}

// Initialization
void M3DMesh::Init(ID3D11Device* device, const std::string& modelFilename)
{
	// TODO: Read file and create buffers for mesh
	loadM3DMesh(modelFilename);

	MeshData.SetVertices(device, &Vertices[0], Vertices.size());
	MeshData.SetIndices(device, &Indices[0], Indices.size());
	MeshData.SetSubsetTable(Subsets);

	// Store number of subsets for rendering
	SubsetCount = Subsets.size();
}

// Model loader
bool M3DMesh::loadM3DMesh(const std::string& modelFilename)
{

	// Local variables
	UINT numVertices = 0;
	UINT numTriangles = 0;
	std::string ignore;

	// TODO: Open .m3d mesh file
	std::ifstream fin(modelFilename);

	if (fin)
	{
		fin >> ignore;                 // ignore file header text
		fin >> ignore >> numVertices;  // read number of vertices
		fin >> ignore >> numTriangles; // read number of triangles

		// Read mesh data
		ReadSubsetTable(fin, 1); // only one material for now
		ReadVertices(fin, numVertices);
		ReadTriangles(fin, numTriangles);

		return true;
	}
	
	return false;
}

void M3DMesh::ReadSubsetTable(std::ifstream& fin, UINT numSubsets)
{
	std::string ignore;
	Subsets.resize(numSubsets);

	// TODO: Read subset table
	// Read subset table
	fin >> ignore; // ignore subset header text

	// Read subset data
	for (UINT i = 0; i < numSubsets; ++i)
	{
		fin >> ignore >> Subsets[i].Id;
		fin >> ignore >> Subsets[i].VertexStart;
		fin >> ignore >> Subsets[i].VertexCount;
		fin >> ignore >> Subsets[i].FaceStart;
		fin >> ignore >> Subsets[i].FaceCount;
	}
}

void M3DMesh::ReadVertices(std::ifstream& fin, UINT numVertices)
{
	std::string ignore;
	Vertices.resize(numVertices);
	
	// TODO: Read vertex data
	fin >> ignore; // ignore vertices header text

	// Read vertex data
	for (UINT i = 0; i < numVertices; ++i)
	{
		fin >> ignore >> Vertices[i].pos.x >> Vertices[i].pos.y >> Vertices[i].pos.z;
		fin >> ignore >> Vertices[i].color.x >> Vertices[i].color.y >> Vertices[i].color.z >> Vertices[i].color.w;
	}
}

void M3DMesh::ReadTriangles(std::ifstream& fin, UINT numTriangles)
{

	std::string ignore;
	Indices.resize(numTriangles * 3);

	// TODO: Read index data
	fin >> ignore; // ignore triangles header text

	// Read index data for all triangles
	for (UINT i = 0; i < numTriangles; ++i)
	{
		fin >> Indices[i * 3 + 0] >> Indices[i * 3 + 1] >> Indices[i * 3 + 2];
	}
}
