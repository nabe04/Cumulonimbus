#pragma once
#include <map>
#include "loader.h"
#include "rename_type_mapping.h"
#include "prefab.h"

namespace cumulonimbus::asset
{
	class PrefabLoader final : public Loader
	{
	public:
		explicit PrefabLoader();
		~PrefabLoader() override = default;

		/**
		 * @brief : �v���t�@�u�̍쐬
		 * @param asset_manager : AssetManager�N���X�̎Q��
		 * @param ent : �v���t�@�u���������G���e�B�e�B
		 */
		void CreatePrefab(AssetManager& asset_manager,const mapping::rename_type::Entity& ent);
	private:
		std::map<mapping::rename_type::UUID, std::unique_ptr<Prefab>> prefabs{};
	};
} // cumulonimbus::asset