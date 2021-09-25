#pragma once

#include <wrl.h>

#include <d3d11.h>
#include <DirectXMath.h>

#include "component_base.h"
#include "shader.h"
#include "rename_type_mapping.h"

namespace shader
{
	struct Vertex;
}
class TextureResource;

namespace cumulonimbus::component
{
	class ObjModelComponent final : public ComponentBase
	{
		//-- Information in the OBJ file --//
		struct Subset
		{
			std::string usemtl;
			u_int		index_start = 0;
			u_int		index_count = 0;
		};
		std::vector<Subset> subsets;

		struct Material
		{
			std::string			newmtl;
			shader::CB_Material data;
			u_int				illum = 1;
			std::string			map_Kd;
			TextureResource* texture = nullptr;
		};
	public:
		explicit ObjModelComponent() = default;
		explicit ObjModelComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, ID3D11Device* device, const std::string_view filename);
		~ObjModelComponent();

		void PreGameUpdate(const float delta_time)override;
		void GameUpdate(const float delta_time)override;
		void RenderImGui() override;

		void Load(ecs::Registry* registry) override {};

		bool LoadOBJ(ID3D11Device* device, std::string_view filename);

		[[nodiscard]] auto* GetVertexBuffer() const{ return vertex_buffer.Get(); }
		[[nodiscard]] auto* GetVertexBufferAddress() const { return vertex_buffer.GetAddressOf(); }
		[[nodiscard]] auto* GetIndexBuffer() const { return index_buffer.Get(); }
		[[nodiscard]] auto* GetIndexBufferAddress() const { return index_buffer.GetAddressOf(); }
		std::vector<Material>& GetMaterials() { return materials; }
		std::vector<Subset>& GetSubsets() { return subsets; }
		[[nodiscard]] float GetTopPos()		const { return top; }
		[[nodiscard]] float GetBottomPos()	const { return bottom; }
		[[nodiscard]] float GetLeftPos()		const{ return left; }
		[[nodiscard]] float GetRightPos()		const{ return right; }
		[[nodiscard]] DirectX::XMFLOAT3 GetCenterPos() const{ return center_pos; }

	private:
		std::vector<Material> materials;

		std::string					dirPath;
		std::vector<u_int>			indices;
		std::vector<shader::Vertex> vertices;
		float top{}, bottom{}, left{}, right{}, front{}, back{};
		DirectX::XMFLOAT3 center_pos{};

		//-- Variables for DirectX --//
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer{};

		void CreateBuffers(ID3D11Device* device, shader::Vertex* vertices, int num_vertices, u_int* indices, int num_indices);

		/*
		* brief : Setting of the most important position
		*/
		void PosComparison(const DirectX::XMFLOAT3 pos);

		void SetCenerLocalPos();
		void SetCenterWorldPos();

	};
}


