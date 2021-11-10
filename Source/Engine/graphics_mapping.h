#pragma once

namespace cumulonimbus::mapping::graphics
{
	enum class ShaderStage
	{
		VS,
		HS,
		DS,
		GS,
		PS,
		CS,

		End
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip,
		PointList,
		LineList,

		End
	};
} // cumulonimbus::mapping::graphics