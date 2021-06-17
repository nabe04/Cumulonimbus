#pragma once

#include <wrl.h>

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "component_base.h"
#include "shader.h"
#include "ecs.h"

namespace shader
{
	struct Vertex;
}
class TextureResource;

namespace cumulonimbus::component
{
	class ObjModelComponent final : public ComponentBase
	{
	private:
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
			TextureResource*    texture = nullptr;
		};
		std::vector<Material> materials;

		std::string					dirPath;
		std::vector<u_int>			indices;
		std::vector<shader::Vertex> vertices;
		float top{}, bottom{}, left{}, right{}, front{}, back{};
		XMFLOAT3 center_pos{};

		//-- Variables for DirectX --//
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer{};
	private:
		void CreateBuffers(ID3D11Device* device, shader::Vertex* vertices, int num_vertices, u_int* indices, int num_indices);

		/*
		* brief : Setting of the most important position
		*/
		void PosComparison(const XMFLOAT3 pos);

		void SetCenerLocalPos();
		void SetCenterWorldPos();

	public:
		explicit ObjModelComponent() = default;
		explicit ObjModelComponent(ecs::Registry* registry, ecs::Entity ent, ID3D11Device* device, const std::string_view filename);
		~ObjModelComponent();

		void NewFrame(const float delta_time)override;
		void Update(const float delta_time)override;
		void RenderImGui() override;

		void Save(const std::string& file_path) override {};
		void Load(const std::string& file_path_and_name) override {};

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
		[[nodiscard]] XMFLOAT3 GetCenterPos() const{ return center_pos; }
	};
}


