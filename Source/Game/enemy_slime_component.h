#pragma once
#include "enemy_base_component.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class EnemySlimeComponent final : public EnemyBaseComponent
	{
	private:
		// 敵(Slime)の状態遷移表
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

		// FBXが持つアニメーションデータ
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
	};
} // cumulonimbus::component
