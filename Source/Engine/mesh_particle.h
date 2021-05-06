#pragma once
#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "ecs.h"
#include "constant_buffer.h"
#include "vertex_buffer.h"
#include "model_data.h"
#include "transform.h"
#include "transform_component.h"
#include "fbx_model_component.h"
#include "shader.h"

class MeshParticle : public Component
{
public:
	struct ParticleData
	{
		ModelData::Vertex				vertices;
		Transform						transform;
		DirectX::SimpleMath::Vector3	position;
		DirectX::SimpleMath::Vector3	normal;
		DirectX::SimpleMath::Vector4	color;
	};

	struct Cb_Particle
	{
		DirectX::SimpleMath::Matrix	 transform_matrix;
		DirectX::SimpleMath::Vector4 position;
		DirectX::SimpleMath::Vector4 color;
	};

	struct Vb_Fixed_S0
	{
		DirectX::SimpleMath::Vector3 local_position;
		DirectX::SimpleMath::Vector3 normal;
	};

	struct Vb_UnFixed_S1
	{
		DirectX::SimpleMath::Matrix	world_transform_matrix;
	};

protected:
	bool is_active = false;
	std::vector<ParticleData>		particle_datas;

private:
	std::unique_ptr<shader::VertexShader> vertex_shader;
	std::unique_ptr<shader::PixelShader>  pixel_shader;
	std::unique_ptr<buffer::ConstantBuffer<Cb_Particle>> constant_buffer;
	std::unique_ptr<buffer::VertexBuffer<Vb_Fixed_S0>> vertex_buffer_fixed_s0;
	std::unique_ptr<buffer::VertexBuffer<Vb_UnFixed_S1>> vertex_buffer_unfixed_s1;

public:
	MeshParticle(Entity* entity);

	void NewFrame(const float delta_time) override;
	void Update(const float delta_time) override;
	void RenderImGui() override;

	void SetParticleDataFromFbx(const FbxModelComponent* comp, const Transform& transform);

	[[nodiscard]] shader::VertexShader* GetVertexShader() { return vertex_shader.get(); }
	[[nodiscard]] shader::PixelShader* GetPixelShader() { return pixel_shader.get(); }
	[[nodiscard]] buffer::VertexBuffer<Vb_Fixed_S0>* GetVertexBuffer_S0() { return vertex_buffer_fixed_s0.get(); }
	[[nodiscard]] buffer::VertexBuffer<Vb_UnFixed_S1>* GetVertexBuffer_S1() { return vertex_buffer_unfixed_s1.get(); }
	[[nodiscard]] buffer::ConstantBuffer<Cb_Particle>* GetConstantBuffer() { return constant_buffer.get(); }
	[[nodiscard]] std::vector<ParticleData>* GetParticleDatas() { return &particle_datas; }

private:
	void CreateInputLayout();
};