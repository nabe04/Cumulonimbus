#pragma once
#include <string>

namespace cumulonimbus::mapping::texture_filename
{
	inline std::string NoImage256()			{ return { "./Data/Assets/Textures/no_image_256.png" }; }
	inline std::string Coffee256()			{ return { "./Data/Assets/Textures/coffee_256.png" }; }
	inline std::string UvChecker()			{ return { "./Data/Assets/Textures/uv_checker.png" }; }
	inline std::string DefaultNormalMap()	{ return { "./Data/Assets/Textures/default_normal_map.png" }; }
	inline std::string White8x8Map()		{ return { "./Data/Assets/Textures/white_8x8_tex.png" }; }
	inline std::string GanfaulNormalMap()	{ return { "./Data/Assets/FBX/ganfaul/character.fbm/Ganfaul_normal.png" }; }
} // cumulonimbus::mapping::texture_filename
