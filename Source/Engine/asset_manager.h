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
		 * @brief : T型のLoaderの取得
		 * @remark : T型はLoaderクラスから派生したクラスを指定する必要がある
		 * @return : T型のポインタ
		 */
		template<class T>
		T* GetLoader()
		{
			return static_cast<T*>(loaders.at(typeid(T).hash_code()).get());
		}

		/**
		 * @brief : Loaderの登録
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
