#pragma once
#include <unordered_map>

#include "enemy_base_component.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class EnemySlimeComponent final : public EnemyBaseComponent
	{
	private:

		/**
		 * @brief : 敵(Slime)の状態遷移表
		 */
		enum class SlimeState
		{
			T_Pose,
			Idle,			// 待機
			Walk,			// 歩き
			Birth,			// 生成
			Death,			// 死亡
			Hit,			// 被弾
			Stun,			// スタン
			Attack_Bite,	// 攻撃(噛みつき)
			Attack_Charge,	// 攻撃(突進)

			End
		};

		/**
		 * @brief : FBXが持つアニメーションデータ
		 */
		enum class AnimationData
		{
			Idle_00,
			Attack_Bite,
			Attack_Charge,
			Birth,
			Death,
			Hit,
			Idle_01,
			Stun,
			Walk,

			End
		};

	public:
		explicit EnemySlimeComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemySlimeComponent()  = default; // for cereal
		~EnemySlimeComponent() override = default; // for cereal

		void Update(float dt) override;
		void RenderImGui()	  override;

		void Save(const std::string& file_path)			 override;
		void Load(const std::string& file_path_and_name) override;

	private:
		// 敵(Slime)の状態管理変数
		StateMachine<SlimeState, void, const float> slime_state{};
		// 行動(状態)遷移用タイマー
		std::unordered_map<SlimeState, RangeFloat> timer_range{};
		// 回転角
		RangeFloat angle_range{};

		/**
		 * @brief : "timer_range"変数の値設定用関数
		 */
		void AddTimerRange(SlimeState state, const RangeFloat& range);

		/**
		 * @brief : enum class(AnimationData)をint型に変換
		 */
		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;

		/**
		 * @brief : StateMachineクラスで管理する敵の状態関数
		 */
		void TPose(float dt);
		void Idle(float dt);
		void Walk(float dt);
		void Birth(float dt);
		void Death(float dt);
		void Hit(float dt);
		void Stun(float dt);
		void AttackBite(float dt);
		void AttackCharge(float dt);
	};
} // cumulonimbus::component
