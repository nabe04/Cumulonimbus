#pragma once
#include <map>
#include <string>

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "actor3d_component.h"
#include "state_machine.h"
#include "collision_component.h"
#include "damageable_component.h"

namespace cumulonimbus::component
{
	class PlayerComponent final : public Actor3DComponent
	{
	private:
		// プレイヤーの状態遷移表
		enum class PlayerState
		{
			T_Pose,
			Idle,							// 待機
			Walk_Front,						// 歩き
			Walk_Back,
			Damage_Small,					// ダメージ
			Damage_Big,
			Jump_Begin,						// ジャンプ
			Jump_Loop,
			Jump_Landing,
			Jump_End,
			Knock_Down_Front_Loop,			// ノックバック
			Knock_Down_Front_Stand_Up,
			Die,							// 死亡
			Attack_Normal_01,				// 通常攻撃(弱)
			Attack_Normal_02,
			Attack_Normal_03,
			Attack_Normal_04_Begin,
			Attacking_Normal_04,
			Attack_Normal_04_End,
			Attacking_Normal_Long_Press,	// 通常攻撃(弱長押し)
			Attack_Normal_Long_Press_End,
			Attack_Strong_01,				// 通常攻撃(強)
			Attack_Strong_02,
			Attack_Strong_03,
			Attack_Strong_04,
			Attack_Round_Up_Begin,			// 切り上げ攻撃
			Attacking_Round_Up,
			Attack_Round_Up_Fall,
			Attack_Round_Up_End,
			Attacking_Jump_01,				// ジャンプ攻撃(弱)
			Attacking_Jump_02,
			Attacking_Jump_03,
			Attacking_Jump_04,
			Attack_Jump_01_End,
			Attack_Jump_02_End,
			Attack_Jump_03_End,
			Attack_Jump_04_End,
			Attack_Jumping_Strong_Begin,	// ジャンプ攻撃(強)
			Attacking_Jump_Strong,
			Attack_Jump_Strong_End,
			Avoid_Dash_Begin,				// ダッシュ回避
			Avoid_Dash_End,
			Dash,
			Dash_Attack,					// ダッシュ攻撃

			End
		};

		// FBXが持つアニメーションデータ
		enum class AnimationData
		{
			T_Pose,
			Attacking_Normal_01,
			Attacking_Normal_02,
			Attacking_Normal_03,
			Attack_Normal_04_Begin,
			Attacking_Normal_04,
			Attack_Normal_04_End,
			Attacking_Normal_Long_Press,
			Attacking_Strong_02,
			Attacking_Strong_03,
			Attacking_Strong_04,
			Attack_Round_Up_Begin,
			Attacking_Round_Up,
			Attack_Round_Up_Fall,
			Attack_Round_Up_End,
			Attacking_Jump_01,
			Attacking_Jump_02,
			Attacking_Jump_03,
			Attacking_Jump_04,
			Attack_Jumping_Strong_Begin,
			Attacking_Jump_Strong,
			Attack_Jump_Strong_End,
			Jump_Landing,
			Attacking_Strong_01,
			Avoid_Dash_Begin,
			Avoid_Dash_End,
			Dash,
			Dash_Attack,
			Walk_Back,
			Walk_Front,
			Idle,
			Attack_Normal_02_End,
			Attack_Normal_Long_Press_End,
			Attack_Jump_01_End,
			Attack_Jump_02_End,
			Attack_Jump_03_End,
			Attack_Jump_04_End,
			Attack_Normal_01,
			Attack_Normal_02,
			Attack_Normal_03,
			Attack_Normal_All,
			Attack_Strong_01,
			Attack_Strong_02,
			Attack_Strong_03,
			Attack_Strong_04,
			Attack_Strong_All,
			Jump_Loop,
			Jump_End,
			Attack_Jump_Strong_All,
			Attack_Normal_04_All,
			Die,
			Die_Loop,
			Jump_Begin,
			Damage_Small,
			Knock_Down_On_Damage,
			Knock_Down_Loop,
			Knock_Down_On_Stand,
			Attack_Normal_01_End,
			Jump_Begin_2,

			End
		};
	public:
		explicit PlayerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PlayerComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const PlayerComponent& copy_comp); // for prefab
		explicit PlayerComponent() = default; // for cereal
		~PlayerComponent() override = default;

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		void Start() override;

