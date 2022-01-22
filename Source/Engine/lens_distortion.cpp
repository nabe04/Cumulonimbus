#include "lens_distortion.h"

#include "cum_imgui_helper.h"
#include "locator.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus::post_effect
{
	LensDistortion::LensDistortion(ID3D11Device* device)
		:PostEffect(device)
	{
		pixel_shader = std::make_unique<shader_system::PixelShader>(mapping::shader_filename::ps::LensDistortion_PS().c_str());
	}

	void LensDistortion::RenderImGui(ecs::Registry* registry)
	{
		if (!ImGui::CollapsingHeader(ICON_FA_FILM"Lens Distortion"))
			return;

		helper::imgui::Image(frame_buffer->render_target_shader_resource_view.Get(), DirectX::SimpleMath::Vector2{ 480,270.f });
	}
} // cumulonimbus::post_effect