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
		 * @brief : �e�N�X�`���T�C�Y�̕ύX
		 * @remark : NDC��ԏ�̌��_(�E�B���h�E�ʒu�Ō����Ƃ���̃E�B���h�E�̒��S)
		 *			 �𒆐S�Ƃ��Ē��_�ʒu�����߂�
		 * @param size : �ύX�̃e�N�X�`���T�C�Y
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
		mapping::rename_type::UUID	texture_id{}; // �e�N�X�`��ID(UUID)
		graphics::GraphicsState		graphics_state{};
		std::shared_ptr<shader_asset::ShaderAsset2DManager> shader_asset_manager{};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		render::PivotType pivot_type{ render::PivotType::Center };

		std::array<shader::VertexSprite, 4> vertices;

		/**
		 * @brief : ����������
		 * @param width : �e�N�X�`�����T�C�Y
		 * @param height : �e�N�X�`���c�T�C�Y
		 */
		void Initialize(float width, float height);
		void CreateVertexBuffer();

		/**
		 * @brief : �X�N���[����ԏ�̍��W��NDC��ԏ�̍��W�ɕϊ�
		 */
		void ConvertScreenToNDC() const;
	};
} // cumulonimbus::component
