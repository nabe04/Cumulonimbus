#include "player_avoid_effect_component.h"

#include "easing.h"
#include "locator.h"
#include "post_effect_manager.h"
#include "time_scale.h"

CEREAL_REGISTER_TYPE(cumulonimbus::component::PlayerAvoidEffectComponent)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cumulonimbus::component::PlayerAvoidEffectComponent, cumulonimbus::component::ComponentBase)
CEREAL_CLASS_VERSION(cumulonimbus::component::PlayerAvoidEffectComponent, 0)


namespace cumulonimbus::component
{
	PlayerAvoidEffectComponent::PlayerAvoidEffectComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry,ent }
	{

	}

	PlayerAvoidEffectComponent::PlayerAvoidEffectComponent(ecs::Registry* registry, const mapping::rename_type::Entity ent, const PlayerAvoidEffectComponent& copy_comp)
		: ComponentBase{ registry,ent }
	{
		*this = copy_comp;
		SetRegistry(registry);
		SetEntity(ent);
	}

	PlayerAvoidEffectComponent::PlayerAvoidEffectComponent(mapping::component_tag::ComponentTag tag)
		:ComponentBase{ tag }
	{
		//PlayerAvoidEffectComponent& avoid_comp = GetRegistry()->GetComponent<PlayerAvoidEffectComponent>(GetEntity());
		//// Stateの追加
		//effect_state.AddState(PlaybackMethod::Expansion, [&avoid_comp](const float dt) { avoid_comp.GetPlaybackFunc<&PlayerAvoidEffectComponent::Expansion>(); });
		//effect_state.AddState(PlaybackMethod::Contraction, [&avoid_comp](const float dt) { avoid_comp.GetPlaybackFunc<&PlayerAvoidEffectComponent::Contraction>(); });
		//effect_state.AddState(PlaybackMethod::Stop, [&avoid_comp](const float dt) { avoid_comp.GetPlaybackFunc<&PlayerAvoidEffectComponent::Stop>(); });

		//effect_state.SetState(PlaybackMethod::Stop);
	}

	void PlayerAvoidEffectComponent::Start()
	{
		PlayerAvoidEffectComponent& avoid_comp = GetRegistry()->GetComponent<PlayerAvoidEffectComponent>(GetEntity());
		// Stateの追加
		effect_state.AddState(EffectState::Begin_Avoid, avoid_comp.GetPlaybackFunc<&PlayerAvoidEffectComponent::BeginAvoid>());
		effect_state.AddState(EffectState::Avoiding   , avoid_comp.GetPlaybackFunc<&PlayerAvoidEffectComponent::Avoiding>());
		effect_state.AddState(EffectState::End_Avoid  , avoid_comp.GetPlaybackFunc<&PlayerAvoidEffectComponent::EndAvoid>());
		effect_state.AddState(EffectState::Stop		  , avoid_comp.GetPlaybackFunc<&PlayerAvoidEffectComponent::Stop>());

		effect_state.SetState(EffectState::Stop);
	}

	void PlayerAvoidEffectComponent::CommonUpdate(float dt)
	{

	}

	void PlayerAvoidEffectComponent::GameUpdate(float dt)
	{
		effect_state.Update(dt);
	}

	void PlayerAvoidEffectComponent::RenderImGui()
	{
		if(GetRegistry()->CollapsingHeader<PlayerAvoidEffectComponent>(GetEntity(),"Player Avoid"))
		{
			if(ImGui::Button("Expansion"))
			{
				effect_state.SetState(EffectState::Begin_Avoid);
			}
			if(ImGui::Button("Contraction"))
			{
				effect_state.SetState(EffectState::End_Avoid);
			}

			ImGui::DragFloat("Distort Start Radius", &distort_avoid_end_radius, 0.1f, 0.f, 5.f);
			//ImGui::DragFloat("Distort Max Radius"  , &distort_max_radius  , 0.1f, 0.f, 5.f);
		}
	}

	void PlayerAvoidEffectComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}

	template <class Archive>
	void PlayerAvoidEffectComponent::load(Archive&& archive, uint32_t version)
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	template <class Archive>
	void PlayerAvoidEffectComponent::save(Archive&& archive, uint32_t version) const
	{
		archive(
			cereal::base_class<ComponentBase>(this)
		);
	}

	template <auto F>
	std::function<void(float)> PlayerAvoidEffectComponent::GetPlaybackFunc()
	{
		return std::bind(F, this, std::placeholders::_1);
	}

	void PlayerAvoidEffectComponent::BeginAvoid(float dt)
	{
		if(effect_state.GetInitialize())
		{
			//-- パラメータの初期化 --//
			// 経過時間
			elapsed_time = 0;
			// 半径
			distort_start_radius   = 0;
			distort_current_radius = 0;
		}

		const auto& time = locator::Locator::GetSystem()->GetTime();
		elapsed_time += time.GetUnscaledDeltaTime();

		//-- イージング処理 --//
		// 半径
		distort_current_radius = Easing::GetEasingVal(elapsed_time, distort_start_radius,
													  distort_start_max_radius - distort_start_radius,
													  easing_end_time, CIRC, ESOUT);

		// タイムスケール
		const float time_scale = Easing::GetEasingVal(elapsed_time, 0.0f,
													  distort_max_time_scale,
													  easing_end_time,
													  CIRC, ESOUT);

		// ノイズスケール
		const float noise_scale = Easing::GetEasingVal(elapsed_time, 0.0f,
													   distort_max_noise_scale,
													   easing_end_time,
													   CIRC, ESOUT);

		// ノイズ減衰率
		const float noise_attenuation = Easing::GetEasingVal(elapsed_time, 0.0f,
															 distort_max_noise_attenuation,
															 easing_end_time,
															 CIRC, ESOUT);


		auto& post_effect_manager = locator::Locator::GetSystem()->GetPostEffectManager();
		auto& distort_cbuff_data  = post_effect_manager.GetDistort().GetCBuffer().GetData();
		// パラメータの代入
		post_effect_manager.GetDistort().SetIsActive(true);
		distort_cbuff_data.distort_radius			 = distort_current_radius;
		distort_cbuff_data.distort_time_scale		 = time_scale;
		distort_cbuff_data.distort_noise_scale		 = distort_max_noise_scale;
		distort_cbuff_data.distort_noise_attenuation = distort_max_noise_attenuation;

		if(elapsed_time > easing_end_time)
		{
			effect_state.SetState(EffectState::Avoiding);
		}
	}

	void PlayerAvoidEffectComponent::Avoiding(float dt)
	{
		if(effect_state.GetInitialize())
		{
			current_avoiding_time = 0;
		}

		const auto& time = locator::Locator::GetSystem()->GetTime();
		current_avoiding_time += time.GetUnscaledDeltaTime();

		if(current_avoiding_time > avoiding_time)
		{
			effect_state.SetState(EffectState::End_Avoid);
		}
	}

	void PlayerAvoidEffectComponent::EndAvoid(float dt)
	{
		if(effect_state.GetInitialize())
		{
			//-- パラメータの初期化 --//
			// 経過時間
			elapsed_time = 0;
			// 半径
			distort_start_radius = distort_current_radius;
		}

		const auto& time = locator::Locator::GetSystem()->GetTime();
		elapsed_time += time.GetUnscaledDeltaTime();

		//-- イージング処理 --//
		// 半径
		distort_current_radius = Easing::GetEasingVal(elapsed_time, distort_start_radius,
													  distort_avoid_end_radius - distort_start_radius,
													  easing_end_time, CIRC, ESOUT);

		// タイムスケール
		const float time_scale = Easing::GetEasingVal(elapsed_time, distort_max_time_scale,
													  -distort_max_time_scale,
													  easing_end_time,
													  CIRC, ESOUT);

		// ノイズスケール
		const float noise_scale = Easing::GetEasingVal(elapsed_time, distort_max_noise_scale,
													   -distort_max_noise_scale,
													   easing_end_time,
													   CIRC, ESOUT);

		// ノイズ減衰率
		const float noise_attenuation = Easing::GetEasingVal(elapsed_time, distort_max_noise_attenuation,
															 -distort_max_noise_attenuation,
															 easing_end_time,
															 CIRC, ESOUT);

		auto& post_effect_manager = locator::Locator::GetSystem()->GetPostEffectManager();
		auto& distort_cbuff_data = post_effect_manager.GetDistort().GetCBuffer().GetData();
		// パラメータの代入
		post_effect_manager.GetDistort().SetIsActive(true);
		distort_cbuff_data.distort_radius			 = distort_current_radius;
		distort_cbuff_data.distort_time_scale		 = time_scale;
		distort_cbuff_data.distort_noise_scale		 = noise_scale;
		distort_cbuff_data.distort_noise_attenuation = noise_attenuation;

		if (elapsed_time > easing_end_time)
		{
			effect_state.SetState(EffectState::Stop);
		}
	}

	void PlayerAvoidEffectComponent::Stop(float dt)
	{
		if (effect_state.GetInitialize())
		{
			auto& post_effect_manager = locator::Locator::GetSystem()->GetPostEffectManager();
			//post_effect_manager.GetDistort().SetIsActive(false);
		}
	}
} // cumulonimbus::component