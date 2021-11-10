#include "3d_standard.h"

#include <memory>

#include <d3d11.h>

#include "imgui.h"
#include "shader_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		Standard3DShader::Standard3DShader()
			:Shader{}
		{
			using namespace mapping::shader_filename;
			vertex_shader = std::make_unique<VertexShader>(vs::Standard3D_VS().c_str());
			pixel_shader = std::make_unique<PixelShader>(ps::Standard3D_PS().c_str());
		}
	} // shader_system

	namespace shader_asset
	{
		Standard3DAsset::Standard3DAsset()
			:ShaderAsset{}
		{
			
		}

	}

} // cumulonimbus
