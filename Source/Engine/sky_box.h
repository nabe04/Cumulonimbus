#pragma once

#include <array>
#include <memory>
#include <string>

#include <d3d11.h>
#include <wrl.h>

#include "component_base.h"
#include "texture.h"
#include "shader.h"
#include "rename_type_mapping.h"

namespace cumulonimbus::component
{
	class SkyBoxComponent final : public ComponentBase
	{
	public:
		/*
		* brief スカイボックスの作成
		* filename : キューブマップのファイル名(番号は書かない)
		*/
		explicit SkyBoxComponent(ecs::Registry* registry, mapping::rename_type::Entity ent,
								 ID3D11Device* device, const char* filename);
		explicit SkyBoxComponent()  = default; // for cereal
		~SkyBoxComponent() override = default;

		void PreGameUpdate(float dt) override {}
		void GameUpdate(float dt)	 override {}
		void RenderImGui()			 override;;

		void Load(ecs::Registry* registry) override;

		template <class Archive>
		void serialize(Archive&& archive);

		/*
		 * brief : SkyBox用 vs,psのセット
		 *         現在セットされているshaderを保持(Deactivateのタイミングでセットしなおすため)
		 */
		void ActivateShader(ID3D11DeviceContext* immediate_context) const
		{
			vertex_shader->Activate(immediate_context);
			pixel_shader->Activate(immediate_context);
		}

		/*
		 * brief : vertex,pixel shaderをdefault vs psにセットしなおす
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
