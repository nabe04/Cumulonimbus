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
			Avoiding,
			End_Avoid,
		};

	public:
		using ComponentBase::ComponentBase;
		explicit PlayerAvoidEffectComponent(ecs::Registry* registry, mapping::rename_type::Entity ent);
		explicit PlayerAvoidEffectComponent(ecs::Registry* registry, mapping::rename_type::Entity ent, const PlayerAvoidEffectComponent& copy_comp);
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

		[[nodiscard]]
		EffectState GetEffectState() const
		{
			return effect_state.GetState();
		}

		void SetEffectState(const EffectState state)
		{
			effect_state.SetState(state);
		}

		void SetAvoidingTime(const float time)
		{
			avoiding_time = time;
		}
	private:
		// �G�t�F�N�g�Đ�State
		StateMachine<EffectState, void, float> effect_state{};

		EffectState playback_method{ EffectState::Stop };
		// �o�ߎ���
		float elapsed_time{};
		// ����p������
		float avoiding_time{ 3.f };
		float current_avoiding_time{};
		// �C�[�W���O�I������
		float easing_end_time{ 0.5f };
		//-- ���a --//
		// Distort�G�t�F�N�g�̌��݂̔��a(����)
		float distort_current_inner_circle{};
		// Distort�G�t�F�N�g�̔��a(����)
		float distort_start_inner_circle{};
		// Distort�G�t�F�N�g�̍ő唼�a(����)
		float distort_max_inner_circle{ 0.05f };
		// Distort�G�t�F�N�g�̌��݂̔��a(�O��)
		float distort_current_outer_circle{};
		// Distort�G�t�F�N�g�̔��a(�O��)
		float distort_start_outer_circle{};
		// Distort�G�t�F�N�g�̍ő唼�a(�O��)
		float distort_max_outer_circle{ 0.65f };
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
		 * @brief : ����J�n
		 */
		void BeginAvoid(float dt);
		/**
		 * @brief : ���
		 */
		void Avoiding(float dt);
		/**
		 * @brief : ����I��
		 */
		void EndAvoid(float dt);
		/**
		 * @brief : �G�t�F�N�g���ʒ�~
		 */
		void Stop(float dt);
	};
} // cumulonimbus::component