		void PreGameUpdate(float dt)		override;
		void GameUpdate(float dt)			override;
		void RenderImGui()					override;

		void Load(ecs::Registry* registry)	override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;


		[[nodiscard]]
		u_int GetCurrentDamageAmount() const
		{
			return current_damage_amount;
		}
	private:
		// プレイヤーの状態管理変数
		StateMachine<PlayerState, void, const float> player_state{};
		// 先行入力(入力なし(リセット) == PlayerState::End)
		PlayerState precede_input{ PlayerState::End };
		// アニメーションキーフレームの調整用(終端フレームの調整)
		std::map<std::string, u_int> adjust_keyframe_map{};
		// アニメーション中断フレーム(先行入力などで使用)
		std::map<AnimationData, u_int> animation_break_frame{};

		// アニメーション遷移時間
		float anim_switch_time{ 0.1f };

		//-- 状態に応じてのスピード設定 --//
		// 歩きの速度
		float walk_speed{ 300.0f };
		// 走りの速度
		float dash_speed{ 700.0f };
		// 通常攻撃04(ジャンプ攻撃)時の速度
		float attack_04_speed{ 700.0f };
		// ダッシュ攻撃の速度
		float dash_attack_speed{ 300.0f };
		// 回避ダッシュ速度
		float avoid_dash_speed{ 900.0f };
		float jump_movement_speed{ 300.0f };
		//-- 状態の応じての攻撃力設定 --//
		// 現在の攻撃力
		u_int current_damage_amount{ 1 };
		// 通常弱攻撃時のダメージ
		u_int damage_normal_01{ 1 };
		u_int damage_normal_02{ 1 };
		u_int damage_normal_03{ 1 };
		u_int damage_normal_04{ 1 };
		// 通常強攻撃時のダメージ
		u_int damage_strong_01{ 2 };
		u_int damage_strong_02{ 2 };
		u_int damage_strong_03{ 2 };
		u_int damage_strong_04{ 2 };
		// 空中弱攻撃時のダメージ
		u_int damage_jump_normal_01{ 1 };
		u_int damage_jump_normal_02{ 1 };
		u_int damage_jump_normal_03{ 1 };
		u_int damage_jump_normal_04{ 1 };
		// 空中強攻撃時のダメージ
		u_int damage_jump_strong{ 2 };

		//-- 各状態からの遷移時間 --//
		float switch_dash_attack_to_idle{ 0.3f };

		//-- 状態フラグ --//
		// ジャンプフラグ
		bool is_jumping{ false };
		// 回避フラグ
		bool is_avoid{ false };

		// パッド入力のデッドゾーン値
		float threshold{ 0.05f };
		// パッド長押し時間
		float long_press_time{ 0.0f };
		// パッド長押しスロット数(長押し判定に使用)
		int   long_press_slot{ 60 };

		// 通常攻撃04(ジャンプで斬りつけ)時のジャンプ強度
		float attack_04_jump_strength{ 500 };

		/**
		 * @brief : State変更時にアニメーション用のメンバ変数の初期化
		 */
		void InitializeAnimationVariable(const PlayerState state = PlayerState::End, const float long_press_time = 0.0f)
		{
			precede_input = state;
			this->long_press_time = long_press_time;
		}

		/**
		 * @brief : enum class(AnimationState)をint型に変換
		 */
		[[nodiscard]]
		int GetAnimDataIndex(AnimationData anim_state) const;

		/**
		 * @brief : レイキャストで使用するパラメータのセット
		 */
		void SetRayCastParameter(float dt);

		/**
		 * @brief : 移動管理関数
		 */
		void Movement(float dt);

		/**
		 * @brief : 向き管理関数
		 */
		void Rotation(float dt);

		/**
		 * @brief	: 当たり判定が行われてた時の処理
		 * @details	: 当たり判定の処理自体はCollisionManagerで行い
		 *			  処理が行われた後の動きの処理をする
		 */
		void Collision() const;

		/**
		 * @brief : カメラワーク管理関数
		 */
		void CameraWork();

		/**
		 * @brief					: アニメーションキーフレーム調整値の設定
		 * @param animation_name	: モデルのアニメーション名
		 * @param keyframe			: 調整後のモデルの終端キーフレーム
		 * @attention				: "animation_name"がモデルのアニメーション名に存在しない場合例外を出す
		 */
		void SetAdjustKeyFrame(const std::string& animation_name, u_int keyframe);

