#include "effekseer_loader.h"

#include <set>
#include <filesystem>

#include "locator.h"

namespace
{
	// エフェクトを保存するまでのパス
	const std::filesystem::path save_parent_path = "./Data/Assets/Effects";
}

namespace cumulonimbus::asset
{
	void EffekseerLoader::Load(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, save_parent_path);
		Load(asset_manager, id);
	}

	void EffekseerLoader::Load(AssetManager& asset_manager, const std::filesystem::path& from, const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void EffekseerLoader::Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id)
	{
		// すでにテクスチャが存在する場合は処理を抜ける
		if (effects.contains(id))
			return;

		// テクスチャの作成
		effects.insert(std::make_pair(
			id,
			std::make_unique<Effect>(asset_manager.GetAssetSheetManager().GetAssetFilename<Effect>(id).c_str()))
		);
		// アセットシート(更新後)の保存
		asset_manager.Save();
	}

	void EffekseerLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id,
		const std::string& changed_name)
	{
		// アセットが存在しない場合処理を抜ける
		if (!effects.contains(asset_id))
			return;

		// テクスチャ拡張子
		const std::string extension = std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Effect>(asset_id) }.extension().string();
		// 変更前のファイルパス(拡張子を含まない)   ./Data/Textures/"変更前のテクスチャ名"
		const std::filesystem::path before_path = std::filesystem::path{ asset_manager.GetAssetSheetManager().GetAssetFilename<asset::Effect>(asset_id) }.replace_extension();

		const std::string asset_name = CompareAndReName<Effect>(asset_manager,
			before_path.parent_path().string() + "/" + changed_name + extension
			).filename().replace_extension().string();

		// 変更後のファイルパス(拡張子を含まない)   ./Data/Textures/"変更後のテクスチャ名"
		const std::filesystem::path after_path = before_path.parent_path().string() + "/" + asset_name;

		// ファイル名の変更
		// 例 : ./Data/Effects/"エフェクトファイル名"/"変更前のエフェクト名".拡張子 -> ./Data/Textures/"変更後のテクスチャ名".拡張子
		std::filesystem::rename(before_path.string() + extension,
			after_path.string() + extension);

		// アセットシート側のファイルパス変更(例 : ./Data/Materials/"変更後のマテリアル名"/"変更後のマテリアル名.mat")
		asset_manager.GetAssetSheetManager().GetSheet<Effect>().sheet.at(asset_id) = after_path.string() + extension;
		// アセットシートの保存
		asset_manager.Save();
	}

	void EffekseerLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID efk_id = asset_manager.GetAssetSheetManager().Search<Effect>(path);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!effects.contains(efk_id))
			return;

		DeleteEffect(efk_id, path);
		asset_manager.Save();
	}

	void EffekseerLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{// アセット(ID)が存在していなければ処理を抜ける
		if (!effects.contains(asset_id))
			return;

		const std::filesystem::path path = asset_manager.GetAssetSheetManager().GetAssetFilename<Effect>(asset_id);
		DeleteEffect(asset_id, path);
		asset_manager.Save();
	}

	bool EffekseerLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			".efk",".efkfc"
		};
		return extensions.contains(extension);
	}

	mapping::rename_type::UUID EffekseerLoader::Convert(AssetManager& asset_manager, const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto filename = std::filesystem::path{ from }.filename().replace_extension().string();
		std::filesystem::create_directory(to.string());
		std::filesystem::create_directory(to.string() + "/" + filename);
		const auto to_path = to.string() + "/" + filename;
		if (!equivalent(from.parent_path(), to_path))
		{
			// コピー先のフォルダ作成&コピー
			std::filesystem::copy(
				from.parent_path(), to_path,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		const std::string copy_str = to_path + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<Effect>().sheet)
		{// 同じアセットが存在していた場合そのアセットのID(UUID)を返す
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{// ID(UUID)の重複がないか確認
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<Effect>().sheet.contains(id))
				continue;
			break;
		}

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<Effect>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	void EffekseerLoader::DeleteEffect(const mapping::rename_type::UUID& efk_id, const std::filesystem::path& delete_path)
	{
		// エフェクトアセットの削除
		effects.erase(efk_id);
		// ファイルの削除
		std::filesystem::remove(delete_path.parent_path());
	}

} // cumulonimbus::asset
