#pragma once
#include "enemy_base_component.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class EnemyForestDemonComponent final : public EnemyBaseComponent
	{
	private:
		/**
		 * @brief : “G(Forest Demon)‚Ìó‘Ô‘JˆÚ•\
		 */
		enum class ForestDemonState
		{
			T_Pose,
			Idle,				// ‘Ò‹@(’Êí)
			Ready,				// ‘Ò‹@(í“¬‘O)
			Walk,				// •à‚«
			Tracking,			// ’ÇÕ
			Death,				// €–S
			Damage,				// ”í’e
			Attack_Normal,		// UŒ‚(’Êí)
			Attack_Mow_Down,	// UŒ‚(“ã‚¬•¥‚¢)

			End
		};

		/**
		 * @brief : FBX‚ª‚ÂƒAƒjƒ[ƒVƒ‡ƒ“ƒf[ƒ^
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
		// “G(ForestDemon)‚Ìó‘ÔŠÇ—•Ï”
		StateMachine<ForestDemonState, void, const float> forest_demon_state{};

		//-- ƒpƒ‰ƒ[ƒ^ --//
		// Quaternion::Slerp‚Ì‰ñ“]ŠÔ”{—¦
		float rotation_time_rate{ 1.0f };
		// •às‚Ì‘¬‚³
		float walk_speed{ 100 };
		// ’ÇÕ’†‚Ì‘¬‚³
		float tracking_speed{ 150 };
		// ’ÇÕ’†’f‚Ì’·‚³
		float tracking_interruption_distance{ 200 };
		// ’ÇÕ‚©‚çUŒ‚‚ÉˆÚ‚é‹——£
		float attack_transition_distance{ 100 };
		// ’ÇÕ‚©‚çUŒ‚‚ÉˆÚ‚éŠp“x(“x”–@)
		float attack_transition_angle{ 90 };
		// ’ÇÕ‚ÉˆÚ‚é‹——£
		float tracking_transition_distance{ 200 };
		// ’ÇÕ‚ÉˆÚ‚éŠp“x()
		float tracking_transition_angle{ 90 };

		//-- ‚»‚Ì‘¼ --//


		/**
		 * @brief : enum class(AnimationData)‚ğintŒ^‚É•ÏŠ·
		 */
		[[nodiscard]] int GetAnimDataIndex(AnimationData anim_data) const;

		//-- StateMachineƒNƒ‰ƒX‚ÅŠÇ—‚·‚é“G‚Ìó‘ÔŠÖ” --//
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
