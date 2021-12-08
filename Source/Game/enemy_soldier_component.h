#pragma once
#include "enemy_base_component.h"

#include "state_machine.h"
#include "keyframe_event.h"

namespace cumulonimbus::component
{
	class EnemySoldierComponent final : public EnemyBaseComponent
	{
	private:
		/**
		 * @brief : 敵(Soldier)の状態遷移表
		 */
		enum class SoldierState
		{
			Idle,
			Wave_Start,
			Walk,
			Tracking,
			Damage,
			Death,
			Attack_01,
			Attack_02,
			Attack_03,

			End
		};

		/**
		 * @brief : FBXが持つアニメーションデータ
		 */
		enum class AnimationData
		{
			Run,
			Idle,
			Damage,
			Die,
			Die_Loop,
			Walk,
			Equip,
			Unarm,
			Attack_02,
			Attack_01,

			End
		};

	public:
		explicit EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemySoldierComponent& copy_comp); // for prefab
		explicit EnemySoldierComponent(mapping::component_tag::ComponentTag tag);
		explicit EnemySoldierComponent()  = default; // for cereal
		~EnemySoldierComponent() override = default;

		void Start() override;

		void GameUpdate(float dt) override;
		void RenderImGui()	  override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

	private:
		// 敵(ForestDemon)の状態管理変数
		StateMachine<SoldierState, void, const float> soldier_state{};
		// キーフレーム中のイベント管理
		std::map<AnimationData, system::KeyframeEvent> keyframe_events{};

		mapping::rename_type::Entity sword_ent{};

		//-- パラメータ --//
		// Quaternion::Slerpの回転時間倍率
		float rotation_time_rate{ 1.0f };
		// 歩行の速さ
		float walk_speed{ 100 };
		// 追跡中の速さ
		float tracking_speed{ 150 };
		// 追跡中断の長さ
		float tracking_interruption_distance{ 200 };
		// 追跡から攻撃に移る距離
		float attack_transition_distance{ 100 };
		// 追跡から攻撃に移る角度(度数法)
		float attack_transition_angle{ 90 };
		// 追跡に移る距離
		float tracking_transition_distance{ 200 };
		// 追跡に移る角度()
		float tracking_transition_angle{ 90 };
		// 突き攻撃に移る距離
		float attack_thrust_distance{ 50 };

		/**
		 * @brief : enum class(AnimationData)をint型に変換
		 */
		[[nodiscard]]
		int GetAnimDataIndex(AnimationData anim_data) const;

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		void InitializeKeyframeEvent();

		void Movement();

		void RegistryKeyframeEvent(AnimationData anim_data, const std::string& key_name);
		[[nodiscard]]
		system::KeyframeEvent& GetKeyframeEvent(AnimationData anim_data);

		void OnAttack(const collision::HitResult& hit_result) override;
		void OnDamaged(const collision::HitResult& hit_result) override;
		void OnDamaged(const DamageData& damage_data, const collision::HitResult& hit_result) override;

		//-- StateMachineクラスで管理する敵の状態関数 --//
		void Idle(float dt);
		void WaveStart(float dt);
		void Walk(float dt);
		void Tracking(float dt);
		void Damage(float dt);
		void Death(float dt);
		void Attack01(float dt);
		void Attack02(float dt);
		void Attack03(float dt);
	};

} // cumulonimbus::component