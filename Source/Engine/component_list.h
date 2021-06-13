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

// TODO: ECSに環境を変えれたらComponent* Createは消しても大丈夫なようにする
namespace component_list
{
	Component* Create(const std::string& component_name);
}
