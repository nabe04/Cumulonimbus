#pragma once

#include <array>
#include <memory>
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <wrl.h>

#include "cereal_helper.h"
#include "texture.h"
#include "shader.h"
#include "constant_buffer.h"
#include "shader_interop_renderer.h"
#include "rename_type_mapping.h"

namespace cumulonimbus
{
	namespace ecs
	{
		class Registry;
	} // ecs

	namespace system
	{
		class System;
	} // system
} // cumulonimbus

namespace cumulonimbus::graphics
{
	class SkyBox final
	{
	public:
		explicit SkyBox(system::System& system, ID3D11Device* device, const std::string& filename = { "" });
		explicit SkyBox() = default; // for cereal
		~SkyBox() = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{
			archive(
				CEREAL_NVP(file_path_and_name),
				CEREAL_NVP(position),
				CEREAL_NVP(scale),
				CEREAL_NVP(angle)
			);
		}

		void Update(float dt);

		/**
		 * @brief : ImGuiでのパラメータ表示関数
		 * @param registry : System::Renderに登録するために必要
		 */
		void RenderImGui(ecs::Registry* registry);

		/**
		 * @brief : ロード時処理
		 * @ramerk : シリアライズが不可能なパラメータの設定
		 */
		void Load(system::System& system);

		void BindShader(ID3D11DeviceContext* immediate_context) const;
		void UnBindShader(ID3D11DeviceContext* immediate_context) const;

		void BindCBuffer(ID3D11DeviceContext* immediate_context, bool set_in_vs = true, bool set_in_ps = true) const;
		void UnBindCBuffer(ID3D11DeviceContext* immediate_context) const;

		[[nodiscard]]
		ID3D11Buffer* GetVertexBuffer() const { return vertex_buffer.Get(); }
		[[nodiscard]]
		ID3D11Buffer* GetIndexBuffer()  const { return index_buffer.Get(); }
		[[nodiscard]]
		ID3D11Buffer** GetVertexBufferAddress() { return vertex_buffer.GetAddressOf(); }
		[[nodiscard]]
		ID3D11Buffer** GetIndexBufferAddress() { return index_buffer.GetAddressOf(); }

		[[nodiscard]]
		ID3D11ShaderResourceView* GetShaderResourceView() const { return texture_view.Get(); }
		[[nodiscard]]
		ID3D11ShaderResourceView** GetShaderResourceViewAddress() { return texture_view.GetAddressOf(); }

		[[nodiscard]]
		const DirectX::SimpleMath::Matrix& GetWorldMatrix() const { return world_mat; }
	private:
		std::array<std::unique_ptr<asset::Texture>, 6>		   cube_textures{};
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	   texture_view{};
		std::unique_ptr<::buffer::ConstantBuffer<TransformCB>> cb_transform{};

		Microsoft::WRL::ComPtr<ID3D11Buffer>  vertex_buffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer>  index_buffer{};
		std::unique_ptr<shader::VertexShader> vertex_shader{};
		std::unique_ptr<shader::PixelShader>  pixel_shader{};

		std::string file_path_and_name{}; // SkyBoxテクスチャがあるファイルパス

		DirectX::SimpleMath::Vector3 position{};
		DirectX::SimpleMath::Vector3 scale{ 1.f,1.f,1.f };
		DirectX::SimpleMath::Vector3 angle{};

		DirectX::SimpleMath::Matrix world_mat		{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix scaling_mat		{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix rotation_mat	{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::SimpleMath::Matrix translation_mat	{ DirectX::SimpleMath::Matrix::Identity };

		void CreateVertexBufferAndIndexBuffer(ID3D11Device* device);
		void CreateTextures(ID3D11Device* device, const char* filename);
	};
} // cumulonimbus::skybox