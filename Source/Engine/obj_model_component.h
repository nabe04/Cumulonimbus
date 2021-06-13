#pragma once

#include <wrl.h>

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "ecs.h"
#include "component_base.h"
#include "blend.h"
#include "rasterizer.h"

#include "shader.h"

class Light;

namespace shader
{
	class MeshShaderState;
	enum class MeshShaderTypes;
	struct Vertex;
}
class TextureResource;

//*************************************************
//
// ObjModelComponent Class
// brief : Components for generating OBJ models
//
//*************************************************
class ObjModelComponent final : public Component
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
	ObjModelComponent(Entity* entity, ID3D11Device* device, const std::string_view filename);
	~ObjModelComponent();

	virtual void NewFrame(const float delta_time)override;
	virtual void Update(const float delta_time)override;
	virtual void RenderImGui() override;

	bool LoadOBJ(ID3D11Device* device, std::string_view filename);

	auto* GetVertexBuffer() const{ return vertex_buffer.Get(); }
	auto* GetVertexBufferAddress() const { return vertex_buffer.GetAddressOf(); }
	auto* GetIndexBuffer() const { return index_buffer.Get(); }
	auto* GetIndexBufferAddress() const { return index_buffer.GetAddressOf(); }
	std::vector<Material>& GetMaterials() { return materials; }
	std::vector<Subset>& GetSubsets() { return subsets; }
	float GetTopPos()		{ return top; }
	float GetBottomPos()	{ return bottom; }
	float GetLeftPos()		{ return left; }
	float GetRightPos()		{ return right; }
	XMFLOAT3 GetCenterPos() { return center_pos; }
};

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


