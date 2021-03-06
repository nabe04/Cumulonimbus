#pragma once
#include "enemy_base_component.h"

#include "behavior_tree.h"
#include "state_machine.h"
#include "keyframe_event.h"

#include "damageable_component.h"

namespace cumulonimbus::component
{
	class ColliderMessageSenderComponent;
} // cumulonimbus::component

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
		 * @brief : 攻撃の種類
		 */
		enum class AttackType
		{
			Atk_Melee = 0,
			Atk_Long_Range,

			End
		};

		/**
		 * @brief : HPに応じての行動パターン
		 */
		enum class AttackPattern
		{
			Pattern_1 = 0,
			Pattern_2,
			Pattern_3,
			Pattern_4,

			End
		};

		/**
		 * @brief : 移動の行動(ビヘイビア)表
		 */
		enum class MovementBehavior
		{
			Idle,
			Tracking,
			Wandering,

			End
		};

		/**
		 * @brief : ダメージの行動(ビヘイビア)表
		 */
		enum class DamageBehavior
		{
			Damage,
			Death,

			End
		};

		/**
		 * @brief :攻撃の行動(ビヘイビア)表
		 */
		enum class AttackBehavior
		{
			// 近距離攻撃
			// HP 75 〜 100(%)での行動
			Begin_Atk_Melee_1,
			Atk_N1,
			Atk_N2,
			Atk_N4,
			End_Atk_Melee_1,

			// HP 50 〜 74(%)での行動
			Begin_Atk_Melee_2,
			Atk_N1_N2,
			Atk_N1_N3,
			Atk_N1_N4,
			Atk_N2_N1,
			Atk_N2_N3,
			Atk_N2_N4,
			End_Atk_Melee_2,

			// HP 25 〜 49(%)での行動
			Begin_Atk_Melee_3,
			End_Atk_Melee_3,

			// HP 1 ~ 24(%)での行動
			Begin_Atk_Melee_4,
			End_Atk_Melee_4,

			// 遠距離攻撃
			// HP 75 〜 100(%)での行動
			Begin_Atk_Long_1,
			Atk_N3,
			Atk_E2,
			End_Atk_Long_1,

			// HP 50 〜 74(%)での行動
			Begin_Atk_Long_2,
			Atk_N3_E2,
			Atk_E2_N3,
			End_Atk_Long_2,

			// HP 25 〜 49(%)での行動
			Begin_Atk_Long_3,
			End_Atk_Long_3,

			// HP 1 ~ 24(%)での行動
			Begin_Atk_Long_4,
			End_Atk_Long_4,

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
		void GameUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

	private:
		// 行動(ビヘイビア)の更新StateMachine
		StateMachine<BossBehavior, void, const float> boss_behavior{};
		// 動き系ビヘイビア
		system::BehaviorTree<MovementBehavior, void, const float, const bool> movement_behavior{};
		// ダメージ系ビヘイビア
		system::BehaviorTree<DamageBehavior, void, const float, const bool> damage_behavior{};
		// 攻撃系ビヘイビア
		system::BehaviorTree<AttackBehavior, void, const float, const bool> attack_behavior{};
		// キーフレーム中のイベント管理
		std::map<AnimationData, system::KeyframeEvent> keyframe_events{};
		//-- パラメータ --//
		// 歩行の速さ
		float walk_speed{ 100.f };
		// 走りの速さ
		float dash_speed{ 150.f };
		// ノックバックレベル
		KnockBackLevel knock_back_level{};
		// ノックバックの速さ
		float knock_back_speed{ 10.f };
		//-- 行動制御 --//
		// 近距離攻撃の境界距離
		float atk_melee_distance{ 100.f };
		// 追跡時間
		float tracing_time{};
		// 最大追跡時間
		float max_tracking_time{ 10.f };

		// 攻撃範囲内か
		bool is_in_attack_range{ false };
		// 次のビヘイビアに移行するか
		bool is_next_sequence{ false };
		// ビヘイビアツリーの一連の処理が完了したか
		bool is_behavior_completed{ false };

		// 当たり判定用プレハブ
		mapping::rename_type::UUID atk_collider_prefab_id{};
		// エネルギーショットのプレハブ
		mapping::rename_type::UUID shot_prefab_id{};
		// エネルギーショット生成時の位置
		std::string spawn_shot_node_name{};

		// 次ビヘイビアでの攻撃種
		AttackType next_attack_type{};
		// 攻撃範囲内に存在するかのフラグ
		std::bitset<static_cast<int>(AttackType::End)> bit_in_atk_range{};
		// 最大履歴数(初期値 : 10)
		u_int max_history_count{ 10 };
		// 攻撃種類の履歴(このリストを元に徘徊時の次の攻撃を決める)
		std::vector<AttackType> attack_history{};
		// ビヘイビア(Move,Attack)の履歴
		std::vector<BossBehavior> behavior_history{};

		//-- ヒットエフェクト系 --//
		// 現在のヒットエフェクトID
		mapping::rename_type::UUID current_hit_effect_id{};
		// ヒットエフェクト(S) ID
		mapping::rename_type::UUID hit_effect_s{};
		// ヒットエフェクト(M) ID
		mapping::rename_type::UUID hit_effect_m{};
		// ヒットエフェクト(L) ID
		mapping::rename_type::UUID hit_effect_l{};

		//-- 初期化 --//
		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;
		/**
		 * @brief : 履歴の初期化
		 * @remark : 攻撃、ビヘイビア
		 */
		void InitializeHistory();


		void OnAttack(const collision::HitResult& hit_result) override;
		void OnDamaged(const DamageData& damage_data, const collision::HitResult& hit_result) override;

		/**
		 * @brief : キーフレームイベントの登録
		 */
		void RegistryKeyframeEvent(AnimationData anim_data, const std::string& key_name);
		/**
		 * @brief : 攻撃種類(attack_history)に履歴の追加
		 */
		void AddAttackHistory(AttackType type);
		/**
		 * @brief : ビヘイビア(behavior_history)に履歴の追加
		 */
		void AddBehaviorHistory(BossBehavior type);

		template <auto F>
		[[nodiscard]]
		std::function<void(float)> GetBehaviorUpdateFunc();
		template<auto F>
		[[nodiscard]]
		std::function<void(float, bool)> GetBehaviorActFunc();
		[[nodiscard]]
		int GetAnimDataIndex(AnimationData anim_data) const;
		[[nodiscard]]
		system::KeyframeEvent& GetKeyframeEvent(AnimationData anim_data);
		/**
		 * @brief : 次の攻撃種類の取得
		 */
		[[nodiscard]]
		AttackType GetNextAttackType() const;
		/**
		 * @brief : 次のビヘイビア種類の取得
		 */
		[[nodiscard]]
		BossBehavior GetNextBehaviorType() const;

		/**
		 * @brief : HPに応じての行動パターンを取得
		 */
		[[nodiscard]]
		AttackPattern GetAttackPattern() const;

		void OnEnterAttackRange(ColliderMessageSenderComponent & sender);
		void OnExitAttackRange(ColliderMessageSenderComponent& sender);

		//-- ビヘイビア更新処理 --//
		/**
		 * @return : true -> 更新中のビヘイビアが終了
		 */
		void BehaviorUpdate(float dt);
		void BehaviorMoveUpdate(float dt);
		void BehaviorDamageUpdate(float dt);
		void BehaviorAttackUpdate(float dt);

		//-- 敵(ボス)の挙動 --//
		/**
		 * @return : true -> 次のビヘイビアに移行
		 */
		// 待機
		void Idle(float dt, bool is_start);
		// 歩き系
		void Tracking(float dt, bool is_start);
		void Wandering(float dt, bool is_start);
		// ダメージ系
		void Damage(float dt, bool is_start);
		void Death(float dt, bool is_start);
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
