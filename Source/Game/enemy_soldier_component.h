//#pragma once
//#include "enemy_base_component.h"
//#include "state_machine.h"
//
//namespace cumulonimbus::component
//{
//	class EnemySoldierComponent final : public EnemyBaseComponent
//	{
//	private:
//		/**
//		 * @brief : “G(Soldier)‚Ìó‘Ô‘JˆÚ•\
//		 */
//		enum class SoldierState
//		{
//			Idle,
//			Walk,
//			Tracking,
//			Damage,
//			Death,
//			Attack_01,
//			Attack_02,
//			Attack_03,
//
//			End
//		};
//
//		/**
//		 * @brief : FBX‚ª‚ÂƒAƒjƒ[ƒVƒ‡ƒ“ƒf[ƒ^
//		 */
//		enum class AnimationData
//		{
//			Walk,
//			Run,
//			Attack_03,
//			Attack_01,
//			Attack_02,
//			Death,
//			Death_Loop,
//			Damage,
//			Attack_All,
//			Idle,
//
//			End
//		};
//
//	public:
//		explicit EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
//		explicit EnemySoldierComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const EnemySoldierComponent& copy_comp); // for prefab
//		explicit EnemySoldierComponent()  = default; // for cereal
//		~EnemySoldierComponent() override = default;
//
//		void GameUpdate(float dt) override;
//		void RenderImGui()	  override;
//
//		void Load(ecs::Registry* registry) override;
//
//		template<class Archive>
//		void load(Archive&& archive, uint32_t version);
//
//		template<class Archive>
//		void save(Archive&& archive, uint32_t version) const;
//
//	private:
//		// “G(ForestDemon)‚Ìó‘ÔŠÇ—•Ï”
//		StateMachine<SoldierState, void, const float> soldier_state{};
//
//		//-- ƒpƒ‰ƒ[ƒ^ --//
//		// Quaternion::Slerp‚Ì‰ñ“]ŠÔ”{—¦
//		float rotation_time_rate{ 1.0f };
//		// •às‚Ì‘¬‚³
//		float walk_speed{ 100 };
//		// ’ÇÕ’†‚Ì‘¬‚³
//		float tracking_speed{ 150 };
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
//		// “Ë‚«UŒ‚‚ÉˆÚ‚é‹——£
//		float attack_thrust_distance{ 50 };
//
//		/**
//		 * @brief : enum class(AnimationData)‚ğintŒ^‚É•ÏŠ·
//		 */
//		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;
//
//		//-- StateMachineƒNƒ‰ƒX‚ÅŠÇ—‚·‚é“G‚Ìó‘ÔŠÖ” --//
//		void Idle(float dt);
//		void Walk(float dt);
//		void Tracking(float dt);
//		void Damage(float dt);
//		void Death(float dt);
//		void Attack01(float dt);
//		void Attack02(float dt);
//		void Attack03(float dt);
//	};
//
//} // cumulonimbus::component