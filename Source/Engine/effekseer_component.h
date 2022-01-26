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
		 * @brief : 生成時間が終わり次第にオブジェクトを削除するか
		 * @remark : true -> 再生時間が終わり次第削除
		 */
		void SetIsDeleteAtEndOfSpawnTime(const bool arg)
		{
			is_delete_at_end_of_spawn_time = arg;
		}

		/**
		 * @brief : オブジェクト削除時にエンティティも同時に削除するか
		 * @remark : true -> エンティティも同時に削除する
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
		// エフェクトが1回以上生成されたか
		bool is_spawn{ false };
		// 生成時間が終わり次第にオブジェクトを削除するか
		bool is_delete_at_end_of_spawn_time{ false };
		// オブジェクト削除時にエンティティも同時に削除するか
		bool is_delete_all_entity{ false };
	};
} // cumulonimbus::component