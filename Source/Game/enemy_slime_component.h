#pragma once
#include "enemy_base_component.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class EnemySlimeComponent final : public EnemyBaseComponent
	{
	private:
		// �G(Slime)�̏�ԑJ�ڕ\
		enum class SlimeState
		{
			T_Pose,
			Idle,			// �ҋ@
			Walk,			// ����
			Birth,			// ����
			Death,			// ���S
			Hit,			// ��e
			Stun,			// �X�^��
			Attack_Bite,	// �U��(���݂�)
			Attack_Charge,	// �U��(�ːi)

			End
		};

		// FBX�����A�j���[�V�����f�[�^
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
		// �G(Slime)�̏�ԊǗ��ϐ�
	};
} // cumulonimbus::component
