#include "collision_component.h"

#include "scene.h"
#include "collision.h"
#include "sphere_collision_component.h"
#include "light.h"
#include "view.h"
#include "component_list.h"

collision_component::collision_component(Entity* entity,CollisionTag tag)
	: Component{ entity }
	, tag{ tag }
{
	shader_state.SetShaderState(shader::MeshShaderTypes::Standard);
}

void collision_component::Update(const float delta_time)
{
	auto* scene = GetEntity()->GetScene();
}

// Ž‚Á‚Ä‚é”»’èî•ñ‚Ìˆ—
bool collision_component::Intersect(collision_component* other)
{
	using namespace intersect;

	EachOtherType each_other_type = EachOtherType::End;

	if (this->type == CollisionType::Sphere && other->GetType() == CollisionType::Sphere)
	{
		each_other_type = EachOtherType::SphereAndSphere;
	}
	if (this->type == CollisionType::InnerSphere && other->GetType() == CollisionType::Sphere)
	{
		each_other_type = EachOtherType::InterSphereAndSphere;
	}

	if (this->is_judge == false || other->IsJudge() == false)
		return false;

	switch (each_other_type)
	{
	case EachOtherType::SphereAndSphere:
		return IntersectSphere(static_cast<Spherecollision_component*>(other));
		break;
	case EachOtherType::InterSphereAndSphere:
		return IntersectInnerSphere(static_cast<Spherecollision_component*>(other));
		break;
	}

	return false;
}

bool collision_component::CompareTag(CollisionTag tag)
{
	return this->tag == tag;
}

void collision_component::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		cereal::base_class<Component>(this),
		CEREAL_NVP(type),
		CEREAL_NVP(tag),

		CEREAL_NVP(shader_state),
		CEREAL_NVP(is_judge)
	);
}

void collision_component::Load(Entity* entity, std::string file_path_and_name)
{
	std::ifstream ifs(file_path_and_name);
	cereal::JSONInputArchive i_archive(ifs);
	i_archive(
		cereal::base_class<Component>(this),
		CEREAL_NVP(type),
		CEREAL_NVP(tag),

		CEREAL_NVP(shader_state),
		CEREAL_NVP(is_judge)
	);

	this->entity = entity;
}




