#pragma once

#include <string>

#include "component_base.h"


class Component;

// TODO: ECS�Ɋ���ς��ꂽ��Component* Create�͏����Ă����v�Ȃ悤�ɂ���
namespace component_list
{
	Component* Create(const std::string& component_name);
}
