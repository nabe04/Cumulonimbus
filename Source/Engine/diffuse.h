#ifndef DIFFUSE_H
#define DIFFUSE_H

#include "shader_interop_renderer.h"

#ifdef __cplusplus
#include "shader.h"
#include "shader_asset.h"
#include "texture.h"
#endif // __cplusplus

#ifdef __cplusplus

namespace shader
{
	class Diffuse final : public Shader<Empty>
	{
	private:
		const char* vs_name = "./Shader/cso/diffuse_vs.cso";
		const char* ps_name = "./Shader/cso/diffuse_ps.cso";

	public:
		Diffuse(ID3D11Device* device);

		void Activate(ID3D11DeviceContext* immediate_context) override;
		void Deactivate(ID3D11DeviceContext* immediate_context) override;
	};
} // shader

namespace cumulonimbus
{
	namespace shader_system
	{
		class DiffuseShader final : public Shader
		{
		public:
			explicit DiffuseShader();
		};
	}  // shader_system

	namespace shader_asset
	{
		class DiffuseAsset final : public ShaderAsset
		{
		public:
			explicit DiffuseAsset(u_int gbuff_shader_slot);
			explicit DiffuseAsset()  = default;
			~DiffuseAsset() override = default;

			void BindCBuffer()   override {}
			void UnbindCBuffer() override {}
			void BindTexture()	 override;
			void UnbindTexture() override;
			void RenderImGui()   override;
		};
	}  // shader_asset

} // cumulonimbus
#endif // __cplusplus

#endif // DIFFUSE_H

