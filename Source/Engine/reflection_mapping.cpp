#include "reflection_mapping.h"

#include "shader_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		ReflectionMappingShader::ReflectionMappingShader()
			:Shader{}
		{
			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<shader_system::VertexShader>(vs::ReflectionMapping_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::ReflectionMappingMTR_PS().c_str());
		}

	} // shader_system

	namespace shader_asset
	{
		ReflectionMappingAsset::ReflectionMappingAsset()
			:ShaderAsset{}
		{

		}
	} // shader_asset

} // cumulonimbus