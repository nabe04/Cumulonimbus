#pragma once

#include <array>
#include <memory>
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <wrl.h>

#include "component_base.h"
#include "texture.h"
#include "shader.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::graphics
{
	class SkyBox final
	{
	public:
		explicit SkyBox(ID3D11Device* device, const std::string& filename = { "" });
		explicit SkyBox() = default; // for cereal
		~SkyBox() = default;

		template<class Archive>
		void serialize(Archive&& archive)
		{

		}

		void Update(float dt);

		/**
		 * @brief : ImGui�ł̃p�����[�^�\���֐�
		 * @param registry : System::Render�ɓo�^���邽�߂ɕK�v
		 */
		void RenderImGui(ecs::Registry* registry);

		void BindShader(ID3D11DeviceContext* immediate_context) const;
		void UnBindShader(ID3D11DeviceContext* immediate_context) const;

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
		std::array<std::unique_ptr<asset::Texture>, 6>	 cube_textures{};
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_view{};

		Microsoft::WRL::ComPtr<ID3D11Buffer>  vertex_buffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer>  index_buffer{};
		std::unique_ptr<shader::VertexShader> vertex_shader{};
		std::unique_ptr<shader::PixelShader>  pixel_shader{};

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

namespace cumulonimbus::component
{
	class SkyBoxComponent final : public ComponentBase
	{
	public:
		/*
		* brief �X�J�C�{�b�N�X�̍쐬
		* filename : �L���[�u�}�b�v�̃t�@�C����(�ԍ��͏����Ȃ�)
		*/
		explicit SkyBoxComponent(ecs::Registry* registry, mapping::rename_type::Entity ent,
								 ID3D11Device* device, const char* filename);
		explicit SkyBoxComponent()  = default; // for cereal
		~SkyBoxComponent() override = default;

		void PreGameUpdate(float dt) override {}
		void GameUpdate(float dt)	 override {}
		void RenderImGui()			 override;

		void Load(ecs::Registry* registry) override;

		template <class Archive>
		void serialize(Archive&& archive);

		/*
		 * brief : SkyBox�p vs,ps�̃Z�b�g
		 *         ���݃Z�b�g����Ă���shader��ێ�(Deactivate�̃^�C�~���O�ŃZ�b�g���Ȃ�������)
		 */
		void ActivateShader(ID3D11DeviceContext* immediate_context) const
		{
			vertex_shader->Activate(immediate_context);
			pixel_shader->Activate(immediate_context);
		}

		/*
		 * brief : vertex,pixel shader��default vs ps�ɃZ�b�g���Ȃ���
		 */
		void DeactivateShader(ID3D11DeviceContext* immediate_context) const
		{
			vertex_shader->Deactivate(immediate_context);
			pixel_shader->Deactivate(immediate_context);
		}

		[[nodiscard]] ID3D11Buffer* GetVertexBuffer() const { return vertex_buffer.Get(); }
		[[nodiscard]] ID3D11Buffer* GetIndexBuffer()  const { return index_buffer.Get(); }
		ID3D11Buffer** GetVertexBufferAddress() { return vertex_buffer.GetAddressOf(); }
		ID3D11Buffer** GetIndexBufferAddress() { return index_buffer.GetAddressOf(); }

		ID3D11ShaderResourceView* GetShaderResoueceView() { return texture_view.Get(); }
		ID3D11ShaderResourceView** GetShaderResoueceViewAddress() { return texture_view.GetAddressOf(); }

	private:
		std::array<std::shared_ptr<TextureResource>, 6> cube_texture{};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer{};
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_view{};

		std::string vs_name = "./Shader/cso/sky_box_vs.cso";
		std::string ps_name = "./Shader/cso/sky_box_ps.cso";
		std::shared_ptr<shader::VertexShader> vertex_shader{};
		std::shared_ptr<shader::PixelShader>  pixel_shader{};

		void CreateVertexBufferAndIndexBuffer(ID3D11Device* device);
		void CreateTextures(ID3D11Device* device, const char* filename);
	};
} // cumulonimbus::component
