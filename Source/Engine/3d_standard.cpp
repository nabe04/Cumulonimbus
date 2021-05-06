#include "3d_standard.h"

#include <memory>

#include <d3d11.h>

#include "imgui.h"

namespace shader
{
	Standard3D::Standard3D(ID3D11Device* device)
		:Shader{ device }
	{
		vertex_shader = std::make_unique<VertexShader>(device, vs_name);

		pixel_shader = std::make_unique<PixelShader>(device, ps_name);
	}

	void Standard3D::Activate(ID3D11DeviceContext* immediate_context)
	{
		material_data->Activate(immediate_context, 6);

		vertex_shader->Activate(immediate_context);
		pixel_shader->Activate(immediate_context);
	}

	void Standard3D::Deactivate(ID3D11DeviceContext* immediate_context)
	{
		vertex_shader->Deactivate(immediate_context);
		pixel_shader->Deactivate(immediate_context);

		material_data->Deactivate(immediate_context);
	}


}