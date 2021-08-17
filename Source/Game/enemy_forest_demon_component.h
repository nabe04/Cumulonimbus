#pragma once
#include <unordered_map>

#include "enemy_base_component.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class EnemyForestDemonComponent final : public EnemyBaseComponent
	{
	private:
		/**
		 * @brief : 敵(Forest Demon)の状態遷移表
		 */
		enum class ForestDemonState
		{
			T_Pose,
			Idle,				// 待機(通常)
			Ready,				// 待機(戦闘前)
			Walk,				// 歩き
			Tracking,			// 追跡
			Death,				// 死亡
			Damage,				// 被弾
			Attack_Normal,		// 攻撃(通常)
			Attack_Mow_Down,	// 攻撃(薙ぎ払い)

			End
		};

		/**
		 * @brief : FBXが持つアニメーションデータ
		 */
		enum class AnimationData
		{
			Attack_Mow_Down,
			Attack_Normal,
			Death,
			Damage,
			Idle_00,
			Idle_01,
			Ready,
			Run,
			Walk,

			End
		};

	public:
		explicit EnemyForestDemonComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemyForestDemonComponent()  = default; // for cereal
		~EnemyForestDemonComponent() override = default;

		void Update(float dt) override;
		void RenderImGui()    override;

		void Save(const std::string& file_path)			 override;
		void Load(const std::string& file_path_and_name) override;

	private:
		// 敵(ForestDemon)の状態管理変数
		StateMachine<ForestDemonState, void, const float> forest_demon_state{};
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

		//-- その他 --//

		/**
		 * @brief		: "random_rotation_angle"メンバ変数の値設定用関数
		 * @brief		: min、maxで指定した範囲内でのランダム値の算出
		 * @param min	: 最小値
		 * @param max	: 最大値
		 */
		void SetRandomRotationAngle(float min, float max);

		/**
		 * @brief : enum class(AnimationData)をint型に変換
		 */
		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;

		//-- StateMachineクラスで管理する敵の状態関数 --//
		void TPose(float dt);
		void Idle(float dt);
		void Ready(float dt);
		void Walk(float dt);
		void Tracking(float dt);
		void Death(float dt);
		void Damage(float dt);
		void AttackNormal(float dt);
		void AttackMowDown(float dt);
	};
} // cumulonimbus::component
