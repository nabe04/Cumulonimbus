#pragma once

#include <string>

#include "ecs.h"

namespace component_list
{
	Component* Create(const std::string& component_name);
}
