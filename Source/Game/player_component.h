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
#include "enum_state_map.h"

namespace cumulonimbus::component
{
	class PlayerComponent final : public Actor3DComponent
	{
	private:
		// プレイヤーの状態を表す
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
		enum class AnimationState
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
			Jump_Begin,
			Jump_Landing,
			Attacking_Strong_01,
			Avoid_Dash_Begin,
			Avoid_Dash_End,
			Dash,
			Dash_Attack,
			Walk_Back,
			Walk_Front,
			Idle,
			Damage_Small,
			Damage_Big,
			Knock_Down_Front_Loop,
			Knock_Down_Front_Stand_Up,
			Die,
			Die_Loop,
			Attack_Normal_01_End,
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

			End
		};

		/**
		 * @brief : アニメーション再生中に次のアニメーションに移行するためのフレーム
		 *			(攻撃などの先行入力でモーション最中にアニメーションを移行するときなどに使用)
		 */
		enum class NextAnimationFrame
		{
			Attack_Normal_01,
			Attack_Normal_02,
			Attack_Normal_03,

			End
		};

	public:
		explicit PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent);
		explicit PlayerComponent() = default; // for cereal
		~PlayerComponent() override = default;

		void NewFrame(float dt) override;
		void Update(float dt)	override;
		void RenderImGui()		override;

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;
	private:
		// プレイヤーの状態変数
		StateMachine<PlayerState, void, const float> player_state{};
		// 先行入力(入力なし(リセット) == PlayerState::End)
		PlayerState precede_input = PlayerState::End;
		// アニメーションキーフレームの調整用(終端フレームの調整)
		std::map<std::string, u_int> adjust_keyframe_map{};
		// アニメーション中断フレーム(先行入力などで使用)
		std::map<AnimationState, u_int> animation_break_frame{};

		//-- 状態に応じてのスピード設定 --//
		float walk_speed = 1000;	 // 歩きの速さ
		float dash_speed = 2000;	 // 走りの速さ
		float avoid_dash_speed = 3000;  // 回避ダッシュ速度
		float jump_movement_speed = 1000;

		// パッド入力のデッドゾーン値
		float threshold = 0.05f;
		// パッド長押し時間
		float long_press_time = 0.0f;
		// パッド長押しスロット数(長押し判定に使用)
		int   long_press_slot = 60;

		// ジャンプ強度
		float jump_strength = 5000;
		// 重力
		float gravity = 1.0f;
		//// 現在のジャンプ速度
		//float current_jump_velocity = 0;

		/**
		 * @brief : State変更時にアニメーション用のメンバ変数の初期化
		 */
		void InitializeAnimationVariable(PlayerState state = PlayerState::End, float long_press_time = 0.0f)
		{
			precede_input = state;
			this->long_press_time = long_press_time;
		}

		/**
		 * @brief : enum class(AnimationState)をint型に変換
		 */
		[[nodiscard]] int GetAnimStateIndex(AnimationState anim_state) const;

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
		 * @brief : カメラワーク管理関数
		 */
		void CameraWork();

		/**
		 * @brief		: 速度調整関数
		 * @param dt	: delta time
		 * @param speed	: xyzそれぞれの方向の速度
		 */
		void AdjustVelocity(float dt, const DirectX::SimpleMath::Vector3& speed);

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
		void SetAnimationBreakFrame(AnimationState state, u_int keyframe);

		/**
		 * @brief : 次のアニメーションがPlayerState::Attacking_Normal_Long_Press
		 *			に遷移して良いかの判定関数。
		 *			long_press_timeの時間で判断
		 */
		[[nodiscard]] bool IsNextAnimationLongPressAttack() const;

		/**
		 * @brief  : 入力がデッドゾーン内にあるかどうか判定
		 * @return : true : デッドゾーン内にある
		 */
		[[nodiscard]] bool IsDeadZone() const;

		/**
		 * @brief			: 現アニメーションキーフレームが"animation_break_frame"で設定したフレームを超えているか
		 * @param state		: "animation_break_frame"に登録されているAnimationState
		 * @return			: true -> 超えている
		 */
		[[nodiscard]] bool IsBreakAnimationFrame(AnimationState state) const;

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
		void AttackingJumpStrong(float dt);		// ジャンプ攻撃(強)
		void AttackJumpStrongEnd(float dt);
		void DashAttack(float dt);				// ダッシュ攻撃
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::PlayerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::PlayerComponent)