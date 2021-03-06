//#pragma once
//#include "enemy_base_component.h"
//#include "state_machine.h"
//
//namespace cumulonimbus::component
//{
//	class EnemyForestDemonComponent final : public EnemyBaseComponent
//	{
//	private:
//		/**
//		 * @brief : G(Forest Demon)ฬ๓ิJฺ\
//		 */
//		enum class ForestDemonState
//		{
//			T_Pose,
//			Idle,				// า@(สํ)
//			Ready,				// า@(ํฌO)
//			Walk,				// เซ
//			Tracking,			// วี
//			Death,				// S
//			Damage,				// ํe
//			Attack_Normal,		// U(สํ)
//			Attack_Mow_Down,	// U(ใฌฅข)
//
//			End
//		};
//
//		/**
//		 * @brief : FBXชยAj[Vf[^
//		 */
//		enum class AnimationData
//		{
//			Attack_Mow_Down,
//			Attack_Normal,
//			Death,
//			Damage,
//			Idle_00,
//			Idle_01,
//			Ready,
//			Run,
//			Walk,
//
//			End
//		};
//
//	public:
//		explicit EnemyForestDemonComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
//		explicit EnemyForestDemonComponent()  = default; // for cereal
//		~EnemyForestDemonComponent() override = default;
//
//		void GameUpdate(float dt) override;
//		void RenderImGui()    override;
//		
//		void Load(ecs::Registry* registry) override;
//
//	private:
//		// G(ForestDemon)ฬ๓ิวฯ
//		StateMachine<ForestDemonState, void, const float> forest_demon_state{};
//
//		//-- p[^ --//
//		// Quaternion::Slerpฬ๑]ิ{ฆ
//		float rotation_time_rate{ 1.0f };
//		// เsฬฌณ
//		float walk_speed{ 100 };
//		// วีฬฌณ
//		float tracking_speed{ 150 };
//		// วีfฬทณ
//		float tracking_interruption_distance{ 200 };
//		// วีฉ็Uษฺ้ฃ
//		float attack_transition_distance{ 100 };
//		// วีฉ็Uษฺ้px(x@)
//		float attack_transition_angle{ 90 };
//		// วีษฺ้ฃ
//		float tracking_transition_distance{ 200 };
//		// วีษฺ้px()
//		float tracking_transition_angle{ 90 };
//
//		//-- ปฬผ --//
//
//
//		/**
//		 * @brief : enum class(AnimationData)๐int^ษฯท
//		 */
//		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;
//
//		//-- StateMachineNXลวท้Gฬ๓ิึ --//
//		void TPose(float dt);
//		void Idle(float dt);
//		void Ready(float dt);
//		void Walk(float dt);
//		void Tracking(float dt);
//		void Death(float dt);
//		void Damage(float dt);
//		void AttackNormal(float dt);
//		void AttackMowDown(float dt);
//	};
//} // cumulonimbus::component
