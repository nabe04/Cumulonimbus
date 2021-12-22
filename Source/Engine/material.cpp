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

	Material::Material(const Material& other)
		:material_data{ other.material_data },
		mat_id{ other.mat_id }
	{
		cb_material = std::make_unique<buffer::ConstantBuffer<MaterialCB>>(locator::Locator::GetDx11Device()->device.Get());
	}

	Material& Material::operator=(const Material& other)
	{
		if(this == &other)
		{
			return *this;
		}

		material_data = other.material_data;
		mat_id		  = other.mat_id;

		cb_material = std::make_unique<buffer::ConstantBuffer<MaterialCB>>(locator::Locator::GetDx11Device()->device.Get());

		return *this;
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
		TextureLoader* tex_loader	= locator::Locator::GetAssetManager()->GetLoader<TextureLoader>();
		Texture& albedo_tex			= tex_loader->GetTexture(material_data.albedo_id);
		Texture& roughness_tex		= tex_loader->GetTexture(material_data.roughness_id);
		Texture& metalness_tex		= tex_loader->GetTexture(material_data.metallic_id);
		Texture& normal_tex			= tex_loader->GetTexture(material_data.normal_id);
		Texture& occlusion_tex		= tex_loader->GetTexture(material_data.occlusion_id);
		// Albedo Texture
		ImSelectTexture(albedo_tex, material_data.albedo_id, albedo_tex.GetFilename(), std::string{ "Albedo" });
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		// Roughness Texture
		ImSelectTexture(roughness_tex, material_data.roughness_id, roughness_tex.GetFilename().c_str(), std::string{ "Roughness" });
		ImSelectColorChannel(roughness_channel, cb_material->GetData().mat_use_roughness_channel, cb_material->GetData().mat_roughness, material_data.roughness_id, std::string{ "Roughness" });
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		// Metalness Texture
		ImSelectTexture(metalness_tex, material_data.metallic_id, metalness_tex.GetFilename().c_str(), std::string{ "Metalness" });
		ImSelectColorChannel(metalness_channel, cb_material->GetData().mat_use_metalness_channel, cb_material->GetData().mat_metalness, material_data.metallic_id, std::string{ "Metalness" });
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		// Normal Texture
		ImSelectTexture(normal_tex, material_data.normal_id, normal_tex.GetFilename().c_str(), std::string{ "Normal" });
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		// Occlusion Texture
		ImSelectTexture(occlusion_tex, material_data.occlusion_id, occlusion_tex.GetFilename().c_str(), std::string{ "Occlusion" });
	}

	void Material::BindMaterial(const mapping::graphics::ShaderStage shader_stage) const
	{
		TextureLoader* loader = locator::Locator::GetAssetManager()->GetLoader<TextureLoader>();
		// Albedoテクスチャのバインド
		loader->GetTexture(material_data.albedo_id).BindTexture(shader_stage, TexSlot_BaseColorMap);
		// Normalマップのバインド
		loader->GetTexture(material_data.normal_id).BindTexture(shader_stage, TexSlot_NormalMap);
		// Roughnessマップのバインド
		loader->GetTexture(material_data.roughness_id).BindTexture(shader_stage, TexSlot_RoughnessMap);
		// Metalnessマップのバインド
		loader->GetTexture(material_data.metallic_id).BindTexture(shader_stage, TexSlot_MetalnessMap);
		// Occlusionマップのバインド
		loader->GetTexture(material_data.occlusion_id).BindTexture(shader_stage, TexSlot_OcclusionMap);

		// コンスタントバッファのバインド
		cb_material->Activate(locator::Locator::GetDx11Device()->immediate_context.Get(), CBSlot_Material, false);
	}

	void Material::UnbindMaterial(const mapping::graphics::ShaderStage shader_stage) const
	{
		TextureLoader* loader = locator::Locator::GetAssetManager()->GetLoader<TextureLoader>();
		// Albedoテクスチャのアンバインド
		loader->GetTexture(material_data.albedo_id).UnbindTexture(shader_stage, TexSlot_BaseColorMap);
		// Normal マップのアンバインド
		loader->GetTexture(material_data.normal_id).UnbindTexture(shader_stage, TexSlot_NormalMap);
		// Roughnessマップのアンバインド
		loader->GetTexture(material_data.roughness_id).UnbindTexture(shader_stage, TexSlot_RoughnessMap);
		// Metalnessマップのアンバインド
		loader->GetTexture(material_data.metallic_id).UnbindTexture(shader_stage, TexSlot_MetalnessMap);
		// Occlusionマップのアンバインド
		loader->GetTexture(material_data.occlusion_id).UnbindTexture(shader_stage, TexSlot_OcclusionMap);

		// コンスタントバッファのアンバインド
		cb_material->Deactivate(locator::Locator::GetDx11Device()->immediate_context.Get());
	}

	void Material::ImSelectTexture(
		Texture& tex, mapping::rename_type::UUID& mat_tex_id,
		const std::string& tex_filename,
		const std::string& label_name)
	{
		AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		TextureLoader* tex_loader = locator::Locator::GetAssetManager()->GetLoader<TextureLoader>();

		ImGui::Text(label_name.c_str());
		helper::imgui::Image(tex);

		const std::string label = "##SelectTexture" + label_name;

		ImGui::Text(ICON_FA_IMAGE);
		ImGui::SameLine();

		if (ImGui::BeginCombo(label.c_str(), tex_filename.c_str()))
		{
			const bool is_none_selected = (mat_tex_id.empty());
			if (ImGui::Selectable("None", is_none_selected, 0, { 500,50 }))
			{
				mat_tex_id.clear();
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
				const bool is_selected = (tex_id == mat_tex_id);
				if (ImGui::Selectable(tex_data->GetFilename().c_str(), is_selected, 0, { 500,50 }))
				{
					mat_tex_id = tex_id;
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
	}

	void Material::ImSelectColorChannel(
		EnumStateMap<ColorChannel>& color_channel,
		float4& use_channel, float& custom_param,
		const mapping::rename_type::UUID& mat_id,
		const std::string& label_name,
		const float custom_param_range_min,
		const float custom_param_range_max)
	{
		// 使用するテクスチャチャンネルの設定
		// (シェーダー側で使用)
		auto ChangeColorChannel = [&use_channel](const ColorChannel color_channel)
		{
			if (color_channel == ColorChannel::R)
			{
				use_channel.x = 1.0f;
				use_channel.y = 0.0f;
				use_channel.z = 0.0f;
				use_channel.w = 0.0f;
				return;
			}
			if (color_channel == ColorChannel::G)
			{
				use_channel.x = 0.0f;
				use_channel.y = 1.0f;
				use_channel.z = 0.0f;
				use_channel.w = 0.0f;
				return;
			}
			if (color_channel == ColorChannel::B)
			{
				use_channel.x = 0.0f;
				use_channel.y = 0.0f;
				use_channel.z = 1.0f;
				use_channel.w = 0.0f;
				return;
			}
			if(color_channel == ColorChannel::A)
			{
				use_channel.x = 0.0f;
				use_channel.y = 0.0f;
				use_channel.z = 0.0f;
				use_channel.w = 1.0f;
				return;
			}

			use_channel.x = 0.0f;
			use_channel.y = 0.0f;
			use_channel.z = 0.0f;
			use_channel.w = 0.0f;
		};

		ImGui::PushID(mat_id.c_str());
		const std::string label_combo = "##SelectColorChannel" + label_name;

		ImGui::Text(ICON_FA_SPINNER);
		ImGui::SameLine();

		if(ImGui::BeginCombo(label_combo.c_str(), color_channel.GetCurrentStateName().c_str()))
		{
			for(const auto& state_name : color_channel.GetStateNames())
			{
				const bool is_selected = (state_name == color_channel.GetCurrentStateName());
				if (ImGui::Selectable(state_name.c_str(), is_selected, 0, { 500,50 }))
				{
					color_channel.SetState(state_name);
					ChangeColorChannel(color_channel.GetCurrentState());
				}
				if(is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if(color_channel.GetCurrentState() == ColorChannel::Custom)
		{// color_channelがColorChannel::Customの場合のみ独自のパラメータを設定出来る
			const std::string label_drag_float = "##DragParam" + label_name;
			ImGui::SliderFloat(label_drag_float.c_str(), &custom_param, custom_param_range_min, custom_param_range_max);
		}

		ImGui::PopID();
	}

} // cumulonimbus::asset
