#pragma once

#include <string>

#include "component_base.h"
#include "actor_component.h"
#include "sprite.h"


class Component;

// TODO: ECSに環境を変えれたらComponent* Createは消しても大丈夫なようにする
namespace component_list
{
	Component* Create(const std::string& component_name);
}
