//#include <array>
//#include <d3d11.h>
//
//#include "scene.h"
//#include "mesh_particle.h"
//
//MeshParticle::MeshParticle(Entity* entity)
//	:Component{ entity }
//{
//	//std::array<D3D11_INPUT_ELEMENT_DESC, 6> layout =
//	//{
//	//		{
//	//		{ "POSITION",					 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,  0 },
//	//		{ "NORMAL",						 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,  0 },
//	//	 	{ "WORLD_TRANSFORM_MATRIX",      0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1 },
//	//		{ "WORLD_TRANSFORM_MATRIX",      1,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1 },
//	//		{ "WORLD_TRANSFORM_MATRIX",      2,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1 },
//	//		{ "WORLD_TRANSFORM_MATRIX",      3,  DXGI_FORMAT_R32G32B32A32_FLOAT, 1,	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA,1 },
//	//	    }
//	//};
//
//	std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout =
//	{
//			{
//			{ "POSITION",					 0,  DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,  0 },
//			{ "NORMAL",						 0,	 DXGI_FORMAT_R32G32B32_FLOAT,	 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,  0 },
//			}
//	};
//
//	vertex_shader   = std::make_unique<shader::VertexShader>(entity->GetScene()->GetFramework()->GetDevice(),"./Shader/cso/cpu_particle_vs.cso", layout.data(), layout.size());
//	pixel_shader    = std::make_unique<shader::PixelShader>(entity->GetScene()->GetFramework()->GetDevice(), "./Shader/cso/cpu_particle_ps.cso");
//	constant_buffer = std::make_unique<buffer::ConstantBuffer<Cb_Particle>>(entity->GetScene()->GetFramework()->GetDevice());
//}
//
//void MeshParticle::NewFrame(const float delta_time)
//{
//
//}
//
//void MeshParticle::Update(const float delta_time)
//{
//
//}
//
//void MeshParticle::RenderImGui()
//{
//
//}
//
//void MeshParticle::SetParticleDataFromFbx(const FbxModelComponent* comp, const Transform& transform)
//{
//	// VertexBufferçÏê¨óp
//	std::vector<Vb_Fixed_S0>   vb_s0{};
//	std::vector<Vb_UnFixed_S1> vb_s1{};
//
//	for (int i = 0; i < comp->GetResource()->GetModelData().meshes.at(0).vertices.size(); ++i)
//	{
//		particle_datas.emplace_back();
//		particle_datas.back().vertices = comp->GetResource()->GetModelData().meshes.at(0).vertices.at(i);
//		particle_datas.back().transform = transform;
//		particle_datas.back().transform.SetScale(0.01f);
//		particle_datas.back().transform.CreateWorldTransformMatrix();
//		particle_datas.back().position = DirectX::SimpleMath::Vector3{ particle_datas.back().vertices.position.x,particle_datas.back().vertices.position.y,particle_datas.back().vertices.position.z };
//		particle_datas.back().normal = comp->GetResource()->GetModelData().meshes.at(0).vertices.at(i).normal;
//		particle_datas.back().color = DirectX::SimpleMath::Vector4{ 1.f,0.f,1.f,1.f };
//
//		// VertexBufferçÏê¨
//		vb_s0.emplace_back();
//		vb_s0.back().local_position = particle_datas.back().position;
//		vb_s0.back().normal = particle_datas.back().normal;
//
//		vb_s1.emplace_back();
//		vb_s1.back().world_transform_matrix = particle_datas.back().transform.GetWorld4x4();
//	}
//
//	//-- VertexBufferÇÃçÏê¨ --//
//	vertex_buffer_fixed_s0   = std::make_unique<buffer::VertexBuffer<Vb_Fixed_S0>>(entity->GetScene()->GetFramework()->GetDevice(), vb_s0);
//	vertex_buffer_unfixed_s1 = std::make_unique<buffer::VertexBuffer<Vb_UnFixed_S1>>(entity->GetScene()->GetFramework()->GetDevice(), vb_s1);
//}
//
//
