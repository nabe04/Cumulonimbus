#pragma once

#include <string>

//#include "component_base.h"
//#include "actor_component.h"
//#include "transform_component.h"
//#include "sprite.h"
//#include "anim_sprite.h"
//#include "sprite_object.h"
//#include "mesh_object.h"
//#include "camera_operation.h"


class Component;

// TODO: ECS�Ɋ���ς��ꂽ��Component* Create�͏����Ă����v�Ȃ悤�ɂ���
namespace component_list
{
	Component* Create(const std::string& component_name);
}
