#include "actor_component.h"
#include <fstream>

ActorComponent::ActorComponent(Entity* entity, ActorType actor_type)
	: Component{ entity }
{
	component_name = "ActorComponent";
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

void ActorComponent::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		CEREAL_NVP(actor_type),

		CEREAL_NVP(blend_state),
		CEREAL_NVP(rasterizer_state),
		CEREAL_NVP(sampler_state),
		CEREAL_NVP(depth_stencil_state),

		CEREAL_NVP(shader_state),
		CEREAL_NVP(shader_state_2d),

		CEREAL_NVP(rendering_buffer)
	);
}

void ActorComponent::Load(Entity* entity, std::string file_path_and_name)
{
	{
		std::ifstream ifs(file_path_and_name);
		cereal::JSONInputArchive i_archive(ifs);
		i_archive(
			CEREAL_NVP(actor_type),

			CEREAL_NVP(blend_state),
			CEREAL_NVP(rasterizer_state),
			CEREAL_NVP(sampler_state),
			CEREAL_NVP(depth_stencil_state),

			CEREAL_NVP(shader_state),
			CEREAL_NVP(shader_state_2d),

			CEREAL_NVP(rendering_buffer)
		);

		this->entity = entity;
	}
}

