#include "diffuse.h"

#include <memory>

#include "framework.h"
#include "shader_filename_mapping.h"

namespace shader
{
	Diffuse::Diffuse(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);
		pixel_shader  = std::make_unique<PixelShader>(device, ps_name);
	}

	void Diffuse::Activate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void Diffuse::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);
	}
}

namespace cumulonimbus
{
	namespace shader_system
	{
		DiffuseShader::DiffuseShader()
			:Shader{}
		{
			vertex_shader = std::make_unique<shader_system::VertexShader>(mapping::shader_filename::vs::Diffuse_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(mapping::shader_filename::ps::Diffuse_PS().c_str());
		}

	}
}
