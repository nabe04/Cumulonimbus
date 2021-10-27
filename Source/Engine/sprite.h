#pragma once
#include <array>
#include <memory>

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "component_base.h"
#include "texture.h"
#include "rename_type_mapping.h"
#include "graphics_state.h"
#include "shader_manager.h"

enum class PivotType
{
	Center,
	BottomCenter,
	TopCenter,
	LeftTop,
	LeftBottom,
	RightTop,
	RightBottom,
};

namespace cumulonimbus::component
{
	//


	// Todo : 現Spriteの描画が完了次第消す
	//-- 旧コンポーネント --//
	class OldSpriteComponent : public ComponentBase
	{
	public:
		explicit OldSpriteComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent,
		                         ID3D11Device* device,
		                         const char* filename, PivotType pivot_type,
		                         int src_left, int src_top,
		                         int src_width, int src_height);
		explicit OldSpriteComponent() = default;	// For cereal
		~OldSpriteComponent() override = default;

		void PreGameUpdate(float delta_time)override {};
		void GameUpdate(float delta_time)	override;
		void RenderImGui()					override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t  version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		[[nodiscard]]
		const graphics::GraphicsState& GetGraphicsState() const { return graphics_state; }
		[[nodiscard]]
		const shader::SpriteShaderTypes& GetShaderState() const { return sprite_shader_state.GetShaderState(); }

		[[nodiscard]]
		int VariableWidth()  const { return variable_width; }
		[[nodiscard]]
		int VariableHeight() const { return variable_height; }
		[[nodiscard]]
		const DirectX::XMFLOAT4& Color() const { return color; }

		[[nodiscard]]
		const std::array<DirectX::XMFLOAT2, 4>& GetVariableTexcoords() const { return variable_texcoords; }
		[[nodiscard]]
		const DirectX::XMFLOAT2& GetSrcPivot() const { return src_pivot; }
		[[nodiscard]]
		ID3D11Buffer* GetVertexBuffer() const { return vertex_buffer.Get(); }
		[[nodiscard]]
		ID3D11Buffer** GetVertexBufferAddress() { return vertex_buffer.GetAddressOf(); }
		[[nodiscard]]
		TextureResource* GetTexture() const { return texture.get(); }

		void SetShaderState(const shader::SpriteShaderTypes& type) { sprite_shader_state.SetShaderState(type); }
		void VariableWidth(int width) { variable_width = width; }
		void VariableHeight(int height) { variable_height = height; }
		void Color(const DirectX::XMFLOAT4& color) { this->color = color; }

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		std::shared_ptr<TextureResource>     texture;

		graphics::GraphicsState   graphics_state{};
		shader::SpriteShaderState sprite_shader_state;

		PivotType			pivot_type{};
		DirectX::XMFLOAT2	src_pivot{};
		int					src_width{};
		int					src_height{};

		std::array<DirectX::XMFLOAT2, 4> variable_texcoords{};
		int variable_width{};
		int variable_height{};

		DirectX::XMFLOAT4 color{ 1.f,1.f,1.f,1.f };

		// For Imgui
		DirectX::XMFLOAT2 image_size = { 100,100 };

		void AdjustSrcTexturePivot(
			PivotType pivot_type,
			int src_left,  int src_top,
			int src_width, int src_height);

		void CreateVertexBuffer(ID3D11Device* device);
	};
} // cumulonimbus::component
