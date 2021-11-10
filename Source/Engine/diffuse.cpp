#include "diffuse.h"

#include <memory>

#include "framework.h"
#include "locator.h"
#include "shader_filename_mapping.h"
#include "texture_filename_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		DiffuseShader::DiffuseShader()
			:Shader{}
		{
			vertex_shader = std::make_unique<shader_system::VertexShader>(mapping::shader_filename::vs::Diffuse_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(mapping::shader_filename::ps::DiffuseMRT_PS().c_str());
		}
	} // shader_system

	namespace shader_asset
	{
		DiffuseAsset::DiffuseAsset()
			:ShaderAsset{}
		{
			material_path.normal_map_name = mapping::texture_filename::DefaultNormalMap();
		}

		void DiffuseAsset::BindTexture()
		{
			if(locator::Locator::GetTextureResourceManager()->Contents(material_path.normal_map_name))
			{
				TextureResource* normal_texture = locator::Locator::GetTextureResourceManager()->GetTextureResources().at(material_path.normal_map_name).get();
				locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, normal_texture, TexSlot_NormalMap);
			}
			else
			{
				TextureResource* default_normal_texture = locator::Locator::GetTextureResourceManager()->GetTextureResources().at(mapping::texture_filename::DefaultNormalMap()).get();
				locator::Locator::GetDx11Device()->BindShaderResource(mapping::graphics::ShaderStage::PS, default_normal_texture, TexSlot_NormalMap);
			}
		}

		void DiffuseAsset::UnbindTexture()
		{
			locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, TexSlot_NormalMap);
		}

		void DiffuseAsset::RenderImGui()
		{
			ModifyMaterialPath(material_path.normal_map_name, "Normal Map");
		}
	}
} // cumulonimbus
