#include "asset_manager.h"

#include <filesystem>
#include <fstream>
#include <set>

#include "file_path_helper.h"
#include "texture.h"
// Loaders
#include "loader.h"
#include "material_loader.h"
#include "model_loader.h"
#include "prefab_loader.h"
#include "scene_loader.h"
#include "texture_loader.h"

namespace
{
	const std::string default_save_path{ "./Data/AssetSheet/asset_sheet" + cumulonimbus::file_path_helper::GetSheetExtension()};
}

namespace cumulonimbus::asset
{
	AssetManager::AssetManager()
	{
		sheet_manager = std::make_unique<AssetSheetManager>();
		RegisterLoader<Material	 , MaterialLoader>();
		RegisterLoader<Model	 , ModelLoader>();
		RegisterLoader<Texture	 , TextureLoader>();
		RegisterLoader<Prefab	 , PrefabLoader>();
		RegisterLoader<SceneAsset, SceneLoader>();

		// デフォルトパスからのロード
		Load();

		// アセットシートの型が消えていた場合の追加
		AssetSheetManager check{};
		for(const auto& [hash, asset_sheet] : check.GetSheets())
		{
			if(!sheet_manager->HasSheet(hash))
			{
				sheet_manager->Register(hash);
			}
		}
	}

	void AssetManager::AddAsset(const std::filesystem::path& path)
	{
		for (auto& value : loaders | std::views::values)
		{
			if (value->Supported(path.extension()))
				value->Load(*this, path);
		}
	}

	void AssetManager::RenameAsset(
		const mapping::rename_type::UUID& asset_id,
		const std::string& changed_name, const std::string& extension)
	{
		for(auto& loader : loaders | std::views::values)
		{
			// 保存したいファイルの拡張子とローダーがサポートしている
			// 拡張子が一致すれば名前変更処理を行う
			if (!loader->Supported(extension))
				continue;

			loader->Rename(*this, asset_id, changed_name);
			break;
		}
	}

	void AssetManager::DeleteAssetAndLoader(const std::filesystem::path& path)
	{
		DeleteLoader(path);
		DeleteAsset(path);
	}

	void AssetManager::DeleteAsset(const std::filesystem::path& path) const
	{
		for (auto& asset_sheet : sheet_manager->GetSheets() | std::views::values)
		{
			// pathとアセットシートに登録されているパスが一致した場合trueになる
			bool is_hit{ false };
			for(auto& [uuid,asset_path] : asset_sheet.sheet)
			{
				if(path == asset_path)
				{
					is_hit = true;
					asset_sheet.sheet.erase(uuid);
					break;
				}
			}
			if (is_hit)
				break;
		}
	}

	void AssetManager::DeleteLoader(const std::filesystem::path& path)
	{
		for (auto& loader : loaders | std::views::values)
		{
			loader->Delete(*this, path);
		}
	}

	void AssetManager::Save(const std::filesystem::path& save_path)
	{
		if(save_path.compare({""}) == 0)
		{// デフォルトのパスに保存
			std::ofstream ofs(default_save_path, std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
		else
		{
			std::ofstream ofs(save_path, std::ios_base::binary);
			cereal::BinaryOutputArchive output_archive(ofs);
			output_archive(*this);
		}
	}

	void AssetManager::Load(const std::filesystem::path& load_path)
	{
		static const std::set<std::filesystem::path> extension
		{
			file_path_helper::GetSheetExtension()
		};

		std::filesystem::create_directory(std::filesystem::path{ default_save_path }.parent_path());

		if(load_path.compare({""}) == 0)
		{// デフォルトのフォルダにある「.sheet」の読み込み
			std::filesystem::path default_path{};
			for(const auto& path : std::filesystem::directory_iterator(std::filesystem::path{ default_save_path }.parent_path()))
			{
				default_path = path;
				if (!extension.contains(default_path.extension()))
					continue;
				{
					std::ifstream ifs(default_path, std::ios_base::binary);
					if (!ifs)
						assert(!"Not found file(AssetManager::Load)");
					cereal::BinaryInputArchive input_archive(ifs);
					input_archive(*this);
				}

				// Loaderへのアセット登録
				AssetLoad();
				return;
			}
			//assert(!"Not found file(AssetManager::Load)");
			return;
		}

		// 任意のフォルダからの読み込み
		if (!extension.contains(load_path.extension()))
			assert(!"The extension is incorrect(AssetManager::Load)");
		{
			std::ifstream ifs(load_path);
			if (!ifs)
				assert(!"Not found file(AssetManager::Load)");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(*this);
		}
		// Loaderへのアセット登録
		AssetLoad();
	}

	void AssetManager::AssetLoad()
	{
		/*
		 * sheets_key : アセット(Texture、Model、Material...)型のハッシュ値
		 * sheets_value : AssetSheet構造体
		 */
		for (const auto& [hash, sheets] : sheet_manager->GetSheets())
		{
			/*
			 * asset_id : アセットID(UUID)
			 * asset_path : アセットIDの指すアセットのパス
			 */
			for (const auto& asset_id : sheets.sheet | std::views::keys)
			{
				auto h = connector.at(hash);
				loaders.at(connector.at(hash))->Load(*this, asset_id);
			}
		}
	}
} // cumulonimbus::asset
