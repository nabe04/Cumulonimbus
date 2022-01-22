#include "distort.h"

#include "locator.h"
#include "shader_filename_mapping.h"
#include "cum_imgui_helper.h"
#include "time_scale.h"

namespace cumulonimbus::post_effect
{
	Distort::Distort(ID3D11Device* device)
		:PostEffect(device)
	{
		pixel_shader = std::make_unique<shader_system::PixelShader>(mapping::shader_filename::ps::Distort_PS().c_str());
		cb_distort	 = std::make_unique<buffer::ConstantBuffer<DistortCB>>(device);

		cb_distort->GetData().distort_scroll_direction = float2{ 1.f,.0f };
		cb_distort->GetData().distort_noise_attenuation = 0.1f;
		cb_distort->GetData().distort_noise_scale = 1;
		cb_distort->GetData().distort_time_scale = 1;
	}

	void Distort::RenderImGui(ecs::Registry* registry)
	{
		if (!ImGui::CollapsingHeader(ICON_FA_FILM"Distort"))
			return;

		ImGui::PushID("Distort");

		auto& cbuff_data = cb_distort->GetData();

		helper::imgui::Image(frame_buffer->render_target_shader_resource_view.Get(), DirectX::SimpleMath::Vector2{ 480,270.f });
		ImGui::Checkbox("Is Active", &is_active);
		ImGui::SliderFloat("Radius"				, &cbuff_data.distort_radius			, 0.0f, 1.0f);
		ImGui::SliderFloat("Time Scale"			, &cbuff_data.distort_time_scale		, 0.0f, 100.0f);
		ImGui::SliderFloat("Noise Scale"		, &cbuff_data.distort_noise_scale		, 0.0f, 100.0f);
		ImGui::SliderFloat("Noise Attenuation"	, &cbuff_data.distort_noise_attenuation	, 0.0f, 10.0f);
		ImGui::SliderFloat2("Scroll Direction"	, &cbuff_data.distort_scroll_direction.x, -1.f, 1.f);

		ImGui::PopID();
	}

	void Distort::Begin(ID3D11DeviceContext* immediate_context) const
	{
		sampler_state->Activate(immediate_context, SamplerState::Mirror, 0);
		pixel_shader->BindPS();
		cb_distort->Activate(immediate_context, CBSlot_Distort);

		auto& cbuff_data = cb_distort->GetData();
		cbuff_data.distort_elapsed_time += locator::Locator::GetSystem()->GetTime().GetUnscaledDeltaTime();
	}

	void Distort::End(ID3D11DeviceContext* immediate_context) const
	{
		cb_distort->Deactivate(immediate_context);
		pixel_shader->UnbindPS();
		sampler_state->Deactivate(immediate_context);
	}
} // cumulonimbus::post_effect
