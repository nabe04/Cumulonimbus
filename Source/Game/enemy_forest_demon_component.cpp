#include "enemy_forest_demon_component.h"

#include "ecs.h"
#include "fbx_model_resource.h"
// components
#include "fbx_model_component.h"
#include "player_component.h"
#include "rigid_body_component.h"
#include "transform_component.h"

// std::unordered_map<std::string, RandomFloat> transition_timer�̃L�[�l�p�ϐ�
namespace
{
	const std::string transition_idle_to_walk{ "idle_to_walk" };
	const std::string transition_walk_to_idle{ "walk_to_idle" };
}

namespace cumulonimbus::component
{
	EnemyForestDemonComponent::EnemyForestDemonComponent(ecs::Registry* registry, mapping::rename_type::Entity ent)
		:EnemyBaseComponent{registry,ent}
	{
		// state�ɉ����Ă̏����̓o�^
		forest_demon_state.AddState(ForestDemonState::T_Pose			, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).TPose(dt); });
		forest_demon_state.AddState(ForestDemonState::Idle				, [ent, registry](const float dt) { registry->GetComponent<EnemyForestDemonComponent>(ent).Idle(dt); });
		forest_demon_state.AddState(ForestDemonState::Ready				, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Ready(dt); });
		forest_demon_state.AddState(ForestDemonState::Walk				, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Walk(dt); });
		forest_demon_state.AddState(ForestDemonState::Tracking			, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Tracking(dt); });
		forest_demon_state.AddState(ForestDemonState::Death				, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Death(dt); });
		forest_demon_state.AddState(ForestDemonState::Damage			, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).Damage(dt); });
		forest_demon_state.AddState(ForestDemonState::Attack_Normal		, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).AttackNormal(dt); });
		forest_demon_state.AddState(ForestDemonState::Attack_Mow_Down	, [ent, registry](const float dt) {registry->GetComponent<EnemyForestDemonComponent>(ent).AttackMowDown(dt); });

		// ����state�̐ݒ�(ForestDemonState::Idle)
		forest_demon_state.SetState(ForestDemonState::Idle);

		// transition_timer�̐ݒ�
		RegisterTransitionTimer(transition_idle_to_walk, 3.f, 7.f);
		RegisterTransitionTimer(transition_walk_to_idle, 3.f, 5.f);

		// yaw��]�ł̂������_���l�ݒ�
		SetRandomRotationAngle(-180.f, 180.f);
	}

	void EnemyForestDemonComponent::Update(const float dt)
	{
		forest_demon_state.Update(dt);
	}

	void EnemyForestDemonComponent::RenderImGui()
	{
	}

	void EnemyForestDemonComponent::Save(const std::string& file_path)
	{
	}

	void EnemyForestDemonComponent::Load(const std::string& file_path_and_name)
	{
	}

	void EnemyForestDemonComponent::SetRandomRotationAngle(float min, float max)
	{
		random_rotation_angle.min = min;
		random_rotation_angle.max = max;
		random_rotation_angle.SetRandomVal();
	}

	int EnemyForestDemonComponent::GetAnimDataIndex(AnimationData anim_data) const
	{
		return static_cast<int>(anim_data);
	}

	void EnemyForestDemonComponent::TPose(const float dt)
	{
	}

	void EnemyForestDemonComponent::Idle(const float dt)
	{
		auto& timer = transition_timer.at(transition_idle_to_walk);

		if(forest_demon_state.GetInitialize())
		{
			// �A�j���[�V�����Z�b�g
			GetRegistry()->GetComponent<FbxModelComponent>(GetEntity()).SwitchAnimation(GetAnimDataIndex(AnimationData::Idle_00));
			// �J�ڎ��Ԃ̃����_���l�̍Đݒ�
			timer.SetRandomVal();
		}

		timer.current_time += dt;

		const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
		if(Search(GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(),tracking_transition_angle,tracking_transition_distance))
		{
			// ���G�͈͓��Ƀv���C���[������Ώ�ԑJ��(ForestDemonState::Tracking)
			forest_demon_state.SetState(ForestDemonState::Tracking);
			timer.current_time = 0.0f;
			return;
		}

		if (timer.current_time < timer.random_val)
			return;

		// ��ԑJ��(ForestDemonState::Walk)
		forest_demon_state.SetState(ForestDemonState::Walk);
		timer.current_time = 0.0f;
	}

	void EnemyForestDemonComponent::Ready(const float dt)
	{
	}

	void EnemyForestDemonComponent::Walk(const float dt)
	{
		auto& fbx_model_comp	= GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		auto& rigid_body_comp	= GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
		auto& transform_comp	= GetRegistry()->GetComponent<TransformComponent>(GetEntity());
		auto& timer				= transition_timer.at(transition_walk_to_idle);
		if(forest_demon_state.GetInitialize())
		{
			// �A�j���[�V�����Z�b�g(AnimationData::Walk)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Walk));
			transform_comp.ActiveQuaternion();
			{// ���f����]�̏����ݒ�
				random_rotation_angle.SetRandomVal();
				random_rotation_angle.current_time = 0;
				const DirectX::SimpleMath::Quaternion rotation_result_q = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0,1,0 }, DirectX::XMConvertToRadians(random_rotation_angle.random_val));
				transform_comp.SetQuaternionSlerp(DirectX::SimpleMath::Quaternion::Identity, rotation_result_q);
			}
			{// ��ԑJ�ڃ^�C�}�[�̏����ݒ�
				timer.current_time = 0;
				timer.SetRandomVal();
			}
		}

		random_rotation_angle.current_time += dt * rotation_time_rate;
		transform_comp.QuaternionSlerp(random_rotation_angle.current_time);

		if (random_rotation_angle.current_time > 1)
		{
			random_rotation_angle.current_time = 1;
		}
		rigid_body_comp.AddForce({ walk_speed,0.0f,walk_speed });

		const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
		if (Search(GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(), tracking_transition_angle, tracking_transition_distance))
		{// ���G�͈͓��Ƀv���C���[������Ώ�ԑJ��(ForestDemonState::Tracking)
			forest_demon_state.SetState(ForestDemonState::Tracking);
			timer.current_time = 0;
		}

		timer.current_time += dt;
		if (timer.current_time < timer.random_val)
			return;

		forest_demon_state.SetState(ForestDemonState::Idle);
		timer.current_time = 0;
	}

	void EnemyForestDemonComponent::Tracking(const float dt)
	{
		auto& fbx_model_comp	= GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		auto& rigid_body_comp	= GetRegistry()->GetComponent<RigidBodyComponent>(GetEntity());
		auto& transform_comp	= GetRegistry()->GetComponent<TransformComponent>(GetEntity());

		if(forest_demon_state.GetInitialize())
		{
			// �A�j���[�V�����Z�b�g(AnimationData::Run)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Run), true);
		}

		float distance = 0; // ���g�ƃv���C���[�Ƃ̋���
		{// �ǐՏ���
			const mapping::rename_type::Entity ent_player = GetRegistry()->GetArray<PlayerComponent>().GetComponents().at(0).GetEntity();
			EnemyBaseComponent::Tracking(
				GetRegistry()->GetComponent<TransformComponent>(ent_player).GetPosition(),
				{ tracking_speed,.0f,tracking_speed },
				distance);
		}

		{// ��Ԃ̐؂�ւ�
			if(distance > tracking_interruption_distance)
			{// ���G�͈͊O�Ȃ�ҋ@��ԂɑJ��
				forest_demon_state.SetState(ForestDemonState::Idle);
			}

			if(distance < attack_transition_distance)
			{// �v���C���[�Ǝ��g�̋����͈͓��Ȃ�U����ԂɑJ��
				forest_demon_state.SetState(ForestDemonState::Attack_Normal);
			}
		}
	}

	void EnemyForestDemonComponent::Death(float dt)
	{
	}

	void EnemyForestDemonComponent::Damage(float dt)
	{
	}

	void EnemyForestDemonComponent::AttackNormal(float dt)
	{
		auto& fbx_model_comp = GetRegistry()->GetComponent<FbxModelComponent>(GetEntity());
		if (forest_demon_state.GetInitialize())
		{
			// �A�j���[�V�����Z�b�g(AnimationData::Attack_Normal)
			fbx_model_comp.SwitchAnimation(GetAnimDataIndex(AnimationData::Attack_Normal), false);
			//�v���C���[�̕����Ɍ�����ς���
			RotateToPlayerDirection();
		}

		if (fbx_model_comp.IsPlayAnimation())
			return;

		forest_demon_state.SetState(ForestDemonState::Idle);
	}

	void EnemyForestDemonComponent::AttackMowDown(float dt)
	{
	}
} // cumulonimbus::component