//void collision_component::CreateShpere(ID3D11Device* device)
//{
//	std::vector<shader::Vertex> vertices;
//	std::vector<int> indices;
//
//	float r = 0.5f; // radian 0.5f = diameter 1.0f
//
//	shader::Vertex topVertex;
//	topVertex.position = DirectX::XMFLOAT4(0.0f, +r, 0.0f, 1.0f);
//	topVertex.normal   = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);
//
//	shader::Vertex bottomVertex;
//	bottomVertex.position = DirectX::XMFLOAT4(0.0f, -r, 0.0f, 1.0f);
//	bottomVertex.normal   = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
//
//	vertices.push_back(topVertex);
//
//	const int stacks = 16;
//	const int slices = 16;
//
//	float phiStep = DirectX::XM_PI / stacks;
//	float thetaStep = 2.0f * DirectX::XM_PI / slices;
//
//	// Compute vertices for each stack ring (do not count the poles as rings).
//	for (u_int i = 1; i <= stacks - 1; ++i)
//	{
//		float phi = i * phiStep;
//		float rsPhi = r * sinf(phi), rcPhi = r * cosf(phi);
//
//		// Vertices of ring.
//		for (u_int j = 0; j <= slices; ++j)
//		{
//			float theta = j * thetaStep;
//
//			shader::Vertex v;
//
//			// spherical to cartesian
//			v.position.x = rsPhi * cosf(theta);
//			v.position.y = rcPhi;
//			v.position.z = rsPhi * sinf(theta);
//			v.position.w = 1.0f;
//
//			v.bone_weight = XMFLOAT4(1, 0, 0, 0);
//			v.bone_index = XMUINT4(0, 0, 0, 0);
//
//			DirectX::XMVECTOR p = DirectX::XMLoadFloat4(&v.position);
//			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));
//
//			vertices.push_back(v);
//		}
//	}
//
//	vertices.push_back(bottomVertex);
//
//	HRESULT hr;
//
//	//-- Create a vertex buffer at the top vertex --//
//	sphere.num_vertices = vertices.size();
//	{
//		CD3D11_BUFFER_DESC bd = {};
//		bd.Usage = D3D11_USAGE_DEFAULT;
//		bd.ByteWidth = static_cast<u_int>(sizeof(shader::Vertex) * vertices.size());
//		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		bd.CPUAccessFlags = 0;
//		bd.MiscFlags = 0;
//		bd.StructureByteStride = 0;
//
//		D3D11_SUBRESOURCE_DATA initData = {};
//		initData.pSysMem = vertices.data();
//		initData.SysMemPitch = 0;
//		initData.SysMemSlicePitch = 0;
//		hr = device->CreateBuffer(&bd, &initData, &sphere.vertex_buffer);
//		if (FAILED(hr))
//			assert(!"CreateBuffer (Vertex Buffer) error");
//	}
//
//	//
//	// Compute indices for top stack.  The top stack was written first to the vertex buffer
//	// and connects the top pole to the first ring.
//	//
//	for (UINT i = 1; i <= slices; ++i)
//	{
//		indices.push_back(0);
//		indices.push_back(i + 1);
//		indices.push_back(i);
//	}
//
//	//
//	// Compute indices for inner stacks (not connected to poles).
//	//
//
//	// Offset the indices to the index of the first vertex in the first ring.
//	// This is just skipping the top pole vertex.
//	u_int base_index = 1;
//	u_int ring_vertex_count = slices + 1;
//	for (u_int i = 0; i < stacks - 2; ++i)
//	{
//		u_int i_rvc = i * ring_vertex_count;
//		u_int i1_rvc = (i + 1) * ring_vertex_count;
//
//		for (u_int j = 0; j < slices; ++j)
//		{
//			indices.push_back(base_index + i_rvc + j);
//			indices.push_back(base_index + i_rvc + j + 1);
//			indices.push_back(base_index + i1_rvc + j);
//
//			indices.push_back(base_index + i1_rvc + j);
//			indices.push_back(base_index + i_rvc + j + 1);
//			indices.push_back(base_index + i1_rvc + j + 1);
//		}
//	}
//
//	//
//	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
//	// and connects the bottom pole to the bottom ring.
//	//
//
//	// South pole vertex was added last.
//	u_int south_pole_index = (u_int)vertices.size() - 1;
//
//	// Offset the indices to the index of the first vertex in the last ring.
//	base_index = south_pole_index - ring_vertex_count;
//
//	for (u_int i = 0; i < slices; ++i)
//	{
//		indices.push_back(south_pole_index);
//		indices.push_back(base_index + i);
//		indices.push_back(base_index + i + 1);
//	}
//
//	//-- Create a index buffer at the top vertex --//
//	sphere.num_indices = indices.size();
//	{
//		D3D11_BUFFER_DESC bd = {};
//		bd.Usage				= D3D11_USAGE_IMMUTABLE;
//		bd.ByteWidth			= static_cast<u_int>(sizeof(shader::Vertex) * sphere.num_indices);
//		bd.BindFlags			= D3D11_BIND_INDEX_BUFFER;
//		bd.CPUAccessFlags		= 0;
//		bd.MiscFlags			= 0;
//		bd.StructureByteStride	= 0;
//
//		D3D11_SUBRESOURCE_DATA initData{};
//		initData.pSysMem			= indices.data();
//		initData.SysMemPitch		= 0;
//		initData.SysMemSlicePitch	= 0;
//
//		hr = device->CreateBuffer(&bd, &initData, sphere.index_buffer.GetAddressOf());
//		if (FAILED(hr))
//			assert(!"CreateBuffer (Index Buffer) error");
//	}
//}