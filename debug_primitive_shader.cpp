#include "debug_primitive_shader.h"

#include <array>

#include "shader_filename_mapping.h"

namespace cumulonimbus::shader_system
{
	DebugPrimitiveShader::DebugPrimitiveShader()
		:Shader{}
	{
		// InputLayoutÇÃê›íË
		std::array<D3D11_INPUT_ELEMENT_DESC, 1> layout =
		{
			{
				{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		};

		using namespace mapping::shader_filename;
		vertex_shader = std::make_unique<shader_system::VertexShader>(vs::DebugPrimitive_VS().c_str(), layout.data(), layout.size());
		pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::DebugPrimitive_PS().c_str());
	}

} // cumulonimbus::shader_system
