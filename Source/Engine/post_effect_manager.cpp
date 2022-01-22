#include "post_effect_manager.h"
#include "cum_imgui_helper.h"
#include "locator.h"
#include "system.h"

namespace cumulonimbus::system
{
	PostEffectManager::PostEffectManager(system::System& system, ID3D11Device* device)
	{
		const auto& window = locator::Locator::GetWindow();
		frame_buffer = std::make_unique<FrameBuffer>(device, window->Width(), window->Height());
		fullscreen_quad = std::make_unique<FullscreenQuad>(device);

		distort			= std::make_unique<post_effect::Distort>(device);
		//lens_distortion = std::make_unique<post_effect::LensDistortion>(device);
		screen_filter	= std::make_unique<post_effect::ScreenFilter>(device);

		system.RegisterRenderFunction(utility::GetHash<PostEffectManager>(), [&](ecs::Registry* registry) {RenderImGui(registry); });
	}

	void PostEffectManager::RenderImGui(ecs::Registry* registry) const
	{
		if(ImGui::CollapsingHeader(ICON_FA_FILM"Post Effects"))
		{
			distort->RenderImGui(registry);
			//lens_distortion->RenderImGui(registry);
			screen_filter->RenderImGui(registry);
		}
	}

	ID3D11ShaderResourceView** PostEffectManager::Generate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** scene_srv_address) const
	{
		//lens_distortion->Generate(immediate_context, scene_srv_address);
		ID3D11ShaderResourceView** srv = nullptr;
		srv = distort->Generate(immediate_context, scene_srv_address);
		srv = screen_filter->Generate(immediate_context, srv);


		frame_buffer->Clear(immediate_context);
		frame_buffer->Activate(immediate_context);

		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, srv, TexSlot_ForFullScreenQuad);
		fullscreen_quad->Blit(immediate_context, true, true, true);
		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, TexSlot_ForFullScreenQuad);

		frame_buffer->Deactivate(immediate_context);

		return frame_buffer->GetRenderTargetSRV();
	}

} // cumulonimbus::system
