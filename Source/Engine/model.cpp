#include "model.h"

#include <fstream>
#include <set>

#include "cereal_helper.h"
#include "file_path_helper.h"

namespace cumulonimbus::asset
{
	template <class Archive>
	void Model::serialize(Archive&& archive)
	{
		archive(
			CEREAL_NVP(model_data)
		);
	}

	Model::Model(const std::filesystem::path& path)
	{
		// pathの拡張子が「.fbx」または「.FBX」の場合
		if (std::set<std::filesystem::path>{".fbx", ".FBX"}.contains(path.extension()))
		{
			Convert(path);
		}
		//if(path.extension() == ".fbx")
	}

	void Model::Save(const std::filesystem::path& path)
	{
		{
			std::ofstream ofs(path.string() + file_path_helper::GetModelExtension());
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
		{
			std::ofstream ofs(path.string() + file_path_helper::GetJsonExtension());
			cereal::JSONOutputArchive output_archive(ofs);
			output_archive(*this);
		}
	}

	void Model::Convert(const std::filesystem::path& path)
	{

	}
} // cumulonimbus::asset
