#pragma once
#include "component_base.h"
#include "effect.h"

namespace cumulonimbus::component
{
	class EffekseerComponent : public ComponentBase
	{
	public:
		explicit EffekseerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EffekseerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EffekseerComponent& copy_comp);
		explicit EffekseerComponent(mapping::component_tag::ComponentTag tag);
		explicit EffekseerComponent()  = default; // for cereal
		~EffekseerComponent() override;

		void Start() override;

		void CommonUpdate(float dt) override;
		void GameUpdate(float dt) override;

		void End() override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		void Play();
		void ChangeEffect(const mapping::rename_type::UUID& efk_id);

		void SetPos(const DirectX::SimpleMath::Vector3& pos) const;
		void SetScale(const DirectX::SimpleMath::Vector3& scale) const;

		/**
		 * @brief : �������Ԃ��I��莟��ɃI�u�W�F�N�g���폜���邩
		 * @remark : true -> �Đ����Ԃ��I��莟��폜
		 */
		void SetIsDeleteAtEndOfSpawnTime(const bool arg)
		{
			is_delete_at_end_of_spawn_time = arg;
		}

		/**
		 * @brief : �I�u�W�F�N�g�폜���ɃG���e�B�e�B�������ɍ폜���邩
		 * @remark : true -> �G���e�B�e�B�������ɍ폜����
		 */
		void SetIsDeleteEntity(const bool arg)
		{
			is_delete_all_entity = arg;
		}

		[[nodiscard]]
		bool IsPlaying() const;
	private:
		mapping::rename_type::UUID effect_id{};
		asset::Effect* effect{};

		Effekseer::Handle handle{ -1 };
		// �G�t�F�N�g��1��ȏ㐶�����ꂽ��
		bool is_spawn{ false };
		// �������Ԃ��I��莟��ɃI�u�W�F�N�g���폜���邩
		bool is_delete_at_end_of_spawn_time{ false };
		// �I�u�W�F�N�g�폜���ɃG���e�B�e�B�������ɍ폜���邩
		bool is_delete_all_entity{ false };
	};
} // cumulonimbus::component