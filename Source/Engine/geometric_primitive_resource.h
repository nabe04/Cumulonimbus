#pragma once

#include <array>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

enum class GeomPrimType
{
	Cube,
	Box,
	Sphere,
	Cylinder,
	Tetrahedron,
	Octahedron,
	Dodecahedron,
	Icosahedron,
	Cone,
	Tours,
	Teapot,

	End,
};

namespace geometric_primitive_resource
{
	struct Mesh
	{
		Mesh()
		{
			vertex_buffer = nullptr;
			num_vertices  = 0;
			index_buffer  = nullptr;
			num_indices   = 0;
		}
		GeomPrimType primitiveType;
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer = nullptr;
		u_int num_vertices = 0;		// Number of vertices
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer = nullptr;
		u_int num_indices = 0;		// Number of indices
	};
}

class GeometricPrimitiveResource
{
private:
	std::array<geometric_primitive_resource::Mesh, static_cast<int>(GeomPrimType::End)> meshes = {};

private:
	void CreatePrimitive(ID3D11Device* device, GeomPrimType type);

public:
	GeometricPrimitiveResource(ID3D11Device* device);

	geometric_primitive_resource::Mesh GetMeshData(GeomPrimType type) { return meshes.at(static_cast<int>(type)); }
};