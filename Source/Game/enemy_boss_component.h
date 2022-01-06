#pragma once
#include "enemy_base_component.h"

#include "behavior_tree.h"
#include "state_machine.h"
#include "keyframe_event.h"

namespace cumulonimbus::component
{
	class EnemyBossComponent : public EnemyBaseComponent
	{
	private:
		/**
		 * @brief : 状態遷移表
		 */
		enum class BossState
		{
			// 待機
			Idle,
			// 歩き系
			Walk,
			Run,
			Tracking,
			// ダメージ系
			Damage,
			Knock_Down,
			Death,
			// 攻撃系
			Attack_Normal_01,
			Attack_Normal_02,
			Attack_Normal_03,
			Attack_Normal_04,
			Attack_Energy_01,
			Attack_Energy_02,
			Attack_Dodge_Back,
			Attack_Combo_01,
			Attack_Combo_02,
			Attack_Combo_03,
			Attack_Combo_04,

			End
		};

		/**
		 * @brief : 全体の行動(ビヘイビア)表
		 */
		enum class BossBehavior
		{
			Move, // 待機、追跡、徘徊
			Attack, // 通常攻撃、エネルギー攻撃、バックジャンプ攻撃
			Damage, // ダメージ、ノックダウン、死亡

			End
		};

		/**
		 * @brief :攻撃の行動(ビヘイビア)表
		 */
		enum class AttackBehavior
		{
			Atk_Normal_01,
			Atk_Normal_02,
			Atk_Normal_03,
			Atk_Normal_04,
			Atk_Energy_01,
			Atk_Energy_02,

			End
		};

		/**
		 * @brief : FBXが持つアニメーションデータ
		 */
		enum class AnimationData
		{
			Damage,
			Walk,
			Run,
			Attack_Combo_All,
			Attack_Normal_01,
			Attack_Normal_02,
			Attack_Normal_03,
			Attack_Normal_04,
			Attack_Dodge_Back,
			Attack_Energy_01,
			Attack_Energy_02,
			Damage_Knock_Down,
			Attack_Combo_01,
			Attack_Combo_02,
			Attack_Combo_03,
			Attack_Combo_04,
			Death,
			Death_Loop,
			Idle,

			End
		};

	public:
		explicit EnemyBossComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit EnemyBossComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemyBossComponent& copy_comp);
		explicit EnemyBossComponent(mapping::component_tag::ComponentTag tag);
		explicit EnemyBossComponent()  = default;
		~EnemyBossComponent() override = default;

		void Start() override;
		void CommonUpdate(float dt) override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

	private:
		// 行動(ビヘイビア)の更新StateMachine
		StateMachine<BossBehavior, void, const float> boss_behavior{};
		//
		system::BehaviorTree<AttackBehavior, void, const float, const bool> attack_behavior{};

		//-- パラメータ --//
		// 歩行の速さ
		float walk_speed{ 100.f };
		// 走りの速さ
		float dash_speed{ 150.f };
		//-- 行動制御 --//
		// 近距離攻撃の境界距離
		float atk_melee_distance{ 100.f };

		// 次のビヘイビアに移行するか
		bool is_next_sequence{ false };
		// ビヘイビアツリーの一連の処理が完了したか
		bool is_behavior_completed{ false };

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		void OnAttack(const collision::HitResult& hit_result) override;

		[[nodiscard]]
		int GetAnimDataIndex(AnimationData anim_data) const;

		//-- ビヘイビア更新処理 --//
		/**
		 * @return : true -> 更新中のビヘイビアが終了
		 */
		void BehaviorUpdate(float dt);
		void BehaviorMoveUpdate(float dt);
		void BehaviorAttackUpdate(float dt);

		template <auto F>
		std::function<void(float)> GetBehaviorUpdateFunc()
		{
			return std::bind(F, this, std::placeholders::_1);
		}

		template<auto F>
		std::function<void(float, bool)> GetBehaviorActFunc()
		{
			return std::bind(F, this, std::placeholders::_1, std::placeholders::_2);
		}

		//-- 敵(ボス)の挙動 --//
		/**
		 * @return : true -> 次のビヘイビアに移行
		 */
		// 待機
		void Idle(float dt, bool is_start);
		// 歩き系
		void Tracking(float dt, bool is_start);
		// 通常攻撃(単撃)
		void AttackNormal01(float dt, bool is_start);
		void AttackNormal02(float dt, bool is_start);
		void AttackNormal03(float dt, bool is_start);
		void AttackNormal04(float dt, bool is_start);
		// コンボ攻撃
		void AttackCombo01(float dt, bool is_start);
		void AttackCombo02(float dt, bool is_start);
		void AttackCombo03(float dt, bool is_start);
		void AttackCombo04(float dt, bool is_start);
		// エネルギー攻撃
		void AttackEnergy01(float dt, bool is_start);
		void AttackEnergy02(float dt, bool is_start);
		// バックジャンプ攻撃
		void AttackDodgeBack(float dt, bool is_start);
	};
} // cumulonimbus::component
