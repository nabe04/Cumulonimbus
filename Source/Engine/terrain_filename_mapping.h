#pragma once
#include <string>

// 地形のマテリアルテクスチャ名(拡張子を除く)
namespace cumulonimbus::mapping::texture_filename
{
	inline std::string TerrainFloor() { return { "Terrain_Floor" }; }
	inline std::string TerrainWall()  { return { "Terrain_Wall" }; }
} // cumulonimbus::mapping::texture_filename
