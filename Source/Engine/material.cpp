#include "material.h"

#include <cassert>
#include <fstream>

#include "cereal_helper.h"
#include "file_path_helper.h"

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

	template <class Archive>
	void Material::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(material_data)
		);
	}

	Material::Material(const MaterialData& data = {})
	{
		material_data = data;
	}

	void Material::Save(const std::filesystem::path& path)
	{
		if (path.extension().compare(file_path_helper::GetMaterialExtension()) != 0)
			assert(!"The file extension is not Åu.matÅv");
		std::filesystem::create_directory(path.parent_path());
		{
			std::ofstream ofs(path, std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
		{
			std::filesystem::path json_path = path;

			std::ofstream ofs(json_path.replace_extension().string() + file_path_helper::GetJsonExtension());
			cereal::JSONOutputArchive output_archive(ofs);
			output_archive(*this);
		}
	}

	void Material::Load(const std::filesystem::path& path)
	{
		std::ifstream ifs(path.string(),std::ios_base::binary);
		if (!ifs)
			assert(!"Not open file(Material::Load)");
		cereal::BinaryInputArchive input_archive(ifs);
		input_archive(*this);
	}


} // cumulonimbus::asset
