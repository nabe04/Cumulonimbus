#include "screen_filter.h"

#include "locator.h"
#include "shader_filename_mapping.h"
#include "cum_imgui_helper.h"

namespace cumulonimbus::post_effect
{
	ScreenFilter::ScreenFilter(ID3D11Device* device)
		:PostEffect(device)
	{
		pixel_shader = std::make_unique<shader_system::PixelShader>(mapping::shader_filename::ps::ScreenFilter_PS().c_str());
		cb_screen_filter = std::make_unique<buffer::ConstantBuffer<ScreenFilterCB>>(device);

		cb_screen_filter->GetData().sfilter_filter_color  = DirectX::XMFLOAT3{ 0.f,0.f,0.f };
		cb_screen_filter->GetData().sfilter_is_grey_scale = false;
	}

	void ScreenFilter::RenderImGui(ecs::Registry* registry)
	{
		if (!ImGui::CollapsingHeader(ICON_FA_FILM"Screen Filter"))
			return;

		ImGui::PushID("Screen Filter");

		auto& cbuff_data = cb_screen_filter->GetData();

		helper::imgui::Image(frame_buffer->render_target_shader_resource_view.Get(), DirectX::SimpleMath::Vector2{ 480,270.f });
		ImGui::Checkbox("Is Active", &is_active);
		ImGui::ColorEdit3("Color", &cbuff_data.sfilter_filter_color.x);
		ImGui::Checkbox("Is Gray Scale", &is_gray_scale);

		if(is_gray_scale)
		{
			cbuff_data.sfilter_is_grey_scale = 1;
		}
		else
		{
			cbuff_data.sfilter_is_grey_scale = 0;
		}

		ImGui::PopID();
	}

	void ScreenFilter::Begin(ID3D11DeviceContext* immediate_context) const
	{
		pixel_shader->BindPS();
		cb_screen_filter->Activate(immediate_context, CBSlot_ScreenFilter);
	}

	void ScreenFilter::End(ID3D11DeviceContext* immediate_context) const
	{
		cb_screen_filter->Deactivate(immediate_context);
		pixel_shader->UnbindPS();
	}
} // cumulonimbus::post_effect
