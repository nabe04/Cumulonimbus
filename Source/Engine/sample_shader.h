#pragma once

#include "shader.h"
#include "shader_asset.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		class SampleShader final : public Shader
		{
		public:
			explicit SampleShader();
			~SampleShader() override = default;
		};
	} // shader_system

	namespace shader_asset
	{
		class SampleShaderAsset final : public ShaderAsset
		{
		public:
			explicit SampleShaderAsset();
			~SampleShaderAsset() override = default;

			void BindCBuffer()	 override {}
			void UnbindCBuffer() override {}
			void BindTexture()	 override;
			void UnbindTexture() override;
			void RenderImGui()	 override;
		};
	} // shader_asset
	
} // cumulonimbus
