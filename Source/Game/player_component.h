#pragma once
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
		// �v���C���[�̏�Ԃ�\��
		enum class PlayerState
		{
			T_Pose,
			Idle,
			Walk,
			Run,
			Evasion,
			Attack_01,
			Attack_02,
			Attack_03,
			Run_Attack,
			Damage,
			Revenge_Guard,
			Die,

			End
		};

		// FBX�����A�j���[�V�����f�[�^
		enum class AnimationState
		{
			T_Pose,
			Idle,
			Walk_Front,
			Walk_Back,
			Attack_01,
			Attack_02,
			Attack_03,
			Run,
			Run_Attack,
			Damage,
			Die,
			Die_Loop,
			Revenge_Guard_Start,
			Revenge_Guard_Loop,
			Revenge_Guard_End,
			Evade_Front,
			Evade_Back,

			End
		};

	public:
		explicit PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent);
		explicit PlayerComponent()  = default; // for cereal
		~PlayerComponent() override = default;

		void NewFrame(float dt) override;
		void Update(float dt)	override;
		void RenderImGui()		override;

		void Save(const std::string& file_path) override;
		void Load(const std::string& file_path_and_name) override;
	private:
		StateMachine<PlayerState, void, const float> player_state{};

		// �p�b�h���͂̃f�b�h�]�[���l
		float threshold = 0.05f;

		/*
		 * brief : enum class(AnimationState)��int�^�ɕϊ�
		 */
		[[nodiscard]] int GetAnimStateIndex(AnimationState anim_state) const;

		/*
		 * brief : ���C�L���X�g�Ŏg�p����p�����[�^�̃Z�b�g
		 */
		void SetRayCastParameter(float dt);

		/*
		 * brief : StateMachine�N���X�ŊǗ�����v���C���[�̏�Ԋ֐�
		 */
		void TPose(float dt);
		void Idle(float dt);
		void Walk(float dt);
		void Run(float dt);
		void RunAttack(float dt);
		void Damage(float dt);
		void Evasion(float dt);
		void Attack01(float dt);
		void Attack02(float dt);
		void Attack03(float dt);
		void RevengeGuard(float dt);
		void Death(float dt);
	};
} // cumulonimbus::component

CEREAL_REGISTER_TYPE(cumulonimbus::component::PlayerComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::ComponentBase, cumulonimbus::component::PlayerComponent)