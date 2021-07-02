#include "sample_shader.h"

#include "locator.h"
#include "shader_filename_mapping.h"
#include "texture_filename_mapping.h"
#include "texture_resource_mapping.h"

namespace cumulonimbus
{
	namespace shader_system
	{
		SampleShader::SampleShader()
			:Shader{}
		{
			using namespace mapping::shader_filename;
			//vertex_shader = std::make_unique<shader_system::VertexShader>(vs::SampleShader_VS().c_str());
			pixel_shader  = std::make_unique<shader_system::PixelShader>(ps::SampleShader_PS().c_str());
		}
	} // shader_system

	namespace shader_asset
	{
		SampleShaderAsset::SampleShaderAsset()
			:ShaderAsset{}
		{
			material_path.normal_map_name = mapping::texture_filename::DefaultNormalMap();
		}

		void SampleShaderAsset::BindTexture()
		{
			if (locator::Locator::GetTextureResourceManager()->Contents(material_path.normal_map_name))
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

		void SampleShaderAsset::UnbindTexture()
		{
			locator::Locator::GetDx11Device()->UnbindShaderResource(mapping::graphics::ShaderStage::PS, TexSlot_NormalMap);
		}

		void SampleShaderAsset::RenderImGui()
		{
			ModifyMaterialPath(material_path.normal_map_name, "Normal Map");
		}
	} // shader_asset
} // cumulonimbus