		/**
		 * @brief					: 次アニメーション移行フレームの設定
		 * @param state				: フレームを設定したいAnimationState
		 * @param keyframe			: 移行フレームの指定
		 */
		void SetAnimationBreakFrame(AnimationData state, u_int keyframe);

		void SetCurrentDamage(const u_int amount)
		{
			current_damage_amount = amount;
		}


		/**
		 * @brief : 次のアニメーションがPlayerState::Attacking_Normal_Long_Press
		 *			に遷移して良いかの判定関数。
		 *			long_press_timeの時間で判断
		 */
		[[nodiscard]]
		bool IsNextAnimationLongPressAttack() const;

		/**
		 * @brief  : 入力がデッドゾーン内にあるかどうか判定
		 * @return : true : デッドゾーン内にある
		 */
		[[nodiscard]]
		bool IsDeadZone() const;

		/**
		 * @brief			: 現アニメーションキーフレームが"animation_break_frame"で設定したフレームを超えているか
		 * @param state		: "animation_break_frame"に登録されているAnimationState
		 * @return			: true -> 超えている
		 */
		[[nodiscard]]
		bool IsBreakAnimationFrame(AnimationData state) const;

		/**
		 * @brief : StateMachineの初期化
		 * @param registry :
		 * @param ent : 自分のエンティティ
		 */
		void InitializeMoveState(ecs::Registry* registry, const mapping::rename_type::Entity& ent);

		/**
		 * @brief : ダメージ処理
		 */
		void OnDamaged(const component::DamageData& damage_data);

		/**
		 * @brief : 他のコリジョンとのヒット時処理
		 */
		void OnHit(const DamageData& damage_data, const collision::HitResult& hit_result);

		/**
		 * @brief : アニメーション遷移時間の取得 & 遷移時間のリセット
		 * @param reset_time : リセット時の遷移時間
		 * @return : リセットされる前の遷移時間
		 */
		float GetAndResetAnimSwitchTime(float reset_time = 0.1f);

		/**
		 * @brief : StateMachineクラスで管理するプレイヤーの状態関数
		 */
		void TPose(float dt);					// T-ポーズ
		void Idle(float dt);					// 待機
		void WalkFront(float dt);				// 歩き
		void WalkBack(float dt);
		void AvoidDashBegin(float dt);			// ダッシュ回避
		void AvoidDashEnd(float dt);
		void Dash(float dt);					// ダッシュ
		void DamageSmall(float dt);				// ダメージ
		void DamageBig(float dt);
		void KnockDownFrontLoop(float dt);		// ノックダウン
		void KnockDownFrontStandUp(float dt);
		void JumpBegin(float dt);				// ジャンプ
		void JumpLoop(float dt);
		void JumpLanding(float dt);
		void JumpEnd(float dt);
		void Die(float dt);						// 死亡
		void AttackingNormal01(float dt);		// 通常攻撃(弱)
		void AttackingNormal02(float dt);
		void AttackingNormal03(float dt);
		void AttackingNormal04(float dt);
		void AttackNormal04Begin(float dt);
		void AttackNormal04End(float dt);
		void AttackingNormalLongPress(float dt); // 通常攻撃(弱長押し)
		void AttackNormalLongPressEnd(float dt);
		void AttackingStrong01(float dt);		 // 通常攻撃(強)
		void AttackingStrong02(float dt);
		void AttackingStrong03(float dt);
		void AttackRoundUpBegin(float dt);		 // 切り上げ攻撃
		void AttackingStrong04(float dt);
		void AttackingRoundUp(float dt);
		void AttackRoundUpFall(float dt);
		void AttackRoundUpEnd(float dt);
		void AttackingJump01(float dt);			 // ジャンプ攻撃(弱)
		void AttackingJump02(float dt);
		void AttackingJump03(float dt);
		void AttackingJump04(float dt);
		void AttackJump01End(float dt);
		void AttackJump02End(float dt);
		void AttackJump03End(float dt);
		void AttackJump04End(float dt);
		void AttackJumpStrongBegin(float dt);	// ジャンプ攻撃(強)
		void AttackingJumpStrong(float dt);
		void AttackJumpStrongEnd(float dt);
		void DashAttack(float dt);				// ダッシュ攻撃
	};
} // cumulonimbus::component