#include "model.h"

#include <set>

namespace cumulonimbus::asset
{
	Model::Model(const std::filesystem::path& path)
	{
		// path�̊g���q���u.fbx�v�܂��́u.FBX�v�̏ꍇ
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
