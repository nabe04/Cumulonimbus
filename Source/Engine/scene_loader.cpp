#include "scene_loader.h"

#include "scene.h"

namespace
{
	// シーンを保存するまでのパス
	const std::filesystem::path save_parent_path = "./Data/Scenes";
}

namespace cumulonimbus::asset
{
	void SceneLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& path)
	{
		const auto id = Convert(asset_manager, path, save_parent_path);
		Load(asset_manager, id);
	}

	void SceneLoader::Load(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		const auto id = Convert(asset_manager, from, to);
		Load(asset_manager, id);
	}

	void SceneLoader::Load(
		AssetManager& asset_manager,
		const mapping::rename_type::UUID& id)
	{
		// すでにシーンが存在する場合処理を抜ける
		if (scenes.contains(id))
			return;

		//シーン(「.scene」ファイルまでのパス)の登録
		SceneAsset scene_asset{};
		scene_asset.scene_file_path = asset_manager.GetAssetSheetManager().GetAssetFilename<SceneAsset>(id).c_str();
		scenes.emplace(id, scene_asset);
		// アセットシート(更新後)の保存
		asset_manager.Save();
	}

	void SceneLoader::Save(
		scene::Scene& scene,
		const std::filesystem::path& path)
	{
		std::filesystem::create_directory(path);
		// 選択されたファイルパスがアセットに存在していない場合登録する
		AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		if (asset_manager.GetAssetSheetManager().Search<SceneAsset>(path).empty())
		{
			Load(asset_manager, path);
		}
		scene.SaveScene(path.string(), path.filename().string());
	}

	void SceneLoader::CreateScene(scene::Scene& scene)
	{
		scene.CreateScene();
	}

	void SceneLoader::OpenScene(
		scene::Scene& scene,
		const std::filesystem::path& path)
	{
		// 選択されたファイルパスがアセットに存在していない場合登録する
		AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		if(asset_manager.GetAssetSheetManager().Search<SceneAsset>(path).empty())
		{
			Load(asset_manager, path);
		}

		locator::Locator::GetSystem()->SetCurrentScenePath(path.string());
		scene.LoadScene(path.parent_path().string(), path.filename().string());
	}

	void SceneLoader::Delete(AssetManager& asset_manager, const std::filesystem::path& path)
	{
		const mapping::rename_type::UUID scene_id = asset_manager.GetAssetSheetManager().Search<SceneAsset>(path);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!scenes.contains(scene_id))
			return;

		DeleteScene(scene_id, path);
		asset_manager.Save();
	}

	void SceneLoader::Delete(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id)
	{
		const std::filesystem::path delete_path = asset_manager.GetAssetSheetManager().GetAssetFilename<SceneAsset>(asset_id);
		// アセット(ID)が存在していなければ処理を抜ける
		if (!scenes.contains(asset_id))
			return;

		DeleteScene(asset_id, delete_path);
		asset_manager.Save();
	}

	bool SceneLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			file_path_helper::GetSceneExtension()
		};

		return extensions.contains(extension);
	}

	mapping::rename_type::UUID SceneLoader::Convert(
		AssetManager& asset_manager,
		const std::filesystem::path& from,
		const std::filesystem::path& to)
	{
		/*
		 * fromには「.scene」フォルダまでのパスが入っているので
		 * その親の親ファイル(./Data/Scenes)フォルダが自分のプロジェクト
		 * に存在するかで判断する
		 * (例 : ./Data/Scenes/"シーン名"/"シーン名".scene)
		 */
		if (!equivalent(from.parent_path().parent_path(), to))
		{
			// コピー先のフォルダ作成&コピー
			std::filesystem::copy(
				from, to,
				std::filesystem::copy_options::recursive |
				std::filesystem::copy_options::overwrite_existing);
		}

		const std::string filename = std::filesystem::path{ from.filename() }.replace_extension().string();
		const std::string copy_str = to.string() + "/" + filename + "/" + filename + file_path_helper::GetSceneExtension();
		const std::filesystem::path copy_path{ copy_str };

		for (const auto& [key, value] : asset_manager.GetAssetSheetManager().GetSheet<SceneAsset>().sheet)
		{// 同じアセットが存在していた場合そのアセットのID(UUID)を返す
			if (copy_path.compare(value) == 0)
				return key;
		}

		mapping::rename_type::UUID id;
		while (true)
		{// ID(UUID)の重複がないか確認
			id = utility::GenerateUUID();
			if (asset_manager.GetAssetSheetManager().GetSheet<SceneAsset>().sheet.contains(id))
				continue;
			break;
		}

		// アセットシートの登録
		asset_manager.GetAssetSheetManager().GetSheet<SceneAsset>().sheet.insert(std::make_pair(id, copy_path.string()));
		return id;
	}

	void SceneLoader::DeleteScene(const mapping::rename_type::UUID& scene_id, const std::filesystem::path& delete_path)
	{
		/*
		 * ※注意 : 「delete_path」には拡張子(「.scene」)まで含まれたパスが入っている
		 *			 ので必ずこの親階層("シーン名")のパスから削除する
		 *		例) ./Scenes/"シーン名"/"シーン名".scene
		 */

		 // 登録したプレファブ情報の削除
		scenes.erase(scene_id);
		// 現プレファブのファイルごと削除
		std::filesystem::remove_all(delete_path.parent_path());
	}

} // cumulonimbus::asset