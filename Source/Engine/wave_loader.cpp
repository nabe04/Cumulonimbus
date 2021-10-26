#include "wave_loader.h"

#include "scene.h"

namespace
{
	// ウェーブを保存するまでのパス
	const std::filesystem::path save_parent_path = "./Data/Assets/Waves";
}

namespace cumulonimbus::asset
{
	void WaveLoader::Load(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, save_parent_path);
		Load(asset_manager, id);
	}

	void WaveLoader::Load(AssetManager& asset_manager, const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void WaveLoader::Load(AssetManager& asset_manager, const mapping::rename_type::UUID& id)
	{
		// すでにシーンが存在する場合処理を抜ける
		if (waves.contains(id))
			return;

		//シーン(「.scene」ファイルまでのパス)の登録
		WaveAsset wave_asset{};
		wave_asset.wave_file_path = asset_manager.GetAssetSheetManager().GetAssetFilename<WaveAsset>(id).c_str();
		waves.emplace(id, wave_asset);
		// アセットシート(更新後)の保存
		asset_manager.Save();
	}

	void WaveLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
	}

	void WaveLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
	}

	bool WaveLoader::Supported(std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			file_path_helper::GetWaveExtension()
		};

		return extensions.contains(extension);
	}

	void WaveLoader::CreateWave(scene::Scene& scene)
	{
		scene.CreateScene();
	}

	void WaveLoader::OpenWave(scene::Scene& scene, const std::filesystem::path& path)
	{
		// 選択されたファイルパスがアセットに存在していない場合登録する
		if (AssetManager& asset_manager = *locator::Locator::GetAssetManager();
			asset_manager.GetAssetSheetManager().Search<WaveAsset>(path).empty())
		{
			Load(asset_manager, path);
		}
		// システムに現在のシーン名を登録
		locator::Locator::GetSystem()->SetCurrentScenePath(path.string());
		scene.LoadScene(path.parent_path().string(), path.filename().string());
	}

	mapping::rename_type::UUID WaveLoader::Convert(AssetManager& asset_manager, const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		if (!equivalent(from.parent_path(), to))
		{
			// コピー先のフォルダ作成&コピー
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		const std::string copy_str = to.string() + "/" + from.filename().string();
		const std::filesystem::path copy_path{ copy_str };

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<WaveAsset>().sheet)
		{// 同じアセットが存在していた場合そのアセットのID(UUID)を返す
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{// ID(UUID)の重複がないか確認
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<WaveAsset>().sheet.contains(id))
				continue;
			break;
		}

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<WaveAsset>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}
} // cumulonimbus::asset