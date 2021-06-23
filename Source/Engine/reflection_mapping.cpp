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
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::ReflectionMapping_PS().c_str());
		}

	}
}

namespace shader
{
	ReflectionMapping::ReflectionMapping(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
		pixel_shader  = std::make_unique<PixelShader>(device, ps_name);
	}

	void ReflectionMapping::Activate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void ReflectionMapping::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);
	}
}