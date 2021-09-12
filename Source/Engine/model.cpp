#include "model.h"

#include <set>

namespace cumulonimbus::asset
{
	Model::Model(const std::filesystem::path& path)
	{
		// pathの拡張子が「.fbx」または「.FBX」の場合
		if (std::set<std::filesystem::path>{".fbx", ".FBX"}.contains(path.extension()))
		{
			Convert(path);
		}
		//if(path.extension() == ".fbx")
	}

	void Model::Convert(const std::filesystem::path& path)
	{

	}
} // cumulonimbus::asset
