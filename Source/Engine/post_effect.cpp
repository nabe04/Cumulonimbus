#include "post_effect.h"

#include "locator.h"
#include "shader_filename_mapping.h"
#include "texture_resource_mapping.h"

namespace cumulonimbus::post_effect
{
	PostEffect::PostEffect(ID3D11Device* device)
	{
		const auto& window = locator::Locator::GetWindow();

		fullscreen_quad = std::make_unique<FullscreenQuad>(device);
		frame_buffer	= std::make_unique<FrameBuffer>(device, window->Width(), window->Height());
		sampler_state   = std::make_unique<Sampler>(device);

		// Pixel Shader‚Í”h¶ƒNƒ‰ƒX‚Å¶¬‚·‚é‚±‚Æ
	}

	ID3D11ShaderResourceView** PostEffect::Generate(ID3D11DeviceContext* immediate_context,
		ID3D11ShaderResourceView** scene_srv_address)
	{
		if (!is_active)
		{
			//frame_buffer->Clear(immediate_context);
			//frame_buffer->render_target_shader_resource_view = &(*scene_srv_address);
			//frame_buffer->render_target_shader_resource_view.CopyTo(scene_srv_address);
			//locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, scene_srv_address, TexSlot_ForFullScreenQuad);
			//fullscreen_quad->Blit(immediate_context);
			//locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, TexSlot_PostProcess);
			return scene_srv_address;
		}


		Begin(immediate_context);
		Render(immediate_context, scene_srv_address);
		End(immediate_context);

		return frame_buffer->GetRenderTargetSRV();
	}

	void PostEffect::Begin(ID3D11DeviceContext* immediate_context) const
	{
		if (!pixel_shader.get())
			return;

		pixel_shader->BindPS();
	}

	void PostEffect::Render(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** scene_srv_address) const
	{
		frame_buffer->Clear(immediate_context);

		frame_buffer->Activate(immediate_context);
		locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, scene_srv_address, TexSlot_PostProcess);

		fullscreen_quad->Blit(immediate_context);

		locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, TexSlot_PostProcess);
		frame_buffer->Deactivate(immediate_context);
	}

	void PostEffect::End(ID3D11DeviceContext* immediate_context) const
	{
		if (!pixel_shader.get())
			return;

		pixel_shader->UnbindPS();
	}
} // cumulonimbus::post_effect
