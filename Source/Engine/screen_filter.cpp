#include "screen_filter.h"

#include "locator.h"
#include "shader_filename_mapping.h"
#include "cum_imgui_helper.h"
#include "texture_loader.h"



namespace cumulonimbus::post_effect
{
	//template <class Archive>
	//void ScreenFilter::load(Archive&& archive, uint32_t version)


	//template <class Archive>
	//void ScreenFilter::save(Archive&& archive, uint32_t version) const
	//


	ScreenFilter::ScreenFilter(ID3D11Device* device)
		:PostEffect(device)
	{
		Initialize(device);
	}

	void ScreenFilter::Initialize(ID3D11Device* device)
	{
		PostEffect::Initialize(device);

		pixel_shader = std::make_unique<shader_system::PixelShader>(mapping::shader_filename::ps::ScreenFilter_PS().c_str());
		cb_screen_filter = std::make_unique<buffer::ConstantBuffer<ScreenFilterCB>>(device);

		cb_screen_filter->GetData().sfilter_filter_color = DirectX::XMFLOAT3{ 0.f,0.f,0.f };
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
		ImGui::Checkbox("Is Gray Scale", &is_gray_scale);
		ImGui::ColorEdit3("Color", &cbuff_data.sfilter_filter_color.x);
		ImGui::DragFloat("Dissolve Threshold", &cbuff_data.sfilter_dissolve_threshold, 0.01f, 0.0f, 1.0f);

		auto& asset_manager = *locator::Locator::GetAssetManager();
		auto& texture_loader = *asset_manager.GetLoader<asset::TextureLoader>();
		texture_loader.SelectableTexture(asset_manager, dissolve_tex_id);

		ImGui::PopID();
	}

	void ScreenFilter::AddDissolveThreshold(const float arg) const
	{
		cb_screen_filter->GetData().sfilter_dissolve_threshold += arg;
	}

	void ScreenFilter::Begin(ID3D11DeviceContext* immediate_context) const
	{
		pixel_shader->BindPS();
		cb_screen_filter->Activate(immediate_context, CBSlot_ScreenFilter);
		auto* dissolve_tex = &locator::Locator::GetAssetManager()->GetLoader<asset::TextureLoader>()->GetTexture(dissolve_tex_id);
		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, dissolve_tex, TexSlot_Dissolve);

		auto& cbuff_data = cb_screen_filter->GetData();
		if (is_gray_scale)
		{
			cbuff_data.sfilter_is_grey_scale = 1;
		}
		else
		{
			cbuff_data.sfilter_is_grey_scale = 0;
		}
	}

	void ScreenFilter::End(ID3D11DeviceContext* immediate_context) const
	{
		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, TexSlot_Dissolve);
		cb_screen_filter->Deactivate(immediate_context);
		pixel_shader->UnbindPS();
	}
} // cumulonimbus::post_effect
