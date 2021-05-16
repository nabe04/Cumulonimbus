#pragma once

#include <memory>

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include "constant_buffer.h"

#include "imgui.h"

class Scene;

namespace shader
{
	HRESULT CreateVertexShaderFromCSO(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements);
	HRESULT CreatePixelShaderFromCSO(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

	//--------------------------------
	//
	// Shader types(3D)
	//
	//--------------------------------
	enum class MeshShaderTypes
	{
		Standard,
		Diffuse,
		Phong,
		Metal,
		Toon,
		ReflectionMapping,
		RefractionMapping,
		SingleColor,

		End,
	};

	//--------------------------------
	//
	// Shader types(2D)
	//
	//--------------------------------
	enum class SpriteShaderTypes
	{
		Standard,

		End,
	};

	//----------------------------------
	//
	// For storing shader values
	// (Used for assigning values from "Scenes" to "Managers", etc.)
	//----------------------------------
	struct Storage
	{
		DirectX::XMFLOAT4 light_dir;
		DirectX::XMFLOAT4 eye_pos;
	};

	//-----------------------------
	//
	//	Vertex data(3D) structure
	//
	//-----------------------------
	struct Vertex
	{
		DirectX::XMFLOAT4 position{};
		DirectX::XMFLOAT3 normal{};
		DirectX::XMFLOAT2 tex{};		// UV coordinates
		DirectX::XMFLOAT4 bone_weight = { 1.0f,0.0f,0.0f,0.0f };
		DirectX::XMUINT4  bone_index = { 0,0,0,0 };
	};

	//-----------------------------
	//
	//	Vertex data(2D) structure
	//
	//-----------------------------
	struct Vertex2D
	{
		DirectX::XMFLOAT2 position{};
		DirectX::XMFLOAT2 texcoord{};
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f };
	};

	struct VertexSprite
	{
		DirectX::XMFLOAT4 position{};
		DirectX::XMFLOAT2 texcoord{};
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f };
	};

	struct Empty
	{
		DirectX::XMFLOAT4 padding{};
	};


	//------------------------------
	//
	// Constant buffer data
	//
	//------------------------------

	static const int MaxBones = 128;
	// Transformation (b0)
	struct CB_CoordinateTransformation
	{
		//XMMATRIX world;
		DirectX::XMFLOAT4X4 mat_view_projection;
		DirectX::XMFLOAT4X4 bone_transforms[MaxBones];
		DirectX::XMFLOAT4X4 transform_matrix;
	};

	// Material (b1)
	struct CB_Material
	{
		DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f }; // Material color
		DirectX::XMFLOAT4   Ka = { 0.2f,0.2f,0.2f ,0.9f };
		DirectX::XMFLOAT4	Kd = { 0.8f,0.8f,0.8f ,0.0f };
		DirectX::XMFLOAT4	Ks = { 1.0f,1.0f,1.0f ,0.0f };

		//-- For Imgui --//
		void EditParam()
		{
			if (ImGui::TreeNode("Material"))
			{
				ImGui::ColorEdit4("Color", (float*)&color);
				ImGui::ColorEdit3("Ambient", (float*)&Ka);
				ImGui::ColorEdit3("Diffuse", (float*)&Kd);
				ImGui::ColorEdit3("Speculer", (float*)&Ks);

				ImGui::TreePop();
			}
		}
	};

	// Light (b2)
	struct CB_Light
	{
		DirectX::XMFLOAT4  light_dir = { .0f,.0f,1.f,1.f };
		DirectX::XMFLOAT4  color     = { 1.f,1.f,1.f,1.f };
		DirectX::XMFLOAT4  ambient	 = { 1.f,1.f,1.f,1.f };
		DirectX::XMFLOAT4  eye_pos	 = { .0f,.0f,.0f,1.f };	// Point of view
		DirectX::XMFLOAT4  position  = {};
		DirectX::XMFLOAT4X4 light_view_projection{};

		void Update(Scene* scene);

		//-- For Imgui --//
		void EditParam()
		{
			if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_None))
			{
				ImGui::DragFloat("Direction X", &light_dir.x, 0.01f, -1.0f, 1.0f);
				ImGui::DragFloat("Direction Y", &light_dir.y, 0.01f, -1.0f, 1.0f);
				ImGui::DragFloat("Direction Z", &light_dir.z, 0.01f, -1.0f, 1.0f);
				ImGui::ColorEdit4("Color", (float*)&color);
				ImGui::ColorEdit3("Ambient", (float*)&ambient);
			}
		}
	};

	class VertexShader
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> default_shader_object = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  default_input_layout = nullptr;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> shader_object = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>  input_layout = nullptr;

	public:
		VertexShader(ID3D11Device* device, const char* cso, D3D11_INPUT_ELEMENT_DESC* input_element_desc = nullptr, UINT num_elements = 0);
		~VertexShader() = default;
		VertexShader(VertexShader&) = delete;
		VertexShader& operator=(VertexShader&) = delete;

		void Activate(ID3D11DeviceContext* immediate_context);
		void Deactivate(ID3D11DeviceContext* immediate_context);
	};

	class PixelShader
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> default_shader_object = nullptr;

		Microsoft::WRL::ComPtr<ID3D11PixelShader> shader_object = nullptr;

	public:
		PixelShader(ID3D11Device* device, const char* cso);
		virtual ~PixelShader() = default;
		PixelShader(PixelShader&) = delete;
		PixelShader& operator=(PixelShader&) = delete;

		void Activate(ID3D11DeviceContext* immediate_context);
		void Deactivate(ID3D11DeviceContext* immediate_context);
	};

	//--------------------------------
	//
	// Shader class
	//
	//--------------------------------
	template <class T>
	class Shader
	{
	protected:
		std::unique_ptr<VertexShader>	vertex_shader = nullptr;
		std::unique_ptr<PixelShader>	pixel_shader  = nullptr;

		std::unique_ptr<buffer::ConstantBuffer<T>> material_data;
	public:
		explicit Shader(ID3D11Device* device)
		{
			material_data = std::make_unique<buffer::ConstantBuffer<T>>(device);
		}
		virtual ~Shader() = default;

		// Each set of shader
		virtual void Activate(ID3D11DeviceContext* immediate_context)   = 0;

		virtual void Deactivate(ID3D11DeviceContext* immediate_context) = 0;

		[[nodiscard]] buffer::ConstantBuffer<T>* GetMaterial() const { return material_data.get(); }
	};
}




