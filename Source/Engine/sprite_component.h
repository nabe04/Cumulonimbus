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
		explicit SpriteComponent(
			ecs::Registry* registry,
			mapping::rename_type::Entity ent,
			const mapping::rename_type::UUID& tex_id = {});
		explicit SpriteComponent()  = default; // for cereal
		~SpriteComponent() override = default;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void serialize(Archive&& archive);

		/**
		 * @brief : テクスチャサイズの変更
		 * @param size : 変更のテクスチャサイズ
		 */
		void ResizeTexture(const DirectX::SimpleMath::Vector2& size);

		void SetPivotType(render::PivotType pivot);

	protected:
		mapping::rename_type::UUID	texture_id{}; // テクスチャID(UUID)
		graphics::GraphicsState		graphics_state{};
		std::shared_ptr<shader_asset::ShaderAsset2DManager> shader_asset{};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		render::PivotType pivot_type{ render::PivotType::Center };

		std::array<shader::VertexSprite, 4> vertices;

		void CreateVertexBuffer();
	};
} // cumulonimbus::component
