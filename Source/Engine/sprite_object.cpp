#include "sprite_object.h"

#include <fstream>

#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>

#include "file_path_helper.h"

namespace cumulonimbus::component
{
	SpriteObjectComponent::SpriteObjectComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:ComponentBase{ registry, ent }
	{

	}
	void SpriteObjectComponent::Load(ecs::Registry* registry)
	{
		SetRegistry(registry);
	}
}
