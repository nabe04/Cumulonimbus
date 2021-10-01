#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <wrl.h>

#include "component_base.h"
#include "graphics_state.h"
#include "rename_type_mapping.h"
#include "shader.h"
#include "shader_asset_manager.h"

namespace cumulonimbus::render
{
	enum class PivotType
	{
		Center,
		BottomCenter,
		TopCenter,
		LeftTop,
		LeftBottom,
		RightTop,
		RightBottom,

		End
	};
} // cumulonimbus::render

namespace cumulonimbus::component
{
	class SpriteComponent : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit SpriteComponent(
			ecs::Registry* registry,
			mapping::rename_type::Entity ent);
		explicit SpriteComponent(const mapping::component_tag::ComponentTag tag);
		explicit SpriteComponent(); // for cereal
		~SpriteComponent() override = default;

		void CommonUpdate(float dt) override;

		void Load(ecs::Registry* registry) override;
		void RenderImGui() override;

		template<class Archive>
		void serialize(Archive&& archive);

		/**
		 * @brief : テクスチャサイズの変更
		 * @remark : NDC空間上の原点(ウィンドウ位置で言うところのウィンドウの中心)
		 *			 を中心として頂点位置を決める
		 * @param size : 変更のテクスチャサイズ
		 */
		void ResizeTexture(const DirectX::SimpleMath::Vector2& size);

		[[nodiscard]]
		const mapping::rename_type::UUID& GetTextureId() const { return texture_id; }
		[[nodiscard]]
		const graphics::GraphicsState& GetGraphicsState() const { return graphics_state; }
		[[nodiscard]]
		ID3D11Buffer** GetVertexBufferAddress() { return vertex_buffer.GetAddressOf(); }
		[[nodiscard]]
		shader_asset::ShaderAsset2DManager* GetShaderAssetManager() const { return shader_asset_manager.get(); }

		void SetPivotType(render::PivotType pivot);

	protected:
		mapping::rename_type::UUID	texture_id{}; // テクスチャID(UUID)
		graphics::GraphicsState		graphics_state{};
		std::shared_ptr<shader_asset::ShaderAsset2DManager> shader_asset_manager{};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		render::PivotType pivot_type{ render::PivotType::Center };

		std::array<shader::VertexSprite, 4> vertices;

		/**
		 * @brief : 初期化処理
		 * @param width : テクスチャ横サイズ
		 * @param height : テクスチャ縦サイズ
		 */
		void Initialize(float width, float height);
		void CreateVertexBuffer();

		/**
		 * @brief : スクリーン空間上の座標をNDC空間上の座標に変換
		 */
		void ConvertScreenToNDC() const;
	};
} // cumulonimbus::component
