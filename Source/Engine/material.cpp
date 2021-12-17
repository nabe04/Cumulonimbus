#include "material.h"

#include <cassert>
#include <fstream>

#include "cereal_helper.h"
#include "cum_imgui_helper.h"
#include "file_path_helper.h"
#include "texture.h"
#include "texture_loader.h"
#include "locator.h"
#include "texture_resource_mapping.h"

CEREAL_CLASS_VERSION(cumulonimbus::asset::MaterialData, 0);

namespace cumulonimbus::asset
{
	template <class Archive>
	void MaterialData::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(color),
			CEREAL_NVP(albedo_id),
			CEREAL_NVP(metallic_id),
			CEREAL_NVP(normal_id),
			CEREAL_NVP(height_id),
			CEREAL_NVP(occlusion_id)
		);
	}

	//template <class Archive>
	//void MaterialData::load(Archive&& archive, uint32_t version)
	//{
	//	archive(
	//		CEREAL_NVP(color),
	//		CEREAL_NVP(albedo_id),
	//		CEREAL_NVP(metallic_id),
	//		CEREAL_NVP(normal_id),
	//		CEREAL_NVP(height_id),
	//		CEREAL_NVP(occlusion_id)
	//	);

	//}

	//template <class Archive>
	//void MaterialData::save(Archive&& archive, uint32_t version) const
	//{
	//	archive(
	//		CEREAL_NVP(color),
	//		CEREAL_NVP(albedo_id),
	//		CEREAL_NVP(metallic_id),
	//		CEREAL_NVP(normal_id),
	//		CEREAL_NVP(height_id),
	//		CEREAL_NVP(occlusion_id)
	//	);
	//}

	template <class Archive>
	void Material::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(material_data)
		);
	}

	Material::Material(const mapping::rename_type::UUID& mat_id)
	{
		this->mat_id = mat_id;
		cb_material  = std::make_unique<buffer::ConstantBuffer<MaterialCB>>(locator::Locator::GetDx11Device()->device.Get());
	}

	Material::Material(const mapping::rename_type::UUID& mat_id, const MaterialData& data = {})
	{
		this->mat_id  = mat_id;
		material_data = data;
		cb_material   = std::make_unique<buffer::ConstantBuffer<MaterialCB>>(locator::Locator::GetDx11Device()->device.Get());
	}

	void Material::Save(const std::filesystem::path& path)
	{
		if (path.extension().compare(file_path_helper::GetMaterialExtension()) != 0)
			assert(!"The file extension is not 「.mat」");
		std::filesystem::create_directory(path.parent_path());
		{
			std::ofstream ofs(path, std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
		//{ // json形式での保存を現在は切っておく
		//	std::filesystem::path json_path = path;

		//	std::ofstream ofs(json_path.replace_extension().string() + file_path_helper::GetJsonExtension());
		//	cereal::JSONOutputArchive output_archive(ofs);
		//	output_archive(*this);
		//}
	}

	void Material::Load(const std::filesystem::path& path)
	{
		std::ifstream ifs(path.string(),std::ios_base::binary);
		if (!ifs)
			assert(!"Not open file(Material::Load)");
		cereal::BinaryInputArchive input_archive(ifs);
		input_archive(*this);
	}

	void Material::RenderImGui()
	{
		AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		TextureLoader* tex_loader = locator::Locator::GetAssetManager()->GetLoader<TextureLoader>();
		Texture& albedo_tex = tex_loader->GetTexture(material_data.albedo_id);
		// Albedo Texture
		ImGui::Text("Albedo");
		ImGui::SameLine();
		helper::imgui::Image(albedo_tex);

		if(IMGUI_LEFT_LABEL(ImGui::BeginCombo, "Select Tecture", albedo_tex.GetFilename().c_str()))
		{
			const bool is_none_selected = (material_data.albedo_id.empty());
			if (ImGui::Selectable("None", is_none_selected, 0, {500,50}))
			{
				material_data.albedo_id.clear();
				const std::string mat_filepath = asset_manager.GetAssetSheetManager().GetAssetFilename<Material>(mat_id);
				Save(mat_filepath);
			}
			if (is_none_selected)
			{
				ImGui::SetItemDefaultFocus();
			}

			for (auto& [tex_id, tex_data] : tex_loader->GetTextures())
			{
				helper::imgui::Image(*tex_data.get());
				ImGui::SameLine();
				const bool is_selected = (tex_id == material_data.albedo_id);
				if (ImGui::Selectable(tex_data->GetFilename().c_str(), is_selected, 0, { 500,50 }))
				{
					material_data.albedo_id = tex_id;
					const std::string mat_filepath = asset_manager.GetAssetSheetManager().GetAssetFilename<Material>(mat_id);
					Save(mat_filepath);
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		//if (ImGui::BeginCombo("Select Texture", albedo_tex.GetFilename().c_str()))
		//{
		//
		//}
	}

	void Material::BindMaterial(const mapping::graphics::ShaderStage shader_stage) const
	{
		TextureLoader* loader = locator::Locator::GetAssetManager()->GetLoader<TextureLoader>();
		// Albedoテクスチャのバインド
		loader->GetTexture(material_data.albedo_id).BindTexture(shader_stage, TexSlot_BaseColorMap);
		// Normal マップのバインド
		loader->GetTexture(material_data.normal_id).BindTexture(shader_stage, TexSlot_NormalMap);
	}

	void Material::UnbindMaterial(const mapping::graphics::ShaderStage shader_stage) const
	{
		TextureLoader* loader = locator::Locator::GetAssetManager()->GetLoader<TextureLoader>();
		// Albedoテクスチャのアンバインド
		loader->GetTexture(material_data.albedo_id).UnbindTexture(shader_stage, TexSlot_BaseColorMap);
		// Normal マップのアンバインド
		loader->GetTexture(material_data.normal_id).UnbindTexture(shader_stage, TexSlot_NormalMap);
	}


} // cumulonimbus::asset
