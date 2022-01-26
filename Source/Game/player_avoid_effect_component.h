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
		// エフェクト再生State
		StateMachine<EffectState, void, float> effect_state{};

		EffectState playback_method{ EffectState::Stop };
		// 経過時間
		float elapsed_time{};
		// 回避継続時間
		float avoiding_time{ 3.f };
		float current_avoiding_time{};
		// イージング終了時間
		float easing_end_time{ 0.5f };
		//-- 半径 --//
		// Distortエフェクトの現在の半径(内側)
		float distort_current_inner_circle{};
		// Distortエフェクトの半径(内側)
		float distort_start_inner_circle{};
		// Distortエフェクトの最大半径(内側)
		float distort_max_inner_circle{ 0.05f };
		// Distortエフェクトの現在の半径(外側)
		float distort_current_outer_circle{};
		// Distortエフェクトの半径(外側)
		float distort_start_outer_circle{};
		// Distortエフェクトの最大半径(外側)
		float distort_max_outer_circle{ 0.65f };
		// Distortエフェクトの回避時半径(最大)
		float distort_avoid_end_radius{ 1.5f };
		//-- タイムスケール --//
		float distort_max_time_scale{ 0.25f };
		//-- ノイズ --//
		float distort_max_noise_scale{ 8.f };
		float distort_max_noise_attenuation{ 3.f };

		template <auto F>
		std::function<void(float)> GetPlaybackFunc();

		/**
		 * @brief : 回避開始
		 */
		void BeginAvoid(float dt);
		/**
		 * @brief : 回避中
		 */
		void Avoiding(float dt);
		/**
		 * @brief : 回避終了
		 */
		void EndAvoid(float dt);
		/**
		 * @brief : エフェクト効果停止
		 */
		void Stop(float dt);
	};
} // cumulonimbus::component
