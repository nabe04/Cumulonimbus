#include "geometric_primitive_resource.h"

#include <GeometricPrimitive.h>

#include "shader.h"

GeometricPrimitiveResource::GeometricPrimitiveResource(ID3D11Device* device)
{
	{
		for (int type_no = 0; type_no < static_cast<int>(GeomPrimType::End); ++type_no)
		{
			CreatePrimitive(device, static_cast<GeomPrimType>(type_no));
		}
	}
}
void GeometricPrimitiveResource::CreatePrimitive(ID3D11Device* device, GeomPrimType type)
{
	std::vector<shader::Vertex> vertices;
	std::vector<int> indices;

	{
		std::vector<DirectX::GeometricPrimitive::VertexType> vertex_datas;
		std::vector<uint16_t> index_datas;

		namespace res = geometric_primitive_resource;
		switch (type)
		{
		case GeomPrimType::Cube:
			DirectX::GeometricPrimitive::CreateCube(vertex_datas, index_datas);
			break;
		case GeomPrimType::Box:
			DirectX::GeometricPrimitive::CreateBox(vertex_datas, index_datas, XMFLOAT3{ 1,1,1 });
			break;
		case GeomPrimType::Sphere:
			DirectX::GeometricPrimitive::CreateSphere(vertex_datas, index_datas);
			break;
		case GeomPrimType::Cylinder:
			DirectX::GeometricPrimitive::CreateCylinder(vertex_datas, index_datas);
			break;
		case GeomPrimType::Tetrahedron:
			DirectX::GeometricPrimitive::CreateTetrahedron(vertex_datas, index_datas);
			break;
		case GeomPrimType::Octahedron:
			DirectX::GeometricPrimitive::CreateOctahedron(vertex_datas, index_datas);
			break;
		case GeomPrimType::Dodecahedron:
			DirectX::GeometricPrimitive::CreateDodecahedron(vertex_datas, index_datas);
			break;
		case GeomPrimType::Icosahedron:
			DirectX::GeometricPrimitive::CreateIcosahedron(vertex_datas, index_datas);
			break;
		case GeomPrimType::Cone:
			DirectX::GeometricPrimitive::CreateCone(vertex_datas, index_datas);
			break;
		case GeomPrimType::Tours:
			DirectX::GeometricPrimitive::CreateTorus(vertex_datas, index_datas, 2);
			break;
		case GeomPrimType::Teapot:
			DirectX::GeometricPrimitive::CreateTeapot(vertex_datas, index_datas, 2);
			break;
		}


		vertices.reserve(vertex_datas.size());
		for (auto& vertex_data : vertex_datas)
		{
			shader::Vertex vertex;
			vertex.position = XMFLOAT4{ vertex_data.position.x,vertex_data.position.y,-vertex_data.position.z,1.0f };
			vertex.tex = vertex_data.textureCoordinate;
			vertex.normal = XMFLOAT3{ vertex_data.normal.x,vertex_data.normal.y ,-vertex_data.normal.z };
			vertex.bone_weight = XMFLOAT4(1, 0, 0, 0);
			vertex.bone_index = XMUINT4(0, 0, 0, 0);

			vertices.emplace_back(vertex);
		}

		indices.reserve(index_datas.size());
		for (auto& index_data : index_datas)
		{
			int index = static_cast<int>(index_data);

			indices.emplace_back(index);
		}
	}


	//-- Create Vertex buffer --//
	meshes.at(static_cast<int>(type)).num_vertices = vertices.size();
	{
		HRESULT hr = E_FAIL;
		CD3D11_BUFFER_DESC bd		= {};
		bd.Usage					= D3D11_USAGE_DEFAULT;
		bd.ByteWidth				= sizeof(shader::Vertex) * vertices.size();
		bd.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags			= 0;
		bd.MiscFlags				= 0;
		bd.StructureByteStride		= 0;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem				= vertices.data();
		init_data.SysMemPitch			= 0;
		init_data.SysMemSlicePitch		= 0;
		hr = device->CreateBuffer(&bd, &init_data, &meshes.at(static_cast<int>(type)).vertex_buffer);
		if (FAILED(hr))
			assert(!"CreateBuffer (Vertex Buffer) error");
	}

	//-- Create Index buffer --//
	meshes.at(static_cast<int>(type)).num_indices = indices.size();
	{
		HRESULT hr = E_FAIL;
		D3D11_BUFFER_DESC bd = {};
		bd.Usage				= D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth			= sizeof(u_int) * meshes.at(static_cast<int>(type)).num_indices;
		bd.BindFlags			= D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags		= 0;
		bd.MiscFlags			= 0;
		bd.StructureByteStride	= 0;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem				= indices.data();
		init_data.SysMemPitch			= 0;
		init_data.SysMemSlicePitch		= 0;
		hr = device->CreateBuffer(&bd, &init_data, meshes.at(static_cast<int>(type)).index_buffer.GetAddressOf());
		if (FAILED(hr))
			assert(!"CreateBuffer (Index Buffer) error");
	}
}