#pragma once
#include "fullscreen_quad.h"
#include "frame_buffer.h"
#include "pixel_shader.h"
#include "sampler.h"

namespace cumulonimbus::ecs
{
	class Registry;
} // cumulonimbus::ecs

namespace cumulonimbus::post_effect
{
	class PostEffect
	{
	public:
		explicit PostEffect() = default;
		explicit PostEffect(ID3D11Device* device);
		virtual ~PostEffect() = default;

		virtual void Initialize(ID3D11Device* device);

		virtual void RenderImGui(ecs::Registry* registry) {}

		virtual ID3D11ShaderResourceView** Generate(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** scene_srv_address);

		template<class Archive>
		void load(Archive&& archive, uint32_t version) {}

		template<class Archive>
		void save(Archive&& archive, uint32_t version) const {}

		void SetIsActive(const bool arg)
		{
			is_active = arg;
		}

		[[nodiscard]]
		FrameBuffer* GetFrameBuffer() const
		{
			return frame_buffer.get();
		}
	protected:
		std::unique_ptr<FullscreenQuad>				fullscreen_quad{};
		std::unique_ptr<FrameBuffer>				frame_buffer{};
		std::unique_ptr<shader_system::PixelShader> pixel_shader{};
		std::unique_ptr<Sampler>					sampler_state{};

		bool is_active{ false }; // ポストエフェクトを実行するか

		virtual void Begin(ID3D11DeviceContext* immediate_context) const;
		virtual void Render(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** scene_srv_address) const;
		virtual void End(ID3D11DeviceContext* immediate_context) const;
	};
} // cumulonimbus::post_effect
