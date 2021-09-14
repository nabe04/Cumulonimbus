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
		Load(path);

		int a;
		a = 0;
	}

	void Model::Save(const std::filesystem::path& path)
	{
		{
			std::ofstream ofs(path.string() + file_path_helper::GetModelExtension(), std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
	}

	void Model::Load(const std::filesystem::path& path)
	{
		// pathÇÃägí£éqÇ™Åu.modelÅvÇÃèÍçá
		if (!std::set<std::filesystem::path>{".model"}.contains(path.extension()))
			assert(!"The extension is different(Model::Model)");

		std::ifstream ifs(path, std::ios_base::binary);
		if (!ifs)
			assert(!"Not open file");
		cereal::BinaryInputArchive input_archive(ifs);
		input_archive(*this);

		auto m = GetModelData();
		int a;
		a = 0;
	}

} // cumulonimbus::asset
