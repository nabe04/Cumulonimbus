#pragma once

namespace cumulonimbus::mapping::shader_assets
{
	enum class ShaderAsset3D
	{
		Standard,
		Diffuse,
		Phong,
		Metal,
		Toon,
		ReflectionMapping,
		RefractionMapping,
		SingleColor,

		End
	};

	enum class ShaderAsset2D
	{
		Standard,

		End
	};
}