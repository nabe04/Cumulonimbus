#pragma once
#include "shader.h"
#include "shader_asset.h"

namespace shader
{
	class Standard3D final : public Shader<Empty>
	{
	private:
		const char* vs_name = "./Shader/cso/3d_standard_vs.cso";
		const char* ps_name = "./Shader/cso/3d_standard_ps.cso";

	public:
		Standard3D(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};

}

namespace cumulonimbus
{
	namespace shader_system
	{
		class Standard3DShader final : public Shader
		{
		public:
			explicit Standard3DShader();
			~Standard3DShader() override = default;
		};
	}

	namespace shader_asset
	{
		class Standard3DAsset final : public ShaderAsset
		{
		public:
			explicit Standard3DAsset(u_int gbuff_shader_slot);
			explicit Standard3DAsset()  = default; // for cereal
			~Standard3DAsset() override = default;

			void BindCBuffer()	 override {}
			void UnbindCBuffer() override {}
			void RenderImGui()	 override {}
		};
	}

}
