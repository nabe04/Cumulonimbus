#include "player_component.h"

namespace cumulonimbus::component
{
	PlayerComponent::PlayerComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent)
		:ComponentBase{registry, ent}
	{
		
	}

	void PlayerComponent::NewFrame(float dt)
	{
	}

	void PlayerComponent::Update(float dt)
	{
	}

	void PlayerComponent::RenderImGui()
	{
	}

	void PlayerComponent::Save(const std::string& file_path)
	{
	}

	void PlayerComponent::Load(const std::string& file_path_and_name)
	{
	}
} // cumulonimbus::component
