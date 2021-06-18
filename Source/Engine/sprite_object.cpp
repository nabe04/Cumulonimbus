#include "sprite_object.h"

#include <fstream>

#include <cereal/types/memory.hpp>

#include "file_path_helper.h"

namespace cumulonimbus::component
{
	SpriteObjectComponent::SpriteObjectComponent(ecs::Registry* const registry, const ecs::Entity ent)
		:ComponentBase{ registry, ent }
	{

	}

	void SpriteObjectComponent::Save(const std::string& file_path)
	{
		const std::string file_path_and_name = file_path + file_path_helper::GetTypeName<SpriteObjectComponent>() + file_path_helper::GetJsonExtension();
		std::ofstream ofs(file_path_and_name);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	void SpriteObjectComponent::Load(const std::string& file_path_and_name)
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(*this);
	}
}
