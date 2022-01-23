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
		// エフェクト再生State
		StateMachine<EffectState, void, float> playback_state{};

		EffectState playback_method{ EffectState::Stop };
		// 経過時間
		float elapsed_time{};
		// イージング終了時間
		float easing_end_time{ 1.f };
		//-- 半径 --//
		// Distortエフェクトの現在の半径
		float distort_current_radius{};
		// Distortエフェクトの回避時半径(開始時)
		float distort_start_radius{};
		// Distortエフェクト最大半径
		float distort_start_max_radius{ 1.f };
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
		 * @brief : エフェクト拡大
		 */
		void BeginAvoid(float dt);
		/**
		 * @brief : エフェクト縮小
		 */
		void EndAvoid(float dt);
		/**
		 * @brief : エフェクト効果停止
		 */
		void Stop(float dt);
	};
} // cumulonimbus::component
