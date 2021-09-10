#pragma once
#include <memory>
#include <filesystem>

#include "rename_type_mapping.h"
#include "generic.h"

namespace cumulonimbus::asset
{
	class AssetSheetManager;
	class Loader;
} // cumulonimbus::asset

namespace cumulonimbus::asset
{
	class AssetManager final
	{
	public:
		AssetManager();

		void AddAsset(const std::filesystem::path& path);

		/**
		 * @brief : T�^��Loader�̎擾
		 * @remark : T�^��Loader�N���X����h�������N���X���w�肷��K�v������
		 * @return : T�^�̃|�C���^
		 */
		template<class T>
		T* GetLoader()
		{
			return static_cast<T*>(loaders.at(typeid(T).hash_code()).get());
		}

		/**
		 * @brief : Loader�̓o�^
		 */
		template<class T>
		void RegisterLoader()
		{
			if (loaders.contains(utility::GetHash<T>()))
				return;

			loaders.insert(std::make_pair(utility::GetHash<T>(), std::make_unique<T>()));
		}
	private:
		std::unique_ptr<AssetSheetManager>   sheet_manager{};
		std::map<mapping::rename_type::Hash, std::unique_ptr<Loader>> loaders{};
 	};
} // cumulonimbus::asset
