#ifndef SINGLE_COLOR_H
#define SINGLE_COLOR_H

#ifdef __cplusplus
#include <memory>

#include <DirectXMath.h>
#include <imgui.h>
#include <cereal/cereal.hpp>

#include "cereal_helper.h"
#include "constant_buffer.h"
#include "shader.h"
#include "shader_asset.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

CBUFFER(SingleColorCB, CBSlot_SingleColor)
{
	float4 single_color;

#ifdef __cplusplus
	template<typename Archive>
	void serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(single_color)
		);
	}
#endif // __cplusplus
};

#ifdef __cplusplus
namespace cumulonimbus
{
	namespace shader_system
	{
		class SingleColorShader final : public Shader
		{
		public:
			explicit SingleColorShader();
			~SingleColorShader() override = default;
		};
	}

	namespace shader_asset
	{
		class SingleColorAsset final : public ShaderAsset
		{
		public:
			explicit SingleColorAsset();
			~SingleColorAsset() override = default;
			
			void BindCBuffer()   override;
			void UnbindCBuffer() override;
			void RenderImGui()   override;

		private:
			std::unique_ptr<buffer::ConstantBuffer<SingleColorCB>> cb_single_color{ nullptr };
		};
	}
}


#endif // __cplusplus

#endif // SINGLE_COLOR_H