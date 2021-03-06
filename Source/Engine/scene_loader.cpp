#include "scene_loader.h"

#include "scene.h"
#include "scene_manager.h"
#include "filename_helper.h"

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

	void SceneLoader::Rename(AssetManager& asset_manager, const mapping::rename_type::UUID& asset_id, const std::string& changed_name)
	{
		// アセットが存在しない場合処理を抜ける
		if (!scenes.contains(asset_id))
			return;

		// 変更前のファイルパス(拡張子を含まない)   ./Data/Scenes/"変更前のシーン名"/"変更前のシーン名"
		const std::filesystem::path	before_path			= std::filesystem::path{ scenes.at(asset_id).scene_file_path }.replace_extension();
		// 変更前の親ファイルパス(拡張子を含まない) ./Data/Scenes/"変更前のシーン名"
		const std::filesystem::path before_parent_path	= before_path.parent_path();

		// 重複確認済みのファイル名取得(拡張子を含まない)
		// asset_name = 重複確認済みのシーン名(ファイル名のみ、拡張子を含まない)
		const std::string asset_name = CompareAndReName<SceneAsset>(asset_manager,
																	before_parent_path.parent_path().string() + "/" +
																	changed_name + "/" + changed_name +
																	file_path_helper::GetSceneExtension()
																	).filename().replace_extension().string();

		// 変更後のファイルパス(拡張子を含まない)   ./Data/Scenes/"変更後のシーン名"/"変更後のシーン名"
		const std::filesystem::path after_path = before_parent_path.parent_path().string() + "/" + asset_name + "/" + asset_name;

		// -- ファイル & フォルダ名の変更 --//
		// 「.scene」ファイルのファイル名変更
		// 例 : ./Data/Scenes/"変更前のシーン名"/"変更前のシーン名.scene" -> ./Data/Scenes/"変更前のシーン名"/"変更後のシーン名.scene"
		std::filesystem::rename(before_path.string()		+ file_path_helper::GetSceneExtension(),
								before_parent_path.string() + "/" + asset_name + file_path_helper::GetSceneExtension());
		// 「.json」ファイルのファイル名変更
		// 例 : ./Data/Scenes/"変更前のシーン名"/"変更前のシーン名.json" -> ./Data/Scenes/"変更前のシーン名"/"変更後のシーン名.json"
		std::filesystem::rename(before_path.string()		+ file_path_helper::GetJsonExtension(),
								before_parent_path.string() + "/" + asset_name + file_path_helper::GetJsonExtension());
		// フォルダ名の変更
		// 例 : ./Data/Scenes/"変更前のシーン名" -> ./Data/Scenes/"変更後のシーン名"
		std::filesystem::rename(before_parent_path, after_path.parent_path());

		// SceneAssetのファイルパス変更( ./Data/Scenes/"変更後のシーン名"/"変更後のシーン名.scene")
		scenes.at(asset_id).scene_file_path = after_path.string() + file_path_helper::GetSceneExtension();
		// アセットシート側のファイルパス変更(例 :  ./Data/Scenes/"変更後のシーン名"/"変更後のシーン名.scene")
		asset_manager.GetAssetSheetManager().GetSheet<SceneAsset>().sheet.at(asset_id) = after_path.string() + file_path_helper::GetSceneExtension();
		// アセットシートの保存
		asset_manager.Save();
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

	bool SceneLoader::HasScene(const mapping::rename_type::UUID& scene_id) const
	{
		if (scenes.contains(scene_id))
			return true;

		return false;
	}


	bool SceneLoader::Supported(const std::filesystem::path extension)
	{
		static const std::set<std::filesystem::path> extensions
		{
			file_path_helper::GetSceneExtension()
		};

		return extensions.contains(extension);
	}

	void SceneLoader::SaveAs(
		scene::Scene& scene,
		const std::filesystem::path& path)
	{
		std::filesystem::create_directory(path);
		// 選択されたファイルパスがアセットに存在していない場合登録する
		if (AssetManager& asset_manager = *locator::Locator::GetAssetManager();
			asset_manager.GetAssetSheetManager().Search<SceneAsset>(path.string() + "/" +
																	path.filename().string() +
																	file_path_helper::GetSceneExtension()).empty())
		{
			Load(asset_manager, path);
		}
		scene.SaveScene(path.string(), path.filename().string());
	}

	void SceneLoader::Save(
		scene::Scene& scene,
		const std::filesystem::path& scene_file_dir)
	{
		scene.SaveScene(scene_file_dir.parent_path().string(), scene_file_dir.filename().replace_extension().string());
	}

	void SceneLoader::Save(
		std::unordered_map<mapping::rename_type::UUID, std::unique_ptr<scene::Scene>>& active_scenes,
		const mapping::rename_type::UUID& save_scene_id)
	{
		if (!active_scenes.contains(save_scene_id))
			assert(!"Don't have scene(SceneLoader::Save)");
		if(!scenes.contains(save_scene_id))
			assert(!"Don't have scene(SceneLoader::Save)");
		// 保存対象のシーンが保存されているファイルまでのパスを取得
		const std::filesystem::path& scene_file_dir = scenes.at(save_scene_id).scene_file_path;
		// シーン保存
		active_scenes.at(save_scene_id)->SaveScene(scene_file_dir.parent_path().string(), scene_file_dir.filename().replace_extension().string());
	}

	void SceneLoader::AddScene(
		scene::SceneManager& scene_manager,
		const mapping::rename_type::UUID& add_scene_id,
		const std::filesystem::path& add_scene_path)
	{
		scene_manager.AddScene(add_scene_id, add_scene_path);
	}

	void SceneLoader::AddScene(
		scene::SceneManager& scene_manager,
		const mapping::rename_type::UUID& add_scene_id)
	{
		const std::filesystem::path& add_scene_path = scenes.at(add_scene_id).scene_file_path;
		AddScene(scene_manager, add_scene_id, add_scene_path);
	}
	void SceneLoader::AddScene(
		scene::SceneManager& scene_manager,
		const std::filesystem::path& add_scene_path)
	{
		const mapping::rename_type::UUID add_scene_id = locator::Locator::GetAssetManager()->GetAssetSheetManager().Search<asset::SceneAsset>(add_scene_path);
		AddScene(scene_manager, add_scene_id, add_scene_path);
	}

	void SceneLoader::DestroyScene(
		scene::SceneManager& scene_manager,
		const mapping::rename_type::UUID& destroy_scene_id)
	{
		scene_manager.DeleteScene(destroy_scene_id);
	}

	void SceneLoader::CreateScene(scene::SceneManager& scene_manager)
	{
		scene_manager.DeleteAllScene();
		scene_manager.CreateNewScene();
	}

	void SceneLoader::OpenScene(
		scene::SceneManager& scene_manager,
		const std::filesystem::path& path)
	{
		// 選択されたファイルパスがアセットに存在していない場合登録する
		if (AssetManager& asset_manager = *locator::Locator::GetAssetManager();
			asset_manager.GetAssetSheetManager().Search<SceneAsset>(path).empty())
		{
			Load(asset_manager, path);
		}

		// 読み込み先のシーンIDの取得
		const mapping::rename_type::UUID scene_id = locator::Locator::GetAssetManager()->GetAssetSheetManager().Search<SceneAsset>(path);
		if (scene_id.empty())
			assert(!"Don't have scene id(SceneLoader::OpenScene)");
		//scene_manager.OpenScene(scene_id, path);
		scene_manager.SwitchScene(scene_id, path);
	}

	void SceneLoader::OpenScene(
		scene::Scene& scene,
		const std::filesystem::path& path)
	{
		// 選択されたファイルパスがアセットに存在していない場合登録する
		if(AssetManager& asset_manager = *locator::Locator::GetAssetManager();
		   asset_manager.GetAssetSheetManager().Search<SceneAsset>(path).empty())
		{
			Load(asset_manager, path);
		}
		// システムに現在のシーン名を登録
		locator::Locator::GetSystem()->SetCurrentScenePath(path.string());
		scene.LoadScene(path.parent_path().string(), path.filename().string());
	}

	std::string SceneLoader::Name(const mapping::rename_type::UUID& scene_id) const
	{
		if (!scenes.contains(scene_id))
			return filename_helper::GetNoTitled();

		return std::filesystem::path{ scenes.at(scene_id).scene_file_path }.filename().replace_extension().string();
	}

	bool SceneLoader::ImSelectableScene(AssetManager& asset_manager, mapping::rename_type::UUID& scene_id)
	{
		std::filesystem::path scene_filename{};
		const asset::AssetSheet& scene_sheet = asset_manager.GetAssetSheetManager().GetSheet<asset::SceneAsset>();
		bool is_dummy = false;

		if (scene_sheet.sheet.contains(scene_id))
		{
			scene_filename = scene_sheet.sheet.at(scene_id);
			scene_filename = scene_filename.filename();
			scene_filename.filename().replace_extension();
			is_dummy = true;
		}
		else
		{
			scene_filename = "None";
		}

		// シーンが選択された場合Trueに
		bool result{ false };

		if (ImGui::BeginCombo("Scenes", scene_filename.string().c_str()))
		{
			{// ダミーエフェクト用
				ImGui::SameLine();
				if (ImGui::Selectable(scene_filename.string().c_str(),
					is_dummy, 0))
				{
					scene_id = { "" };
				}
				if (is_dummy)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			for (const auto& [id, tex_filepath] : scene_sheet.sheet)
			{
				ImGui::PushID(id.c_str());

				const bool is_selected = (scene_id == id);
				std::filesystem::path tex_filename = tex_filepath;
				if (ImGui::Selectable(tex_filename.filename().replace_extension().string().c_str(),
					is_selected, 0))
				{
					result = true;
					scene_id = id;
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}

				ImGui::PopID();
			}
			ImGui::EndCombo();
		}

		return result;
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