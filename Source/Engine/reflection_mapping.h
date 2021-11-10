#ifndef REFLECTION_MAPPING_H
#define REFLECTION_MAPPING_H

#ifdef __cplusplus
#include <cereal/cereal.hpp>
#include <cereal/types/polymorphic.hpp>

#include "shader.h"
#include "shader_asset.h"
#endif // __cplusplus

#include "shader_interop_renderer.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		class ReflectionMappingShader final : public Shader
		{
		public:
			explicit  ReflectionMappingShader();
			~ReflectionMappingShader() override = default;
		};
	}

	namespace shader_asset
	{
		class ReflectionMappingAsset final : public ShaderAsset
		{
		public:
			explicit ReflectionMappingAsset();
			~ReflectionMappingAsset() override = default;

			void BindCBuffer()	 override {}
			void UnbindCBuffer() override {}
			void RenderImGui()   override {}
		};
	}
}

//namespace shader
//{
//	class ReflectionMapping : public Shader<Empty>
//	{
//	private:
//		const char* vs_name = "./Shader/cso/reflection_mapping_vs.cso";
//		const char* ps_name = "./Shader/cso/reflection_mapping_ps.cso";
//
//	public:
//		explicit ReflectionMapping(ID3D11Device* device);
//
//		void Activate(ID3D11DeviceContext* immediate_context) override;
//		void Deactivate(ID3D11DeviceContext* immediate_context) override;
//	};
//}

#endif // REFLECTION_MAPPING_H