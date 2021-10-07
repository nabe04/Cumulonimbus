#include "asset_manager.h"

#include <filesystem>
#include <fstream>
#include <set>

#include "texture.h"
#include "file_path_helper.h"
// Loaders
#include "material_loader.h"
#include "model_loader.h"
#include "texture_loader.h"
#include "prefab_loader.h"
#include "loader.h"

namespace
{
	const std::string default_save_path{ "./Data/AssetSheet/asset_sheet" + cumulonimbus::file_path_helper::GetSheetExtension()};
}

namespace cumulonimbus::asset
{
	AssetManager::AssetManager()
	{
		sheet_manager = std::make_unique<AssetSheetManager>();
		RegisterLoader<Material	, MaterialLoader>();
		RegisterLoader<Model	, ModelLoader>();
		RegisterLoader<Texture	, TextureLoader>();
		RegisterLoader<Prefab	, PrefabLoader>();

		// �f�t�H���g�p�X����̃��[�h
		Load();

		// �A�Z�b�g�V�[�g�̌^�������Ă����ꍇ�̒ǉ�
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
		for (auto&& [key, value] : loaders)
		{
			if (value->Supported(path.extension()))
				value->Load(*this, path);
		}
	}

	void AssetManager::RenameAsset(const mapping::rename_type::UUID& asset_id, const std::filesystem::path& path)
	{
		for (auto& [hash, asset_sheet] : sheet_manager->GetSheets())
		{
			for (auto& [uuid, asset_path] : asset_sheet.sheet)
			{
				if(uuid == asset_id)
				{
					std::filesystem::rename(asset_path, path);
					asset_path = path.string();
					Save();
					return;
				}
			}
		}
	}

	void AssetManager::DeleteAssetAndLoader(const std::filesystem::path& path)
	{
		DeleteLoader(path);
		DeleteAsset(path);
	}

	void AssetManager::DeleteAsset(const std::filesystem::path& path) const
	{
		// path�ƃA�Z�b�g�V�[�g�ɓo�^����Ă���p�X����v�����ꍇtrue�ɂȂ�
		bool is_hit{ false };

		for (auto& [hash, asset_sheet] : sheet_manager->GetSheets())
		{
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
		for (auto& [hash, loader] : loaders)
		{
			loader->Delete(*this, path);
		}
	}

	void AssetManager::Save(const std::filesystem::path& save_path)
	{
		if(save_path.compare({""}) == 0)
		{// �f�t�H���g�̃p�X�ɕۑ�
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
		{// �f�t�H���g�̃t�H���_�ɂ���u.sheet�v�̓ǂݍ���
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

				// �A�Z�b�g�V�[�g�̌^�������Ă����ꍇ�̒ǉ�
				AssetSheetManager check{};
				for (const auto& [hash, asset_sheet] : check.GetSheets())
				{
					if (!sheet_manager->HasSheet(hash))
					{
						sheet_manager->Register(hash);
					}
				}

				// Loader�ւ̃A�Z�b�g�o�^
				AssetLoad();
				return;
			}
			//assert(!"Not found file(AssetManager::Load)");
			return;
		}

		// �C�ӂ̃t�H���_����̓ǂݍ�
		if (!extension.contains(load_path.extension()))
			assert(!"The extension is incorrect(AssetManager::Load)");
		{
			std::ifstream ifs(load_path);
			if (!ifs)
				assert(!"Not found file(AssetManager::Load)");
			cereal::BinaryInputArchive input_archive(ifs);
			input_archive(*this);
		}
		// Loader�ւ̃A�Z�b�g�o�^
		AssetLoad();
	}

	void AssetManager::AssetLoad()
	{
		/*
		 * sheets_key : �A�Z�b�g(Texture�AModel�AMaterial...)�^�̃n�b�V���l
		 * sheets_value : AssetSheet�\����
		 */
		for (const auto& [hash, sheets] : sheet_manager->GetSheets())
		{
			/*
			 * asset_id : �A�Z�b�gID(UUID)
			 * asset_path : �A�Z�b�gID�̎w���A�Z�b�g�̃p�X
			 */
			for (const auto& [asset_id, asset_path] : sheets.sheet)
			{
				auto h = connector.at(hash);
				loaders.at(connector.at(hash))->Load(*this, asset_id);
			}
		}
	}
} // cumulonimbus::asset
