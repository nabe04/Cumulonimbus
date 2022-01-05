#pragma once
#include "enemy_base_component.h"

#include "state_machine.h"
#include "keyframe_event.h"

namespace cumulonimbus::component
{
	class EnemyBossComponent : public EnemyBaseComponent
	{
	private:
		/**
		 * @brief : �G(Boss)�̏�ԑJ�ڕ\
		 */
		enum class BossState
		{
			// �ҋ@
			Idle,
			// �����n
			Walk,
			Run,
			Tracking,
			// �_���[�W�n
			Damage,
			Knock_Down,
			Death,
			// �U���n
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
		 * @brief : FBX�����A�j���[�V�����f�[�^
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

		void Load(ecs::Registry* registry);

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;

	private:
		// �G(Boss)�̏�ԊǗ��ϐ�
		StateMachine<BossState, void, const float> boss_state{};
		//

		//-- �p�����[�^ --//
		// ���s�̑���
		float walk_speed{ 100.f };
		// ����̑���
		float dash_speed{ 150.f };

		void Initialize(ecs::Registry* registry, mapping::rename_type::Entity ent) override;

		[[nodiscard]]
		int GetAnimDataIndex(AnimationData anim_data) const;
	};
} // cumulonimbus::component
