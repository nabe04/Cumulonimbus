#pragma once

namespace cumulonimbus::mapping::component_tag
{
	enum class ComponentTag
	{
		NoTag,
		Mesh,
		Sprite,
		Physics,
		Light,
		Camera,
		Collider,
		Spawner,
		Transition,
		Game,

		End
	};
} //  cumulonimbus::mapping::component_tag
