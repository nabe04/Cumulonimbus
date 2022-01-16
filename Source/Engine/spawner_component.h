#pragma once
#include "component_base.h"

namespace cumulonimbus::component
{
	class SpawnerComponent : public ComponentBase
	{
	public:
		using ComponentBase::ComponentBase;
		explicit SpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit SpawnerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const SpawnerComponent& copy_comp);
		explicit SpawnerComponent() = default;
		~SpawnerComponent() override = default;

		void Load(ecs::Registry* registry) override;

		void SceneUpdate(float dt) override;

		void RenderImGui() override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);
		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

		/**
		 * @brief : アセットの生成関数
		 * @remark : 生成したいタイミングで一度だけ呼ぶ
		 * @param is_first : 初めの生成か(true -> 初期パラメータがリセットされる)
		 * @param pos : 生成位置
		 */
		virtual void Spawn(bool is_first ,const DirectX::SimpleMath::Vector3& pos = {}, const DirectX::SimpleMath::Vector3& direction = { 0,0,1 });
	protected:
		// 生成するアセットID
		mapping::rename_type::UUID asset_id{};
		// 生成されているか
		bool is_spawn{ false };
		// 生成数
		int spawn_count{ 1 };
		// 現在の生成数
		int current_count{};
		// 生成時間間隔
		float spawn_interval_time{};
		// 経過時間(使用は自由)
		float elapsed_time{};
		// 生成時間間隔
		float spawn_interval_distance{};
		// 最初の生成位置
		DirectX::SimpleMath::Vector3 root_pos{};
		// 生成方向
		DirectX::SimpleMath::Vector3 direction{ 0,0,1 };

		/**
		 * @brief : 「Spawn」ボタンのImGui上で実装
		 * @return : ボタンが押されるとSpawn関数が走る
		 */
		void ImSpawnButton();
	};
} // cumulonimbus::component
