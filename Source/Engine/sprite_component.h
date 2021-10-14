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
		explicit SpriteComponent(); // for cereal & Inspector View��ł�AddComponent�p
		explicit SpriteComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit SpriteComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SpriteComponent& copy_comp); // for prefab
		explicit SpriteComponent(mapping::component_tag::ComponentTag tag);
		SpriteComponent(const SpriteComponent& other);
		SpriteComponent& operator=(const SpriteComponent& other);
		~SpriteComponent() override = default;

		void CommonUpdate(float dt) override;
		void PostCommonUpdate(float dt) override {}

		void Load(ecs::Registry* registry) override;
		void RenderImGui() override;

		//template<class Archive>
		//void serialize(Archive&& archive);

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

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
		shader_asset::ShaderAsset2DManager* GetShaderAssetManager() { return &shader_asset_manager; }

		void SetTextureId(const mapping::rename_type::UUID tex_id) { texture_id = tex_id; }
		void SetPivotType(render::PivotType pivot);

	protected:
		mapping::rename_type::UUID			texture_id{}; // �e�N�X�`��ID(UUID)
		graphics::GraphicsState				graphics_state{};
		shader_asset::ShaderAsset2DManager	shader_asset_manager{};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer{};
		render::PivotType					 pivot_type{ render::PivotType::Center };

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
