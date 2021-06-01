#include "sprite_object.h"


#include <fstream>
#include <cereal/types/polymorphic.hpp>

SpriteObject::SpriteObject(Entity* entity)
	:Component{ entity }
{
	component_name = "SpriteObject";
}

void SpriteObject::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
		cereal::base_class<Component>(this),
		CEREAL_NVP(component_name),
		CEREAL_NVP(blend_state),
		CEREAL_NVP(rasterizer_state),
		CEREAL_NVP(sampler_state),
		CEREAL_NVP(depth_stencil_state),
		CEREAL_NVP(sprite_shader_state)
	);
}

void SpriteObject::Load(Entity* entity, std::string file_path_and_name)
{
	std::ifstream ifs(file_path_and_name);
	cereal::JSONInputArchive i_archive(ifs);
	i_archive(
		cereal::base_class<Component>(this),
		CEREAL_NVP(component_name),
		CEREAL_NVP(blend_state),
		CEREAL_NVP(rasterizer_state),
		CEREAL_NVP(sampler_state),
		CEREAL_NVP(depth_stencil_state),
		CEREAL_NVP(sprite_shader_state)
	);

	this->entity = entity;
}
