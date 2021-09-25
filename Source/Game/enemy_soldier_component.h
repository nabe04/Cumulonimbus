#pragma once
#include "enemy_base_component.h"
#include "state_machine.h"

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
			Walk,
			Run,
			Attack_03,
			Attack_01,
			Attack_02,
			Death,
			Death_Loop,
			Damage,
			Attack_All,
			Idle,

			End
		};

	public:
		explicit EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemySoldierComponent()  = default;
		~EnemySoldierComponent() override = default;

		void GameUpdate(float dt) override;
		void RenderImGui()	  override;

		void Load(ecs::Registry* registry) override;

	private:
		// 敵(ForestDemon)の状態管理変数
		StateMachine<SoldierState, void, const float> soldier_state{};
		// モデルのyaw回転(度数法)のランダム値
		RandomFloat random_rotation_angle{};

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
		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;

		//-- StateMachineクラスで管理する敵の状態関数 --//
		void Idle(float dt);
		void Walk(float dt);
		void Tracking(float dt);
		void Damage(float dt);
		void Death(float dt);
		void Attack01(float dt);
		void Attack02(float dt);
		void Attack03(float dt);
	};

} // cumulonimbus::component