#pragma once

#include <string>

class Component;

// TODO: ECS‚ÉŠÂ‹«‚ğ•Ï‚¦‚ê‚½‚çComponent* Create‚ÍÁ‚µ‚Ä‚à‘åä•v‚È‚æ‚¤‚É‚·‚é
namespace component_list
{
	Component* Create(const std::string& component_name);
}
