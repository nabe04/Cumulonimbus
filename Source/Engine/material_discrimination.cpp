#include "material_discrimination.h"

#include <cassert>

#include "terrain_filename_mapping.h"

namespace cumulonimbus::utility
{
	MaterialDiscrimination::MaterialDiscrimination()
	{
		RegisterTerrain(mapping::texture_filename::TerrainFloor(), TerrainAttribute::Floor);
		RegisterTerrain(mapping::texture_filename::TerrainWall() , TerrainAttribute::Wall);
	}

	void MaterialDiscrimination::RegisterTerrain(const std::string& key_name, TerrainAttribute attribute)
	{
		if(terrain_map.contains(key_name))
		{
			terrain_map.at(key_name).SetState(attribute);
		}

		terrain_map.insert(std::make_pair(key_name, EnumStateMap{ attribute }));
	}

	TerrainAttribute MaterialDiscrimination::GetTerrainAttribute(const std::string& key_name)
	{
		if (!terrain_map.contains(key_name))
			return TerrainAttribute::End;
			//assert(!"No key registered(MaterialDiscrimination::GetTerrainAttribute)");

		return terrain_map.at(key_name).GetCurrentState();
	}
} // cumulonimbus::utility
