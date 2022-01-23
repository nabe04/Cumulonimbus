#pragma once
#include "component_base.h"
#include "state_machine.h"

namespace cumulonimbus::component
{
	class PlayerAvoidEffectComponent final : public ComponentBase
	{
	public:
		enum class EffectState
		{
			Stop,
			Begin_Avoid,
			End_Avoid,
		};

	public:
		using ComponentBase::ComponentBase;
		explicit PlayerAvoidEffectComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PlayerAvoidEffectComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const PlayerAvoidEffectComponent& copy_comp);
		explicit PlayerAvoidEffectComponent(mapping::component_tag::ComponentTag tag);
		explicit PlayerAvoidEffectComponent() = default;
		~PlayerAvoidEffectComponent() override = default;

		void Start() override;

		void CommonUpdate(float dt) override;

		void GameUpdate(float dt) override;

		void RenderImGui() override;

		void Load(ecs::Registry* registry) override;

		template<class Archive>
		void load(Archive&& archive, uint32_t version);

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const;
	private:
		// �G�t�F�N�g�Đ�State
		StateMachine<EffectState, void, float> playback_state{};

		EffectState playback_method{ EffectState::Stop };
		// �o�ߎ���
		float elapsed_time{};
		// �C�[�W���O�I������
		float easing_end_time{ 1.f };
		//-- ���a --//
		// Distort�G�t�F�N�g�̌��݂̔��a
		float distort_current_radius{};
		// Distort�G�t�F�N�g�̉�������a(�J�n��)
		float distort_start_radius{};
		// Distort�G�t�F�N�g�ő唼�a
		float distort_start_max_radius{ 1.f };
		// Distort�G�t�F�N�g�̉�������a(�ő�)
		float distort_avoid_end_radius{ 1.5f };
		//-- �^�C���X�P�[�� --//
		float distort_max_time_scale{ 0.25f };
		//-- �m�C�Y --//
		float distort_max_noise_scale{ 8.f };
		float distort_max_noise_attenuation{ 3.f };

		template <auto F>
		std::function<void(float)> GetPlaybackFunc();

		/**
		 * @brief : �G�t�F�N�g�g��
		 */
		void BeginAvoid(float dt);
		/**
		 * @brief : �G�t�F�N�g�k��
		 */
		void EndAvoid(float dt);
		/**
		 * @brief : �G�t�F�N�g���ʒ�~
		 */
		void Stop(float dt);
	};
} // cumulonimbus::component
