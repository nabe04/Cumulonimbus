#include "material3d_manager.h"

#include "3d_standard.h"
#include "diffuse.h"
#include "phong.h"
#include "metal.h"
#include "toon.h"
#include "reflection_mapping.h"
#include "refraction_mapping.h"
#include "single_color.h"

namespace cumulonimbus::shader_asset
{
	Material3DManager::Material3DManager()
	{
		using namespace mapping::shader_assets;
		//ì¬‚µ‚½ShaderAsset‚Ì“o˜^
		RegisterShaderAsset<shader_asset::Standard3DAsset>			(ShaderAsset3D::Standard);
		RegisterShaderAsset<shader_asset::DiffuseAsset>				(ShaderAsset3D::Diffuse);
		RegisterShaderAsset<shader_asset::PhongAsset>				(ShaderAsset3D::Phong);
		RegisterShaderAsset<shader_asset::MetalAsset>				(ShaderAsset3D::Metal);
		RegisterShaderAsset<shader_asset::ToonAsset>				(ShaderAsset3D::Toon);
		RegisterShaderAsset<shader_asset::ReflectionMappingAsset>	(ShaderAsset3D::ReflectionMapping);
		RegisterShaderAsset<shader_asset::RefractionMappingAsset>	(ShaderAsset3D::RefractionMapping);
		RegisterShaderAsset<shader_asset::SingleColorAsset>			(ShaderAsset3D::SingleColor);
	}


	void Material3DManager::BindAsset(mapping::shader_assets::ShaderAsset3D asset)
	{
		shader_asset_map.at(asset)->BindCBuffer();
	}

	void Material3DManager::UnbindAsset(mapping::shader_assets::ShaderAsset3D asset)
	{
		shader_asset_map.at(asset)->UnbindCBuffer();
	}

	void Material3DManager::RenderImGuiComboShader()
	{
		if (ImGui::BeginCombo("Shader", shader_assets.GetCurrentStateName().c_str()))
		{
			for (auto& name : shader_assets.GetStateNames())
			{
				bool is_selected = (shader_assets.GetCurrentState() == shader_assets.GetStateMap().at(name));

				if (ImGui::Selectable(name.c_str(), is_selected))
				{
					shader_assets.SetState(shader_assets.GetStateMap().at(name));
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}

	void Material3DManager::RenderImGuiShaderParameter()
	{
		shader_asset_map.at(shader_assets.GetCurrentState())->RenderImGui();
	}

	void Material3DManager::SetMaterialPathForAllShaderAsset3D(const shader_asset::MaterialPath& material_path)
	{
		for (auto& asset : shader_asset_map)
		{
			asset.second->SetMaterialPath(material_path);
		}
	}

	void Material3DManager::SetMaterialPath(mapping::shader_assets::ShaderAsset3D asset,
											   const shader_asset::MaterialPath& material_path)
	{
		shader_asset_map.at(asset)->SetMaterialPath(material_path);
	}
}
