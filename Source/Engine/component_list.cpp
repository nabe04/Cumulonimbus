#include "component_list.h"

#include "transform.h"
#include "transform_component.h"
#include "sprite_object.h"
#include "sprite.h"
#include "anim_sprite.h"

namespace component_list
{
	Component* Create(const std::string& component_name)
	{
		if(component_name == "Transform")
		{
			return new TransformComponent{};
		}
		else if(component_name == "SpriteObject")
		{
			return new SpriteObject{};
		}
		else if(component_name == "Sprite")
		{
			return new SpriteComponent{};
		}
		else if(component_name == "AnimSprite")
		{
			return new AnimSpriteComponent{};
		}
		return nullptr;
	}
}
