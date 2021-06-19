#include "actor_component.h"
#include <fstream>

#include <cereal/archives/json.hpp>

#include "component_base.h"
#include "file_path_helper.h"

namespace cumulonimbus::component
{
	ActorComponent::ActorComponent(ecs::Registry* const registry, const mapping::rename_type::Entity ent, ActorType actor_type)
		: ComponentBase{ registry,ent }
	{
		this->actor_type.SetState(actor_type);
	}

	void ActorComponent::RenderImGui()
	{
		if (actor_type.GetCurrentState() == ActorType::Actor3D)
		{
			shader_state.RenderImGui();
		}
		else if (actor_type.GetCurrentState() == ActorType::Actor2D)
		{
			//shader_state_2D-
		}
	};

	void ActorComponent::Save(const std::string& file_path)
	{
		const std::string file_path_and_name = file_path + file_path_helper::GetTypeName<ActorComponent>() + file_path_helper::GetJsonExtension();
		std::ofstream ofs(file_path_and_name);
		cereal::JSONOutputArchive o_archive(ofs);
		o_archive(*this);
	}

	void ActorComponent::Load(const std::string& file_path_and_name)
	{
		{
			std::ifstream ifs(file_path_and_name);
			cereal::JSONInputArchive i_archive(ifs);
			i_archive(*this);
		}
	}
}
