#pragma once
#include <string>
#include <unordered_map>

#include "enum_state_map.h"

namespace cumulonimbus::utility
{
	/**
	 * @brief : 地形の属性
	 */
	enum class TerrainAttribute
	{
		Floor,
		Wall,

		End
	};

	/**
	 * @brief : マテリアル判別クラス
	 */
	class MaterialDiscrimination final
	{
	public:
		explicit MaterialDiscrimination();
		~MaterialDiscrimination()			= default;

		/**
		 * @brief			: 名前(std::string)を指定して登録されている
		 *					  地形マップ(terrain_map)から地形属性の取得
		 * @param key_name	: terrain_mapのキー値
		 */
		TerrainAttribute GetTerrainAttribute(const std::string& key_name);
	private:
		std::unordered_map<std::string, EnumStateMap<TerrainAttribute>> terrain_map{};

		/**
		 * @brief			: 地形の追加
		 * @details 		: key_nameが重複する場合上書きする
		 * @param key_name	: 地形名(terrain_mapのキー値になる)
		 * @param attribute	: 地形属性
		 */
		void RegisterTerrain(const std::string& key_name, TerrainAttribute attribute);
	};
} // cumulonimbus::utility
