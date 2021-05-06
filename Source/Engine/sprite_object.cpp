#include "sprite_object.h"


#include <fstream>
#include <cereal/types/polymorphic.hpp>

SpriteObject::SpriteObject(Entity* entity)
	:Component{ entity }
{
	component_name = "SpriteObject";
	sprite_shader_state = std::make_unique<shader::SpriteShaderState>();
}

void SpriteObject::Save(std::string file_path)
{
	const std::string file_path_and_name = file_path + component_name + ".json";
	std::ofstream ofs(file_path_and_name);
	cereal::JSONOutputArchive o_archive(ofs);
	o_archive(
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
		CEREAL_NVP(component_name),
		CEREAL_NVP(blend_state),
		CEREAL_NVP(rasterizer_state),
		CEREAL_NVP(sampler_state),
		CEREAL_NVP(depth_stencil_state),
		CEREAL_NVP(sprite_shader_state)
	);

	this->entity = entity;
}


CEREAL_REGISTER_TYPE(SpriteObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, SpriteObject)