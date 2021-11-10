//#pragma once
//#include <unordered_map>
//
//#include "enemy_base_component.h"
//#include "state_machine.h"
//
//namespace cumulonimbus::component
//{
//	class EnemySlimeComponent final : public EnemyBaseComponent
//	{
//	private:
//		/**
//		 * @brief : “G(Slime)‚Ìó‘Ô‘JˆÚ•\
//		 */
//		enum class SlimeState
//		{
//			T_Pose,
//			Idle,			// ‘Ò‹@
//			Walk,			// •à‚«
//			Tracking,		// ’ÇÕ
//			Birth,			// ¶¬
//			Death,			// €–S
//			Damage,			// ”í’e
//			Stun,			// ƒXƒ^ƒ“
//			Attack_Bite,	// UŒ‚(Šš‚İ‚Â‚«)
//			Attack_Charge,	// UŒ‚(“Ëi)
//
//			End
//		};
//
//		/**
//		 * @brief : FBX‚ª‚ÂƒAƒjƒ[ƒVƒ‡ƒ“ƒf[ƒ^
//		 */
//		enum class AnimationData
//		{
//			Idle_00,
//			Attack_Bite,
//			Attack_Charge,
//			Birth,
//			Death,
//			Damage,
//			Idle_01,
//			Stun,
//			Walk,
//
//			End
//		};
//
//	public:
//		explicit EnemySlimeComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
//		explicit EnemySlimeComponent()  = default; // for cereal
//		~EnemySlimeComponent() override = default;
//
//		void GameUpdate(float dt) override;
//		void RenderImGui()	  override;
//
//		void Load(ecs::Registry* registry) override;
//
//	private:
//		// “G(Slime)‚Ìó‘ÔŠÇ—•Ï”
//		StateMachine<SlimeState, void, const float> slime_state{};
//
//		//-- ƒpƒ‰ƒ[ƒ^ --//
//		// Quaternion::Slerp‚Ì‰ñ“]ŠÔ”{—¦
//		float rotation_time_rate{ 1.0f };
//		// •às‚Ì‘¬‚³
//		float walk_speed{ 100 };
//		// ’ÇÕ’†’f‚Ì’·‚³
//		float tracking_interruption_distance{ 200 };
//		// ’ÇÕ‚©‚çUŒ‚‚ÉˆÚ‚é‹——£
//		float attack_transition_distance{ 100 };
//		// ’ÇÕ‚©‚çUŒ‚‚ÉˆÚ‚éŠp“x(“x”–@)
//		float attack_transition_angle{ 90 };
//		// ’ÇÕ‚ÉˆÚ‚é‹——£
//		float tracking_transition_distance{ 200 };
//		// ’ÇÕ‚ÉˆÚ‚éŠp“x()
//		float tracking_transition_angle{ 90 };
//
//		//-- ‚»‚Ì‘¼ --//
//
//		/**
//		 * @brief : enum class(AnimationData)‚ğintŒ^‚É•ÏŠ·
//		 */
//		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;
//
//		//-- StateMachineƒNƒ‰ƒX‚ÅŠÇ—‚·‚é“G‚Ìó‘ÔŠÖ” --//
//		void TPose(float dt);
//		void Idle(float dt);
//		void Walk(float dt);
//		void Tracking(float dt);
//		void Birth(float dt);
//		void Death(float dt);
//		void Damage(float dt);
//		void Stun(float dt);
//		void AttackBite(float dt);
//		void AttackCharge(float dt);
//	};
//} // cumulonimbus::component
