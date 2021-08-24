#pragma once
#include <string>
#include <unordered_map>

#include "enum_state_map.h"

namespace cumulonimbus::utility
{
	/**
	 * @brief : �n�`�̑���
	 */
	enum class TerrainAttribute
	{
		Floor,
		Wall,

		End
	};

	/**
	 * @brief : �}�e���A�����ʃN���X
	 */
	class MaterialDiscrimination final
	{
	public:
		explicit MaterialDiscrimination();
		~MaterialDiscrimination()			= default;

		/**
		 * @brief			: ���O(std::string)���w�肵�ēo�^����Ă���
		 *					  �n�`�}�b�v(terrain_map)����n�`�����̎擾
		 * @param key_name	: terrain_map�̃L�[�l
		 */
		TerrainAttribute GetTerrainAttribute(const std::string& key_name);
	private:
		std::unordered_map<std::string, EnumStateMap<TerrainAttribute>> terrain_map{};

		/**
		 * @brief			: �n�`�̒ǉ�
		 * @details 		: key_name���d������ꍇ�㏑������
		 * @param key_name	: �n�`��(terrain_map�̃L�[�l�ɂȂ�)
		 * @param attribute	: �n�`����
		 */
		void RegisterTerrain(const std::string& key_name, TerrainAttribute attribute);
	};
} // cumulonimbus::utility
