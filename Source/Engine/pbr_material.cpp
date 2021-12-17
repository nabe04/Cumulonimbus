//#include "pbr_material.h"
//
//#include "texture_filename_mapping.h"
//#include "locator.h"
//
//namespace cumulonimbus::shader_asset
//{
//	PBRMaterial::PBRMaterial()
//	{
//		cb_pbr_material = std::make_unique<buffer::ConstantBuffer<MaterialCB>>(locator::Locator::GetDx11Device()->device.Get());
//		cb_pbr_material->GetData().material.material_use_custom = true;
//
//		SetDefaultMaterialFilename();
//		SetCustomMaterial(true);
//	}
//
//	void PBRMaterial::BindCBuffer()
//	{
//		cb_pbr_material->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Material);
//	}
//
//	void PBRMaterial::UnbindCBuffer()
//	{
//		cb_pbr_material->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
//	}
//
//	void PBRMaterial::BindTexture()
//	{
//		// TextureResourceManagerからmaterial_pathの持つパスのテクスチャを取得
//		TextureResource* albedo_texture = locator::Locator::GetTextureResourceManager()->GetTextureResources().at(material_path.albedo_map_name).get();
//		TextureResource* normal_map		= locator::Locator::GetTextureResourceManager()->GetTextureResources().at(material_path.normal_map_name).get();
//		TextureResource* pbr_map		= locator::Locator::GetTextureResourceManager()->GetTextureResources().at(material_path.pbr_map_name).get();
//		TextureResource* roughness_map	= locator::Locator::GetTextureResourceManager()->GetTextureResources().at(material_path.roughness_map_name).get();
//		TextureResource* specular_map	= locator::Locator::GetTextureResourceManager()->GetTextureResources().at(material_path.specular_map_name).get();
//		TextureResource* ao_map			= locator::Locator::GetTextureResourceManager()->GetTextureResources().at(material_path.ao_map_name).get();
//
//		// shader_resource_viewへのバインド
//		using namespace mapping::graphics;
//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, albedo_texture, TexSlot_BaseColorMap);
//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, normal_map	 , TexSlot_NormalMap);
//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, pbr_map		 , TexSlot_PBRMap);
//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, roughness_map , TexSlot_RoughnessMap);
//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, specular_map  , TexSlot_SpecularMap);
//		locator::Locator::GetDx11Device()->BindShaderResource(ShaderStage::PS, ao_map		 , TexSlot_AOMap);
//	}
//
//	void PBRMaterial::UnbindTexture()
//	{
//		using namespace mapping::graphics;
//		locator::Locator::GetDx11Device()->UnbindShaderResource(ShaderStage::PS, TexSlot_BaseColorMap);
//		locator::Locator::GetDx11Device()->UnbindShaderResource(ShaderStage::PS, TexSlot_NormalMap);
//		locator::Locator::GetDx11Device()->UnbindShaderResource(ShaderStage::PS, TexSlot_PBRMap);
//		locator::Locator::GetDx11Device()->UnbindShaderResource(ShaderStage::PS, TexSlot_RoughnessMap);
//		locator::Locator::GetDx11Device()->UnbindShaderResource(ShaderStage::PS, TexSlot_SpecularMap);
//		locator::Locator::GetDx11Device()->UnbindShaderResource(ShaderStage::PS, TexSlot_AOMap);
//	}
//
//	void PBRMaterial::SetDefaultMaterialFilename()
//	{
//		using namespace mapping::texture_filename;
//		material_path.normal_map_name	 = DefaultNormalMap();
//		material_path.albedo_map_name	 = White8x8Map();
//		material_path.pbr_map_name		 = White8x8Map();
//		material_path.roughness_map_name = White8x8Map();
//		material_path.metalness_map_name = White8x8Map();
//		material_path.specular_map_name  = White8x8Map();
//		material_path.ao_map_name		 = White8x8Map();
//	}
//
//	void PBRMaterial::RenderImGui()
//	{
//		ImGui::Checkbox("Custom Parameter", &use_custom_parameter);
//		cb_pbr_material->GetData().material.material_use_custom = use_custom_parameter;
//
//		if(use_custom_parameter)
//		{
//			if (ImGui::TreeNode("PBR Material Parameter"))
//			{
//				ImGui::Checkbox(  "Custom Parameter", &use_custom_parameter);
//				ImGui::ColorEdit3("Base Color"		, (float*)&cb_pbr_material->GetData().material.base_color);
//				ImGui::ColorEdit3("Emissive Color"	, (float*)&cb_pbr_material->GetData().material.emissive_color);
//				ImGui::DragFloat( "Roughness"		, &cb_pbr_material->GetData().material.roughness   , 0.01f, 0.0f, 1.0f);
//				ImGui::DragFloat( "Metalness;"		, &cb_pbr_material->GetData().material.metalness	  , 0.01f, 0.0f, 1.0f);
//				ImGui::DragFloat( "Reflectance;"	, &cb_pbr_material->GetData().material.reflectance , 0.01f, 0.0f, 1.0f);
//
//				ImGui::TreePop();
//			}
//		}
//		else
//		{
//			// TODO: PBRテクスチャの設定を行えるようにする(FBXModelComponentの処理を参考に)
//		}
//
//	}
//
//	void PBRMaterial::SetCustomMaterial(bool flg) const
//	{
//		cb_pbr_material->GetData().material.material_use_custom = flg;
//	}
//
//}  // cumulonimbus::shader_asset