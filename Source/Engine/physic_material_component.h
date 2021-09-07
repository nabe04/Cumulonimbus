#pragma once
#include "component_base.h"

namespace cumulonimbus::component
{
	class PhysicMaterialComponent : public ComponentBase
	{
	public:
		/**
		 * @attention : �ڐG���Ă��� 2 �̃R���C�_�[�ɈقȂ� combine ���[�h���ݒ肳��Ă���Ƃ��͓��ʂōł������D��x�����@�\���g�p�����B
		 *				 Average < Minimum < Multiply < Maximum
		 *				 ����}�e���A���� Average ���[�h�������A���� Maximum ���[�h�����Ƃ��A combine �@�\�� Maximum �Ŏg�p�����B
		 */
		enum class CombineSetting
		{
			Average,
			Minimum,
			Maximum,
			Multiply,

			End
		};

	public:
		using ComponentBase::ComponentBase;
		explicit PhysicMaterialComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PhysicMaterialComponent()  = default;
		~PhysicMaterialComponent() override = default;

		void Update(float dt) override;
		void RenderImGui() override;
		void Load(ecs::Registry* registry) override;

		[[nodiscard]] float GetDynamicFriction()			const { return dynamic_friction; }
		[[nodiscard]] float GetStaticFriction()				const { return static_friction; }
		[[nodiscard]] float GetBounciness()					const { return bounciness; }
		[[nodiscard]] CombineSetting GetFrictionCombine()	const { return friction_combine; }
		[[nodiscard]] CombineSetting GetBounceCombine()		const { return bounce_combine; }

		void SetDynamicFriction(const float friction)			{ dynamic_friction = friction; }
		void SetStaticFriction(const float friction)			{ static_friction = friction; }
		void SetBounciness(const float bounciness)				{ this->bounciness = bounciness; }
		void SetFrictionCombine(const CombineSetting setting)	{ friction_combine = setting; }
		void SetBounceCombine(const CombineSetting setting)		{ bounce_combine = setting; }

	private:
		float dynamic_friction = 0.5f;	// �ړ����Ă��镨�̂ɑ΂��門�C�B0 ���� 1�̊Ԃ̒l���g�p�B
		float static_friction  = 0.5f;  // �ʏ�ŐÎ~���Ă���I�u�W�F�N�g�Ɏg�p����門�C�B�ʏ�́A0 ���� 1 �̊Ԃ̒l���g�p�B
		float bounciness	   = 0.5f;  // �\�ʂɂǂꂾ���e�������邩�B0 �̏ꍇ�͒e���Ȃ��B1 �̏ꍇ�̓G�l���M�[�����邱�ƂȂ����˂�B
		CombineSetting friction_combine = CombineSetting::Average;	// �Փ˂���I�u�W�F�N�g�Ԃ̖��C���ǂ��������邩
		CombineSetting bounce_combine   = CombineSetting::Average;	// �Փ˂���I�u�W�F�N�g�Ԃ̒��˕Ԃ��x�������ǂ��������邩�B
	};
} // cumulonimbus::component
